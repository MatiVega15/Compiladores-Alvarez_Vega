#include "AnalizadorSemantico.h"

#include <stdio.h>
#include <string.h>

/**
 * Contexto utilizado durante el análisis semántico.
 * 
 * Contiene:
 * - La Tabla de Símbolos utilizada durante el análisis.
 * - La cantidad de errores semánticos encontrados.
 * - El archivo donde se registra la información del análisis.
 * - El modo de depuración.
 * - Un puntero al nodo raíz del AST.
 * - El tipo de retorno de la función corriente.
 */
typedef struct {
    TablaSimbolos *ts;
    int errores;

    FILE *salida_sem;
    int modo_debug;

    NodoAST *programa;

    TipoDato tipo_retorno_actual;
} ContextoSemantico;

/* =========== Funciones auxiliares privadas =========== */

// Análisis principal.

static void analizar_programa (NodoAST *nodo, ContextoSemantico *contexto);
static void verificar_main (ContextoSemantico *contexto);

// Declaraciones.

static void analizar_declaracion (NodoAST *nodo, ContextoSemantico *contexto);
static void analizar_declaracion_variable (NodoAST *nodo, ContextoSemantico *contexto);
static void analizar_declaracion_funcion (NodoAST *nodo, ContextoSemantico *contexto);
static void analizar_parametros (NodoAST *nodo, ContextoSemantico *contexto);
static void analizar_parametro (NodoAST *nodo, ContextoSemantico *contexto);

// Bloques y sentencias.

static void analizar_bloque (NodoAST *nodo, ContextoSemantico *contexto);
static void analizar_sentencia (NodoAST *nodo, ContextoSemantico *contexto);
static void analizar_if (NodoAST *nodo, ContextoSemantico *contexto);
static void analizar_while (NodoAST *nodo, ContextoSemantico *contexto);
static void analizar_return (NodoAST *nodo, ContextoSemantico *contexto);

// Expresiones.

static void analizar_asignacion (NodoAST *nodo, ContextoSemantico *contexto);
static TipoDato analizar_expresion (NodoAST *nodo, ContextoSemantico *contexto);
static TipoDato analizar_identificador (NodoAST *nodo, ContextoSemantico *contexto);
static TipoDato analizar_operacion_aritmetica (NodoAST *nodo, ContextoSemantico *contexto);
static TipoDato analizar_modulo (NodoAST *nodo, ContextoSemantico *contexto);
static TipoDato analizar_comparacion (NodoAST *nodo, ContextoSemantico *contexto);
static TipoDato analizar_igualdad (NodoAST *nodo, ContextoSemantico *contexto);
static TipoDato analizar_operacion_logica (NodoAST *nodo, ContextoSemantico *contexto);
static TipoDato analizar_negacion (NodoAST *nodo, ContextoSemantico *contexto);
static TipoDato analizar_menos_unario (NodoAST *nodo, ContextoSemantico *contexto);
static TipoDato analizar_llamada (NodoAST *nodo, ContextoSemantico *contexto);
static NodoAST *buscar_declaracion_funcion (const char *nombre, ContextoSemantico *contexto);
static int analizar_argumentos (NodoAST *llamada, NodoAST *funcion, ContextoSemantico *contexto);

// Declaración y símbolos.

static Simbolo *declarar_identificador (NodoAST *identificador, TipoDato tipo, ClaseSimbolo clase, ContextoSemantico *contexto);
static void registrar_declaracion (NodoAST *identificador, TipoDato tipo, ClaseSimbolo clase, ContextoSemantico *contexto);

// Retornos.

static int garantiza_retorno_bloque (NodoAST *nodo);
static int garantiza_retorno_sentencia (NodoAST *nodo);

// Tipos y compatibilidad.

static int es_tipo_numerico (TipoDato tipo);
static int tipos_compatibles (TipoDato destino, TipoDato origen);
static TipoDato tipo_resultado_aritmetico (TipoDato izquierdo, TipoDato derecho);

// Utilidades.

static const char *tipo_a_string (TipoDato tipo);
static const char *clase_a_string (ClaseSimbolo clase);

// Evaluación de constantes.

static int evaluar_constante_numerica (NodoAST *nodo, double *valor);
static int es_constante_cero (NodoAST *nodo);

// Diagnóstico.

static void registrar_semantica (ContextoSemantico *contexto, int linea, int columna, const char *mensaje);
static void debug_semantico (ContextoSemantico *contexto, const char *mensaje);
static void error_semantico (ContextoSemantico *contexto, int linea, int columna, const char *mensaje);
static void warning_semantico (ContextoSemantico *contexto, int linea, int columna, const char *mensaje);

/* =========== Función principal =========== */

int analizar_semantica (NodoAST *arbol, TablaSimbolos *ts, FILE *salida_sem, int modo_debug) {
    ContextoSemantico contexto;

    // Se verifica que existan el AST y la TS.
    if (arbol == NULL || ts == NULL) {
        return 1;
    }

    // El nodo raíz debe representar un programa.
    if (arbol -> tipo != AST_PROGRAMA) {
        return 1;
    }

    // Se inicializa el contexto del análisis.
    contexto.ts = ts;
    contexto.errores = 0;
    contexto.salida_sem = salida_sem;
    contexto.modo_debug = modo_debug;
    contexto.programa = arbol;
    contexto.tipo_retorno_actual = TIPO_NO_DEFINIDO;

    // Se comienza el análisis desde la raíz del AST.
    analizar_programa (arbol, &contexto);

    // Se devuelve la cantidad de errores encontrados.
    return contexto.errores;
}

/* =========== Funciones auxiliares privadas =========== */

/**
 * Analiza semánticamente el nodo raíz del programa.
 * 
 * El nodo AST_PROGRAMA contiene directamente las declaraciones
 * globales del programa como hijos, manteniendo el mismo orden
 * en que aparecen en el código fuente.
 */
static void analizar_programa (NodoAST *nodo, ContextoSemantico *contexto) {
    debug_semantico (contexto, "Entrando al programa.");

    // Se recorren las declaraciones globales en su orden de aparición.
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        analizar_declaracion (nodo -> hijos [i], contexto);
    }

    // Se verifica la existencia y la forma correcta de main.
    verificar_main (contexto);

    debug_semantico (contexto, "Saliendo del programa.");
}

/**
 * Verifica las restricciones semánticas correspondientes
 * a la función main.
 * 
 * Todo programa debe contener una función denominada main
 * y dicha función no puede recibir parámetros.
 */
static void verificar_main (ContextoSemantico *contexto) {
    NodoAST *main = buscar_declaracion_funcion ("main", contexto);

    // Todo programa debe contener una función main.
    if (main == NULL) {
        error_semantico (contexto, 0, 0, "El programa debe contener una función 'main'.");
        return;
    }

    // Los hijos parámetros están luego del nombre y antes del bloque.
    int cantidad_parametros = main -> cantidad_hijos - 2;

    // Main con parámetros.
    if (cantidad_parametros != 0) {
        NodoAST *identificador = main -> hijos [0];

        error_semantico (contexto, identificador -> linea, identificador -> columna, "La función 'main' no puede recibir parámetros.");
    }
}

/**
 * Analiza una declaración, global o local, del programa.
 * 
 * Las declaraciones pueden corresponder a variables o
 * bien a funciones.
 */
static void analizar_declaracion (NodoAST *nodo, ContextoSemantico *contexto) {
    // Se determina el tipo de declaración que representa el nodo.
    switch (nodo -> tipo) {
        case AST_DECLARACION_VARIABLE:
            analizar_declaracion_variable (nodo, contexto);
            break;
        case AST_DECLARACION_FUNCION:
            analizar_declaracion_funcion (nodo, contexto);
            break;
        default:
            break;   
    }
}

/**
 * Analiza semánticamente una declaración de variable.
 * 
 * Cada hijo del nodo AST_DECLARACION_VARIABLE corresponde
 * a un identificador declarado con el mismo tipo.
 */
static void analizar_declaracion_variable (NodoAST *nodo, ContextoSemantico *contexto) {
    TipoDato tipo = nodo -> tipo_dato;

    // Se recorren todos los identificadores declarados.
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        NodoAST *identificador = nodo -> hijos [i];

        // Se declara el identificador como variable.
        Simbolo *simbolo = declarar_identificador (identificador, tipo, SIMBOLO_VARIABLE, contexto);

        // Si la declaración no fue válida, se continúa con el siguiente identificador.
        if (simbolo == NULL) {
            continue;
        }

        registrar_declaracion (identificador, tipo, SIMBOLO_VARIABLE, contexto);

        char mensaje [200];
        snprintf (mensaje, sizeof (mensaje), "Variable '%s' declarada como %s.", identificador -> valor.identificador, tipo_a_string (tipo));
        debug_semantico (contexto, mensaje);
    }
}

/**
 * Analiza semánticamente una declaración de función. 
 * 
 * La función se inserta en el nivel actual de la TS.
 * 
 * Se abre un nuevo nivel correspondiente al ámbito de
 * la función y se analizan sus parámetros.
 * 
 * El mismo nivel se utiliza para las variables locales.
 */
static void analizar_declaracion_funcion (NodoAST *nodo, ContextoSemantico *contexto) {    
    // El primer hijo es el identificador de la función.
    NodoAST *identificador = nodo -> hijos [0];

    // El nodo debe representar un identificador.
    if (identificador == NULL || identificador -> tipo != AST_IDENTIFICADOR) {
        return;
    }

    // Tipo de retorno de la función.
    TipoDato tipo = nodo -> tipo_dato;

    // Se declara la función en el nivel actual.
    Simbolo *simbolo = declarar_identificador (identificador, tipo, SIMBOLO_FUNCION, contexto);

    // Si la declaración no fue válida, no se analiza el contenido de la función.
    if (simbolo == NULL) {
        return;
    }

    registrar_declaracion (identificador, tipo, SIMBOLO_FUNCION, contexto);

    char mensaje [200];
    snprintf (mensaje, sizeof (mensaje), "Función '%s' declarada con retorno %s.", identificador -> valor.identificador, tipo_a_string (tipo));
    debug_semantico (contexto, mensaje);

    // Se guarda el tipo de retorno de la función anterior.
    TipoDato tipo_retorno_anterior = contexto -> tipo_retorno_actual;

    // Se establece el tipo de retorno de la función actual.
    contexto -> tipo_retorno_actual = tipo;

    // Se abre el nivel correspondiente al ámbito de la función.
    debug_semantico (contexto, "Abriendo ámbito de función.");
    abrir_nivel (contexto -> ts);

    // Se analizan los parámetros de la función.
    analizar_parametros (nodo, contexto);

    // El último hijo es el bloque.
    NodoAST *bloque = nodo -> hijos [nodo -> cantidad_hijos - 1];

    // Se analiza el bloque de la función.
    analizar_bloque (bloque, contexto);

    // Las funciones no void deben garantizar un return en todos los caminos posibles de ejecución.
    if (tipo != TIPO_VOID && !garantiza_retorno_bloque (bloque)) {
        error_semantico (contexto, identificador -> linea, identificador -> columna, "La función no void debe retornar un valor en todos los caminos de ejecución.");
    }

    // Se cierra el ámbito de la función.
    cerrar_nivel (contexto -> ts);
    debug_semantico (contexto, "Cerrando ámbito de función.");

    // Se restaura el tipo de retorno anterior.
    contexto -> tipo_retorno_actual = tipo_retorno_anterior;
}

/**
 * Analiza los parámetros de una función.
 * 
 * En el AST, el primer hijo de AST_DECLARACION_FUNCION
 * corresponde al nombre de la función y el último hijo
 * corresponde a su bloque.
 * 
 * Los hijos ubicados entre ambos extremos representan
 * los parámetros de la función.
 */
static void analizar_parametros (NodoAST *nodo, ContextoSemantico *contexto) {
    // Se recorren únicamente los hijos que corresponden a parámetros.
    for (int i = 1; i < nodo -> cantidad_hijos - 1; i ++) {
        NodoAST *parametro = nodo -> hijos [i];

        // El nodo debe representar un parámetro.
        if (parametro == NULL || parametro -> tipo != AST_PARAMETRO) {
            continue;
        }

        analizar_parametro (parametro, contexto);
    }
}

/**
 * Analiza semánticamente un parámetro de función.
 * 
 * El nodo AST_PARAMETRO contiene como único hijo
 * al identificador del parámetro.
 * 
 * El parámetro se inserta en el nivel correspondiente
 * al ámbito de la función.
 */
static void analizar_parametro (NodoAST *nodo, ContextoSemantico *contexto) {
    // El único hijo es el identificador.
    NodoAST *identificador = nodo -> hijos [0];

    // El nodo debe representar un identificador.
    if (identificador == NULL || identificador -> tipo != AST_IDENTIFICADOR) {
        return;
    }

    // Tipo declarado para el parámetro.
    TipoDato tipo = nodo -> tipo_dato;

    // Se declara el identificador como parámetro.
    Simbolo *simbolo = declarar_identificador (identificador, tipo, SIMBOLO_PARAMETRO, contexto);

    // Si la declaración no fue válida, no se continúa con el registro del parámetro.
    if (simbolo == NULL) {
        return;
    }

    // Se asocia el nodo del AST al símbolo de la TS.
    nodo -> simbolo = simbolo;

    registrar_declaracion (identificador, tipo, SIMBOLO_PARAMETRO, contexto);

    char mensaje [200];
    snprintf (mensaje, sizeof (mensaje), "Parámetro '%s' declarado como %s.", identificador -> valor.identificador, tipo_a_string (tipo));
    debug_semantico (contexto, mensaje);
}

/**
 * Analiza semánticamente un bloque del programa.
 * 
 * Las declaraciones de variables locales se analizan antes
 * de las sentencias.
 * 
 * El ámbito del bloque no se abre ni se cierra, ya que esa
 * responsabilidad es del invocante de la función.
 */
static void analizar_bloque (NodoAST *nodo, ContextoSemantico *contexto) {
    // El nodo debe representar un bloque.
    if (nodo == NULL || nodo -> tipo != AST_BLOQUE) {
        return;
    }

    debug_semantico (contexto, "Analizando bloque.");

    // Se recorren los hijos en el orden original.
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        NodoAST *hijo = nodo -> hijos [i];

        if (hijo == NULL) {
            continue;
        }
        
        // Declaraciones locales.
        if (hijo -> tipo == AST_DECLARACION_VARIABLE) {
            analizar_declaracion_variable (hijo, contexto);
        }
        // Sentencias.
        else {
            analizar_sentencia (hijo, contexto);
        }
    }

    debug_semantico (contexto, "Bloque analizado.");
}

/**
 * Analiza semánticamente una sentencia.
 * 
 * La función identifica el tipo de sentencia y delega su
 * análisis a la función correspondiente.
 * 
 * Los bloques anidados crean un nuevo nivel de la TS para
 * permitir variables locales y shadowing.
 */
static void analizar_sentencia (NodoAST *nodo, ContextoSemantico *contexto) {
    // No se puede analizar una sentencia inexistente.
    if (nodo == NULL) {
        return;
    }

    switch (nodo -> tipo) {
        case AST_ASIGNACION:
            analizar_asignacion (nodo, contexto);
            break;
        case AST_LLAMADA:
            analizar_llamada (nodo, contexto);
            break;
        case AST_IF:
            analizar_if (nodo, contexto);
            break;
        case AST_WHILE:
            analizar_while (nodo, contexto);
            break;
        case AST_RETURN:
            analizar_return (nodo, contexto);
            break;
        case AST_SENTENCIA_VACIA:
            break;
        case AST_BLOQUE:
            debug_semantico (contexto, "Abriendo ámbito de bloque.");
            abrir_nivel (contexto -> ts);

            analizar_bloque (nodo, contexto);
            
            cerrar_nivel (contexto -> ts);
            debug_semantico (contexto, "Cerrando ámbito de bloque.");
            break;
        default:
            break;
    }
}

/**
 * Analiza semánticamente una sentencia if.
 * 
 * Verifica que la condición sea de tipo boolean y
 * analiza semánticamente los bloques then y else,
 * creando un nuevo nivel de la TS para cada bloque.
 */
static void analizar_if (NodoAST *nodo, ContextoSemantico *contexto) {
    // El nodo debe representar una sentencia if.
    if (nodo == NULL || nodo -> tipo != AST_IF) {
        return;
    }

    // Se analiza la condición (primer hijo).
    TipoDato tipo_condicion = analizar_expresion (nodo -> hijos [0], contexto);

    // La condición de un if debe ser de tipo boolean.
    if (tipo_condicion != TIPO_BOOLEAN && tipo_condicion != TIPO_NO_DEFINIDO) {
        error_semantico (contexto, nodo -> hijos [0] -> linea, nodo -> hijos [0] -> columna, "La condición de un if debe ser de tipo boolean.");
    }

    char mensaje [200];
    snprintf (mensaje, sizeof (mensaje), "Condición de if analizada: tipo %s.", tipo_a_string (tipo_condicion));
    debug_semantico (contexto, mensaje);

    // El bloque then constituye un nuevo ámbito.
    debug_semantico (contexto, "Abriendo ámbito de bloque then (if).");
    abrir_nivel (contexto -> ts);

    // Se analiza el bloque then (segundo hijo).
    analizar_bloque (nodo -> hijos [1], contexto);

    // Se cierra el ámbito del bloque then.
    cerrar_nivel (contexto -> ts);
    debug_semantico (contexto, "Cerrando ámbito de bloque then (if).");

    // Si existe el bloque else, constituye un nuevo ámbito.
    if (nodo -> cantidad_hijos >= 3) {
        debug_semantico (contexto, "Abriendo ámbito de bloque else (if).");
        abrir_nivel (contexto -> ts);

        // Se analiza el bloque else (tercer hijo).
        analizar_bloque (nodo -> hijos [2], contexto);

        // Se cierra el ámbito del bloque else.
        cerrar_nivel (contexto -> ts);
        debug_semantico (contexto, "Cerrando ámbito de bloque else (if).");
    }

    registrar_semantica (contexto, nodo -> linea, nodo -> columna, "Sentencia if analizada.");
}

/**
 * Analiza semánticamente una sentencia while.
 * 
 * Verifica que la condición sea de tipo boolean y analiza
 * semánticamente el bloque que constituye el cuerpo del ciclo.
 * 
 * El cuerpo del while constituye un nuevo ámbito de la TS.
 */
static void analizar_while (NodoAST *nodo, ContextoSemantico *contexto) {
    // El nodo debe representar una sentencia while.
    if (nodo == NULL || nodo -> tipo != AST_WHILE) {
        return;
    }

    // Se analiza la condición (primer hijo).
    TipoDato tipo_condicion = analizar_expresion (nodo -> hijos [0], contexto);

    // La condición de un while debe ser de tipo boolean.
    if (tipo_condicion != TIPO_BOOLEAN && tipo_condicion != TIPO_NO_DEFINIDO) {
        error_semantico (contexto, nodo -> hijos [0] -> linea, nodo -> hijos [0] -> columna, "La condición de un while debe ser de tipo boolean.");
    }

    char mensaje [200];
    snprintf (mensaje, sizeof (mensaje), "Condición de while analizada: tipo %s.", tipo_a_string (tipo_condicion));
    debug_semantico (contexto, mensaje);

    // El cuerpo del while constituye un nuevo ámbito.
    debug_semantico (contexto, "Abriendo ámbito de bloque while.");
    abrir_nivel (contexto -> ts);

    // Se analiza el bloque (segundo hijo).
    analizar_bloque (nodo -> hijos [1], contexto);

    // Se cierra el ámbito del cuerpo del while.
    cerrar_nivel (contexto -> ts);
    debug_semantico (contexto, "Cerrando ámbito de bloque while.");

    registrar_semantica (contexto, nodo -> linea, nodo -> columna, "Sentencia while analizada.");
}

/**
 * Analiza semánticamente una sentencia return.
 * 
 * Verifica que:
 * - Una función void utilice return sin expresión.
 * - Una función no void utilice return con una expresión.
 * - El tipo de la expresión retornada sea compatible con
 *   el tipo de retorno de la función.
 * 
 * Se permite conversión implícita entre int y float.
 */
static void analizar_return (NodoAST *nodo, ContextoSemantico *contexto) {
    // El nodo debe representar un retorno.
    if (nodo == NULL || nodo -> tipo != AST_RETURN) {
        return;
    }

    // Se obtiene el tipo de retorno de la función actual.
    TipoDato tipo_retorno = contexto -> tipo_retorno_actual;

    // Return sin expresión.
    if (nodo -> cantidad_hijos == 0) {
        // Una función no void debe retornar un valor.
        if (tipo_retorno != TIPO_VOID) {
            error_semantico (contexto, nodo -> linea, nodo -> columna, "Una función no void debe retornar un valor.");
            return;
        }

        // El return vacío es válido en una función void.
        registrar_semantica (contexto, nodo -> linea, nodo -> columna, "Return sin expresión válido.");

        debug_semantico (contexto, "Return sin expresión analizado correctamente.");

        return;
    }

    // Se obtiene la expresión retornada.
    NodoAST *expresion = nodo -> hijos [0];

    // Se analiza semánticamente la expresión.
    TipoDato tipo_expresion = analizar_expresion (expresion, contexto);

    // Si la expresión contiene un error semántico, se detiene la comprobación.
    if (tipo_expresion == TIPO_NO_DEFINIDO) {
        return;
    }

    // Una función void no puede retornar un valor.
    if (tipo_retorno == TIPO_VOID) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "Una función void no puede retornar un valor.");
        return;
    }

    // Se verifica la compatibilidad de tipos.
    if (!tipos_compatibles (tipo_retorno, tipo_expresion)) {
        char mensaje [200];
        snprintf (mensaje, sizeof (mensaje), "No se puede retornar un valor de tipo %s en una función de tipo %s.", tipo_a_string (tipo_expresion), tipo_a_string (tipo_retorno));
        error_semantico (contexto, nodo -> linea, nodo -> columna, mensaje);

        return;
    }

    // Si los tipos son diferentes pero compatibles, se registra la conversión implícita.
    if (tipo_retorno != tipo_expresion) {
        char mensaje [200];
        snprintf (mensaje, sizeof (mensaje), "Conversión implícita de %s a %s en el return.", tipo_a_string (tipo_expresion), tipo_a_string (tipo_retorno));
        warning_semantico (contexto, expresion -> linea, expresion -> columna, mensaje);
    }

    registrar_semantica (contexto, nodo -> linea, nodo -> columna, "Return con expresión válido.");
    
    char mensaje [200];
    snprintf (mensaje, sizeof (mensaje), "Return válido: expresión de tipo %s, función retorna %s.", tipo_a_string (tipo_expresion), tipo_a_string (tipo_retorno));
    debug_semantico (contexto, mensaje);
}

/**
 * Analiza semánticamente una asignación.
 * 
 * Verifica que:
 * - El destino esté declarado.
 * - El destino sea una variable o parámetro.
 * - La expresión asignada sea semánticamente válida.
 * - El tipo de la expresión sea compatible con el destino.
 * 
 * Se permite conversión implícita entre int y float.
 * No se permite conversiones entre tipos numéricos y boolean.
 * 
 * Si la asignación es válida, el símbolo correspondiente
 * queda marcado como inicializado.
 */
static void analizar_asignacion (NodoAST *nodo, ContextoSemantico *contexto) {
    // El nodo debe representar una asignación.
    if (nodo == NULL || nodo -> tipo != AST_ASIGNACION) {
        return;
    }

    // El primer hijo representa el destino.
    NodoAST *identificador = nodo -> hijos [0];

    // El destino debe ser un identificador.
    if (identificador == NULL || identificador -> tipo != AST_IDENTIFICADOR) {
        return;
    }

    // Se busca el símbolo correspondiente al destino.
    Simbolo *simbolo = buscar_elemento (contexto -> ts, identificador -> valor.identificador);

    // El destino debe haber sido declarado previamente.
    if (simbolo == NULL) {
        error_semantico (contexto, identificador -> linea, identificador -> columna, "El identificador utilizado en la asignación no está declarado.");
        return;
    }

    // Una asignación solamente puede modificar variables o parámetros.
    if (simbolo -> clase != SIMBOLO_VARIABLE && simbolo -> clase != SIMBOLO_PARAMETRO) {
        error_semantico (contexto, identificador -> linea, identificador -> columna, "El identificador utilizado en la asignación no es una variable.");
        return;
    }

    // Se asocia el nodo del AST con su símbolo.
    identificador -> simbolo = simbolo;
    identificador -> tipo_dato = simbolo -> tipo;

    char mensaje [200];
    snprintf (mensaje, sizeof (mensaje), "Analizando asignación a '%s'.", identificador -> valor.identificador);
    debug_semantico (contexto, mensaje);

    // Se analiza la expresión ubicada a la derecha.
    TipoDato tipo_expresion = analizar_expresion (nodo -> hijos [1], contexto);

    // Si la expresión no tiene un tipo válido, la asignación no es válida.
    if (tipo_expresion == TIPO_NO_DEFINIDO) {
        return;
    }

    // Se verifica la compatibilidad entre ambos tipos.
    if (!tipos_compatibles (simbolo -> tipo, tipo_expresion)) {
        char mensaje [200];
        snprintf (mensaje, sizeof (mensaje), "No se puede asignar un valor de tipo %s a una variable de tipo %s.", tipo_a_string (tipo_expresion), tipo_a_string (simbolo -> tipo));
        error_semantico (contexto, identificador -> linea, identificador -> columna, mensaje);
        return;
    }

    // Si los tipos son diferentes pero compatibles, se registra la conversión implícita.
    if (simbolo -> tipo != tipo_expresion) {
        char mensaje [200];
        snprintf (mensaje, sizeof (mensaje), "Conversión implícita de %s a %s.", tipo_a_string (tipo_expresion), tipo_a_string (simbolo -> tipo));
        warning_semantico (contexto, identificador -> linea, identificador -> columna, mensaje);
    }

    // La asignación fue válida, el destino queda inicializado.
    simbolo -> inicializada = 1;

    registrar_semantica (contexto, identificador -> linea, identificador -> columna, "Asignación válida.");

    char mensaje2 [200];
    snprintf (mensaje2, sizeof (mensaje2), "Asignación: destino '%s' (%s), expresión (%s).", identificador -> valor.identificador, tipo_a_string (simbolo -> tipo), tipo_a_string (tipo_expresion));
    debug_semantico (contexto, mensaje2);
}

/**
 * Analiza una expresión.
 * 
 * Esta función solamente determina qué tipo de expresión
 * representa el nodo y delega el análisis específico a la
 * función correspondiente.
 */
static TipoDato analizar_expresion (NodoAST *nodo, ContextoSemantico *contexto) {
    if (nodo == NULL) {
        return TIPO_NO_DEFINIDO;
    }

    switch (nodo -> tipo) {
        // Literales.
        case AST_NUMERO:
            return TIPO_INT;
        case AST_REAL:
            return TIPO_FLOAT;
        case AST_TRUE:
        case AST_FALSE:
            return TIPO_BOOLEAN;
            
        // Identificadores.
        case AST_IDENTIFICADOR:
            return analizar_identificador (nodo, contexto);

        // Operadores aritméticos.
        case AST_SUMA:
        case AST_RESTA:
        case AST_MULTIPLICACION:
        case AST_DIVISION:
            return analizar_operacion_aritmetica (nodo, contexto);
        case AST_MODULO:
            return analizar_modulo (nodo, contexto);

        // Operadores relacionales.
        case AST_MENOR:
        case AST_MAYOR:
            return analizar_comparacion (nodo, contexto);

        // Igualdad.
        case AST_IGUAL:
            return analizar_igualdad (nodo, contexto);

        // Operadores lógicos.
        case AST_AND:
        case AST_OR:
            return analizar_operacion_logica (nodo, contexto);

        // Operadores unarios.
        case AST_NEGACION:
            return analizar_negacion (nodo, contexto);
        case AST_MENOS_UNARIO:
            return analizar_menos_unario (nodo, contexto);

        // Llamada a funciones.
        case AST_LLAMADA: {
            TipoDato tipo = analizar_llamada (nodo, contexto);

            // Una función void no puede ser utilizada en una expresión.
            if (tipo == TIPO_VOID) {
                error_semantico (contexto, nodo -> linea, nodo -> columna, "Una función void no puede utilizarse como expresión.");
                return TIPO_NO_DEFINIDO;
            }

            return tipo;
        }

        default:
            return TIPO_NO_DEFINIDO;
    }
}

/**
 * Analiza un identificador utilizado dentro de una expresión.
 * 
 * Verifica que el identificador esté declarado, que corresponda
 * a una variable o parámetro y que haya sido inicializado antes
 * de su utilización.
 */
static TipoDato analizar_identificador (NodoAST *nodo, ContextoSemantico *contexto) {
    Simbolo *simbolo = buscar_elemento (contexto -> ts, nodo -> valor.identificador);

    // El identificador debe estar declarado.
    if (simbolo == NULL) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "El identificador utilizado en la expresión no está declarado.");
        return TIPO_NO_DEFINIDO;
    }

    // Una expresión solamente puede utilizar variables o parámetros como valores.
    if (simbolo -> clase != SIMBOLO_VARIABLE && simbolo -> clase != SIMBOLO_PARAMETRO) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "El identificador utilizado en la expresión no representa una variable.");
        return TIPO_NO_DEFINIDO;
    }

    // El identificador debe estar inicializado.
    if (!simbolo -> inicializada) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "La variable utilizada en la expresión no está inicializada.");
        return TIPO_NO_DEFINIDO;
    }

    // Se asocia el nodo del AST con su símbolo.
    nodo -> simbolo = simbolo;
    nodo -> tipo_dato = simbolo -> tipo;

    char mensaje [200];
    snprintf (mensaje, sizeof (mensaje), "Identificador '%s' resuelto como %s de tipo %s.", nodo -> valor.identificador, clase_a_string (simbolo -> clase), tipo_a_string (simbolo -> tipo));
    debug_semantico (contexto, mensaje);

    return simbolo -> tipo;
}

/**
 * Analiza una operación aritmética binaria.
 * 
 * Los operadores +, -, * y / requieren operandos numéricos.
 * 
 * Si ambos operandos son int, el resultado es int.
 * Si al menos uno es float, el resultado es float.
 */
static TipoDato analizar_operacion_aritmetica (NodoAST *nodo, ContextoSemantico *contexto) {
    TipoDato izquierdo = analizar_expresion (nodo -> hijos [0], contexto);
    TipoDato derecho = analizar_expresion (nodo -> hijos [1], contexto);

    // Si alguno de los operandos contiene un error, se frena el análisis.
    if (izquierdo == TIPO_NO_DEFINIDO || derecho == TIPO_NO_DEFINIDO) {
        return TIPO_NO_DEFINIDO;
    }

    // Los operadores aritméticos requieren operandos numéricos.
    if (!es_tipo_numerico (izquierdo) || !(es_tipo_numerico (derecho))) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "Los operadores aritméticos requieren operandos numéricos.");
        return TIPO_NO_DEFINIDO;
    }

    // Una división no puede tener un divisor constante igual a cero.
    if (nodo -> tipo == AST_DIVISION && es_constante_cero (nodo -> hijos [1])) {
        error_semantico (contexto, nodo -> hijos [1] -> linea, nodo -> hijos [1] -> columna, "No se puede dividir por cero.");
        return TIPO_NO_DEFINIDO;
    }

    TipoDato resultado = tipo_resultado_aritmetico (izquierdo, derecho);
    nodo -> tipo_dato = resultado;

    const char *operador;
    switch (nodo -> tipo) {
        case AST_SUMA: operador = "+"; break;
        case AST_RESTA: operador = "-"; break;
        case AST_MULTIPLICACION: operador = "*"; break;
        default: operador = "/"; break;
    }

    char mensaje [200];
    snprintf (mensaje, sizeof (mensaje), "Operación '%s': %s y %s -> %s.", operador, tipo_a_string (izquierdo), tipo_a_string (derecho), tipo_a_string (resultado));
    debug_semantico (contexto, mensaje);

    return resultado;
}

/**
 * Analiza una operación módulo.
 * 
 * El operador % solamente admite operandos de tipo int
 * y siempre produce un resultado de tipo int.
 */
static TipoDato analizar_modulo (NodoAST *nodo, ContextoSemantico *contexto) {
    TipoDato izquierdo = analizar_expresion (nodo -> hijos [0], contexto);
    TipoDato derecho = analizar_expresion (nodo -> hijos [1], contexto);

    // Si alguno de los operandos contiene un error, se frena el análisis.
    if (izquierdo == TIPO_NO_DEFINIDO || derecho == TIPO_NO_DEFINIDO) {
        return TIPO_NO_DEFINIDO;
    }

    // El módulo solamente admite enteros.
    if (izquierdo != TIPO_INT || derecho != TIPO_INT) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "El operador módulo requiere operandos de tipo int.");
        return TIPO_NO_DEFINIDO;
    }

    // Un módulo no puede tener un divisor constante igual a cero.
    if (es_constante_cero (nodo -> hijos [1])) {
        error_semantico (contexto, nodo -> hijos [1] -> linea, nodo -> hijos [1] -> columna, "No se puede calcular el módulo por cero.");
        return TIPO_NO_DEFINIDO;
    }

    nodo -> tipo_dato = TIPO_INT;

    debug_semantico (contexto, "Operación módulo válida: resultado int.");

    return TIPO_INT;
}

/**
 * Analiza una comparación relacional.
 * 
 * Los operadores < y > requieren operandos numéricos
 * y producen un resultado de tipo boolean.
 */
static TipoDato analizar_comparacion (NodoAST *nodo, ContextoSemantico *contexto) {
    TipoDato izquierdo = analizar_expresion (nodo -> hijos [0], contexto);
    TipoDato derecho = analizar_expresion (nodo -> hijos [1], contexto);

    // Si alguno de los operandos contiene un error, se frena el análisis.
    if (izquierdo == TIPO_NO_DEFINIDO || derecho == TIPO_NO_DEFINIDO) {
        return TIPO_NO_DEFINIDO;
    }

    // Las comparaciones solamente admiten operandos numéricos.
    if (!es_tipo_numerico (izquierdo) || !(es_tipo_numerico (derecho))) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "Los operadores relacionales requieren operandos numéricos.");
        return TIPO_NO_DEFINIDO;
    }

    nodo -> tipo_dato = TIPO_BOOLEAN;

    debug_semantico (contexto, "Comparación relacional válida: resultado boolean.");

    return TIPO_BOOLEAN;
}

/**
 * Analiza una operación de igualdad.
 * 
 * El operador == requiere que ambos operandos tengan
 * exactamente el mismo tipo. No se realiza conversión
 * implícita entre int y float para esta operación.
 */
static TipoDato analizar_igualdad (NodoAST *nodo, ContextoSemantico *contexto) {
    TipoDato izquierdo = analizar_expresion (nodo -> hijos [0], contexto);
    TipoDato derecho = analizar_expresion (nodo -> hijos [1], contexto);

    // Si alguno de los operandos contiene un error, se frena el análisis.
    if (izquierdo == TIPO_NO_DEFINIDO || derecho == TIPO_NO_DEFINIDO) {
        return TIPO_NO_DEFINIDO;
    }

    // Ambos operandos deben tener exactamente el mismo tipo.
    if (izquierdo != derecho) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "El operador == requiere operandos del mismo tipo.");
        return TIPO_NO_DEFINIDO;
    }

    nodo -> tipo_dato = TIPO_BOOLEAN;

    debug_semantico (contexto, "Igualdad válida: resultado boolean.");

    return TIPO_BOOLEAN;
}

/**
 * Analiza una operación lógica binaria.
 * 
 * Los operadores && y || solamente admiten operandos
 * de tipo boolean y producen un resultado boolean.
 */
static TipoDato analizar_operacion_logica (NodoAST *nodo, ContextoSemantico *contexto) {
    TipoDato izquierdo = analizar_expresion (nodo -> hijos [0], contexto);
    TipoDato derecho = analizar_expresion (nodo -> hijos [1], contexto);

    // Si alguno de los operandos contiene un error, se frena el análisis.
    if (izquierdo == TIPO_NO_DEFINIDO || derecho == TIPO_NO_DEFINIDO) {
        return TIPO_NO_DEFINIDO;
    }

    // Los operadores lógicos solamente admiten boolean.
    if (izquierdo != TIPO_BOOLEAN || derecho != TIPO_BOOLEAN) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "Los operadores lógicos requieren operandos boolean.");
        return TIPO_NO_DEFINIDO;
    }

    nodo -> tipo_dato = TIPO_BOOLEAN;

    debug_semantico (contexto, "Operación lógica válida: resultado boolean.");

    return TIPO_BOOLEAN;
}

/**
 * Analiza una negación lógica.
 * 
 * El operador ! solamente admite un operando boolean
 * y produce un resultado boolean.
 */
static TipoDato analizar_negacion (NodoAST *nodo, ContextoSemantico *contexto) {
    TipoDato tipo = analizar_expresion (nodo -> hijos [0], contexto);

    // Si el operando contiene un error, se frena el análisis.
    if (tipo == TIPO_NO_DEFINIDO) {
        return TIPO_NO_DEFINIDO;
    }

    // La negación solamente admite boolean.
    if (tipo != TIPO_BOOLEAN) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "El operador ! requiere un operando boolean.");
        return TIPO_NO_DEFINIDO;
    }

    nodo -> tipo_dato = TIPO_BOOLEAN;

    debug_semantico (contexto, "Negación lógica válida: resultado boolean.");

    return TIPO_BOOLEAN;
}

/**
 * Analiza el operador menos unario.
 * 
 * El operador - solamente admite operandos numéricos.
 * El tipo del resultado coincide con el tipo del operando.
 */
static TipoDato analizar_menos_unario (NodoAST *nodo, ContextoSemantico *contexto) {
    TipoDato tipo = analizar_expresion (nodo -> hijos [0], contexto);

    // Si el operando contiene un error, se frena el análisis.
    if (tipo == TIPO_NO_DEFINIDO) {
        return TIPO_NO_DEFINIDO;
    }

    // El menos unario solamente admite tipos numéricos.
    if (!es_tipo_numerico (tipo)) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "El operador menos unario requiere un operando numérico.");
        return TIPO_NO_DEFINIDO;
    }

    nodo -> tipo_dato = tipo;

    debug_semantico (contexto, "Menos unario válido.");

    return tipo;
}

/**
 * Analiza semánticamente una llamada a una función.
 * 
 * Verifica que:
 * - La función haya sido declarada previamente.
 * - El identificador corresponda a una función.
 * - La cantidad de argumentos coincida con la cantidad
 *   de parámetros declarados.
 * - Los tipos de los argumentos sean compatibles con
 *   los tipos de los parámetros correspondientes.
 * 
 * La función no determina si una llamada void puede
 * utilizarse como expresión.
 */
static TipoDato analizar_llamada (NodoAST *nodo, ContextoSemantico *contexto) {
    // El nodo debe representar una llamada.
    if (nodo == NULL || nodo -> tipo != AST_LLAMADA) {
        return TIPO_NO_DEFINIDO;
    }

    // Se verifica que la función haya sido declarada en la TS.
    Simbolo *simbolo = buscar_elemento (contexto -> ts, nodo -> valor.identificador);

    if (simbolo == NULL) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "La función utilizada en la llamada no está declarada.");
        return TIPO_NO_DEFINIDO;
    }

    // El identificador debe corresponder a una función.
    if (simbolo -> clase != SIMBOLO_FUNCION) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "El identificador utilizado en la llamada no es una función.");
        return TIPO_NO_DEFINIDO;
    }

    // Se asocia el tipo al nodo.
    nodo -> simbolo = simbolo;
    nodo -> tipo_dato = simbolo -> tipo;

    // Se busca la declaración de la función en el AST para obtener los parámetros.
    NodoAST *funcion = buscar_declaracion_funcion (nodo -> valor.identificador, contexto);

    if (funcion == NULL) {
        error_semantico (contexto, nodo -> linea, nodo -> columna, "No se encontró la declaración de la función en el AST.");
        return TIPO_NO_DEFINIDO;
    }

    // Se verifica la cantidad y los tipos de los argumentos.
    if (!analizar_argumentos (nodo, funcion, contexto)) {
        return TIPO_NO_DEFINIDO;
    }

    registrar_semantica (contexto, nodo -> linea, nodo -> columna, "Llamada a función válida.");

    char mensaje [200];
    snprintf (mensaje, sizeof (mensaje), "Llamada a función '%s' válida, retorna %s.", nodo -> valor.identificador, tipo_a_string (simbolo -> tipo));
    debug_semantico (contexto, mensaje);

    return simbolo -> tipo;
}

/**
 * Busca la declaración de una función dentro del AST del programa.
 * 
 * Recorre las declaraciones globales del nodo raíz y compara el
 * nombre de cada función con el nombre recibido.
 * 
 * Retorna el nodo AST_DECLARACION_FUNCION encontrado o NULL si
 * no existe una declaración con ese nombre.
 */
static NodoAST *buscar_declaracion_funcion (const char *nombre, ContextoSemantico *contexto) {
    // Se obtiene la raíz del AST.
    NodoAST *programa = contexto -> programa;

    // La raíz debe representar un programa.
    if (programa == NULL || programa -> tipo != AST_PROGRAMA) {
        return NULL;
    }

    // Se recorren las declaraciones globales.
    for (int i = 0; i < programa -> cantidad_hijos; i ++) {
        NodoAST *declaracion = programa -> hijos [i];

        // Solamente interesan las declaraciones de funciones.
        if (declaracion -> tipo != AST_DECLARACION_FUNCION) {
            continue;
        }

        // El primer hijo contiene el nombre de la función.
        NodoAST *identificador = declaracion -> hijos [0];

        if (identificador == NULL || identificador -> tipo != AST_IDENTIFICADOR) {
            continue;
        }

        // Se compara el nombre de la función con el solicitado.
        if (strcmp (identificador -> valor.identificador, nombre) == 0) {
            return declaracion;
        }
    }

    return NULL;
}

/**
 * Analiza los argumentos de una llamada a función.
 * 
 * Compara los argumentos presentes en el nodo AST_LLAMADA
 * con los parámetros de la declaración de la función.
 * 
 * Verifica que:
 * - La cantidad de argumentos coincida con la cantidad
 *   de parámetros.
 * - Cada argumento sea una expresión semánticamente válida.
 * - El tipo de cada argumento sea compatible con el tipo del
 *   parámetro correspondiente.
 * 
 * Se permite conversión implícita entre int y float.
 * 
 * Retorna 1 si todos los argumentos son válidos
 * y 0 si se encuentra algún error.
 */
static int analizar_argumentos (NodoAST *llamada, NodoAST *funcion, ContextoSemantico *contexto) {
    int argumentos = llamada -> cantidad_hijos;

    // En la declaración, el primer hijo es el nombre, el último el bloque.
    int parametros = funcion -> cantidad_hijos - 2;

    int valido = 1;

    // Se verifica que la cantidad de argumentos coincida con la cantidad de parámetros.
    if (argumentos != parametros) {
        char mensaje [200];
        snprintf (mensaje, sizeof (mensaje), "La función '%s' requiere %d argumentos, pero recibió %d.", llamada -> valor.identificador, parametros, argumentos);
        error_semantico (contexto, llamada -> linea, llamada -> columna, mensaje);       
        
        valido = 0;
    }

    // Se analiza cada argumento que tenga un parámetro correspondiente.
    int cantidad = argumentos < parametros ? argumentos : parametros;

    for (int i = 0; i < cantidad; i ++) {
        // Los argumentos son hijos directos del nodo AST_LLAMADA.
        NodoAST *argumento = llamada -> hijos [i];

        // Los parámetros se encuentran entre el nombre y el bloque.
        NodoAST *parametro = funcion -> hijos [i + 1];
        TipoDato tipo_parametro = parametro -> tipo_dato;

        // Se analiza el tipo de la expresión del argumento.
        TipoDato tipo_argumento = analizar_expresion (argumento, contexto);

        char mensaje [200];
        snprintf (mensaje, sizeof (mensaje), "Argumento %d de '%s': tipo %s; parámetro esperado %s.", i + 1, llamada -> valor.identificador, tipo_a_string (tipo_argumento), tipo_a_string (tipo_parametro));
        debug_semantico (contexto, mensaje);
        
        // Si la expresión contiene un error, no se puede comprobar su compatibilidad.
        if (tipo_argumento == TIPO_NO_DEFINIDO) {
            valido = 0;
            continue;
        }

        // Se verifica la compatibilidad entre ambos tipos.
        if (!tipos_compatibles (tipo_parametro, tipo_argumento)) {
            char mensaje [200];
            snprintf (mensaje, sizeof (mensaje), "El argumento %d de '%s' es de tipo %s, pero se esperaba %s.", i + 1, llamada -> valor.identificador, tipo_a_string (tipo_argumento), tipo_a_string (tipo_parametro));
            error_semantico (contexto, llamada -> linea, llamada -> columna, mensaje);       
            
            valido = 0;
            continue;
        }

        // Si los tipos son diferentes pero compatibles, se registra la conversión implícita.
        if (tipo_parametro != tipo_argumento) {
            char mensaje [200];
            snprintf (mensaje, sizeof (mensaje), "Conversión implícita de %s a %s, en el argumento %d de '%s'.", tipo_a_string (tipo_argumento), tipo_a_string (tipo_parametro), i + 1, llamada -> valor.identificador);
            warning_semantico (contexto, argumento -> linea, argumento -> columna, mensaje);
        }
    }

    return valido;
}

/**
 * Declara un identificador en la TS.
 * 
 * Centraliza la lógica común a las declaraciones
 * de variables, parámetros y funciones.
 * 
 * Si la inserción es exitosa:
 * - Se asocia el símbolo con el identificador del AST.
 * - Se establece el tipo del identificador.
 * 
 * Retorna el símbolo insertado o NULL si la
 * declaración no pudo realizarse.
 */
static Simbolo *declarar_identificador (NodoAST *identificador, TipoDato tipo, ClaseSimbolo clase, ContextoSemantico *contexto) {
    // El nodo recibido debe ser un identificador.
    if (identificador == NULL || identificador -> tipo != AST_IDENTIFICADOR) {
        return NULL;
    }

    // Se intenta insertar el identificador en el nivel actual de la TS.
    Simbolo *simbolo = insertar_elemento (contexto -> ts, identificador -> valor.identificador, tipo, clase);

    // El identificador ya existía en el nivel.
    if (simbolo == NULL) {
        error_semantico (contexto, identificador -> linea, identificador -> columna, "El identificador ya fue declarado en este nivel.");
        return NULL;
    }

    // Se asocia el símbolo con el nodo del AST.
    identificador -> simbolo = simbolo;

    // Se establece el tipo del identificador.
    identificador -> tipo_dato = tipo;

    return simbolo;
}

/**
 * Registra una declaración en el archivo .sem.
 * 
 * Centraliza el formato utilizado para registrar
 * variables, parámetros y funciones.
 */
static void registrar_declaracion (NodoAST *identificador, TipoDato tipo, ClaseSimbolo clase, ContextoSemantico *contexto) {
    char mensaje [200];

    snprintf (mensaje, sizeof (mensaje), "Declaración de %s '%s' de tipo %s.", clase_a_string (clase), identificador -> valor.identificador, tipo_a_string (tipo));

    registrar_semantica (contexto, identificador -> linea, identificador -> columna, mensaje);
}

/**
 * Determina si un bloque garantiza que la ejecución
 * termine realizando un return.
 * 
 * Un bloque garantiza el retorno si alguna de sus
 * sentencias garantiza el retorno.
 */
static int garantiza_retorno_bloque (NodoAST *nodo) {
    // El nodo debe representar un bloque.
    if (nodo == NULL || nodo -> tipo != AST_BLOQUE) {
        return 0;
    }

    // Se recorren declaraciones y sentencias del bloque.
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        NodoAST *hijo = nodo -> hijos [i];

        // Las declaraciones no modifican el flujo de retorno.
        if (hijo == NULL || hijo -> tipo == AST_DECLARACION_VARIABLE) {
            continue;
        }

        // Si se encuentra una sentencia que garantiza el retorno, el resto del bloque es inalcanzable.
        if (garantiza_retorno_sentencia (hijo)) {
            return 1;
        }
    }

    // Ninguna sentencia garantiza el retorno.
    return 0;
}

/**
 * Determina si una sentencia garantiza que la ejecución termine
 * realizando un return.
 * 
 * Esta función solamente analiza la estructura del AST.
 * No realiza comprobaciones de tipos ni modifica la TS.
 */
static int garantiza_retorno_sentencia (NodoAST *nodo) {
    if (nodo == NULL) {
        return 0;
    }

    switch (nodo -> tipo) {
        // Un return siempre finaliza la ejecución.
        case AST_RETURN:
            return 1;
        
        // Un if garantiza retorno cuando tiene las dos ramas y ambas garantizan retorno.
        case AST_IF:
            if (nodo -> cantidad_hijos < 3) {
                return 0;
            }

            // El hijo 1 es el bloque then, el 2 el else.
            return garantiza_retorno_bloque (nodo -> hijos [1]) && garantiza_retorno_bloque (nodo -> hijos [2]); 
        
        // Un bloque anidado garantiza retorno si su contenido lo garantiza.
        case AST_BLOQUE:
            return garantiza_retorno_bloque (nodo);

        // Un while no garantiza retorno porque el cuerpo puede no ejecutarse ninguna vez.
        case AST_WHILE:
            return 0;

        // Las asignaciones, llamadas y sentencias vacías no finalizan necesariamente la ejecución.
        case AST_ASIGNACION:
        case AST_LLAMADA:
        case AST_SENTENCIA_VACIA:
            return 0;

        default:
            return 0;
    }
}

/**
 * Determina si un tipo es numérico.
 * 
 * Los tipos numéricos del lenguaje son int y float.
 */
static int es_tipo_numerico (TipoDato tipo) {
    return tipo == TIPO_INT || tipo == TIPO_FLOAT;
}

/**
 * Determina si dos tipos son compatibles para una asignación.
 * 
 * Se permite compatibilidad entre int y float en ambos sentidos.
 * 
 * Los valores boolean solamente son compatibles con boolean.
 */
static int tipos_compatibles (TipoDato destino, TipoDato origen) {
    // Los tipos iguales siempre son compatibles.
    if (destino == origen) {
        return 1;
    }

    // Los tipos int y float son compatibles entre sí.
    if (es_tipo_numerico (destino) && es_tipo_numerico (origen)) {
        return 1;
    }

    return 0;
}

/**
 * Determina el tipo resultante de una operación aritmética.
 * 
 * Dos operandos int producen int.
 * Si alguno de los operandos es float, el resultado es float.
 */
static TipoDato tipo_resultado_aritmetico (TipoDato izquierdo, TipoDato derecho) {
    // Al menos un operando de tipo float.
    if (izquierdo == TIPO_FLOAT || derecho == TIPO_FLOAT) {
        return TIPO_FLOAT;
    }

    return TIPO_INT;
}

/**
 * Devuelve una representación textual de un tipo de dato.
 */
static const char *tipo_a_string (TipoDato tipo) {
    switch (tipo) {
        case TIPO_INT:
            return "int";
        case TIPO_BOOLEAN:
            return "boolean";
        case TIPO_FLOAT:
            return "float";
        case TIPO_VOID:
            return "void";
        case TIPO_NO_DEFINIDO:
            return "no definido";
        default:
            return "desconocido";
    }
}

/**
 * Devuelve una representación textual de una clase de símbolo.
 */
static const char *clase_a_string (ClaseSimbolo clase) {
    switch (clase) {
        case SIMBOLO_VARIABLE:
            return "variable";
        case SIMBOLO_FUNCION:
            return "función";
        case SIMBOLO_PARAMETRO:
            return "parámetro";
        default:
            return "identificador";
    }
}

/**
 * Intenta evaluar una expresión numérica cuyo valor
 * puede determinarse completamente durante el análisis.
 * 
 * Si la expresión es constante, guarda su valor en el
 * parámetro 'valor' y devuelve 1.
 * 
 * Si la expresión depende de un identificador, una
 * llamada a función u otro elemento cuyo valor no
 * puede conocerse en compilación, devuelve 0.
 */
static int evaluar_constante_numerica (NodoAST *nodo, double *valor) {
    // Se verifica que el nodo y el destino sean válidos.
    if (nodo == NULL || valor == NULL) {
        return 0;
    }

    switch (nodo -> tipo) {
        // Una constante entera puede evaluarse directamente.
        case AST_NUMERO:
            *valor = nodo -> valor.numero;
            return 1;
        
        // Una constante real puede evaluarse directamente.
        case AST_REAL:
            *valor = nodo -> valor.real;
            return 1;

        // Para el menos unario se evalúa su operando.
        case AST_MENOS_UNARIO: {
            double operando;

            if (!evaluar_constante_numerica (nodo -> hijos [0], &operando)) {
                return 0;
            }

            *valor = -operando;
            return 1;
        }

        // Para las operaciones binarias ambos operandos deben ser constantes.
        case AST_SUMA:
        case AST_RESTA:
        case AST_MULTIPLICACION:
        case AST_DIVISION: {
            double izquierdo;
            double derecho;

            // Se evalúa el operando izquierdo.
            if (!evaluar_constante_numerica (nodo -> hijos [0], &izquierdo)) {
                return 0;
            }

            // Se evalúa el operando derecho.
            if (!evaluar_constante_numerica (nodo -> hijos [1], &derecho)) {
                return 0;
            }

            // Se calcula el resultado según el operador.
            if (nodo -> tipo == AST_SUMA) {
                *valor = izquierdo + derecho;
            }
            else if (nodo -> tipo == AST_RESTA) {
                *valor = izquierdo - derecho;
            }
            else if (nodo -> tipo == AST_MULTIPLICACION) {
                *valor = izquierdo * derecho;
            }
            // Para una división se evita dividir por cero.
            else {
                if (derecho == 0.0) {
                    return 0;
                }

                TipoDato tipo_izquierdo = nodo -> hijos [0] -> tipo_dato;
                TipoDato tipo_derecho = nodo -> hijos [1] -> tipo_dato;

                // División entera (ambos int).
                if (tipo_izquierdo == TIPO_INT && tipo_derecho == TIPO_INT) {
                    *valor = (double) ((int) izquierdo / (int) derecho);
                }
                // División real (al menos un float).
                else {
                    *valor = izquierdo / derecho;
                }
            }

            return 1;
        }
        case AST_MODULO: {
            double izquierdo;
            double derecho;

            // Se evalúa el operando izquierdo.
            if (!evaluar_constante_numerica (nodo -> hijos [0], &izquierdo)) {
                return 0;
            }

            // Se evalúa el operando derecho.
            if (!evaluar_constante_numerica (nodo -> hijos [1], &derecho)) {
                return 0;
            }

            TipoDato tipo_izquierdo = nodo -> hijos [0] -> tipo_dato;
            TipoDato tipo_derecho = nodo -> hijos [1] -> tipo_dato;
            
            // El módulo solamente está definido para enteros.
            if (tipo_izquierdo != TIPO_INT || tipo_derecho != TIPO_INT) {
                return 0;
            }

            // No se puede calcular un módulo por cero.
            if (derecho == 0.0) {
                return 0;
            }

            *valor = (double) ((int) izquierdo % (int) derecho);
            return 1;
        }
        
        default:
            return 0;
    }
}

/**
 * Determina si una expresión numérica puede evaluarse
 * en compilación y su resultado es exactamente 0.
 * 
 * Se utiliza para detectar divisiones o módulos por cero
 * cuyo divisor puede determinarse durante el análisis.
 */
static int es_constante_cero (NodoAST *nodo) {
    double valor;

    // Se intenta evaluar la expresión completamente.
    if (!evaluar_constante_numerica (nodo, &valor)) {
        return 0;
    }

    // La expresión es constante y su resultado es cero.
    return valor == 0.0;
}

/**
 * Registra información relevante del análisis semántico.
 * 
 * La información se escribe en el archivo .sem cuando
 * existe una salida semántica configurada.
 * 
 * Este registro se realiza tanto si el análisis encuentra
 * errores como si finaliza correctamente.
 */
static void registrar_semantica (ContextoSemantico *contexto, int linea, int columna, const char *mensaje) {
    // Se verifica que exista un archivo de salida semántica.
    if (contexto -> salida_sem == NULL) {
        return;
    }

    // Se registra la información junto con su ubicación.
    fprintf (contexto -> salida_sem, "Línea %d, columna %d - %s\n", linea, columna, mensaje);
}

/**
 * Registra información detallada de depuración.
 * 
 * La información solamente se muestra por consola cuando
 * el modo de depuración se encuentra habilitado.
 */
static void debug_semantico (ContextoSemantico *contexto, const char *mensaje) {
    // El modo debug solamente muestra información si está habilitado.
    if (!contexto -> modo_debug) {
        return;
    }

    // Se muestra la información detallada por consola.
    fprintf (stdout, "[SEM] %s\n", mensaje);
}

/**
 * Registra e informa un error semántico.
 * 
 * Incrementa el contador de errores y muestra el mensaje
 * junto con la línea y columna donde se produjo.
 * 
 * Los errores se informan mediante la salida estándar de errores.
 */
static void error_semantico (ContextoSemantico *contexto, int linea, int columna, const char *mensaje) {
    // Se incrementa el contador de errores.
    contexto -> errores ++;

    // Se informa el error por la salida estándar de errores.
    fprintf (stderr, "ERROR SEMÁNTICO: línea %d, columna %d - %s\n", linea, columna, mensaje);
}

/**
 * Registra un warning producido durante el análisis semántico.
 * 
 * Los warnings no incrementan la cantidad de errores, ya que
 * no hacen que el programa sea semánticamente inválido.
 */
static void warning_semantico (ContextoSemantico *contexto, int linea, int columna, const char *mensaje) {
    // Se informa la advertencia por la salida estándar de errores.
    fprintf (stderr, "WARNING SEMÁNTICO: línea %d, columna %d - %s\n", linea, columna, mensaje);
}
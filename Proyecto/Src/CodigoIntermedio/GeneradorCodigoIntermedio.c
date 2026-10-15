#include "GeneradorCodigoIntermedio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Tamaño de los búferes para nombres de temporales y etiquetas. */
#define TAM_NOMBRE_GENERADO 32

/**
 * Mantiene el estado utilizado durante la generación del código.
 * 
 * Los contadores son independientes para que los temporales y
 * las etiquetas tengan numeraciones separadas.
 */
typedef struct {
    ListaCuadruplos *lista;

    int siguiente_temporal;
    int siguiente_etiqueta;

    int modo_debug;
    int error;
} ContextoGenerador;

/* =========== Funciones auxiliares privadas =========== */

// Depuración y errores.

static void debug_generador (ContextoGenerador *contexto, const char *mensaje);
static void registrar_error (ContextoGenerador *contexto, const char *mensaje);

// Creación de operandos.

static Operando crear_operando_temporal_nuevo (ContextoGenerador *contexto, TipoDato tipo);
static Operando crear_operando_etiqueta_nueva (ContextoGenerador *contexto);
static Operando crear_operando_identificador (ContextoGenerador *contexto, NodoAST *nodo);
static Operando crear_operando_literal (ContextoGenerador *contexto, NodoAST *nodo);

// Generadores.

static Operando generar_expresion (NodoAST *nodo, ContextoGenerador *contexto);
static Operando generar_operacion_binaria (NodoAST *nodo, ContextoGenerador *contexto, CodigoOperacion operacion);
static Operando generar_operacion_unaria (NodoAST *nodo, ContextoGenerador *contexto, CodigoOperacion operacion);
static Operando generar_llamada (NodoAST *nodo, ContextoGenerador *contexto, int conservar_resultado);
static int generar_condicion (NodoAST *nodo, ContextoGenerador *contexto, const Operando *etiqueta_verdadera, const Operando *etiqueta_falsa);
static Operando generar_expresion_booleana (NodoAST *nodo, ContextoGenerador *contexto);
static int generar_declaracion_funcion (NodoAST *nodo, ContextoGenerador *contexto);
static int generar_bloque (NodoAST *nodo, ContextoGenerador *contexto);
static int generar_sentencia (NodoAST *nodo, ContextoGenerador *contexto);
static int generar_asignacion (NodoAST *nodo, ContextoGenerador *contexto);
static int generar_if (NodoAST *nodo, ContextoGenerador *contexto);
static int generar_while (NodoAST *nodo, ContextoGenerador *contexto);
static int generar_return (NodoAST *nodo, ContextoGenerador *contexto);

/* =========== Función principal =========== */

ListaCuadruplos *generar_codigo_intermedio (NodoAST *arbol, int modo_debug) {
    // El árbol debe existir y representar un programa.
    if (arbol == NULL || arbol -> tipo != AST_PROGRAMA) {
        fprintf (stderr, "ERROR EN GENERACIÓN DE CÓDIGO INTERMEDIO: el AST no representa un programa válido.\n");
        return NULL;
    }

    ContextoGenerador contexto;

    contexto.lista = crear_lista_cuadruplos ();
    contexto.siguiente_temporal = 1;
    contexto.siguiente_etiqueta = 1;
    contexto.modo_debug = modo_debug;
    contexto.error = 0;

    // No se puede continuar si no se pudo crear la lista.
    if (contexto.lista == NULL) {
        fprintf (stderr, "ERROR EN GENERACIÓN DE CÓDIGO INTERMEDIO: no se pudo crear la lista de cuádruplos.\n");
        return NULL;
    }

    debug_generador (&contexto, "Comienza la generación del código intermedio.");

    // Se recorren las declaraciones del programa en orden.
    for (int i = 0; i < arbol -> cantidad_hijos && !contexto.error; i ++) {
        NodoAST *declaracion = arbol -> hijos [i];

        if (declaracion == NULL) {
            registrar_error (&contexto, "El programa contiene una declaración nula.");
            break;
        }

        switch (declaracion -> tipo) {
            // Las declaraciones de variables no generan instrucciones ejecutables.
            case AST_DECLARACION_VARIABLE:
                debug_generador (&contexto, "Se omite una declaración global de variable.");
                break;
            
            // Cada función genera su propio bloque de código.
            case AST_DECLARACION_FUNCION:
                generar_declaracion_funcion (declaracion, &contexto);
                break;

            default:
                registrar_error (&contexto, "Se encontró un nodo no válido en el nivel global.");
                break;
        }
    }

    // Si ocurrió algún error, no se devuelve código parcial.
    if (contexto.error) {
        debug_generador (&contexto, "La generación finalizó con errores.");
        liberar_lista_cuadruplos (contexto.lista);
        return NULL;
    }

    debug_generador (&contexto, "La generación del código intermedio finalizó correctamente.");

    if (modo_debug) {
        printf ("[CODINT] Total de cuádruplos generados: %d\n", cantidad_cuadruplos (contexto.lista));
    }

    // La lista pasa a ser responsabilidad del llamador.
    return contexto.lista;
}

/* =========== Funciones auxiliares privadas =========== */

/**
 * Imprime un mensaje de depuración cuando el modo DEBUG está activo.
 */
static void debug_generador (ContextoGenerador *contexto, const char *mensaje) {
    if (contexto != NULL && contexto -> modo_debug && mensaje != NULL) {
        printf ("[CODINT] %s\n", mensaje);
    }
}

/**
 * Registra un error y detiene la generación posterior.
 */
static void registrar_error (ContextoGenerador *contexto, const char *mensaje) {
    if (contexto == NULL) {
        return;
    }

    contexto -> error = 1;

    fprintf (stderr, "ERROR EN GENERACIÓN DE CÓDIGO INTERMEDIO: %s\n", mensaje != NULL ? mensaje : "Error desconocido.");

    if (contexto -> modo_debug) {
        fprintf (stderr, "[CODINT] La generación se detendrá.\n");
    }
}

/**
 * Crea un operando temporal con un nombre único.
 * 
 * Los temporales utilizan una numeración independiente
 * de las etiquetas de control de flujo.
 */
static Operando crear_operando_temporal_nuevo (ContextoGenerador *contexto, TipoDato tipo) {
    char nombre [TAM_NOMBRE_GENERADO];
    snprintf (nombre, sizeof (nombre), "T%d", contexto -> siguiente_temporal ++);

    Operando temporal = crear_operando_temporal (nombre, tipo);

    // Un operando vacío indica que no se pudo crear el temporal.
    if (temporal.tipo != OPERANDO_TEMPORAL) {
        registrar_error (contexto, "No se pudo crear un operando temporal.");
    }

    return temporal;
}

/**
 * Crea una etiqueta con un nombre único.
 * 
 * Las etiquetas se utilizan para representar los destinos
 * de los saltos y las bifurcaciones del código intermedio.
 */
static Operando crear_operando_etiqueta_nueva (ContextoGenerador *contexto) {
    char nombre [TAM_NOMBRE_GENERADO];
    snprintf (nombre, sizeof (nombre), "L%d", contexto -> siguiente_etiqueta ++);

    Operando etiqueta = crear_operando_etiqueta (nombre);

    // Se verifica que la etiqueta se haya creado correctamente.
    if (etiqueta.tipo != OPERANDO_ETIQUETA) {
        registrar_error (contexto, "No se pudo crear un operando etiqueta.");
    }

    return etiqueta;
}

/**
 * Crea un operando asociado a un identificador del AST.
 * 
 * El análisis semántico debe haber asociado previamente el
 * identificador con su símbolo correspondiente.
 */
static Operando crear_operando_identificador (ContextoGenerador *contexto, NodoAST *nodo) {
    if (nodo == NULL || nodo -> tipo != AST_IDENTIFICADOR || nodo -> simbolo == NULL) {
        registrar_error (contexto, "No se pudo obtener el símbolo de un identificador.");
        return crear_operando_vacio ();
    }

    Operando operando = crear_operando_simbolo (nodo -> simbolo, nodo -> valor.identificador);

    // Se verifica que el operando se haya creado correctamente.
    if (operando.tipo != OPERANDO_SIMBOLO) {
        registrar_error (contexto, "No se pudo crear el operando de un identificador.");
    }

    return operando;
}

/**
 * Crea un operando para un literal del AST.
 * 
 * Los literales se representan mediante constantes.
 * No necesitan un temporal para conservar su valor.
 */
static Operando crear_operando_literal (ContextoGenerador *contexto, NodoAST *nodo) {
    if (nodo == NULL) {
        registrar_error (contexto, "No se puede generar un literal a partir de un nodo nulo.");
        return crear_operando_vacio ();
    }

    char valor [TAM_NOMBRE_GENERADO];
    TipoDato tipo;

    // Se determina el valor del literal según su tipo.
    switch (nodo -> tipo) {
        case AST_NUMERO:
            snprintf (valor, sizeof (valor), "%d", nodo -> valor.numero);
            tipo = TIPO_INT;
            break;
        case AST_REAL:
            snprintf (valor, sizeof (valor), "%g", nodo -> valor.real);
            tipo = TIPO_FLOAT;
            break;
        case AST_TRUE:
            snprintf (valor, sizeof (valor), "TRUE");
            tipo = TIPO_BOOLEAN;
            break;
        case AST_FALSE:
            snprintf (valor, sizeof (valor), "FALSE");
            tipo = TIPO_BOOLEAN;
            break;
        default:
            registrar_error (contexto, "El nodo no representa un literal válido.");
            return crear_operando_vacio ();
    }

    Operando constante = crear_operando_constante (valor, tipo);

    // Se verifica que la constante se haya creado correctamente.
    if (constante.tipo != OPERANDO_CONSTANTE) {
        registrar_error (contexto, "No se pudo crear el operando de una constante.");
    }

    return constante;
}

/**
 * Genera una expresión del AST.
 * 
 * Devuelve un operando que representa el resultado:
 * una constante, un símbolo o un temporal.
 */
static Operando generar_expresion (NodoAST *nodo, ContextoGenerador *contexto) {
    if (nodo == NULL) {
        registrar_error (contexto, "Se intentó generar una expresión a partir de un nodo nulo.");
        return crear_operando_vacio ();
    }

    switch (nodo -> tipo) {
        // Literales numéricos y booleanos.
        case AST_NUMERO:
        case AST_REAL:
        case AST_TRUE:
        case AST_FALSE:
            return crear_operando_literal (contexto, nodo);

        // Variables y parámetros.
        case AST_IDENTIFICADOR:
            return crear_operando_identificador (contexto, nodo);

        // Operaciones aritméticas.
        case AST_SUMA:
            return generar_operacion_binaria (nodo, contexto, CI_SUMAR);
        case AST_RESTA:
            return generar_operacion_binaria (nodo, contexto, CI_RESTAR);
        case AST_MULTIPLICACION:
            return generar_operacion_binaria (nodo, contexto, CI_MULTIPLICAR);
        case AST_DIVISION:
            return generar_operacion_binaria (nodo, contexto, CI_DIVIDIR);
        case AST_MODULO:
            return generar_operacion_binaria (nodo, contexto, CI_MODULO);

        // Comparaciones relacionales.
        case AST_MENOR:
            return generar_operacion_binaria (nodo, contexto, CI_MENOR);
        case AST_MAYOR:
            return generar_operacion_binaria (nodo, contexto, CI_MAYOR);
        case AST_IGUAL:
            return generar_operacion_binaria (nodo, contexto, CI_IGUAL);

        // Operaciones lógicas.
        case AST_AND:
        case AST_OR:
        case AST_NEGACION:
            return generar_expresion_booleana (nodo, contexto);

        // Menos unario.
        case AST_MENOS_UNARIO:
            return generar_operacion_unaria (nodo, contexto, CI_MENOS_UNARIO);

        // Llamadas a funciones.
        case AST_LLAMADA:
            return generar_llamada (nodo, contexto, 1);

        default:
            registrar_error (contexto, "El nodo AST no representa una expresión válida.");
            return crear_operando_vacio ();
    }
}

/**
 * Genera una operación binaria.
 * 
 * Primero genera ambos operandos y después agrega
 * el cuádruplo que realiza la operación.
 * 
 * El resultado se almacena en un nuevo temporal.
 */
static Operando generar_operacion_binaria (NodoAST *nodo, ContextoGenerador *contexto, CodigoOperacion operacion) {
    if (nodo == NULL || nodo -> cantidad_hijos != 2) {
        registrar_error (contexto, "La operación binaria tiene una estructura inválida.");
        return crear_operando_vacio ();
    }

    debug_generador (contexto, "Generando una operación binaria.");

    // Se genera el operando izquierdo.
    Operando izquierdo = generar_expresion (nodo -> hijos [0], contexto);

    // Se genera el operando derecho.
    Operando derecho = generar_expresion (nodo -> hijos [1], contexto);
    
    // Se crea el temporal que recibirá el resultado.
    Operando resultado = crear_operando_temporal_nuevo (contexto, nodo -> tipo_dato);
    
    // Fallo en la creación de cualquiera de los operandos.
    if (contexto -> error) {
        liberar_operando (&izquierdo);
        liberar_operando (&derecho);
        liberar_operando (&resultado);
        return crear_operando_vacio ();
    }

    // Se inserta el cuádruplo.
    if (!agregar_cuadruplo (contexto -> lista, operacion, &izquierdo, &derecho, &resultado)) {
        registrar_error (contexto, "No se pudo agregar un cuádruplo de operación binaria.");
    }

    // Se liberan las estructuras locales (la lista guarda su copia).
    liberar_operando (&izquierdo);
    liberar_operando (&derecho);

    // El temporal se devuelve al generador de expresiones.
    if (contexto -> error) {
        liberar_operando (&resultado);
        return crear_operando_vacio ();
    }

    debug_generador (contexto, "Operación binaria generada correctamente.");

    return resultado;
}

/**
 * Genera una operación unaria.
 * 
 * El operando se obtiene de la expresión hija.
 * El resultado se almacena en un temporal nuevo.
 */
static Operando generar_operacion_unaria (NodoAST *nodo, ContextoGenerador *contexto, CodigoOperacion operacion) {
    if (nodo == NULL || nodo -> cantidad_hijos != 1) {
        registrar_error (contexto, "La operación unaria tiene una estructura inválida.");
        return crear_operando_vacio ();
    }

    debug_generador (contexto, "Generando una operación unaria.");

    // Se genera el operando.
    Operando operando = generar_expresion (nodo -> hijos [0], contexto);

    Operando vacio = crear_operando_vacio ();

    // Se crea el temporal que recibirá el resultado.
    Operando resultado = crear_operando_temporal_nuevo (contexto, nodo -> tipo_dato);

    // Fallo en la creación de cualquiera de los operandos.
    if (contexto -> error) {
        liberar_operando (&operando);
        liberar_operando (&resultado);
        return crear_operando_vacio ();
    }

    // Se inserta el cuádruplo.
    if (!agregar_cuadruplo (contexto -> lista, operacion, &operando, &vacio, &resultado)) {
        registrar_error (contexto, "No se pudo agregar un cuádruplo de operación unaria.");
    }

    // Se liberan las estructuras locales (la lista guarda su copia).
    liberar_operando (&operando);
    liberar_operando (&vacio);

    // El temporal se devuelve al generador de expresiones.
    if (contexto -> error) {
        liberar_operando (&resultado);
        return crear_operando_vacio ();
    }

    debug_generador (contexto, "Operación unaria generada correctamente.");

    return resultado;
}

/**
 * Genera una llamada a una función.
 * 
 * Cada argumento se evalúa en orden y se representa
 * mediante un cuádruplo CI_ARGUMENTO.
 * 
 * La llamada se representa mediante CI_LLAMADA:
 * 
 *      CALL funcion cantidad_argumentos resultado
 * 
 * Si la llamada se utiliza como sentencia, no se conserva
 * su resultado. Si se utiliza como expresión, se crea un
 * temporal para almacenar el valor devuelto.
 * 
 */
static Operando generar_llamada (NodoAST *nodo, ContextoGenerador *contexto, int conservar_resultado) {
    if (nodo == NULL || nodo -> tipo != AST_LLAMADA || nodo -> simbolo == NULL || nodo -> simbolo -> clase != SIMBOLO_FUNCION) {
        registrar_error (contexto, "La llamada no tiene asociado un símbolo de función válido.");
        return crear_operando_vacio ();
    }

    debug_generador (contexto, "Generando una llamada a función.");

    Operando funcion = crear_operando_simbolo (nodo -> simbolo, nodo -> valor.identificador);

    if (funcion.tipo != OPERANDO_SIMBOLO) {
        registrar_error (contexto, "No se pudo crear el operando de la función llamada.");
        liberar_operando (&funcion);
        return crear_operando_vacio ();
    }

    // Se generan y registran cada argumento en orden.
    for (int i = 0; i < nodo -> cantidad_hijos && !contexto -> error; i ++) {
        Operando argumento = generar_expresion (nodo -> hijos [i], contexto);

        if (contexto -> error) {
            liberar_operando (&argumento);
            break;
        }

        char posicion [TAM_NOMBRE_GENERADO];
        snprintf (posicion, sizeof (posicion), "%d", i);

        Operando indice = crear_operando_constante (posicion, TIPO_INT);

        if (indice.tipo != OPERANDO_CONSTANTE) {
            registrar_error (contexto, "No se pudo crear el operando de posición de un argumento.");
            liberar_operando (&argumento);
            liberar_operando (&indice);
            break;
        }

        Operando vacio = crear_operando_vacio ();

        if (!agregar_cuadruplo (contexto -> lista, CI_ARGUMENTO, &argumento, &indice, &vacio)) {
            registrar_error (contexto, "No se pudo agregar el cuádruplo de un argumento.");
        }

        liberar_operando (&argumento);
        liberar_operando (&indice);
        liberar_operando (&vacio);
    }

    if (contexto -> error) {
        liberar_operando (&funcion);
        return crear_operando_vacio ();
    }

    char cantidad [TAM_NOMBRE_GENERADO];
    snprintf (cantidad, sizeof (cantidad), "%d", nodo -> cantidad_hijos);

    Operando numero_argumentos = crear_operando_constante (cantidad, TIPO_INT);

    if (numero_argumentos.tipo != OPERANDO_CONSTANTE) {
        registrar_error (contexto, "No se pudo crear el operando de cantidad de argumentos.");
        liberar_operando (&funcion);
        liberar_operando (&numero_argumentos);
        return crear_operando_vacio ();
    }

    // Una función void no produce un valor utilizable.
    int produce_resultado = conservar_resultado && nodo -> tipo_dato != TIPO_VOID;

    Operando resultado = crear_operando_vacio ();

    if (produce_resultado) {
        resultado = crear_operando_temporal_nuevo (contexto, nodo -> tipo_dato);
    }

    if (contexto -> error) {
        liberar_operando (&funcion);
        liberar_operando (&numero_argumentos);
        liberar_operando (&resultado);
        return crear_operando_vacio ();
    }

    if (!agregar_cuadruplo (contexto -> lista, CI_LLAMADA, &funcion, &numero_argumentos, &resultado)) {
        registrar_error (contexto, "No se pudo agregar el cuádruplo de llamada.");
    }

    liberar_operando (&funcion);
    liberar_operando (&numero_argumentos);

    if (contexto -> error) {
        liberar_operando (&resultado);
        return crear_operando_vacio ();
    }

    // Una llamada usada como sentencia no devuelve un operando.
    if (!produce_resultado) {
        liberar_operando (&resultado);
        return crear_operando_vacio ();
    }

    debug_generador (contexto, "Llamada a función generada correctamente.");

    return resultado;
}

/**
 * Genera una condición y transfiere el control a una etiqueta.
 * 
 * Si la condición es verdadera, el control continúa en
 * etiqueta_verdadera; en caso contrario, en etiqueta_falsa.
 * 
 * AND y OR se generan con cortocircuito.
 */
static int generar_condicion (NodoAST *nodo, ContextoGenerador *contexto, const Operando *etiqueta_verdadera, const Operando *etiqueta_falsa) {
    if (nodo == NULL || etiqueta_verdadera == NULL || etiqueta_falsa == NULL) {
        registrar_error (contexto, "La condición o sus etiquetas son inválidas.");
        return 0;
    }

    // Para A && B, si A es falso no se evalúa B.
    if (nodo ->  tipo == AST_AND) {
        if (nodo -> cantidad_hijos != 2) {
            registrar_error (contexto, "La operación AND tiene una estructura inválida.");
            return 0;
        }
        
        Operando intermedia = crear_operando_etiqueta_nueva (contexto);

        if (contexto -> error) {
            liberar_operando (&intermedia);
            return 0;
        }

        // Si A es verdadero, se evalúa B.
        generar_condicion (nodo -> hijos [0], contexto, &intermedia, etiqueta_falsa);

        if (!contexto -> error) {
            Operando vacio = crear_operando_vacio ();
            
            if (!agregar_cuadruplo (contexto -> lista, CI_ETIQUETA, &vacio, &vacio, &intermedia)) {
                registrar_error (contexto, "No se pudo agregar la etiqueta intermedia de una condición.");
            }
        }

        if (!contexto -> error) {
            generar_condicion (nodo -> hijos [1], contexto, etiqueta_verdadera, etiqueta_falsa);
        }

        liberar_operando (&intermedia);

        return !contexto -> error;
    }

    // Para A || B, si A es verdadero no se evalúa B.
    if (nodo ->  tipo == AST_OR) {
        if (nodo -> cantidad_hijos != 2) {
            registrar_error (contexto, "La operación OR tiene una estructura inválida.");
            return 0;
        }
        
        Operando intermedia = crear_operando_etiqueta_nueva (contexto);

        if (contexto -> error) {
            liberar_operando (&intermedia);
            return 0;
        }

        // Si A es falso, se evalúa B.
        generar_condicion (nodo -> hijos [0], contexto, etiqueta_verdadera, &intermedia);

        if (!contexto -> error) {
            Operando vacio = crear_operando_vacio ();
            
            if (!agregar_cuadruplo (contexto -> lista, CI_ETIQUETA, &vacio, &vacio, &intermedia)) {
                registrar_error (contexto, "No se pudo agregar la etiqueta intermedia de una condición.");
            }
        }

        if (!contexto -> error) {
            generar_condicion (nodo -> hijos [1], contexto, etiqueta_verdadera, etiqueta_falsa);
        }

        liberar_operando (&intermedia);

        return !contexto -> error;
    }

    // La negación invierte los destinos verdadero y falso.
    if (nodo -> tipo == AST_NEGACION) {
        if (nodo -> cantidad_hijos != 1) {
            registrar_error (contexto, "La negación lógica tiene una estructura inválida.");
            return 0;
        }

        return generar_condicion (nodo -> hijos [0], contexto, etiqueta_falsa, etiqueta_verdadera);
    }

    // Las demás condiciones se evalúan como expresiones.
    Operando condicion = generar_expresion (nodo, contexto);

    if (contexto -> error) {
        liberar_operando (&condicion);
        return 0;
    }

    Operando vacio = crear_operando_vacio ();

    // IF_FALSE condición - etiqueta_falsa.
    if (!agregar_cuadruplo (contexto -> lista, CI_SALTO_SI_FALSO, &condicion, &vacio, etiqueta_falsa)) {
        registrar_error (contexto, "No se pudo agregar el salto condicional.");
    }

    // GOTO etiqueta_verdadera.
    if (!contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_SALTO, &vacio, &vacio, etiqueta_verdadera)) {
            registrar_error (contexto, "No se pudo agregar el salto incondicional.");
        }
    }

    liberar_operando (&condicion);
    liberar_operando (&vacio);
    
    return !contexto -> error;
}

/**
 * Convierte una expresión booleana en un valor booleano.
 * 
 * La expresión se evalúa mediante saltos y etiquetas.
 * El resultado final se almacena en un temporal boolean.
 */
static Operando generar_expresion_booleana (NodoAST *nodo, ContextoGenerador *contexto) {
    Operando resultado = crear_operando_temporal_nuevo (contexto, TIPO_BOOLEAN);

    if (contexto -> error) {
        liberar_operando (&resultado);
        return crear_operando_vacio ();
    }

    Operando etiqueta_verdadera = crear_operando_etiqueta_nueva (contexto);
    Operando etiqueta_falsa = crear_operando_etiqueta_nueva (contexto);
    Operando etiqueta_fin = crear_operando_etiqueta_nueva (contexto);

    if (contexto -> error) {
        liberar_operando (&resultado);
        liberar_operando (&etiqueta_verdadera);
        liberar_operando (&etiqueta_falsa);
        liberar_operando (&etiqueta_fin);
        return crear_operando_vacio ();
    }

    generar_condicion (nodo, contexto, &etiqueta_verdadera, &etiqueta_falsa);

    Operando vacio = crear_operando_vacio ();

    // Rama verdadera.
    if (!contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_ETIQUETA, &vacio, &vacio, &etiqueta_verdadera)) {
            registrar_error (contexto, "No se pudo agregar la etiqueta de la rama verdadera.");
        }
    }

    if (!contexto -> error) {
        Operando verdadero = crear_operando_constante ("TRUE", TIPO_BOOLEAN);

        if (verdadero.tipo != OPERANDO_CONSTANTE) {
            registrar_error (contexto, "No se pudo crear la constante TRUE.");
        }
        else {
            if (!agregar_cuadruplo (contexto -> lista, CI_ASIGNAR, &verdadero, &vacio, &resultado)) {
                registrar_error (contexto, "No se pudo asignar TRUE al temporal boolean.");
            }
        }

        liberar_operando (&verdadero);
    }

    if (!contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_SALTO, &vacio, &vacio, &etiqueta_fin)) {
            registrar_error (contexto, "No se pudo agregar el salto al final de la expresión booleana.");
        }
    }

    // Rama falsa.
    if (!contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_ETIQUETA, &vacio, &vacio, &etiqueta_falsa)) {
            registrar_error (contexto, "No se pudo agregar la etiqueta de la rama falsa.");
        }
    }

    if (!contexto -> error) {
        Operando falso = crear_operando_constante ("FALSE", TIPO_BOOLEAN);

        if (falso.tipo != OPERANDO_CONSTANTE) {
            registrar_error (contexto, "No se pudo crear la constante FALSE.");
        }
        else {
            if (!agregar_cuadruplo (contexto -> lista, CI_ASIGNAR, &falso, &vacio, &resultado)) {
                registrar_error (contexto, "No se pudo asignar FALSE al temporal boolean.");
            }
        }

        liberar_operando (&falso);
    }

    // Punto de continuación.
    if (!contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_ETIQUETA, &vacio, &vacio, &etiqueta_fin)) {
            registrar_error (contexto, "No se pudo agregar la etiqueta final de la expresión booleana.");
        }
    }

    liberar_operando (&vacio);
    liberar_operando (&etiqueta_verdadera);
    liberar_operando (&etiqueta_falsa);
    liberar_operando (&etiqueta_fin);

    if (contexto -> error) {
        liberar_operando (&resultado);
        return crear_operando_vacio ();
    }

    return resultado;
}

/**
 * Genera el código correspondiente a una declaración de función.
 * 
 * Las declaraciones de funciones generan una marca de inicio,
 * sus parámetros, el cuerpo y una marca de finalización.
 */
static int generar_declaracion_funcion (NodoAST *nodo, ContextoGenerador *contexto) {
    if (nodo == NULL || nodo -> tipo != AST_DECLARACION_FUNCION || nodo -> cantidad_hijos < 2) {
        registrar_error (contexto, "La declaración de función tiene una estructura inválida.");
        return 0;
    }

    NodoAST *identificador = nodo -> hijos [0];

    NodoAST *bloque = nodo -> hijos [nodo -> cantidad_hijos - 1];

    if (identificador == NULL || identificador -> tipo != AST_IDENTIFICADOR || identificador -> simbolo == NULL || bloque == NULL || bloque -> tipo != AST_BLOQUE) {
        registrar_error (contexto, "La función no tiene un identificador o cuerpo válido.");
        return 0;
    }

    debug_generador (contexto, "Generando una declaración de función.");

    Operando funcion = crear_operando_simbolo (identificador -> simbolo, identificador -> valor.identificador);

    if (funcion.tipo != OPERANDO_SIMBOLO) {
        registrar_error (contexto, "No se pudo crear el operando de la función.");
        liberar_operando (&funcion);
        return 0;
    }

    Operando vacio = crear_operando_vacio ();

    // Marca el comienzo de la función.
    if (!agregar_cuadruplo (contexto -> lista, CI_INICIO_FUNCION, &funcion, &vacio, &vacio)) {
        registrar_error (contexto, "No se pudo agregar el inicio de la función.");
    }

    // Los hijos intermedios pertenecen a los parámetros.
    for (int i = 1; i < nodo -> cantidad_hijos - 1 && !contexto -> error; i ++) {
        NodoAST *parametro = nodo -> hijos [i];

        if (parametro == NULL || parametro -> tipo != AST_PARAMETRO || parametro -> cantidad_hijos != 1) {
            registrar_error (contexto, "La declaración contiene un parámetro inválido.");
            break;
        }

        Operando operando_parametro = crear_operando_identificador (contexto, parametro -> hijos [0]);

        if (contexto -> error) {
            liberar_operando (&operando_parametro);
            break;
        }

        if (!agregar_cuadruplo (contexto -> lista, CI_PARAMETRO, &operando_parametro, &vacio, &vacio)) {
            registrar_error (contexto, "No se pudo agregar un parámetro de función.");
        }

        liberar_operando (&operando_parametro);
    }

    // Se genera el cuerpo de la función.
    if (!contexto -> error) {
        generar_bloque (bloque, contexto);
    }

    // Marca el final de la función.
    if (!contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_FIN_FUNCION, &funcion, &vacio, &vacio)) {
            registrar_error (contexto, "No se pudo agregar el final de la función.");
        }
    }

    if (!contexto -> error) {
        debug_generador (contexto, "Declaración de función generada correctamente.");
    }

    liberar_operando (&funcion);
    liberar_operando (&vacio);

    return !contexto -> error;
}

/**
 * Genera las instrucciones de un bloque.
 * 
 * Las declaraciones locales no generan instrucciones ejecutables.
 * Las sentencias se procesan en el orden en que aparecen.
 */
static int generar_bloque (NodoAST *nodo, ContextoGenerador *contexto) {
    if (nodo == NULL || nodo -> tipo != AST_BLOQUE) {
        registrar_error (contexto, "Se encontró un bloque inválido."); 
        return 0;
    }

    for (int i = 0; i < nodo -> cantidad_hijos && !contexto -> error; i ++) {
        NodoAST *hijo = nodo -> hijos [i];

        if (hijo == NULL) {
            registrar_error (contexto, "El bloque contiene un nodo nulo.");
            break;
        }

        // Las declaraciones locales no generan código ejecutable.
        if (hijo -> tipo == AST_DECLARACION_VARIABLE) {
            debug_generador (contexto, "Se omite una declaración local de variable.");
            continue;
        }

        generar_sentencia (hijo, contexto);
    }

    return !contexto -> error;
}

/**
 * Genera el código correspondiente a una sentencia.
 * 
 * Cada tipo de sentencia delega su generación a la
 * función auxiliar correspondiente.
 */
static int generar_sentencia (NodoAST *nodo, ContextoGenerador *contexto) {
    if (nodo == NULL) {
        registrar_error (contexto, "Se intentó generar una sentencia nula.");
        return 0;
    }

    switch (nodo -> tipo) {
        case AST_BLOQUE:
            return generar_bloque (nodo, contexto);
        case AST_ASIGNACION:
            return generar_asignacion (nodo, contexto);
        case AST_IF:
            return generar_if (nodo, contexto);
        case AST_WHILE:
            return generar_while (nodo, contexto);
        case AST_RETURN:
            return generar_return (nodo, contexto);
        case AST_LLAMADA: {
            // Se utiliza como sentencia, no se conserva un valor de retorno.
            Operando resultado = generar_llamada (nodo, contexto, 0);

            liberar_operando (&resultado);

            return !contexto -> error;
        }
        case AST_SENTENCIA_VACIA:
            // Una sentencia vacía no genera instrucciones.
            return 1;
        default:
            registrar_error (contexto, "El nodo no representa una sentencia válida.");
            return 0;
    }
}

/**
 * Genera una asignación.
 * 
 * El resultado de evaluar la expresión se copia al
 * símbolo correspondiente a la variable destino.
 */
static int generar_asignacion (NodoAST *nodo, ContextoGenerador *contexto) {
    if (nodo == NULL || nodo -> tipo != AST_ASIGNACION || nodo -> cantidad_hijos != 2) {
        registrar_error (contexto, "La asignación tiene una estructura inválida.");
        return 0;
    }

    debug_generador (contexto, "Generando una asignación.");

    // El primer hijo identifica la variable destino.
    Operando destino = crear_operando_identificador (contexto, nodo -> hijos [0]);

    // El segundo hijo contiene la expresión asignada.
    Operando valor = generar_expresion (nodo -> hijos [1], contexto);

    if (contexto -> error) {
        liberar_operando (&destino);
        liberar_operando (&valor);
        return 0;
    }

    Operando vacio = crear_operando_vacio ();

    // ASSIGN valor - destino.
    if (!agregar_cuadruplo (contexto -> lista, CI_ASIGNAR, &valor, &vacio, &destino)) {
        registrar_error (contexto, "No se pudo agregar el cuádruplo de asignación.");
    }

    liberar_operando (&destino);
    liberar_operando (&valor);
    liberar_operando (&vacio);

    if (!contexto -> error) {
        debug_generador (contexto, "Asignación generada correctamente.");
    }

    return !contexto -> error;
}

/**
 * Genera una sentencia if.
 * 
 * La condición dirige el control a la rama verdadera o falsa.
 * Si existe una rama else, ambas ramas convergen en una etiqueta
 * final común.
 */
static int generar_if (NodoAST *nodo, ContextoGenerador *contexto) {
    if (nodo == NULL || nodo -> tipo != AST_IF || (nodo -> cantidad_hijos != 2 && nodo -> cantidad_hijos != 3)) {
        registrar_error (contexto, "La sentencia if tiene una estructura inválida.");
        return 0;
    }

    debug_generador (contexto, "Generando una sentencia if.");

    Operando etiqueta_verdadera = crear_operando_etiqueta_nueva (contexto);
    Operando etiqueta_falsa = crear_operando_etiqueta_nueva (contexto);
    Operando etiqueta_fin = crear_operando_vacio ();

    // La etiqueta final solamente es necesaria si existe un else.
    if (nodo -> cantidad_hijos == 3) {
        etiqueta_fin = crear_operando_etiqueta_nueva (contexto);
    }

    if (contexto -> error) {
        liberar_operando (&etiqueta_verdadera);
        liberar_operando (&etiqueta_falsa);
        liberar_operando (&etiqueta_fin);
        return 0;
    }

    // La condición selecciona una de las dos ramas.
    generar_condicion (nodo -> hijos [0], contexto, &etiqueta_verdadera, &etiqueta_falsa);

    Operando vacio = crear_operando_vacio ();

    // Comienzo de la rama verdadera.
    if (!contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_ETIQUETA, &vacio, &vacio, &etiqueta_verdadera)) {
            registrar_error (contexto, "No se pudo agregar la etiqueta de la rama verdadera del if.");
        }
    }

    if (!contexto -> error) {
        generar_bloque (nodo -> hijos [1], contexto);
    }

    // Si existe else, se salta la rama falsa al terminar then.
    if (nodo -> cantidad_hijos == 3 && !contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_SALTO, &vacio, &vacio, &etiqueta_fin)) {
            registrar_error (contexto, "No se pudo agregar el salto al final de la rama then.");
        }
    }

    // La etiqueta falsa también es necesaria cuando no existe else.
    if (!contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_ETIQUETA, &vacio, &vacio, &etiqueta_falsa)) {
            registrar_error (contexto, "No se pudo agregar la etiqueta de la rama falsa del if.");
        }
    }

    // Se genera la rama del else, si existe.
    if (nodo -> cantidad_hijos == 3 && !contexto -> error) {
        generar_bloque (nodo -> hijos [2], contexto);
    }

    // Con else, ambas ramas convergen acá.
    if (nodo -> cantidad_hijos == 3 && !contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_ETIQUETA, &vacio, &vacio, &etiqueta_fin)) {
            registrar_error (contexto, "No se pudo asegurar la etiqueta final del if.");
        }
    }

    liberar_operando (&vacio);
    liberar_operando (&etiqueta_verdadera);
    liberar_operando (&etiqueta_falsa);
    liberar_operando (&etiqueta_fin);

    return !contexto -> error;
}

/**
 * Genera una sentencia while.
 * 
 * El código vuelve a evaluar la condición al finalizar
 * el cuerpo del bucle.
 */
static int generar_while (NodoAST *nodo, ContextoGenerador *contexto) {
    if (nodo == NULL || nodo -> tipo != AST_WHILE || nodo -> cantidad_hijos != 2) {
        registrar_error (contexto, "La sentencia while tiene una estructura inválida.");
        return 0;
    }
    
    debug_generador (contexto, "Generando una sentencia while.");

    Operando etiqueta_inicio = crear_operando_etiqueta_nueva (contexto);
    Operando etiqueta_cuerpo = crear_operando_etiqueta_nueva (contexto);
    Operando etiqueta_fin = crear_operando_etiqueta_nueva (contexto);

    if (contexto -> error) {
        liberar_operando (&etiqueta_inicio);
        liberar_operando (&etiqueta_cuerpo);
        liberar_operando (&etiqueta_fin);
        return 0;
    }

    Operando vacio = crear_operando_vacio ();

    // Marca el inicio del bucle.
    if (!agregar_cuadruplo (contexto -> lista, CI_ETIQUETA, &vacio, &vacio, &etiqueta_inicio)) {
        registrar_error (contexto, "No se pudo agregar la etiqueta inicial del while.");
    }

    // Evalúa la condición y selecciona cuerpo o salida.
    if (!contexto -> error) {
        generar_condicion (nodo -> hijos [0], contexto, &etiqueta_cuerpo, &etiqueta_fin);
    }

    // Comienzo del cuerpo.
    if (!contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_ETIQUETA, &vacio, &vacio, &etiqueta_cuerpo)) {
            registrar_error (contexto, "No se pudo agregar la etiqueta del cuerpo del while.");
        }
    }

    if (!contexto -> error) {
        generar_bloque (nodo -> hijos [1], contexto);
    }

    // Al terminar el cuerpo, se vuelve a evaluar la condición.
    if (!contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_SALTO, &vacio, &vacio, &etiqueta_inicio)) {
            registrar_error (contexto, "No se pudo agregar el salto de retorno del while.");
        }
    }

    // Salida del bucle.
    if (!contexto -> error) {
        if (!agregar_cuadruplo (contexto -> lista, CI_ETIQUETA, &vacio, &vacio, &etiqueta_fin)) {
            registrar_error (contexto, "No se pudo agregar la etiqueta final del while.");
        }
    }

    liberar_operando (&vacio);
    liberar_operando (&etiqueta_inicio);
    liberar_operando (&etiqueta_cuerpo);
    liberar_operando (&etiqueta_fin);

    return !contexto -> error;
}

/**
 * Genera una sentencia return.
 * 
 * Si contiene una expresión, su resultado se almacena
 * en el primer argumento del cuádruplo CI_RETURN.
 * 
 * Si no contiene expresión, se genera un return vacío.
 */
static int generar_return (NodoAST *nodo, ContextoGenerador *contexto) {
    if (nodo == NULL || nodo -> tipo != AST_RETURN || nodo -> cantidad_hijos > 1) {
        registrar_error (contexto, "La sentencia return tiene una estructura inválida.");
        return 0;
    }

    debug_generador (contexto, "Generando una sentencia return.");

    Operando valor = crear_operando_vacio ();

    // Un return puede no tener expresión.
    if (nodo -> cantidad_hijos == 1) {
        valor = generar_expresion (nodo -> hijos [0], contexto);
    }

    if (contexto -> error) {
        liberar_operando (&valor);
        return 0;
    }

    Operando vacio = crear_operando_vacio ();

    if (!agregar_cuadruplo (contexto -> lista, CI_RETURN, &valor, &vacio, &vacio)) {
        registrar_error (contexto, "No se pudo agregar el cuádruplo return.");
    }

    liberar_operando (&valor);
    liberar_operando (&vacio);

    if (!contexto -> error) {
        debug_generador (contexto, "Sentencia return generada correctamente.");
    }

    return !contexto -> error;
}
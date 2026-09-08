#include "AnalizadorSemantico.h"
#include <stdio.h>

/**
 * Contexto utilizado durante el análisis semántico.
 * 
 * Contiene la Tabla de Símbolos utilizada durante el análisis
 * y la cantidad de errores semánticos encontrados.
 */
typedef struct {
    TablaSimbolos *ts;
    int errores;
} ContextoSemantico;

/* Funciones auxiliares privadas. */
static void analizar_programa (NodoAST *nodo, ContextoSemantico *contexto);
static void analizar_declaraciones (NodoAST *nodo, ContextoSemantico *contexto); 
static void analizar_declaracion (NodoAST *nodo, ContextoSemantico *contexto);
static void analizar_sentencias (NodoAST *nodo, ContextoSemantico *contexto);
static void analizar_sentencia (NodoAST *nodo, ContextoSemantico *contexto);
static TipoDato analizar_expresion (NodoAST *nodo, ContextoSemantico *contexto);
static TipoDato analizar_identificador (NodoAST *nodo, ContextoSemantico *contexto);
static const char *tipo_a_string (TipoDato tipo);
static void error_semantico (ContextoSemantico *contexto, int linea, const char *mensaje);

int analizar_semantica (NodoAST *arbol, TablaSimbolos *ts) {
    ContextoSemantico contexto;

    // Se verifican que existan el AST y la TS.
    if (arbol == NULL || ts == NULL) {
        return 1;
    }

    // Se inicializa el contexto del análisis.
    contexto.ts = ts;
    contexto.errores = 0;

    // Se comienza el análisis desde la raíz del AST.
    analizar_programa (arbol, &contexto);

    // Se devuelve la cantidad de errores encontrados.
    return contexto.errores;
}

/* =========== Funciones auxiliares privadas =========== */

/**
 * Analiza semánticamente el nodo raíz del programa.
 * 
 * Registra la función main en la Tabla de Símbolos
 * y analiza sus declaraciones y sentencias.
 */
static void analizar_programa (NodoAST *nodo, ContextoSemantico *contexto) {
    Simbolo *main;

    // Se registra main como una función en la TS.
    main = insertar_elemento (contexto -> ts, "main", nodo -> tipo_dato, SIMBOLO_FUNCION);

    // Se verifica que main haya sido insertado correctamente.
    if (main == NULL) {
        error_semantico (contexto, nodo -> linea, "no se pudo insertar la función 'main' en la Tabla de Símbolos.");
    }

    // El primer hijo contiene las declaraciones.
    analizar_declaraciones (nodo -> hijos [0], contexto);

    // El segundo hijo contiene las sentencias.
    analizar_sentencias (nodo -> hijos [1], contexto);
}

/**
 * Analiza recursivamente la lista de declaraciones.
 * 
 * Recorre los nodos AST_DECLARACIONES y analiza cada
 * declaración individual contenida en la lista.
 */
static void analizar_declaraciones (NodoAST *nodo, ContextoSemantico *contexto) {
    // Se recorren los hijos de la lista de declaraciones.
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        NodoAST *hijo = nodo -> hijos [i];
        
        // El hijo puede ser una declaración individual.
        if (hijo -> tipo == AST_DECLARACION) {
            analizar_declaracion (hijo, contexto);
        }

        // O puede ser otra lista de declaraciones.
        else {
            analizar_declaraciones (hijo, contexto);
        }
    }
}

/**
 * Analiza una declaración individual de variable.
 * 
 * Inserta la variable en la Tabla de Símbolos y verifica que no
 * exista otra variable con el mismo nombre en el nivel actual.
 */
static void analizar_declaracion (NodoAST *nodo, ContextoSemantico *contexto) {
    NodoAST *identificador;
    Simbolo *simbolo;

    identificador = nodo -> hijos [0];

    // Se inserta la variable en el nivel actual de la TS.
    simbolo = insertar_elemento (contexto -> ts, identificador -> valor.identificador, nodo -> tipo_dato, SIMBOLO_VARIABLE);

    // La variable ya existe en el nivel actual.
    if (simbolo == NULL) {
        char mensaje [200];
        snprintf (mensaje, sizeof (mensaje), "la variable '%s' ya fue declarada.", identificador -> valor.identificador);
        error_semantico (contexto, identificador -> linea, mensaje);

        return;
    }

    // Se asocia el identificador del AST con su símbolo.
    identificador -> simbolo = simbolo;

    // Se guarda el tipo de la variable en el nodo.
    identificador -> tipo_dato = simbolo -> tipo;
}

/**
 * Analiza recursivamente la lista de sentencias.
 * 
 * Recorre los nodos AST_SENTENCIAS y analiza cada
 * sentencia individual contenida en la lista.
 */
static void analizar_sentencias (NodoAST *nodo, ContextoSemantico *contexto) {
    // Se recorren los elementos de la lista.
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        NodoAST *hijo = nodo -> hijos [i];
        
        // La lista puede continuar recursivamente.
        if (hijo -> tipo == AST_SENTENCIAS) {
            analizar_sentencias (hijo, contexto);
        }

        // En otro caso se trata de una sentencia individual.
        else {
            analizar_sentencia (hijo, contexto);
        }
    }
}

/**
 * Analiza una sentencia individual.
 * 
 * Verifica las reglas semánticas correspondientes
 * a asignaciones y sentencias return.
 */
static void analizar_sentencia (NodoAST *nodo, ContextoSemantico *contexto) {
    NodoAST *identificador;
    NodoAST *expresion;

    TipoDato tipo_identificador;
    TipoDato tipo_expresion;

    Simbolo *main;

    switch (nodo -> tipo) {
        case AST_ASIGNACION:
            identificador = nodo -> hijos [0];
            expresion = nodo -> hijos [1];

            // Se analiza el identificador del lado izquierdo.
            tipo_identificador = analizar_identificador (identificador, contexto);

            // Se analiza la expresión del lado derecho.
            tipo_expresion = analizar_expresion (expresion, contexto);

            // Se comparan los tipos si ambos están definidos.
            if (tipo_identificador != TIPO_NO_DEFINIDO && tipo_expresion != TIPO_NO_DEFINIDO && tipo_identificador != tipo_expresion) {
                char mensaje [250];
                snprintf (mensaje, sizeof (mensaje), "no se puede asignar una expresión de tipo %s a una variable de tipo %s.", tipo_a_string (tipo_expresion), tipo_a_string (tipo_identificador));
                error_semantico (contexto, nodo -> linea, mensaje);
            }

            break;
        case AST_RETURN:
            // Se busca main para obtener su tipo de retorno.
            main = buscar_elemento (contexto -> ts, "main");

            if (main == NULL) {
                error_semantico (contexto, nodo -> linea, "no se encontró la función 'main' en la TS.");
            
                break;
            }

            if (nodo -> cantidad_hijos == 0) {
                // return; solamente es válido en un main void.
                if (main -> tipo != TIPO_VOID) {
                    char mensaje [250];
                    snprintf (mensaje, sizeof (mensaje), "'return;' no es válido en un main que retorna %s.", tipo_a_string (main -> tipo));
                    error_semantico (contexto, nodo -> linea, mensaje);
                }
            }
            else {
                // return <expresion>; se analiza la expresión de retorno.
                tipo_expresion = analizar_expresion (nodo -> hijos [0], contexto);

                // return E no es válido en un main void.
                if (main -> tipo == TIPO_VOID) {
                    error_semantico (contexto, nodo -> linea, "'return' con expresión no es válido en un main de tipo void.");
                }
                // Si ambos tipos están definidos, se comparan.
                else if (tipo_expresion != TIPO_NO_DEFINIDO && tipo_expresion != main -> tipo) {
                    char mensaje [250];
                    snprintf (mensaje, sizeof (mensaje), "el tipo de la expresión de retorno es %s, pero main debe retornar %s.", tipo_a_string (tipo_expresion), tipo_a_string (main -> tipo));
                    error_semantico (contexto, nodo -> linea, mensaje);
                }
            }

            break;
        default:
            error_semantico (contexto, nodo -> linea, "sentencia no reconocida por el analizador semántico.");

            break;
    }
}

/**
 * Analiza una expresión y determina su tipo de dato.
 * 
 * Verifica constantes, identificadores y operadores aritméticos,
 * determinando el tipo resultante de cada expresión.
 */
static TipoDato analizar_expresion (NodoAST *nodo, ContextoSemantico *contexto) {
    TipoDato tipo_izquierdo;
    TipoDato tipo_derecho;

    switch (nodo -> tipo) {
        case AST_NUMERO:
            // Todo número entero es de tipo int.
            nodo -> tipo_dato = TIPO_INT;
            
            return TIPO_INT;
        case AST_TRUE:
            // Las constantes booleanas son de tipo bool.
            nodo -> tipo_dato = TIPO_BOOL;
            
            return TIPO_BOOL;
        case AST_FALSE:
            // Las constantes booleanas son de tipo bool.
            nodo -> tipo_dato = TIPO_BOOL;
            
            return TIPO_BOOL;
        case AST_IDENTIFICADOR:
            // Se busca y asocia el identificador con su símbolo.
            return analizar_identificador (nodo, contexto);
        case AST_SUMA:
            // Se analiza el operando izquierdo.
            tipo_izquierdo = analizar_expresion (nodo -> hijos [0], contexto);

            // Se analiza el operando derecho.
            tipo_derecho = analizar_expresion (nodo -> hijos [1], contexto);

            // Se evitan errores adicionales si un tipo es desconocido.
            if (tipo_izquierdo == TIPO_NO_DEFINIDO || tipo_derecho == TIPO_NO_DEFINIDO) {
                nodo -> tipo_dato = TIPO_NO_DEFINIDO;

                return TIPO_NO_DEFINIDO;
            }

            // La suma solamente admite operandos enteros.
            if (tipo_izquierdo != TIPO_INT || tipo_derecho != TIPO_INT) {
                error_semantico (contexto, nodo -> linea, "el operador '+' requiere operandos de tipo int.");
            
                nodo -> tipo_dato = TIPO_NO_DEFINIDO;

                return TIPO_NO_DEFINIDO;
            }

            // Una suma válida produce un entero.
            nodo -> tipo_dato = TIPO_INT;

            return TIPO_INT;
        case AST_MULTIPLICACION:
            // Se analiza el operando izquierdo.
            tipo_izquierdo = analizar_expresion (nodo -> hijos [0], contexto);

            // Se analiza el operando derecho.
            tipo_derecho = analizar_expresion (nodo -> hijos [1], contexto);

            // Se evitan errores adicionales si un tipo es desconocido.
            if (tipo_izquierdo == TIPO_NO_DEFINIDO || tipo_derecho == TIPO_NO_DEFINIDO) {
                nodo -> tipo_dato = TIPO_NO_DEFINIDO;

                return TIPO_NO_DEFINIDO;
            }

            // La multiplicación solamente admite operandos enteros.
            if (tipo_izquierdo != TIPO_INT || tipo_derecho != TIPO_INT) {
                error_semantico (contexto, nodo -> linea, "el operador '*' requiere operandos de tipo int.");
            
                nodo -> tipo_dato = TIPO_NO_DEFINIDO;

                return TIPO_NO_DEFINIDO;
            }

            // Una multiplicación válida produce un entero.
            nodo -> tipo_dato = TIPO_INT;

            return TIPO_INT;
        default:
            error_semantico (contexto, nodo -> linea, "nodo de expresión no reconocido.");
        
            nodo -> tipo_dato = TIPO_NO_DEFINIDO;

            return TIPO_NO_DEFINIDO;

    }
}

/**
 * Analiza un identificador utilizado en una expresión o asignación.
 * 
 * Busca el identificador en la Tabla de Símbolos y, si existe, asocia el
 * nodo del AST con el símbolo correspondiente y determina su tipo de dato.
 */
static TipoDato analizar_identificador (NodoAST *nodo, ContextoSemantico *contexto) {
    Simbolo *simbolo;

    // Se busca el identificador en la TS.
    simbolo = buscar_elemento (contexto -> ts, nodo -> valor.identificador);

    // Si no existe, se trata de una variable no declarada.
    if (simbolo == NULL) {
        char mensaje [200];
        snprintf (mensaje, sizeof (mensaje), "el identificador '%s' no fue declarado.", nodo -> valor.identificador);
        error_semantico (contexto, nodo -> linea, mensaje);

        nodo -> tipo_dato = TIPO_NO_DEFINIDO;

        return TIPO_NO_DEFINIDO;
    }

    // Se asocia el nodo con el símbolo encontrado.
    nodo -> simbolo = simbolo;

    // Se guarda el tipo del símbolo en el nodo.
    nodo -> tipo_dato = simbolo -> tipo;

    return simbolo -> tipo;
}

/**
 * Devuelve una representación textual de un tipo de dato.
 */
static const char *tipo_a_string (TipoDato tipo) {
    // Se determina la representación textual del tipo.
    switch (tipo) {
        case TIPO_INT:
            return "int";
        case TIPO_BOOL:
            return "bool";
        case TIPO_VOID:
            return "void";
        case TIPO_NO_DEFINIDO:
            return "no definido";
        default:
            return "desconocido";
    }
}

/**
 * Registra e informa un error semántico.
 * 
 * Incrementa el contador de errores y muestra el mensaje
 * junto con la línea donde se produjo cada error.
 */
static void error_semantico (ContextoSemantico *contexto, int linea, const char *mensaje) {
    // Se incrementa el contador de errores.
    contexto -> errores ++;

    // Se muestra la línea y el mensaje correspondiente.
    fprintf (stderr, "\nERROR SEMÁNTICO: línea %d - %s\n", linea, mensaje);
}
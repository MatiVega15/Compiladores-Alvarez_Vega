#include "Interprete.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Representa una variable durante la ejecución.
 * 
 * Esta estructura almacena su valor actual dinámicamente.
 * 
 * Formado por:
 * - Una referencia al símbolo correspondiente de la TS.
 * - El valor actual de la variable.
 * - Un indicador de si la variable ya fue inicializada (1) o no (0).
 * - Una lista de variables del entorno de ejecución.
 */
typedef struct ValorVariable {
    Simbolo *simbolo;
    ValorRuntime valor;
    int inicializada;

    struct ValorVariable *siguiente;
} ValorVariable;

/**
 * Representa el contexto utilizado durante la interpretación.
 * 
 * Contiene el estado dinámico de las variables en ejecución.
 * 
 * Formado por:
 * - Una lista de variables y sus valores actuales.
 * - Un indicador de si se encontró un return durante la ejecución (1) o no (0).
 * - El valor producido por un return con expresión.
 * - Un indicador de si hubo errores.
 */
typedef struct {
    ValorVariable *variables;

    int hubo_retorno;
    ValorRuntime valor_retorno;
    int error;
} ContextoInterprete;

/* Funciones auxiliares privadas. */
static void inicializar_declaraciones (NodoAST *nodo, ContextoInterprete *contexto);
static void inicializar_declaracion (NodoAST *nodo, ContextoInterprete *contexto);
static void interpretar_sentencias (NodoAST *nodo, ContextoInterprete *contexto);
static void interpretar_sentencia (NodoAST *nodo, ContextoInterprete *contexto);
static void interpretar_asignacion (NodoAST *nodo, ContextoInterprete *contexto);
static void interpretar_return (NodoAST *nodo, ContextoInterprete *contexto);
static ValorRuntime evaluar_expresion (NodoAST *nodo, ContextoInterprete *contexto);
static ValorRuntime evaluar_identificador (NodoAST *nodo, ContextoInterprete *contexto);
static ValorVariable *buscar_variable (ContextoInterprete *contexto, Simbolo *simbolo);
static ValorVariable *crear_variable (Simbolo *simbolo);
static void liberar_variables (ValorVariable *variables);
static ValorRuntime crear_valor_entero (int valor);
static ValorRuntime crear_valor_booleano (int valor);
static ValorRuntime crear_valor_invalido (void);
static void error_interprete (ContextoInterprete *contexto, int linea, const char *mensaje);

int interpretar_programa (NodoAST *arbol, ValorRuntime *valor_retorno) {
    ContextoInterprete contexto;

    // Verificación de parámetros.
    if (arbol == NULL || valor_retorno == NULL) {
        fprintf (stderr, "ERROR DE INTERPRETACIÓN: parámetros inválidos.\n\n");

        return 1;
    }

    // Se inicializa el contexto de ejecución.
    contexto.variables = NULL;
    contexto.hubo_retorno = 0;
    contexto.valor_retorno = crear_valor_invalido ();
    contexto.error = 0;

    // Se crea el estado dinámico correspondiente a cada variable declarada.
    inicializar_declaraciones (arbol -> hijos [0], &contexto);

    // Se ejecutan las sentencias en el orden en que aparecen en el programa.
    interpretar_sentencias (arbol -> hijos [1], &contexto);

    // Se devuelve el valor producido por main.
    *valor_retorno = contexto.valor_retorno;

    // Se libera el estado dinámico creado por el intérprete.
    liberar_variables (contexto.variables);

    return contexto.error ? 1 : 0;
}

/* =========== Funciones auxiliares privadas =========== */

/**
 * Recorre recursivamente el nodo de declaraciones.
 * 
 * Crea el estado dinámico correspondiente a cada variable.
 */
static void inicializar_declaraciones (NodoAST *nodo, ContextoInterprete *contexto) {
    // Se recorren todos los hijos recursivamente.
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        NodoAST *hijo = nodo -> hijos [i];

        // Una declaración puede ser única.
        if (hijo -> tipo == AST_DECLARACION) {
            inicializar_declaracion (hijo, contexto);
        }
        // O puede ser una lista de declaraciones.
        else {
            inicializar_declaraciones (hijo, contexto);
        }
    }
}

/**
 * Inicializa una declaración individual.
 * 
 * La variable comienza sin valor.
 */
static void inicializar_declaracion (NodoAST *nodo, ContextoInterprete *contexto) {
    NodoAST *identificador;
    ValorVariable *variable;

    identificador = nodo -> hijos [0];

    // Se crea e incorpora la correspondiente variable en el entorno de ejecución.
    variable = crear_variable (identificador -> simbolo);
    variable -> siguiente = contexto -> variables;
    contexto -> variables = variable;
}

/**
 * Interpreta la lista de sentencias del programa.
 * 
 * Las sentencias se ejecutan secuencialmente, en el
 * mismo orden en el que aparecen en el código fuente.
 */
static void interpretar_sentencias (NodoAST *nodo, ContextoInterprete *contexto) {
    NodoAST *primera_sentencia;
    NodoAST *resto_sentencias;

    if (nodo == NULL || contexto -> hubo_retorno) {
        return;
    }
    
    // Se ejecuta la primera sentencia.
    primera_sentencia = nodo -> hijos [0];
    interpretar_sentencia (primera_sentencia, contexto);

    // Si no hubo return, se continúa con el resto.
    if (!contexto -> hubo_retorno && nodo -> cantidad_hijos > 1) {
        resto_sentencias = nodo -> hijos [1];

        interpretar_sentencias (resto_sentencias, contexto);
    }
}

/**
 * Interpreta una sentencia individual.
 * 
 * El análisis sintáctico y semántico ya determinaron
 * qué tipo de sentencia representa cada nodo.
 */
static void interpretar_sentencia (NodoAST *nodo, ContextoInterprete *contexto) {
    switch (nodo -> tipo) {
        case AST_ASIGNACION:
            interpretar_asignacion (nodo, contexto);
            break;
        case AST_RETURN:
            interpretar_return (nodo, contexto);
            break;
        default:
            break;   
    }
}

/**
 * Ejecuta una asignación.
 * 
 * Evalúa la expresión de la derecha y la almacena del lado izquierdo.
 */
static void interpretar_asignacion (NodoAST *nodo, ContextoInterprete *contexto) {
    NodoAST *identificador;
    NodoAST *expresion;

    ValorVariable *variable;
    ValorRuntime valor;

    identificador = nodo -> hijos [0];
    expresion = nodo -> hijos [1];

    variable = buscar_variable (contexto, identificador -> simbolo);

    // Se evalúa la expresión de la derecha.
    valor = evaluar_expresion (expresion, contexto);

    if (valor.tipo == TIPO_NO_DEFINIDO) {
        return;
    }

    // Se almacena el resultado.
    variable -> valor = valor;

    // La variable pasa a estar inicializada.
    variable -> inicializada = 1;
}

/**
 * Ejecuta una sentencia return.
 */
static void interpretar_return (NodoAST *nodo, ContextoInterprete *contexto) {
    // Se marca que se produjo el retorno.
    contexto -> hubo_retorno = 1;

    // Return sin expresión.
    if (nodo -> cantidad_hijos == 0) {
        return;
    }

    // Return con expresión.
    contexto -> valor_retorno = evaluar_expresion (nodo -> hijos [0], contexto);
}

/**
 * Evalúa una expresión y devuelve su valor durante la ejecución.
 * 
 * El tipo de cada expresión ya fue determinado por el análisis semántico.
 */
static ValorRuntime evaluar_expresion (NodoAST *nodo, ContextoInterprete *contexto) {
    ValorRuntime izquierda;
    ValorRuntime derecha;

    switch (nodo -> tipo) {
        case AST_NUMERO:
            return crear_valor_entero (nodo -> valor.numero);
        case AST_TRUE:
            return crear_valor_booleano (1);
        case AST_FALSE:
            return crear_valor_booleano (0);
        case AST_IDENTIFICADOR:
            return evaluar_identificador (nodo, contexto);
        case AST_SUMA:
            izquierda = evaluar_expresion (nodo -> hijos [0], contexto);
            
            // Se detiene si la expresión izquierda produjo un error.
            if (izquierda.tipo == TIPO_NO_DEFINIDO) {
                return crear_valor_invalido ();
            }

            derecha = evaluar_expresion (nodo -> hijos [1], contexto);
            
            // Se detiene si la expresión derecha produjo un error.
            if (derecha.tipo == TIPO_NO_DEFINIDO) {
                return crear_valor_invalido ();
            }

            return crear_valor_entero (izquierda.entero + derecha.entero);
        case AST_MULTIPLICACION:
            izquierda = evaluar_expresion (nodo -> hijos [0], contexto);
            
            // Se detiene si la expresión izquierda produjo un error.
            if (izquierda.tipo == TIPO_NO_DEFINIDO) {
                return crear_valor_invalido ();
            }

            derecha = evaluar_expresion (nodo -> hijos [1], contexto);
            
            // Se detiene si la expresión derecha produjo un error.
            if (derecha.tipo == TIPO_NO_DEFINIDO) {
                return crear_valor_invalido ();
            }

            return crear_valor_entero (izquierda.entero * derecha.entero);
        default:
            return crear_valor_invalido ();
    }
}

/**
 * Obtiene el valor actual de un identificador.
 * 
 * El símbolo asociado al identificador permite encontrar su
 * correspondiente valor dentro del entorno de ejecución.
 */
static ValorRuntime evaluar_identificador (NodoAST *nodo, ContextoInterprete *contexto) {
    ValorVariable *variable;

    variable = buscar_variable (contexto, nodo -> simbolo);

    // Variable declarada pero no inicializada.
    if (!variable -> inicializada) {
        error_interprete (contexto, nodo -> linea, "se intentó utilizar una variable sin inicializar.");
    
        return crear_valor_invalido ();
    }

    // Se devuelve el valor almacenado actualmente.
    return variable -> valor;
}

/**
 * Busca una variable dentro del entorno de ejecución.
 * 
 * La búsqueda se realiza utilizando el puntero al símbolo.
 */
static ValorVariable *buscar_variable (ContextoInterprete *contexto, Simbolo *simbolo) {
    ValorVariable *actual;

    actual = contexto -> variables;

    while (actual != NULL) {
        if (actual -> simbolo == simbolo) {
            return actual;
        }

        actual = actual -> siguiente;
    }

    return NULL;
}

/**
 * Crea una variable dentro del entorno de ejecución.
 * 
 * La variable comienza sin inicializar.
 */
static ValorVariable *crear_variable (Simbolo *simbolo) {
    ValorVariable *variable;

    variable = malloc (sizeof (ValorVariable));

    if (variable == NULL) {
        fprintf (stderr, "Error: no se pudo reservar memoria para una variable de ejecución.\n");

        exit (EXIT_FAILURE);
    }

    variable -> simbolo = simbolo;
    // El valor todavía no está definido.
    variable -> valor = crear_valor_invalido ();
    variable -> inicializada = 0;
    variable -> siguiente = NULL;

    return variable;
}

/**
 * Libera todas las variables del entorno de ejecución.
 */
static void liberar_variables (ValorVariable *variables) {
    ValorVariable *actual;
    ValorVariable *siguiente;

    actual = variables;

    while (actual != NULL) {
        siguiente = actual -> siguiente;

        free (actual);

        actual = siguiente;
    }
}

/**
 * Crea un valor entero.
 */
static ValorRuntime crear_valor_entero (int valor) {
    ValorRuntime resultado;

    resultado.tipo = TIPO_INT;
    resultado.entero = valor;

    return resultado;
}

/**
 * Crea un valor booleano.
 * 
 * Se utiliza 0 para false, 1 para true.
 */
static ValorRuntime crear_valor_booleano (int valor) {
    ValorRuntime resultado;

    resultado.tipo = TIPO_BOOL;
    resultado.booleano = valor;

    return resultado;
}

/**
 * Crea un valor sin tipo definido.
 */
static ValorRuntime crear_valor_invalido (void) {
    ValorRuntime resultado;

    resultado.tipo = TIPO_NO_DEFINIDO;
    resultado.entero = 0;

    return resultado;
}

/**
 * Informa un error producido durante la ejecución.
 */
static void error_interprete (ContextoInterprete *contexto, int linea, const char *mensaje) {
    fprintf (stderr, "\nERROR DE INTERPRETACIÓN: línea %d - %s\n", linea, mensaje);

    contexto -> error = 1;
}
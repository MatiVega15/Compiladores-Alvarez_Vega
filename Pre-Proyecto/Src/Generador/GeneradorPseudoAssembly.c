#include "GeneradorPseudoAssembly.h"
#include "TS.h"
#include <stdio.h>

/**
 * Contexto del generador.
 * 
 * Contiene la información necesaria durante
 * la generación del pseudo-assembly.
 */
typedef struct {
    FILE *archivo;
    int siguiente_temporal;
} ContextoGenerador;

/* Tamaño constante de los operandos. */
#define TAM_OPERANDO 50

/* Funciones auxiliares privadas. */
static void generar_sentencias (NodoAST *nodo, ContextoGenerador *contexto);
static void generar_sentencia (NodoAST *nodo, ContextoGenerador *contexto);
static void generar_expresion (NodoAST *nodo, ContextoGenerador *contexto, char *resultado);

void generar_pseudo_assembly (NodoAST *arbol, FILE *archivo) {
    // Se verifica que el árbol y el archivo sean válidos.
    if (arbol == NULL || archivo == NULL) {
        return;
    }

    // El nodo raíz debe representar un programa.
    if (arbol -> tipo != AST_PROGRAMA) {
        return;
    }

    ContextoGenerador contexto;
    contexto.archivo = archivo;
    contexto.siguiente_temporal = 1;

    // Solamente se recorre la parte de las sentencias, las declaraciones no generan instrucciones.
    generar_sentencias (arbol -> hijos [1], &contexto);
}

/* =========== Funciones auxiliares privadas =========== */

/**
 * Recorre las sentencias del AST en el mismo orden
 * en que aparecen en el programa fuente.
 */
static void generar_sentencias (NodoAST *nodo, ContextoGenerador *contexto) {
    // Si es una lista de sentencias, se la recorre recursivamente.
    if (nodo -> tipo == AST_SENTENCIAS) {
        for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
            generar_sentencias (nodo -> hijos [i], contexto);
        }
    }
    // Si es una sentencia individual, se genera su código.
    else {
        generar_sentencia (nodo, contexto);
    }
}

/**
 * Genera el código correspondiente a una sentencia.
 */
static void generar_sentencia (NodoAST *nodo, ContextoGenerador *contexto) {
    // Una asignación genera una instrucción MOV.
    if (nodo -> tipo == AST_ASIGNACION) {
        char resultado [TAM_OPERANDO];

        generar_expresion (nodo -> hijos [1], contexto, resultado);

        fprintf (contexto -> archivo, "MOV %s %s\n", resultado, nodo -> hijos [0] -> simbolo -> nombre);
    }
    // Un return genera una instrucción RETURN.
    else if (nodo -> tipo == AST_RETURN) {
        // Return sin expresión.
        if (nodo -> cantidad_hijos == 0) {
            fprintf (contexto -> archivo, "RETURN\n");
        }
        // Return con expresión.
        else {
            char resultado [TAM_OPERANDO];

            generar_expresion (nodo -> hijos [0], contexto, resultado);

            fprintf (contexto -> archivo, "RETURN %s\n", resultado);
        }
    }
}

/**
 * Genera el código de una expresión y almacena
 * el operando donde se encuentra su resultado.
 */
static void generar_expresion (NodoAST *nodo, ContextoGenerador *contexto, char *resultado) {
    switch (nodo -> tipo) {
        case AST_NUMERO:
            // Las constantes enteras se utilizan directamente como operandos.
            sprintf (resultado, "%d", nodo -> valor.numero);
            break;
        case AST_TRUE:
            // Las constantes booleanas se representan como TRUE.
            sprintf (resultado, "TRUE");
            break;
        case AST_FALSE:
            // Las constantes booleanas se representan como FALSE.
            sprintf (resultado, "FALSE");
            break;
        case AST_IDENTIFICADOR:
            // Para un identificador se utiliza el nombre asociado a su símbolo.
            sprintf (resultado, "%s", nodo -> simbolo -> nombre);
            break;
        case AST_SUMA: {
            // Una suma necesita generar un temporal para guardar su resultado.
            char izquierdo [TAM_OPERANDO];
            char derecho [TAM_OPERANDO];
            
            // Operando izquierdo.
            generar_expresion (nodo -> hijos [0], contexto, izquierdo);
            // Operando derecho.
            generar_expresion (nodo -> hijos [1], contexto, derecho);

            // Se asigna un nuevo temporal al resultado.
            sprintf (resultado, "T%d", contexto -> siguiente_temporal ++);
            // Una suma genera una instrucción SUM.
            fprintf (contexto -> archivo, "SUM %s %s %s\n", izquierdo, derecho, resultado);
            break;
        }
        case AST_MULTIPLICACION: {
            // Una multiplicación necesita generar un temporal para guardar su resultado.
            char izquierdo [TAM_OPERANDO];
            char derecho [TAM_OPERANDO];

            // Operando izquierdo.
            generar_expresion (nodo -> hijos [0], contexto, izquierdo);
            // Operando derecho.
            generar_expresion (nodo -> hijos [1], contexto, derecho);

            // Se asigna un nuevo temporal al resultado.
            sprintf (resultado, "T%d", contexto -> siguiente_temporal ++);
            // Una multiplicación genera una instrucción MUL.
            fprintf (contexto -> archivo, "MUL %s %s %s\n", izquierdo, derecho, resultado);
            break;
        }
        default:
            resultado [0] = '\0';
            break;
    }
}
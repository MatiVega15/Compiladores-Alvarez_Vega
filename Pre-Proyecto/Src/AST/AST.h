#ifndef AST_H
#define AST_H

#include "Tipos.h"
#include <stdio.h>

/**
 * Declaración adelantada de Simbolo.
 * 
 * El Árbol Sintáctico Abstracto (AST) mantiene referencias
 * a símbolos de la Tabla de Símbolos (TS), pero no necesita
 * conocer su definición completa.
 */
typedef struct Simbolo Simbolo;

/**
 * Representa los distintos tipos de nodos que pueden
 * formar parte del Árbol Sintáctico Abstracto (AST).
 */
typedef enum {
    AST_PROGRAMA,
    AST_DECLARACIONES,
    AST_SENTENCIAS,
    AST_DECLARACION,
    AST_ASIGNACION,
    AST_RETURN,
    AST_SUMA,
    AST_MULTIPLICACION,
    AST_NUMERO,
    AST_IDENTIFICADOR,
    AST_TRUE,
    AST_FALSE
} TipoNodo;

/**
 * Almacena el valor asociado a determinados tipos de nodos.
 * 
 * - Los nodos AST_NUMERO utilizan el campo 'numero'.
 * - Los nodos AST_IDENTIFICADOR utilizan el campo 'identificador'.
 * - Los demás tipos de nodos no necesitan almacenar un valor.
 */
typedef union {
    int numero;
    char *identificador;
} ValorNodo;

/**
 * Representa un nodo del Árbol Sintáctico Abstracto (AST).
 * 
 * Cada nodo posee:
 * 
 * - Un tipo que indica qué representa dentro del lenguaje.
 * - Un tipo de dato asociado, si lo tiene y si ya fue determinado.
 * - Un valor, cuando corresponde.
 * - Una cantidad variable de hijos.
 * - Un arreglo de punteros a sus nodos hijos.
 * - La línea del código fuente asociada al nodo.
 * - Una referencia al símbolo correspondiente, si existe.
 */
typedef struct NodoAST {
    TipoNodo tipo;
    TipoDato tipo_dato;
    ValorNodo valor;

    struct NodoAST **hijos;
    int cantidad_hijos;

    int linea;
    Simbolo *simbolo;
} NodoAST;

/**
 * Crea un nuevo nodo del Árbol Sintáctico Abstracto (AST).
 * 
 * Recibe el tipo de nodo que se desea crear y la línea del código
 * fuente asociada al nodo. Devuelve un puntero al nodo creado.
 * 
 * El nodo se crea inicialmente sin hijos y sin símbolo asociado.
 */
NodoAST *crear_nodo (TipoNodo tipo, int linea);

/**
 * Agrega un hijo al nodo padre.
 * 
 * La cantidad de hijos del nodo se incrementa automáticamente.
 */
void agregar_hijo (NodoAST *padre, NodoAST *hijo);

/**
 * Libera toda la memoria utilizada por el Árbol Sintáctico
 * Abstracto (AST) a partir de la raíz indicada.
 */
void liberar_arbol (NodoAST *raiz);

/**
 * Imprime el Árbol Sintáctico Abstracto (AST) por la salida
 * estándar con una representación jerárquica.
 */
void imprimir_arbol (const NodoAST *raiz);

/**
 * Genera una representación del Árbol Sintáctico Abstracto (AST) en
 * formato DOT, el cual puede ser utilizado con Graphviz para generar
 * una representación gráfica del árbol.
 * 
 * El parámetro 'archivo' debe ser un archivo abierto en modo escritura.
 */
void generar_dot (const NodoAST *raiz, FILE *archivo);

#endif
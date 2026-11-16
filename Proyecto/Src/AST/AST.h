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
 * aparecer en el Árbol Sintáctico Abstracto (AST).
 */
typedef enum {
    /* Programas y declaraciones. */
    AST_PROGRAMA,
    AST_DECLARACION_VARIABLE,
    AST_DECLARACION_FUNCION,
    AST_DECLARACION_FUNCION_EXTERNA,
    AST_PARAMETRO,

    /* Sentencias y bloques. */
    AST_BLOQUE,
    AST_ASIGNACION,
    AST_LLAMADA,
    AST_IF,
    AST_WHILE,
    AST_RETURN,
    AST_SENTENCIA_VACIA,

    /* Operadores binarios. */
    AST_SUMA,
    AST_RESTA,
    AST_MULTIPLICACION,
    AST_DIVISION,
    AST_MODULO,
    AST_MENOR,
    AST_MAYOR,
    AST_IGUAL,
    AST_AND,
    AST_OR,

    /* Operadores unarios. */
    AST_NEGACION,
    AST_MENOS_UNARIO,

    /* Valores y referencias. */
    AST_IDENTIFICADOR,
    AST_NUMERO,
    AST_REAL,
    AST_TRUE,
    AST_FALSE
} TipoNodo;

/**
 * Representa el valor asociado a determinados tipos de nodos del AST.
 * 
 * - Los nodos AST_NUMERO utilizan el campo 'numero'.
 * - Los nodos AST_IDENTIFICADOR utilizan el campo 'identificador'.
 * - Los nodos AST_REAL utilizan el campo 'real'.
 * - Los nodos AST_LLAMADA utilizan el campo 'identificador'.
 * - Los demás tipos de nodos no necesitan almacenar un valor.
 */
typedef union {
    int numero;
    double real;
    char *identificador;
} ValorNodo;

/**
 * Representa un nodo del Árbol Sintáctico Abstracto (AST).
 * 
 * El AST utiliza una representación n-aria para las construcciones
 * que pueden contener una cantidad variable de elementos, como
 * programas, bloques, parámetros y argumentos.
 * 
 * Convención posicional de hijos según el tipo de nodo:
 * 
 * - AST_PROGRAMA: [0..n-1] declaraciones globales (variables, extern, funciones).
 * - AST_DECLARACION_VARIABLE: [0..n-1] identificadores declarados.
 * - AST_DECLARACION_FUNCION: [0] identificador, [1..n-2] parámetros, [n-1] bloque.
 * - AST_DECLARACION_FUNCION_EXTERNA: [0] identificador, [1..n-1] parámetros (sin bloque).
 * - AST_PARAMETRO: [0] identificador del parámetro.
 * - AST_BLOQUE: [0..k-1] declaraciones locales, [k..n-1] sentencias.
 * - AST_ASIGNACION: [0] identificador destino, [1] expresión.
 * - AST_LLAMADA: [0..n-1] expresiones de los argumentos.
 * - AST_IF: [0] condición, [1] bloque then, [2] bloque else (opcional).
 * - AST_WHILE: [0] condición, [1] bloque cuerpo.
 * - AST_RETURN: [0] expresión de retorno (opcional).
 * - Operaciones binarias: [0] operando izquierdo, [1] operando derecho.
 * - Operaciones unarias: [0] operando único.
 * - Nodos hoja (0 hijos): AST_SENTENCIA_VACIA, AST_IDENTIFICADOR, AST_NUMERO,
 *   AST_REAL, AST_TRUE, AST_FALSE.
 * 
 * Cada nodo posee:
 * 
 * - Un tipo de nodo que indica qué representa dentro del lenguaje.
 * - Un tipo de dato asociado, si lo tiene y si ya fue determinado.
 * - Un valor, cuando corresponde.
 * - Una cantidad variable de hijos.
 * - Un arreglo de punteros a sus nodos hijos.
 * - Una capacidad dinámica para el arreglo.
 * - La línea y la columna del código fuente asociada al nodo.
 * - Una referencia al símbolo correspondiente, si existe.
 */
typedef struct NodoAST {
    TipoNodo tipo;
    TipoDato tipo_dato;
    ValorNodo valor;

    struct NodoAST **hijos;
    int cantidad_hijos;
    int capacidad_hijos;

    int linea;
    int columna;

    Simbolo *simbolo;
} NodoAST;

/**
 * Crea un nuevo nodo del Árbol Sintáctico Abstracto (AST).
 * 
 * Recibe el tipo de nodo que se desea crear y la ubicación en el
 * código fuente asociada al nodo. Devuelve un puntero al nodo creado.
 * 
 * El nodo se crea inicialmente sin hijos y con su tipo de dato
 * establecido como TIPO_NO_DEFINIDO.
 */
NodoAST *crear_nodo (TipoNodo tipo, int linea, int columna);

/**
 * Agrega un hijo al final del arreglo de hijos del nodo padre.
 * 
 * La capacidad del arreglo se incrementa geométricamente
 * cuando sea necesario.
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
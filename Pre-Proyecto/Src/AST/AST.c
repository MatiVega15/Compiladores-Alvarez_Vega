#include "AST.h"
#include <stdlib.h>

/* Funciones auxiliares privadas. */
static void imprimir_nodo (const NodoAST *nodo, int nivel);
static const char *nombre_tipo_nodo (TipoNodo tipo);
static const char *nombre_tipo_dato (TipoDato tipo);
static int generar_dot_nodo (const NodoAST *nodo, FILE *archivo, int *contador);
static void generar_etiqueta_nodo (const NodoAST *nodo, FILE *archivo, int identificador);

NodoAST *crear_nodo (TipoNodo tipo) {
    // Se reserva memoria para el AST.
    NodoAST *nodo = malloc (sizeof (NodoAST));
    
    if (nodo == NULL) {
        fprintf (stderr, "Error: no se pudo reservar memoria para un nodo.\n");
        exit (EXIT_FAILURE);
    }

    nodo -> tipo = tipo;

    // El tipo de dato se determina posteriormente.
    nodo -> tipo_dato = TIPO_NO_DEFINIDO;

    // Inicialmente el nodo no tiene hijos.
    nodo -> hijos = NULL;
    nodo -> cantidad_hijos = 0;

    return nodo;
}

void agregar_hijo (NodoAST *padre, NodoAST *hijo) {
    // Se amplía el arreglo para almacenar un hijo adicional.
    NodoAST **nuevos_hijos = realloc (padre -> hijos, (padre -> cantidad_hijos + 1) * sizeof (NodoAST *));
    
    if (nuevos_hijos == NULL) {
        fprintf (stderr, "Error: no se pudo reservar memoria para agregar un hijo.\n");
        exit (EXIT_FAILURE);
    }

    padre -> hijos = nuevos_hijos;

    // El nuevo hijo se agrega al final del arreglo.
    padre -> hijos [padre -> cantidad_hijos] = hijo;

    // Se actualiza la cantidad de hijos.
    padre -> cantidad_hijos ++;
}

void liberar_arbol (NodoAST *raiz) {
    // Caso base.
    if (raiz == NULL) {
        return;
    }

    // Se liberan recursivamente todos los hijos.
    for (int i = 0; i < raiz -> cantidad_hijos; i ++) {
        liberar_arbol (raiz -> hijos [i]);
    }

    // Se libera el arreglo de hijos.
    free (raiz -> hijos);

    // Se libera el nodo.
    free (raiz);

}

void imprimir_arbol (const NodoAST *raiz) {
    imprimir_nodo (raiz, 0);
}

void generar_dot (const NodoAST *raiz, FILE *archivo) {
    int contador = 0;

    // Se inicia el grafo dirigido.
    fprintf (archivo, "digraph AST {\n");

    generar_dot_nodo (raiz, archivo, &contador);
    
    // Se finaliza el grafo.
    fprintf (archivo, "}\n");
}

/* =========== Funciones auxiliares privadas =========== */

/**
 * Imprime recursivamente un nodo del árbol y todos sus descendientes.
 * 
 * La indentación permite representar visualmente la jerarquía del AST.
 */
static void imprimir_nodo (const NodoAST *nodo, int nivel) {
    if (nodo == NULL) {
        return;
    }

    // Se imprime una indentación correspondiente al nivel del nodo.
    for (int i = 0; i < nivel; i ++) {
        printf ("   ");
    }

    // Se imprime el tipo de nodo.
    printf ("%s", nombre_tipo_nodo (nodo -> tipo));

    // Se imprime el valor cuando corresponde.
    switch (nodo -> tipo) {
        case AST_NUMERO:
            printf (": %d", nodo -> valor.numero);
            break;
        case AST_IDENTIFICADOR:
            printf (": %s", nodo -> valor.identificador);
            break;
        default:
            break;
    }

    // Se imprime el tipo de dato si fue determinado.
    if (nodo -> tipo_dato != TIPO_NO_DEFINIDO) {
        printf (" [%s]", nombre_tipo_dato (nodo -> tipo_dato));
    }

    printf ("\n");

    // Se imprimen recursivamente los hijos.
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        imprimir_nodo (nodo -> hijos [i], nivel + 1);
    }
}

/**
 * Devuelve una representación textual del tipo de nodo recibido.
 */
static const char *nombre_tipo_nodo (TipoNodo tipo) {
    // Se determina el tipo del nodo.
    switch (tipo) {
        case AST_PROGRAMA:
            return "PROGRAMA";
        case AST_DECLARACIONES:
            return "DECLARACIONES";
        case AST_SENTENCIAS:
            return "SENTENCIAS";
        case AST_DECLARACION:
            return "DECLARACIÓN";
        case AST_ASIGNACION:
            return "=";
        case AST_RETURN:
            return "RETURN";
        case AST_SUMA:
            return "+";
        case AST_MULTIPLICACION:
            return "*";
        case AST_NUMERO:
            return "NUMERO";
        case AST_IDENTIFICADOR:
            return "IDENTIFICADOR";
        case AST_TRUE:
            return "TRUE";
        case AST_FALSE:
            return "FALSE";
        default:
            return "DESCONOCIDO";
    }
}

/**
 * Devuelve una representación textual del tipo de dato recibido.
 */
static const char *nombre_tipo_dato (TipoDato tipo) {
    // Se determina el tipo del dato.
    switch (tipo) {
    case TIPO_INT:
        return "INT";
    case TIPO_BOOL:
        return "BOOL";
    case TIPO_VOID:
        return "VOID";
    case TIPO_NO_DEFINIDO:
        return "NO_DEFINIDO";
    default:
        return "DESCONOCIDO";
    }
}

/**
 * Genera recursivamente la representación DOT de un nodo y sus descendientes.
 * 
 * A cada nodo se le asigna un identificador numérico único.
 * Se generan las relaciones entre cada nodo padre y sus hijos.
 */
static int generar_dot_nodo (const NodoAST *nodo, FILE *archivo, int *contador) {
    if (nodo == NULL) {
        return -1;
    }

    // Se asigna un identificador único al nodo actual.
    int identificador_padre = (*contador) ++;
    generar_etiqueta_nodo (nodo, archivo, identificador_padre);

    // Se procesan recursivamente todos los hijos.
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        int identificador_hijo = generar_dot_nodo (nodo -> hijos [i], archivo, contador);

        // Se genera la relación entre el padre y el hijo.
        fprintf (archivo, "    nodo%d -> nodo%d;\n", identificador_padre, identificador_hijo);
    }

    return identificador_padre;
}

/**
 * Genera la etiqueta DOT correspondiente a un nodo.
 * 
 * La etiqueta contiene el nombre del tipo de nodo, su valor
 * cuando corresponde y su tipo de dato si está definido.
 */
static void generar_etiqueta_nodo (const NodoAST *nodo, FILE *archivo, int identificador) {
    fprintf (archivo, "    nodo%d [label=\"%s", identificador, nombre_tipo_nodo (nodo -> tipo));

    // Se agrega el valor cuando corresponde.
    switch (nodo -> tipo) {
        case AST_NUMERO:
            fprintf (archivo, ": %d", nodo -> valor.numero);
            break;
        case AST_IDENTIFICADOR:
            fprintf (archivo, ": %s", nodo -> valor.identificador);
            break;
        default:
            break;
    }

    // Se agrega el tipo de dato cuando está definido.
    if (nodo -> tipo_dato != TIPO_NO_DEFINIDO) {
        fprintf (archivo, " [%s]", nombre_tipo_dato (nodo -> tipo_dato));
    }

    fprintf (archivo, "\"];\n");
}
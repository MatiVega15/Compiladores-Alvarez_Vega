#include "AST.h"

#include <stdio.h>
#include <stdlib.h>

/* Funciones auxiliares privadas. */
static void imprimir_nodo (const NodoAST *nodo, int nivel);
static const char *nombre_tipo_nodo (TipoNodo tipo);
static const char *nombre_tipo_dato (TipoDato tipo);
static int generar_dot_nodo (const NodoAST *nodo, FILE *archivo, int *contador);
static void generar_etiqueta_nodo (const NodoAST *nodo, FILE *archivo, int identificador);

NodoAST *crear_nodo (TipoNodo tipo, int linea, int columna) {
    // Se reserva memoria para el AST.
    NodoAST *nodo = malloc (sizeof (NodoAST));
    
    if (nodo == NULL) {
        fprintf (stderr, "Error: no se pudo reservar memoria para un nodo del AST.\n");
        exit (EXIT_FAILURE);
    }

    nodo -> tipo = tipo;

    // El tipo de dato se determina posteriormente.
    nodo -> tipo_dato = TIPO_NO_DEFINIDO;
    nodo -> valor.identificador = NULL;

    // Inicialmente el nodo no tiene hijos.
    nodo -> hijos = NULL;
    nodo -> cantidad_hijos = 0;
    nodo -> capacidad_hijos = 0;

    nodo -> linea = linea;
    nodo -> columna = columna;

    return nodo;
}

void agregar_hijo (NodoAST *padre, NodoAST *hijo) {
    if (padre == NULL || hijo == NULL) {
        return;
    }
    
    // Si no queda espacio disponible, se amplía la capacidad del arreglo.
    if (padre -> cantidad_hijos == padre -> capacidad_hijos) {
        int nueva_capacidad;

        if (padre -> capacidad_hijos == 0) {
            nueva_capacidad = 2;
        }
        else {
            nueva_capacidad = padre -> capacidad_hijos * 2;
        }

        NodoAST **nuevos_hijos = realloc (padre -> hijos, nueva_capacidad * sizeof (NodoAST *));
    
        if (nuevos_hijos == NULL) {
            fprintf (stderr, "Error: no se pudo ampliar el arreglo de hijos del AST.\n");
            exit (EXIT_FAILURE);
        }

        padre -> hijos = nuevos_hijos;
        padre -> capacidad_hijos = nueva_capacidad;
    }
    
    // El nuevo hijo se agrega en la primera posición disponible.
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

    // Los nodos que almacenan un identificador liberan su cadena.
    if (raiz -> tipo == AST_IDENTIFICADOR || raiz -> tipo == AST_LLAMADA) {
        free (raiz -> valor.identificador);
    }

    // Se libera el arreglo de hijos.
    free (raiz -> hijos);

    // Se libera el nodo.
    free (raiz);

}

void imprimir_arbol (const NodoAST *raiz) {
    if (raiz == NULL) {
        return;
    }
    
    imprimir_nodo (raiz, 0);
}

void generar_dot (const NodoAST *raiz, FILE *archivo) {
    if (archivo == NULL) {
        return;
    }
    
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
        case AST_REAL:
            printf (": %g", nodo -> valor.real);
            break;
        case AST_IDENTIFICADOR:
            printf (": %s", nodo -> valor.identificador);
            break;
        case AST_LLAMADA:
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
        /* Programas y declaraciones. */
        case AST_PROGRAMA:
            return "PROGRAMA";
        case AST_DECLARACION_VARIABLE:
            return "DECLARACION VARIABLE";
        case AST_DECLARACION_FUNCION:
            return "DECLARACION FUNCIÓN";
        case AST_PARAMETRO:
            return "PARÁMETRO";
        
        /* Sentencias y bloques. */
        case AST_BLOQUE:
            return "BLOQUE";
        case AST_ASIGNACION:
            return "=";
        case AST_LLAMADA:
            return "LLAMADA";
        case AST_IF:
            return "IF";
        case AST_WHILE:
            return "WHILE";   
        case AST_RETURN:
            return "RETURN";
        case AST_SENTENCIA_VACIA:
            return "SENTENCIA VACÍA";

        /* Operadores binarios. */
        case AST_SUMA:
            return "+";
        case AST_RESTA:
            return "-";
        case AST_MULTIPLICACION:
            return "*";
        case AST_DIVISION:
            return "/";
        case AST_MODULO:
            return "%";
        case AST_MENOR:
            return "<";
        case AST_MAYOR:
            return ">";
        case AST_IGUAL:
            return "==";
        case AST_AND:
            return "&&";
        case AST_OR:
            return "||";

        /* Operadores unarios. */
        case AST_NEGACION:
            return "!";
        case AST_MENOS_UNARIO:
            return "- (UNARIO)";

        /* Valores y referencias. */
        case AST_IDENTIFICADOR:
            return "IDENTIFICADOR";
        case AST_NUMERO:
            return "NUMERO";
        case AST_REAL:
            return "REAL";
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
    case TIPO_BOOLEAN:
        return "BOOLEAN";
    case TIPO_FLOAT:
        return "FLOAT";
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
    if (nodo == NULL || archivo == NULL) {
        return;
    }
    
    fprintf (archivo, "    nodo%d [label=\"%s", identificador, nombre_tipo_nodo (nodo -> tipo));

    // Se agrega el valor cuando corresponde.
    switch (nodo -> tipo) {
        case AST_NUMERO:
            fprintf (archivo, ": %d", nodo -> valor.numero);
            break;
        case AST_REAL:
            fprintf (archivo, ": %g", nodo -> valor.real);
            break;
        case AST_IDENTIFICADOR:
            fprintf (archivo, ": %s", nodo -> valor.identificador);
            break;
        case AST_LLAMADA:
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
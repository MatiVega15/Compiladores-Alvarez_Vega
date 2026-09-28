#include "AST.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Función auxiliar.
static char *copiar_cadena (const char *cadena);

/**
 * Programa que se representa:
 * 
 *  boolean main () {
 *      boolean resultado;
 *      
 *      resultado = true && false || !true;
 *  }
 */
int main (void) {
    
    /**
     * =========
     * PROGRAMA 
     * =========
     */
    NodoAST *programa = crear_nodo (AST_PROGRAMA, 1, 1);

    /**
     * ======================
     * DECLARACIÓN DE FUNCIÓN
     * ======================
     */
    NodoAST *funcion = crear_nodo (AST_DECLARACION_FUNCION, 1, 1);
    funcion -> tipo_dato = TIPO_BOOLEAN;

    NodoAST *nombre_funcion = crear_nodo (AST_IDENTIFICADOR, 1, 9);
    nombre_funcion -> valor.identificador = copiar_cadena ("main");

    agregar_hijo (funcion, nombre_funcion);

    /**
     * ======
     * BLOQUE
     * ======
     */
    NodoAST *bloque = crear_nodo (AST_BLOQUE, 1, 17);
    
    agregar_hijo (funcion, bloque);
    agregar_hijo (programa, funcion);

    /**
     * =======================
     * DECLARACIÓN DE VARIABLE
     * =======================
     */
    NodoAST *declaracion = crear_nodo (AST_DECLARACION_VARIABLE, 2, 5);
    declaracion -> tipo_dato = TIPO_BOOLEAN;
    
    NodoAST *resultado = crear_nodo (AST_IDENTIFICADOR, 2, 13);
    resultado -> valor.identificador = copiar_cadena ("resultado");

    agregar_hijo (declaracion, resultado);
    agregar_hijo (bloque, declaracion);

    /**
     * ==========
     * ASIGNACIÓN
     * ==========
     */
    NodoAST *asignacion = crear_nodo (AST_ASIGNACION, 4, 5);
    
    NodoAST *destino = crear_nodo (AST_IDENTIFICADOR, 4, 5);
    destino -> valor.identificador = copiar_cadena ("resultado");

    agregar_hijo (asignacion, destino);

    /**
     * =========
     * EXPRESIÓN
     * =========
     */
    NodoAST *or = crear_nodo (AST_OR, 4, 31);

    NodoAST *and = crear_nodo (AST_AND, 4, 22);
    
    NodoAST *true_1 = crear_nodo (AST_TRUE, 4, 17);
    NodoAST *false_1 = crear_nodo (AST_FALSE, 4, 25);

    agregar_hijo (and, true_1);
    agregar_hijo (and, false_1);

    NodoAST *negacion = crear_nodo (AST_NEGACION, 4, 34);
    NodoAST *true_2 = crear_nodo (AST_TRUE, 4, 35);

    agregar_hijo (negacion, true_2);    

    agregar_hijo (or, and);
    agregar_hijo (or, negacion);

    agregar_hijo (asignacion, or);
    agregar_hijo (bloque, asignacion);

    /**
     * ======
     * PRUEBA 
     * ======
     */
    printf ("AST del programa: \n\n");
    imprimir_arbol (programa);

    // Se genera la representación DOT del AST.
    FILE *archivo = fopen ("Src/Test/Resultados/AST/Prueba04_AST_ExpresionesBooleanas.dot", "w");

    if (archivo == NULL) {
        perror ("Error al crear el archivo DOT");
        liberar_arbol (programa);
        return 1;
    }

    generar_dot (programa, archivo);

    fclose (archivo);

    printf ("\nRepresentación DOT generada en 'Src/Test/Resultados/AST/Prueba04_AST_ExpresionesBooleanas.dot'.");

    /**
     * ==========
     * LIBERACIÓN 
     * ==========
     */
    liberar_arbol (programa);

    return 0;
}

/**
 * Copia una cadena reservando memoria dinámica.
 */
static char *copiar_cadena (const char *cadena) {
    char *copia = malloc (strlen (cadena) + 1);

    if (copia == NULL) {
        perror ("Error al copiar la cadena");
        exit (EXIT_FAILURE);
    }

    strcpy (copia, cadena);

    return copia;
}
#include "AST.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Función auxiliar.
static char *copiar_cadena (const char *cadena);

/**
 * Programa que se representa:
 * 
 *  int main () {
 *
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
    funcion -> tipo_dato = TIPO_INT;

    NodoAST *nombre_funcion = crear_nodo (AST_IDENTIFICADOR, 1, 5);
    nombre_funcion -> valor.identificador = copiar_cadena ("main");

    agregar_hijo (funcion, nombre_funcion);

    /**
     * ======
     * BLOQUE
     * ======
     */
    NodoAST *bloque = crear_nodo (AST_BLOQUE, 1, 13);

    agregar_hijo (funcion, bloque);
    agregar_hijo (programa, funcion);

    /**
     * ======
     * PRUEBA 
     * ======
     */
    printf ("AST del programa: \n\n");
    imprimir_arbol (programa);

    // Se genera la representación DOT del AST.
    FILE *archivo = fopen ("Src/Test/Resultados/AST/Prueba01_AST_ProgramaVacio.dot", "w");

    if (archivo == NULL) {
        perror ("Error al crear el archivo DOT");
        liberar_arbol (programa);
        return 1;
    }

    generar_dot (programa, archivo);

    fclose (archivo);

    printf ("\nRepresentación DOT generada en 'Src/Test/Resultados/AST/Prueba01_AST_ProgramaVacio.dot'.");

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
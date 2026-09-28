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
 *      int x;
 *      boolean bandera;
 *      float precio;
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
     * =========================
     * DECLARACIÓN DE VARIABLE x
     * =========================
     */
    NodoAST *declaracion_x = crear_nodo (AST_DECLARACION_VARIABLE, 2, 5);
    declaracion_x -> tipo_dato = TIPO_INT;
    
    NodoAST *x = crear_nodo (AST_IDENTIFICADOR, 2, 9);
    x -> valor.identificador = copiar_cadena ("x");

    agregar_hijo (declaracion_x, x);
    agregar_hijo (bloque, declaracion_x);

    /**
     * ===============================
     * DECLARACIÓN DE VARIABLE bandera
     * ===============================
     */
    NodoAST *declaracion_bandera = crear_nodo (AST_DECLARACION_VARIABLE, 3, 5);
    declaracion_bandera -> tipo_dato = TIPO_BOOLEAN;
    
    NodoAST *bandera = crear_nodo (AST_IDENTIFICADOR, 3, 13);
    bandera -> valor.identificador = copiar_cadena ("bandera");

    agregar_hijo (declaracion_bandera, bandera);
    agregar_hijo (bloque, declaracion_bandera);

    /**
     * ==============================
     * DECLARACIÓN DE VARIABLE precio
     * ==============================
     */
    NodoAST *declaracion_precio = crear_nodo (AST_DECLARACION_VARIABLE, 4, 5);
    declaracion_precio -> tipo_dato = TIPO_FLOAT;
    
    NodoAST *precio = crear_nodo (AST_IDENTIFICADOR, 4, 11);
    precio -> valor.identificador = copiar_cadena ("precio");

    agregar_hijo (declaracion_precio, precio);
    agregar_hijo (bloque, declaracion_precio);

    /**
     * ======
     * PRUEBA 
     * ======
     */
    printf ("AST del programa: \n\n");
    imprimir_arbol (programa);

    // Se genera la representación DOT del AST.
    FILE *archivo = fopen ("Src/Test/Resultados/AST/Prueba02_AST_DeclaracionesVariables.dot", "w");

    if (archivo == NULL) {
        perror ("Error al crear el archivo DOT");
        liberar_arbol (programa);
        return 1;
    }

    generar_dot (programa, archivo);

    fclose (archivo);

    printf ("\nRepresentación DOT generada en 'Src/Test/Resultados/AST/Prueba02_AST_DeclaracionesVariables.dot'.");

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
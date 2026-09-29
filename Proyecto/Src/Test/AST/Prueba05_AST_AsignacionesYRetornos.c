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
 *      
 *      x = 10;
 *      x = x + 5;
 *      return x;
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
     * =======================
     * DECLARACIÓN DE VARIABLE
     * =======================
     */
    NodoAST *declaracion = crear_nodo (AST_DECLARACION_VARIABLE, 2, 5);
    declaracion -> tipo_dato = TIPO_INT;
    
    NodoAST *x = crear_nodo (AST_IDENTIFICADOR, 2, 9);
    x -> valor.identificador = copiar_cadena ("x");

    agregar_hijo (declaracion, x);
    agregar_hijo (bloque, declaracion);

    /**
     * ==========
     * ASIGNACIÓN
     * ==========
     */
    NodoAST *asignacion_1 = crear_nodo (AST_ASIGNACION, 4, 5);
    
    NodoAST *destino_1 = crear_nodo (AST_IDENTIFICADOR, 4, 5);
    destino_1 -> valor.identificador = copiar_cadena ("x");

    NodoAST *numero_10 = crear_nodo (AST_NUMERO, 4, 9);
    numero_10 -> valor.numero = 10;

    agregar_hijo (asignacion_1, destino_1);
    agregar_hijo (asignacion_1, numero_10);
    agregar_hijo (bloque, asignacion_1);

    /**
     * ==========
     * ASIGNACIÓN
     * ==========
     */
    NodoAST *asignacion_2 = crear_nodo (AST_ASIGNACION, 5, 5);
    
    NodoAST *destino_2 = crear_nodo (AST_IDENTIFICADOR, 5, 5);
    destino_2 -> valor.identificador = copiar_cadena ("x");

    NodoAST *suma = crear_nodo (AST_SUMA, 5, 11);

    NodoAST *x_expresion = crear_nodo (AST_IDENTIFICADOR, 5, 9);
    x_expresion -> valor.identificador = copiar_cadena ("x");

    NodoAST *numero_5 = crear_nodo (AST_NUMERO, 5, 13);
    numero_5 -> valor.numero = 5;

    agregar_hijo (suma, x_expresion);
    agregar_hijo (suma, numero_5);

    agregar_hijo (asignacion_2, destino_2);
    agregar_hijo (asignacion_2, suma);
    agregar_hijo (bloque, asignacion_2);

    /**
     * =======
     * RETORNO
     * =======
     */
    NodoAST *retorno = crear_nodo (AST_RETURN, 6, 5);
    
    NodoAST *x_retorno = crear_nodo (AST_IDENTIFICADOR, 6, 12);
    x_retorno -> valor.identificador = copiar_cadena ("x");

    agregar_hijo (retorno, x_retorno);
    agregar_hijo (bloque, retorno);

    /**
     * ======
     * PRUEBA 
     * ======
     */
    printf ("AST del programa: \n\n");
    imprimir_arbol (programa);

    // Se genera la representación DOT del AST.
    FILE *archivo = fopen ("Src/Test/Resultados/AST/Prueba05_AST_AsignacionesYRetornos.dot", "w");

    if (archivo == NULL) {
        perror ("Error al crear el archivo DOT");
        liberar_arbol (programa);
        return 1;
    }

    generar_dot (programa, archivo);

    fclose (archivo);

    printf ("\nRepresentación DOT generada en 'Src/Test/Resultados/AST/Prueba05_AST_AsignacionesYRetornos.dot'.");

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
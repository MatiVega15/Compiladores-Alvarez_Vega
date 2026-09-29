#include "AST.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Función auxiliar.
static char *copiar_cadena (const char *cadena);

/**
 * Programa que se representa:
 * 
 *  void main () {
 *      int x;
 *      
 *      if (x > 0) {
 *          x = x - 1;
 *      } else {
 *          x = x + 1;
 *      }
 * 
 *      while (x < 10) {
 *          x = x + 1;
 *      }
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
    funcion -> tipo_dato = TIPO_VOID;

    NodoAST *nombre_funcion = crear_nodo (AST_IDENTIFICADOR, 1, 6);
    nombre_funcion -> valor.identificador = copiar_cadena ("main");

    agregar_hijo (funcion, nombre_funcion);

    /**
     * ======
     * BLOQUE
     * ======
     */
    NodoAST *bloque = crear_nodo (AST_BLOQUE, 1, 14);
    
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
     * ============
     * SENTENCIA IF
     * ============
     */
    NodoAST *sentencia_if = crear_nodo (AST_IF, 4, 5);
    
    /**
     * ===============
     * CONDICIÓN X > 0
     * ===============
     */
    NodoAST *mayor = crear_nodo (AST_MAYOR, 4, 11);

    NodoAST *x_condicion_if = crear_nodo (AST_IDENTIFICADOR, 4, 9);
    x_condicion_if -> valor.identificador = copiar_cadena ("x");

    NodoAST *numero_0 = crear_nodo (AST_NUMERO, 4, 13);
    numero_0 -> valor.numero = 0;

    agregar_hijo (mayor, x_condicion_if);
    agregar_hijo (mayor, numero_0);
    agregar_hijo (sentencia_if, mayor);

    /**
     * =========
     * BLOQUE IF
     * =========
     */
    NodoAST *bloque_if = crear_nodo (AST_BLOQUE, 4, 16);

    NodoAST *asignacion_if = crear_nodo (AST_ASIGNACION, 5, 9);

    NodoAST *destino_if = crear_nodo (AST_IDENTIFICADOR, 5, 9);
    destino_if -> valor.identificador = copiar_cadena ("x");

    NodoAST *resta = crear_nodo (AST_RESTA, 5, 15);

    NodoAST *x_resta = crear_nodo (AST_IDENTIFICADOR, 5, 13);
    x_resta -> valor.identificador = copiar_cadena ("x");

    NodoAST *numero_1 = crear_nodo (AST_NUMERO, 5, 17);
    numero_1 -> valor.numero = 1;

    agregar_hijo (resta, x_resta);
    agregar_hijo (resta, numero_1);

    agregar_hijo (asignacion_if, destino_if);
    agregar_hijo (asignacion_if, resta);
    agregar_hijo (bloque_if, asignacion_if);
    agregar_hijo (sentencia_if, bloque_if);

    /**
     * ===========
     * BLOQUE ELSE
     * ===========
     */
    NodoAST *bloque_else = crear_nodo (AST_BLOQUE, 6, 12);

    NodoAST *asignacion_else = crear_nodo (AST_ASIGNACION, 7, 9);

    NodoAST *destino_else = crear_nodo (AST_IDENTIFICADOR, 7, 9);
    destino_else -> valor.identificador = copiar_cadena ("x");

    NodoAST *suma_else = crear_nodo (AST_SUMA, 7, 15);

    NodoAST *x_suma_else = crear_nodo (AST_IDENTIFICADOR, 7, 13);
    x_suma_else -> valor.identificador = copiar_cadena ("x");

    NodoAST *numero_1_else = crear_nodo (AST_NUMERO, 7, 17);
    numero_1_else -> valor.numero = 1;

    agregar_hijo (suma_else, x_suma_else);
    agregar_hijo (suma_else, numero_1_else);

    agregar_hijo (asignacion_else, destino_else);
    agregar_hijo (asignacion_else, suma_else);
    agregar_hijo (bloque_else, asignacion_else);
    agregar_hijo (sentencia_if, bloque_else);

    agregar_hijo (bloque, sentencia_if);

    /**
     * ===============
     * SENTENCIA WHILE
     * ===============
     */
    NodoAST *sentencia_while = crear_nodo (AST_WHILE, 10, 5);
    
    /**
     * ================
     * CONDICIÓN X < 10
     * ================
     */
    NodoAST *menor = crear_nodo (AST_MENOR, 10, 14);

    NodoAST *x_condicion_while = crear_nodo (AST_IDENTIFICADOR, 10, 12);
    x_condicion_while -> valor.identificador = copiar_cadena ("x");

    NodoAST *numero_10 = crear_nodo (AST_NUMERO, 10, 16);
    numero_10 -> valor.numero = 10;

    agregar_hijo (menor, x_condicion_while);
    agregar_hijo (menor, numero_10);
    agregar_hijo (sentencia_while, menor);

    /**
     * ============
     * BLOQUE WHILE
     * ============
     */
    NodoAST *bloque_while = crear_nodo (AST_BLOQUE, 10, 20);

    NodoAST *asignacion_while = crear_nodo (AST_ASIGNACION, 11, 9);

    NodoAST *destino_while = crear_nodo (AST_IDENTIFICADOR, 11, 9);
    destino_while -> valor.identificador = copiar_cadena ("x");

    NodoAST *suma_while = crear_nodo (AST_SUMA, 11, 15);

    NodoAST *x_suma_while = crear_nodo (AST_IDENTIFICADOR, 11, 13);
    x_suma_while -> valor.identificador = copiar_cadena ("x");

    NodoAST *numero_1_while = crear_nodo (AST_NUMERO, 11, 17);
    numero_1_while -> valor.numero = 1;

    agregar_hijo (suma_while, x_suma_while);
    agregar_hijo (suma_while, numero_1_while);

    agregar_hijo (asignacion_while, destino_while);
    agregar_hijo (asignacion_while, suma_while);
    agregar_hijo (bloque_while, asignacion_while);
    agregar_hijo (sentencia_while, bloque_while);

    agregar_hijo (bloque, sentencia_while);

    /**
     * ======
     * PRUEBA 
     * ======
     */
    printf ("AST del programa: \n\n");
    imprimir_arbol (programa);

    // Se genera la representación DOT del AST.
    FILE *archivo = fopen ("Src/Test/Resultados/AST/Prueba06_AST_BloquesYControl.dot", "w");

    if (archivo == NULL) {
        perror ("Error al crear el archivo DOT");
        liberar_arbol (programa);
        return 1;
    }

    generar_dot (programa, archivo);

    fclose (archivo);

    printf ("\nRepresentación DOT generada en 'Src/Test/Resultados/AST/Prueba06_AST_BloquesYControl.dot'.");

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
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
 *      int resultado;
 *      
 *      resultado = 10 + 5 * 2 - 8 / 4 % 3;
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
    
    NodoAST *resultado = crear_nodo (AST_IDENTIFICADOR, 2, 9);
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
     * ====================
     * EXPRESIÓN 10 + 5 * 2
     * ====================
     */
    NodoAST *suma = crear_nodo (AST_SUMA, 4, 20);
    
    NodoAST *numero_10 = crear_nodo (AST_NUMERO, 4, 17);
    numero_10 -> valor.numero = 10;

    NodoAST *multiplicacion = crear_nodo (AST_MULTIPLICACION, 4, 24);
    
    NodoAST *numero_5 = crear_nodo (AST_NUMERO, 4, 22);
    numero_5 -> valor.numero = 5;

    NodoAST *numero_2 = crear_nodo (AST_NUMERO, 4, 26);
    numero_2 -> valor.numero = 2;

    agregar_hijo (multiplicacion, numero_5);
    agregar_hijo (multiplicacion, numero_2);

    agregar_hijo (suma, numero_10);
    agregar_hijo (suma, multiplicacion);

    /**
     * ===================
     * EXPRESIÓN 8 / 4 % 3
     * ===================
     */
    NodoAST *division = crear_nodo (AST_DIVISION, 4, 32);
    
    NodoAST *numero_8 = crear_nodo (AST_NUMERO, 4, 30);
    numero_8 -> valor.numero = 8;

    NodoAST *numero_4 = crear_nodo (AST_NUMERO, 4, 34);
    numero_4 -> valor.numero = 4;

    NodoAST *modulo = crear_nodo (AST_MODULO, 4, 36);

    NodoAST *numero_3 = crear_nodo (AST_NUMERO, 4, 38);
    numero_3 -> valor.numero = 3;

    agregar_hijo (division, numero_8);
    agregar_hijo (division, numero_4);

    agregar_hijo (modulo, division);
    agregar_hijo (modulo, numero_3);

    /**
     * ==================
     * EXPRESIÓN completa
     * ==================
     */
    NodoAST *resta = crear_nodo (AST_RESTA, 4, 28);

    agregar_hijo (resta, suma);
    agregar_hijo (resta, modulo);

    agregar_hijo (asignacion, resta);
    agregar_hijo (bloque, asignacion);

    /**
     * ======
     * PRUEBA 
     * ======
     */
    printf ("AST del programa: \n\n");
    imprimir_arbol (programa);

    // Se genera la representación DOT del AST.
    FILE *archivo = fopen ("Src/Test/Resultados/AST/Prueba03_AST_ExpresionesAritmeticas.dot", "w");

    if (archivo == NULL) {
        perror ("Error al crear el archivo DOT");
        liberar_arbol (programa);
        return 1;
    }

    generar_dot (programa, archivo);

    fclose (archivo);

    printf ("\nRepresentación DOT generada en 'Src/Test/Resultados/AST/Prueba03_AST_ExpresionesAritmeticas.dot'.");

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
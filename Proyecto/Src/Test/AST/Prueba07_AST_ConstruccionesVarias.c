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
 *      boolean resultado;
 *      
 *      ;
 * 
 *      return;
 * 
 *      resultado = x == 10;
 *      x = -x;
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

    NodoAST *nombre_funcion = crear_nodo (AST_IDENTIFICADOR, 1, 6);
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
     * =================================
     * DECLARACIÓN DE VARIABLE resultado
     * =================================
     */
    NodoAST *declaracion_resultado = crear_nodo (AST_DECLARACION_VARIABLE, 3, 5);
    declaracion_resultado -> tipo_dato = TIPO_BOOLEAN;
    
    NodoAST *resultado = crear_nodo (AST_IDENTIFICADOR, 3, 9);
    resultado -> valor.identificador = copiar_cadena ("resultado");

    agregar_hijo (declaracion_resultado, resultado);
    agregar_hijo (bloque, declaracion_resultado);

    /**
     * ===============
     * SENTENCIA VACÍA
     * ===============
     */
    NodoAST *sentencia_vacia = crear_nodo (AST_SENTENCIA_VACIA, 5, 5);
    
    agregar_hijo (bloque, sentencia_vacia);
    
    /**
     * ======
     * RETURN
     * ======
     */
    NodoAST *return_vacio = crear_nodo (AST_RETURN, 7, 5);
    
    agregar_hijo (bloque, return_vacio);

    /**
     * ==========
     * ASIGNACIÓN
     * ==========
     */
    NodoAST *asignacion_igual = crear_nodo (AST_ASIGNACION, 9, 5);

    NodoAST *destino_resultado = crear_nodo (AST_IDENTIFICADOR, 9, 5);
    destino_resultado -> valor.identificador = copiar_cadena ("resultado");

    agregar_hijo (asignacion_igual, destino_resultado);

    /**
     * ===========
     * OPERADOR ==
     * ===========
     */
    NodoAST *igual = crear_nodo (AST_IGUAL, 9, 19);

    NodoAST *x_igual = crear_nodo (AST_IDENTIFICADOR, 9, 17);
    x_igual -> valor.identificador = copiar_cadena ("x");

    NodoAST *numero_10 = crear_nodo (AST_NUMERO, 9, 22);
    numero_10 -> valor.numero = 10;

    agregar_hijo (igual, x_igual);
    agregar_hijo (igual, numero_10);

    agregar_hijo (asignacion_igual, igual);
    agregar_hijo (bloque, asignacion_igual);

    /**
     * ==========
     * ASIGNACIÓN
     * ==========
     */
    NodoAST *asignacion_unario = crear_nodo (AST_ASIGNACION, 10, 5);

    NodoAST *destino_x = crear_nodo (AST_IDENTIFICADOR, 10, 5);
    destino_x -> valor.identificador = copiar_cadena ("x");

    agregar_hijo (asignacion_unario, destino_x);

    /**
     * ============
     * MENOS UNARIO
     * ============
     */
    NodoAST *menos_unario = crear_nodo (AST_MENOS_UNARIO, 10, 9);

    NodoAST *x_unario = crear_nodo (AST_IDENTIFICADOR, 10, 11);
    x_unario -> valor.identificador = copiar_cadena ("x");

    agregar_hijo (menos_unario, x_unario);
    agregar_hijo (asignacion_unario, menos_unario);
    agregar_hijo (bloque, asignacion_unario);

    /**
     * ======
     * PRUEBA 
     * ======
     */
    printf ("AST del programa: \n\n");
    imprimir_arbol (programa);

    // Se genera la representación DOT del AST.
    FILE *archivo = fopen ("Src/Test/Resultados/AST/Prueba07_AST_ConstruccionesVarias.dot", "w");

    if (archivo == NULL) {
        perror ("Error al crear el archivo DOT");
        liberar_arbol (programa);
        return 1;
    }

    generar_dot (programa, archivo);

    fclose (archivo);

    printf ("\nRepresentación DOT generada en 'Src/Test/Resultados/AST/Prueba07_AST_ConstruccionesVarias.dot'.");

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
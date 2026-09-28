#include "AST.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Función auxiliar.
static char *copiar_cadena (const char *cadena);

/**
 * Programa que se representa:
 * 
 *  int inc (int x) {
 *      return x + 1;
 *  }
 * 
 *  void main () {    
 *      int y;
 *      y = 4;
 *      if (y == 1) {
 *          return 1;
 *      } else {
 *          return inc (y);
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
    NodoAST *funcion_inc = crear_nodo (AST_DECLARACION_FUNCION, 1, 1);
    funcion_inc -> tipo_dato = TIPO_INT;

    NodoAST *nombre_inc = crear_nodo (AST_IDENTIFICADOR, 1, 5);
    nombre_inc -> valor.identificador = copiar_cadena ("inc");

    agregar_hijo (funcion_inc, nombre_inc);

    /**
     * =========
     * PARÁMETRO
     * =========
     */
    NodoAST *parametro_x = crear_nodo (AST_PARAMETRO, 1, 10);
    parametro_x -> tipo_dato = TIPO_INT;
    
    NodoAST *x_parametro = crear_nodo (AST_IDENTIFICADOR, 1, 14);
    x_parametro -> valor.identificador = copiar_cadena ("x");

    agregar_hijo (parametro_x, x_parametro);
    agregar_hijo (funcion_inc, parametro_x);

    /**
     * ======
     * BLOQUE
     * ======
     */
    NodoAST *bloque_inc = crear_nodo (AST_BLOQUE, 1, 17);
    
    agregar_hijo (funcion_inc, bloque_inc);
    agregar_hijo (programa, funcion_inc);

    /**
     * ======
     * RETURN
     * ======
     */
    NodoAST *return_inc = crear_nodo (AST_RETURN, 2, 5);

    /**
     * =========
     * EXPRESIÓN
     * =========
     */
    NodoAST *suma_inc = crear_nodo (AST_SUMA, 2, 14);

    NodoAST *x_suma_inc = crear_nodo (AST_IDENTIFICADOR, 2, 12);
    x_suma_inc -> valor.identificador = copiar_cadena ("x");

    NodoAST *numero_1_inc = crear_nodo (AST_NUMERO, 2, 16);
    numero_1_inc -> valor.numero = 1;

    agregar_hijo (suma_inc, x_suma_inc);
    agregar_hijo (suma_inc, numero_1_inc);
    agregar_hijo (return_inc, suma_inc);
    agregar_hijo (bloque_inc, return_inc);

    /**
     * ======================
     * DECLARACIÓN DE FUNCIÓN
     * ======================
     */
    NodoAST *funcion_main = crear_nodo (AST_DECLARACION_FUNCION, 5, 1);
    funcion_main -> tipo_dato = TIPO_VOID;

    NodoAST *nombre_main = crear_nodo (AST_IDENTIFICADOR, 5, 6);
    nombre_main -> valor.identificador = copiar_cadena ("main");

    agregar_hijo (funcion_main, nombre_main);

    /**
     * ======
     * BLOQUE
     * ======
     */
    NodoAST *bloque_main = crear_nodo (AST_BLOQUE, 5, 14);
    
    agregar_hijo (funcion_main, bloque_main);
    agregar_hijo (programa, funcion_main);

    /**
     * =======================
     * DECLARACIÓN DE VARIABLE
     * =======================
     */
    NodoAST *declaracion_y = crear_nodo (AST_DECLARACION_VARIABLE, 6, 5);
    declaracion_y -> tipo_dato = TIPO_INT;
    
    NodoAST *y_declaracion = crear_nodo (AST_IDENTIFICADOR, 6, 9);
    y_declaracion -> valor.identificador = copiar_cadena ("y");

    agregar_hijo (declaracion_y, y_declaracion);
    agregar_hijo (bloque_main, declaracion_y);

    /**
     * ==========
     * ASIGNACIÓN
     * ==========
     */
    NodoAST *asignacion_y = crear_nodo (AST_ASIGNACION, 7, 5);

    NodoAST *destino_y = crear_nodo (AST_IDENTIFICADOR, 7, 5);
    destino_y -> valor.identificador = copiar_cadena ("y");

    NodoAST *numero_4 = crear_nodo (AST_NUMERO, 7, 9);
    numero_4 -> valor.numero = 4;

    agregar_hijo (asignacion_y, destino_y);
    agregar_hijo (asignacion_y, numero_4);
    agregar_hijo (bloque_main, asignacion_y);

    /**
     * ============
     * SENTENCIA IF
     * ============
     */
    NodoAST *sentencia_if = crear_nodo (AST_IF, 8, 5);
    
    /**
     * ================
     * CONDICIÓN Y == 1
     * ================
     */
    NodoAST *igual = crear_nodo (AST_IGUAL, 8, 11);

    NodoAST *y_condicion_if = crear_nodo (AST_IDENTIFICADOR, 8, 9);
    y_condicion_if -> valor.identificador = copiar_cadena ("y");

    NodoAST *numero_1 = crear_nodo (AST_NUMERO, 8, 14);
    numero_1 -> valor.numero = 1;

    agregar_hijo (igual, y_condicion_if);
    agregar_hijo (igual, numero_1);
    agregar_hijo (sentencia_if, igual);

    /**
     * =========
     * BLOQUE IF
     * =========
     */
    NodoAST *bloque_if = crear_nodo (AST_BLOQUE, 8, 17);

    NodoAST *return_if = crear_nodo (AST_RETURN, 9, 9);

    NodoAST *numero_1_return = crear_nodo (AST_NUMERO, 9, 16);
    numero_1_return -> valor.numero = 1;

    agregar_hijo (return_if, numero_1_return);
    agregar_hijo (bloque_if, return_if);
    agregar_hijo (sentencia_if, bloque_if);

    /**
     * ===========
     * BLOQUE ELSE
     * ===========
     */
    NodoAST *bloque_else = crear_nodo (AST_BLOQUE, 10, 12);

    NodoAST *return_else = crear_nodo (AST_RETURN, 11, 9);

    /**
     * =======
     * LLAMADA
     * =======
     */
    NodoAST *llamada_inc = crear_nodo (AST_LLAMADA, 11, 16);
    llamada_inc -> valor.identificador = copiar_cadena ("inc");

    NodoAST *y_argumento = crear_nodo (AST_IDENTIFICADOR, 11, 21);
    y_argumento -> valor.identificador = copiar_cadena ("y");

    agregar_hijo (llamada_inc, y_argumento);
    agregar_hijo (return_else, llamada_inc);
    agregar_hijo (bloque_else, return_else);
    agregar_hijo (sentencia_if, bloque_else);
    agregar_hijo (bloque_main, sentencia_if);

    /**
     * ======
     * PRUEBA 
     * ======
     */
    printf ("AST del programa: \n\n");
    imprimir_arbol (programa);

    // Se genera la representación DOT del AST.
    FILE *archivo = fopen ("Src/Test/Resultados/AST/Prueba08_AST_ProgramaCompleto.dot", "w");

    if (archivo == NULL) {
        perror ("Error al crear el archivo DOT");
        liberar_arbol (programa);
        return 1;
    }

    generar_dot (programa, archivo);

    fclose (archivo);

    printf ("\nRepresentación DOT generada en 'Src/Test/Resultados/AST/Prueba08_AST_ProgramaCompleto.dot'.");

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
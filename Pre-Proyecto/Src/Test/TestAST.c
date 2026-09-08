#include "AST.h"
#include <stdio.h>

/**
 * Programa que se representa:
 * 
 *  int main () {
 *      int x;
 *      bool bandera;
 * 
 *      x = 10 + 2 * 3;
 *      bandera = true;
 *      return x;
 *  }
 */
int main (void) {
    
    /**
     * =========
     * PROGRAMA 
     * =========
     */

    NodoAST *programa = crear_nodo (AST_PROGRAMA, 1);
    programa -> tipo_dato = TIPO_INT;

    /**
     * =============
     * DECLARACIONES 
     * =============
     */

    NodoAST *declaraciones = crear_nodo (AST_DECLARACIONES, 2);

    /**
     * int x;
     */
    
    NodoAST *declaracion_x = crear_nodo (AST_DECLARACION, 2);
    declaracion_x -> tipo_dato = TIPO_INT;

    NodoAST *x_declarado = crear_nodo (AST_IDENTIFICADOR, 2);
    x_declarado -> valor.identificador = "x";
    
    agregar_hijo (declaracion_x, x_declarado);

    /**
     * bool bandera;
     */

    NodoAST *declaracion_bandera = crear_nodo (AST_DECLARACION, 3);
    declaracion_bandera -> tipo_dato = TIPO_BOOL;

    NodoAST *bandera_declarada = crear_nodo (AST_IDENTIFICADOR, 3);
    bandera_declarada -> valor.identificador = "bandera";
    
    agregar_hijo (declaracion_bandera, bandera_declarada);

    /**
     * ==========
     * SENTENCIAS 
     * ==========
     */

    NodoAST *sentencias = crear_nodo (AST_SENTENCIAS, 5);

    /**
     * x = 10 + 2 * 3;
     */

    NodoAST *asignacion_x = crear_nodo (AST_ASIGNACION, 5);
    
    NodoAST *x_asignacion = crear_nodo (AST_IDENTIFICADOR, 5);
    x_asignacion -> valor.identificador = "x";
    
    NodoAST *suma = crear_nodo (AST_SUMA, 5);
    
    NodoAST *numero_10 = crear_nodo (AST_NUMERO, 5);
    numero_10 -> valor.numero = 10;
    
    NodoAST *multiplicacion = crear_nodo (AST_MULTIPLICACION, 5);
    
    NodoAST *numero_2 = crear_nodo (AST_NUMERO, 5);
    numero_2 -> valor.numero = 2;
    
    NodoAST *numero_3 = crear_nodo (AST_NUMERO, 5);
    numero_3 -> valor.numero = 3;
    
    // 2 * 3
    
    agregar_hijo (multiplicacion, numero_2);
    agregar_hijo (multiplicacion, numero_3);
    
    // 10 + (2 * 3)

    agregar_hijo (suma, numero_10);
    agregar_hijo (suma, multiplicacion);
    
    // x = 10 + (2 * 3)
    
    agregar_hijo (asignacion_x, x_asignacion);
    agregar_hijo (asignacion_x, suma);

    /**
     * bandera = true;
     */

    NodoAST *asignacion_bandera = crear_nodo (AST_ASIGNACION, 6);
    
    NodoAST *bandera_asignacion = crear_nodo (AST_IDENTIFICADOR, 6);
    bandera_asignacion -> valor.identificador = "bandera";
    
    NodoAST *verdadero = crear_nodo (AST_TRUE, 6);
    verdadero -> tipo_dato = TIPO_BOOL;
    
    agregar_hijo (asignacion_bandera, bandera_asignacion);
    agregar_hijo (asignacion_bandera, verdadero);

    /**
     * return x;
     */

    NodoAST *retorno = crear_nodo (AST_RETURN, 7);
    
    NodoAST *x_retorno = crear_nodo (AST_IDENTIFICADOR, 7);
    x_retorno -> valor.identificador = "x";
    
    agregar_hijo (retorno, x_retorno);

    /**
     * ==========================
     * CONSTRUCCIÓN DEL PROGRAMA 
     * ==========================
     */

    agregar_hijo (declaraciones, declaracion_x);
    agregar_hijo (declaraciones, declaracion_bandera);

    agregar_hijo (sentencias, asignacion_x);
    agregar_hijo (sentencias, asignacion_bandera);
    agregar_hijo (sentencias, retorno);

    agregar_hijo (programa, declaraciones);
    agregar_hijo (programa, sentencias);

    /**
     * ======
     * PRUEBA 
     * ======
     */

    printf ("\nAST del programa: \n\n");
    imprimir_arbol (programa);

    // Se genera la representación DOT del AST.
    FILE *archivo = fopen ("Src/Test/Resultados/TestAST.dot", "w");

    if (archivo == NULL) {
        perror ("Error al crear el archivo DOT");
        liberar_arbol (programa);
        return 1;
    }

    generar_dot (programa, archivo);

    fclose (archivo);

    printf ("\nRepresentación DOT generada en 'Src/Test/Resultados/TestAST.dot'.\n\n");

    /**
     * ==========
     * LIBERACIÓN 
     * ==========
     */

    liberar_arbol (programa);

    return 0;
}
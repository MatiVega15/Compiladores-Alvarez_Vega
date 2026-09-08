#ifndef GENERADOR_PSEUDO_ASSEMBLY_H
#define GENERADOR_PSEUDO_ASSEMBLY_H

#include "AST.h"
#include <stdio.h>

/**
 * Genera código pseudo-assembly de tres direcciones
 * a partir del Árbol Sintáctico Abstracto (AST).
 * 
 * El AST debe haber sido construido correctamente
 * por el analizador sintáctico y validado por el
 * analizador semántico.
 * 
 * El código generado se escribe en el archivo recibido.
 * 
 * Formato de las instrucciones utilizadas:
 * 
 *  - SUM operador1 operador2 resultado
 *  - MUL operador1 operador2 resultado
 *  - MOV operador resultado
 *  - RETURN operador
 *  - RETURN
 */
void generar_pseudo_assembly (NodoAST *arbol, FILE *archivo);

#endif
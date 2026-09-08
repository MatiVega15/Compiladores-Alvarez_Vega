#ifndef ANALIZADOR_SEMANTICO_H
#define ANALIZADOR_SEMANTICO_H

#include "AST.h"
#include "TS.h"

/**
 * Analiza semánticamente el Árbol Sintáctico Abstracto (AST),
 * utilizando la Tabla de Símbolos (TS) para verificar las
 * reglas semánticas del lenguaje.
 * 
 * Se asume que el AST fue construido correctamente por el
 * analizador sintáctico.
 * 
 * Durante el análisis:
 * - Verifica declaraciones y usos de variables.
 * - Verifica tipos en asignaciones y expresiones.
 * - Verifica los retornos.
 * - Asocia los identificadores del AST con sus símbolos.
 * - Determina el tipo de las expresiones.
 * 
 * Retorna 0 si no se encontraron errores semánticos.
 * Retorna un valor mayor a 0 si se encontraron errores.
 */
int analizar_semantica (NodoAST *arbol, TablaSimbolos *ts);

#endif
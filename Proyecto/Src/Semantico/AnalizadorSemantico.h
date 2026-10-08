#ifndef ANALIZADOR_SEMANTICO_H
#define ANALIZADOR_SEMANTICO_H

#include "AST.h"
#include "TS.h"

#include <stdio.h>

/**
 * Analiza semánticamente el Árbol Sintáctico Abstracto (AST),
 * utilizando la Tabla de Símbolos (TS) para verificar las
 * reglas semánticas del lenguaje.
 * 
 * Se asume que el AST fue construido correctamente por el
 * analizador sintáctico.
 * 
 * Durante el análisis:
 * - Verifica declaraciones y usos de identificadores.
 * - Verifica tipos en asignaciones y expresiones.
 * - Verifica las reglas asociadas a las funciones.
 * - Verifica los retornos.
 * - Asocia los identificadores del AST con sus símbolos.
 * - Determina el tipo de las expresiones.
 * - Registra información relevante en el archivo .sem.
 * - Puede mostrar información detallada por consola en modo debug.
 * 
 * Retorna 0 si no se encontraron errores semánticos.
 * Retorna un valor mayor a 0 si se encontraron errores.
 */
int analizar_semantica (NodoAST *arbol, TablaSimbolos *ts, FILE *salida_sem, int modo_debug);

#endif
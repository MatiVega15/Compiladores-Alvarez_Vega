#ifndef OPTIMIZADOR_AST_H
#define OPTIMIZADOR_AST_H

#include "AST.h"
#include "TS.h"

/**
 * Agrupa las banderas de configuración para los pases de optimización del AST.
 */
typedef struct {
    int plegado_constantes;        // Activa el pase de Constant Folding.
    int eliminacion_codigo_muerto; // Activa el pase de Dead Code Elimination.

    int modo_debug;                // Activa las trazas de depuración por consola.
} OpcionesOptimizacion;

/**
 * Punto de entrada principal para el optimizador a nivel del AST.
 * 
 * Recibe el AST (ya validado por el analizador semántico) y ejecuta
 * de manera iterativa (hasta alcanzar un punto fijo) los pases activos
 * en 'opciones'.
 * 
 * Modifica el AST directamente en memoria, liberando los subárboles
 * reemplazados o podados.
 * 
 * Retorna el puntero a la raíz del AST optimizado (o el mismo puntero
 * sin cambios si 'ast' es NULL o no corresponde a AST_PROGRAMA).
 */
NodoAST *optimizar_ast (NodoAST *ast, OpcionesOptimizacion opciones);

#endif
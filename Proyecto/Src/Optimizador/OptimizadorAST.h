#ifndef OPTIMIZADOR_AST_H
#define OPTIMIZADOR_AST_H

#include "AST.h"
#include "TS.h"

/**
 * Agrupa las banderas de optimización disponibles.
 */
typedef struct {
    int plegado_constantes; // Activa el Constant Folding.

    int modo_debug; // Activa la salida de depuración.
} OpcionesOptimizacion;

/**
 * Punto de entrada principal para el optimizador del nivel del AST.
 * 
 * Recibe el AST (ya validado por el analizador semántico) y aplica
 * las optimizaciones seleccionadas.
 */
NodoAST *optimizar_ast (NodoAST *ast, OpcionesOptimizacion opciones);

#endif
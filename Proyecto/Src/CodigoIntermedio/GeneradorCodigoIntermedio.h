#ifndef GENERADOR_CODIGO_INTERMEDIO_H
#define GENERADOR_CODIGO_INTERMEDIO_H

#include "AST.h"
#include "Cuadruplos.h"

/**
 * Genera el código intermedio de un programa a partir de su AST.
 * 
 * El árbol debe haber sido validado previamente por el analizador
 * semántico. El generador utiliza los tipos y símbolos asociados
 * a sus nodos, sin volver a realizar el análisis semántico.
 * 
 * Retorna una lista de cuádruplos si la generación fue exitosa,
 * o NULL si ocurrió un error.
 * 
 * La lista devuelta pertenece al llamador, que debe liberarla
 * mediante liberar_lista_cuadruplos ().
 * 
 * El generador no libera el AST ni la TS.
 * 
 * Si modo_debug es distinto de 0, activa los mensajes de depuración.
 */
ListaCuadruplos *generar_codigo_intermedio (NodoAST *arbol, int modo_debug);

#endif
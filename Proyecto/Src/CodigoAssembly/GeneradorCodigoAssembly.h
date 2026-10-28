#ifndef GENERADOR_CODIGO_ASSEMBLY_H
#define GENERADOR_CODIGO_ASSEMBLY_H

#include "Cuadruplos.h"

#include <stdio.h>

/**
 * Genera el código Assembly x86-64 a partir de la lista de cuádruplos.
 * 
 * Escribe las instrucciones en el archivo de salida especificado.
 * 
 * Actualmente, solo soporta operaciones con tipos int y boolean.
 * Para operaciones de tipo float, emite un comentario TODO sin
 * romper la compilación del programa.
 * 
 * Retorna 1 si la generación fue exitosa, o 0 si ocurrió un error.
 */
int generar_codigo_assembly (ListaCuadruplos *lista, TablaSimbolos *ts, FILE *archivo_salida, int modo_debug);

#endif
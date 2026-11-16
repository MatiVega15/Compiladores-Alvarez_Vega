#ifndef GENERADOR_CODIGO_ASSEMBLY_H
#define GENERADOR_CODIGO_ASSEMBLY_H

#include "Cuadruplos.h"

#include <stdio.h>

/**
 * Genera el código Assembly x86-64 (sintaxis AT&T, convención System V AMD64 ABI)
 * a partir de la lista de cuádruplos y la Tabla de Símbolos.
 * 
 * Emite las secciones '.data' (variables globales) y '.text' (funciones) en
 * 'archivo_salida'. No libera 'lista' ni 'ts', ni cierra el archivo de salida.
 * 
 * Parámetros:
 * - lista: lista de cuádruplos generada por el backend intermedio.
 * - ts: tabla de símbolos utilizada para resolver variables globales.
 * - archivo_salida: flujo abierto en modo escritura donde se vuelca el código.
 * - modo_debug: si es distinto de 0, imprime trazas de traducción y optimización.
 * - optimizar_offsets: si es distinto de 0, reutiliza ranuras de pila de temporales
 *   mediante análisis de vida útil (Linear Scan).
 * - optimizar_potencias: si es distinto de 0, aplica reducción de fuerza (shlq/sarq)
 *   en multiplicaciones y divisiones enteras por potencias de 2.
 * 
 * Actualmente soporta operaciones con tipos int y boolean. Para operaciones
 * de tipo float, emite un comentario TODO sin interrumpir la generación.
 * 
 * Retorna 1 si la generación fue exitosa, o 0 si ocurrió un error.
 */
int generar_codigo_assembly (ListaCuadruplos *lista, TablaSimbolos *ts, FILE *archivo_salida, int modo_debug, int optimizar_offsets, int optimizar_potencias);

#endif
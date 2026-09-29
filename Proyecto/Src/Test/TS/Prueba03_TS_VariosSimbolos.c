#include "TS.h"

#include <stdio.h>
#include <string.h>

/**
 * Prueba la inserción de varios símbolos en el mismo nivel de la tabla de símbolos (TS).
 * 
 * Verifica que:
 * - Todos los símbolos se insertan correctamente.
 * - Todos los símbolos almacenan sus datos correctos.
 * - Las símbolos variables comienzan sin inicializar.
 * - Los símbolos parámetros comienzan inicializados.
 */
int main (void) {
    printf ("========== PRUEBA: VARIOS SÍMBOLOS ==========\n");

    TablaSimbolos *ts = iniciar_TS ();

    if (ts == NULL) {
        fprintf (stderr, "ERROR: no se pudo inicializar la tabla de símbolos.\n");
        return 1;
    }
    
    printf ("Tabla de símbolos creada correctamente.\n");

    Simbolo *simbolo_x = insertar_elemento (ts, "x", TIPO_INT, SIMBOLO_VARIABLE);

    if (simbolo_x == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'x'.\n");
        liberar_TS (ts);
        return 1;
    }

    Simbolo *simbolo_b = insertar_elemento (ts, "bandera", TIPO_BOOLEAN, SIMBOLO_VARIABLE);

    if (simbolo_b == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'bandera'.\n");
        liberar_TS (ts);
        return 1;
    }

    Simbolo *simbolo_real = insertar_elemento (ts, "real", TIPO_FLOAT, SIMBOLO_VARIABLE);

    if (simbolo_real == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'real'.\n");
        liberar_TS (ts);
        return 1;
    }

    Simbolo *simbolo_parametro = insertar_elemento (ts, "parametro", TIPO_INT, SIMBOLO_PARAMETRO);

    if (simbolo_parametro == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'parametro'.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Todos los símbolos fueron insertados correctamente.\n");

    Simbolo *encontrado_x = buscar_elemento (ts, "x");
    Simbolo *encontrado_b = buscar_elemento (ts, "bandera");
    Simbolo *encontrado_real = buscar_elemento (ts, "real");
    Simbolo *encontrado_parametro = buscar_elemento (ts, "parametro");

    if (encontrado_x == NULL || encontrado_b == NULL || encontrado_real == NULL || encontrado_parametro == NULL) {
        fprintf (stderr, "ERROR: no se pudieron encontrar todos los símbolos insertados.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Todos los símbolos fueron encontrados correctamente.\n");

    if (strcmp (encontrado_x -> nombre, "x") != 0 || encontrado_x -> tipo != TIPO_INT || encontrado_x -> clase != SIMBOLO_VARIABLE) {
        fprintf (stderr, "ERROR: los datos del símbolo 'x' no coinciden.\n");
        liberar_TS (ts);
        return 1;
    }

    if (strcmp (encontrado_b -> nombre, "bandera") != 0 || encontrado_b -> tipo != TIPO_BOOLEAN || encontrado_b -> clase != SIMBOLO_VARIABLE) {
        fprintf (stderr, "ERROR: los datos del símbolo 'bandera' no coinciden.\n");
        liberar_TS (ts);
        return 1;
    }

    if (strcmp (encontrado_real -> nombre, "real") != 0 || encontrado_real -> tipo != TIPO_FLOAT || encontrado_real -> clase != SIMBOLO_VARIABLE) {
        fprintf (stderr, "ERROR: los datos del símbolo 'real' no coinciden.\n");
        liberar_TS (ts);
        return 1;
    }

    if (strcmp (encontrado_parametro -> nombre, "parametro") != 0 || encontrado_parametro -> tipo != TIPO_INT || encontrado_parametro -> clase != SIMBOLO_PARAMETRO) {
        fprintf (stderr, "ERROR: los datos del símbolo 'parametro' no coinciden.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Los nombres, tipos y clases son correctos.\n");

    if (encontrado_x -> inicializada != 0 || encontrado_b -> inicializada != 0 || encontrado_real -> inicializada != 0) {
        fprintf (stderr, "ERROR: las variables no comienzan sin inicializar.\n");
        liberar_TS (ts);
        return 1;
    }

    if (encontrado_parametro -> inicializada != 1) {
        fprintf (stderr, "ERROR: el parámetro no comienza inicializado.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El estado de inicialización de variables y parámetros es correcto.\n");

    liberar_TS (ts);

    printf ("Tabla de símbolos liberada correctamente.\n");
    
    printf ("========== PRUEBA FINALIZADA ==========");

    return 0;
}
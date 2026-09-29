#include "TS.h"

#include <stdio.h>

/**
 * Prueba la liberación completa de la tabla de símbolos (TS).
 * 
 * Verifica que:
 * - Se pueden crear varios niveles.
 * - Se pueden insertar símbolos en diferentes niveles.
 * - liberar_TS () puede liberar la tabla con varios niveles abiertos.
 */
int main (void) {
    printf ("========== PRUEBA: LIBERACIÓN ==========\n");

    TablaSimbolos *ts = iniciar_TS ();

    if (ts == NULL) {
        fprintf (stderr, "ERROR: no se pudo inicializar la tabla de símbolos.\n");
        return 1;
    }
    
    printf ("Tabla de símbolos creada correctamente.\n");

    Simbolo *x = insertar_elemento (ts, "x", TIPO_INT, SIMBOLO_VARIABLE);
    
    if (x == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'x'.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Símbolo 'x' insertado correctamente.\n");

    abrir_nivel (ts);

    if (ts -> nivel_actual == NULL) {
        fprintf (stderr, "ERROR: no se pudo abrir el primer nivel interno.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Primer nivel abierto correctamente.\n");

    Simbolo *y = insertar_elemento (ts, "y", TIPO_BOOLEAN, SIMBOLO_VARIABLE);
    
    if (y == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'y'.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Símbolo 'y' insertado correctamente.\n");

    abrir_nivel (ts);

    if (ts -> nivel_actual == NULL) {
        fprintf (stderr, "ERROR: no se pudo abrir el segundo nivel interno.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Segundo nivel abierto correctamente.\n");

    Simbolo *z = insertar_elemento (ts, "z", TIPO_FLOAT, SIMBOLO_VARIABLE);
    
    if (z == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'z'.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Símbolo 'z' insertado correctamente.\n");

    printf ("Varios niveles y símbolos creados correctamente.\n");

    liberar_TS (ts);

    printf ("Tabla de símbolos liberada correctamente.\n");
    
    printf ("========== PRUEBA FINALIZADA ==========");

    return 0;
}
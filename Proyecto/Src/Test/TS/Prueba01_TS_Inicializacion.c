#include "TS.h"

#include <stdio.h>

/**
 * Prueba la inicialización de la tabla de símbolos (TS).
 * 
 * Verifica que:
 * - Puede crearse una tabla de símbolos.
 * - La tabla comienza con un nivel inicial abierto.
 * - El nivel inicial comienza sin símbolos.
 * - El nivel inicial no tiene un nivel anterior.
 */
int main (void) {
    printf ("========== PRUEBA: INICIALIZACIÓN ==========\n");

    TablaSimbolos *ts = iniciar_TS ();

    if (ts == NULL) {
        fprintf (stderr, "ERROR: no se pudo inicializar la tabla de símbolos.\n");
        return 1;
    }
    
    printf ("Tabla de símbolos creada correctamente.\n");

    if (ts -> nivel_actual == NULL) {
        fprintf (stderr, "ERROR: no se creó el nivel inicial.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Nivel inicial creado correctamente.\n");

    if (ts -> nivel_actual -> simbolos != NULL) {
        fprintf (stderr, "ERROR: el nivel inicial no está vacío.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El nivel inicial está vacío correctamente.\n");

    if (ts -> nivel_actual -> anterior != NULL) {
        fprintf (stderr, "ERROR: el nivel inicial tiene un nivel anterior.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El nivel inicial no tiene un nivel anterior.\n");

    liberar_TS (ts);

    printf ("Tabla de símbolos liberada correctamente.\n");
    
    printf ("========== PRUEBA FINALIZADA ==========");

    return 0;
}
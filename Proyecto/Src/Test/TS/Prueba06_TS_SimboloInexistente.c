#include "TS.h"

#include <stdio.h>

/**
 * Prueba la búsqueda de un símbolo inexistente de la tabla de símbolos (TS).
 * 
 * Verifica que:
 * - La inserción de símbolos funciona correctamente.
 * - La búsqueda de un símbolo no insertado fracasa correctamente.
 */
int main (void) {
    printf ("========== PRUEBA: SÍMBOLO INEXISTENTE ==========\n");

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

    Simbolo *simbolo_parametro = insertar_elemento (ts, "parametro", TIPO_BOOLEAN, SIMBOLO_PARAMETRO);

    if (simbolo_parametro == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'parametro'.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Símbolos de prueba insertados correctamente.\n");

    Simbolo *encontrado = buscar_elemento (ts, "inexistente");

    if (encontrado != NULL) {
        fprintf (stderr, "ERROR: se encontró un símbolo que no fue declarado.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("La búsqueda de un símbolo inexistente devuelve NULL correctamente.\n");

    liberar_TS (ts);

    printf ("Tabla de símbolos liberada correctamente.\n");

    printf ("========== PRUEBA FINALIZADA ==========");

    return 0;
}
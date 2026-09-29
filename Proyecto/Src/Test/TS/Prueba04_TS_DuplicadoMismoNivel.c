#include "TS.h"

#include <stdio.h>
#include <string.h>

/**
 * Prueba que no se pueden insertar elementos duplicados en el mismo nivel de la tabla de símbolos (TS).
 * 
 * Verifica que:
 * - Puede insertarse un primer elemento en el nivel actual.
 * - No puede insertarse otro elemento con el mismo nombre en el nivel actual.
 * - El elemento original permanece sin modificaciones.
 */
int main (void) {
    printf ("========== PRUEBA: SÍMBOLO DUPLICADO ==========\n");

    TablaSimbolos *ts = iniciar_TS ();

    if (ts == NULL) {
        fprintf (stderr, "ERROR: no se pudo inicializar la tabla de símbolos.\n");
        return 1;
    }
    
    printf ("Tabla de símbolos creada correctamente.\n");

    Simbolo *simbolo_main = insertar_elemento (ts, "main", TIPO_VOID, SIMBOLO_FUNCION);

    if (simbolo_main == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'main'.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Primer símbolo 'main' insertado correctamente.\n");

    Simbolo *simbolo_duplicado = insertar_elemento (ts, "main", TIPO_VOID, SIMBOLO_FUNCION);

    if (simbolo_duplicado != NULL) {
        fprintf (stderr, "ERROR: se permitió insertar un símbolo duplicado.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El símbolo duplicado fue rechazado correctamente.\n");

    Simbolo *encontrado = buscar_elemento (ts, "main");

    if (encontrado == NULL) {
        fprintf (stderr, "ERROR: el símbolo original 'main' no se encuentra en la TS.\n");
        liberar_TS (ts);
        return 1;
    }

    if (strcmp (encontrado -> nombre, "main") != 0 || encontrado -> tipo != TIPO_VOID || encontrado -> clase != SIMBOLO_FUNCION) {
        fprintf (stderr, "ERROR: el símbolo original fue alterado.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El símbolo original permanece sin modificaciones.\n");

    liberar_TS (ts);

    printf ("Tabla de símbolos liberada correctamente.\n");
    
    printf ("========== PRUEBA FINALIZADA ==========");

    return 0;
}
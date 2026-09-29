#include "TS.h"

#include <stdio.h>
#include <string.h>

/**
 * Prueba la inserción de un símbolo en la tabla de símbolos (TS).
 * 
 * Verifica que:
 * - Se puede insertar un elemento en la tabla.
 * - Se puede buscar el elemento insertado en la tabla.
 * - El elemento encontrado guarda correctamente su nombre, tipo y clase.
 * - El elemento comienza sin inicializar (0).
 * - El elemento comienza sin una dirección asociada (-1).
 */
int main (void) {
    printf ("========== PRUEBA: INSERCIÓN ==========\n");

    TablaSimbolos *ts = iniciar_TS ();

    if (ts == NULL) {
        fprintf (stderr, "ERROR: no se pudo inicializar la tabla de símbolos.\n");
        return 1;
    }
    
    printf ("Tabla de símbolos creada correctamente.\n");

    if (!insertar_elemento (ts, "x", TIPO_INT, SIMBOLO_VARIABLE)) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'x'.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Símbolo 'x' insertado correctamente.\n");

    Simbolo *simbolo = buscar_elemento (ts, "x");

    if (simbolo == NULL) {
        fprintf (stderr, "ERROR: no se encontró el símbolo 'x' después de insertarlo.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Símbolo 'x' encontrado correctamente.\n");

    if (strcmp (simbolo -> nombre, "x") != 0) {
        fprintf (stderr, "ERROR: el nombre del símbolo no coincide.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El nombre del símbolo es correcto.\n");

    if (simbolo -> tipo != TIPO_INT) {
        fprintf (stderr, "ERROR: el tipo del símbolo no coincide.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El tipo del símbolo es correcto.\n");

    if (simbolo -> clase != SIMBOLO_VARIABLE) {
        fprintf (stderr, "ERROR: la clase del símbolo no coincide.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("La clase del símbolo es correcta.\n");

    if (simbolo -> inicializada != 0) {
        fprintf (stderr, "ERROR: el símbolo debería comenzar como no inicializado.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El símbolo comienza como no inicializado.\n");

    if (simbolo -> direccion != -1) {
        fprintf (stderr, "ERROR: la dirección inicial debería ser -1.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("La dirección inicial es correcta.\n");

    liberar_TS (ts);

    printf ("Tabla de símbolos liberada correctamente.\n");
    
    printf ("========== PRUEBA FINALIZADA ==========");

    return 0;
}
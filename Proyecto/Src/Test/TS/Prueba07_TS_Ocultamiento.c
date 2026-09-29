#include "TS.h"

#include <stdio.h>

/**
 * Prueba el ocultamiento de símbolos entre diferentes niveles de la tabla de símbolos (TS).
 * 
 * Verifica que:
 * - Se pueden declarar varios símbolos en un mismo nivel.
 * - Se pueden declarar símbolos con el mismo nombre en niveles diferentes.
 * - La búsqueda prioriza el nivel actual.
 * - La búsqueda continúa en niveles anteriores cuando el símbolo no existe en el nivel actual.
 * - Al cerrar un nivel, los símbolos de ese nivel dejan de estar disponibles.
 */
int main (void) {
    printf ("========== PRUEBA: OCULTAMIENTO ==========\n");

    TablaSimbolos *ts = iniciar_TS ();

    if (ts == NULL) {
        fprintf (stderr, "ERROR: no se pudo inicializar la tabla de símbolos.\n");
        return 1;
    }
    
    printf ("Tabla de símbolos creada correctamente.\n");

    Simbolo *x_externo = insertar_elemento (ts, "x", TIPO_INT, SIMBOLO_VARIABLE);

    if (x_externo == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'x' externo.\n");
        liberar_TS (ts);
        return 1;
    }

    Simbolo *y_externo = insertar_elemento (ts, "y", TIPO_BOOLEAN, SIMBOLO_VARIABLE);

    if (y_externo == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'y' externo.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Símbolos del nivel externo insertados correctamente.\n");

    abrir_nivel (ts);

    if (ts -> nivel_actual == NULL) {
        fprintf (stderr, "ERROR: no se pudo abrir el nivel interno.\n");
        liberar_TS (ts);
        return 1;
    }

    Simbolo *x_interno = insertar_elemento (ts, "x", TIPO_INT, SIMBOLO_VARIABLE);

    if (x_interno == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'x' interno.\n");
        liberar_TS (ts);
        return 1;
    }

    Simbolo *z_interno = insertar_elemento (ts, "z", TIPO_INT, SIMBOLO_VARIABLE);

    if (z_interno == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'z' interno.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Símbolos del nivel interno insertados correctamente.\n");

    Simbolo *encontrado = buscar_elemento (ts, "x");

    if (encontrado != x_interno) {
        fprintf (stderr, "ERROR: La búsqueda no priorizó el símbolo 'x' del nivel interno.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("La búsqueda prioriza correctamente el 'x' interno.\n");

    encontrado = buscar_elemento (ts, "y");

    if (encontrado != y_externo) {
        fprintf (stderr, "ERROR: la búsqueda no encontró el símbolo 'y' del nivel externo.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("La búsqueda encuentra correctamente el 'y' del nivel externo.\n");

    encontrado = buscar_elemento (ts, "z");

    if (encontrado != z_interno) {
        fprintf (stderr, "ERROR: la búsqueda no encontró el símbolo 'z' del nivel interno.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("La búsqueda encuentra correctamente el 'z' del nivel interno.\n");
    
    cerrar_nivel (ts);

    if (ts -> nivel_actual == NULL) {
        fprintf (stderr, "ERROR: no se recuperó el nivel externo.\n");
        liberar_TS (ts);
        return 1;
    }

    encontrado = buscar_elemento (ts, "x");

    if (encontrado != x_externo) {
        fprintf (stderr, "ERROR: después de cerrar el nivel interno no se recuperó el 'x' externo.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("La búsqueda recuperó correctamente el 'x' externo.\n");

    encontrado = buscar_elemento (ts, "y");

    if (encontrado != y_externo) {
        fprintf (stderr, "ERROR: el símbolo 'y' externo dejó de estar disponible.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El símbolo 'y' externo continúa disponible correctamente.\n");

    encontrado = buscar_elemento (ts, "z");

    if (encontrado != NULL) {
        fprintf (stderr, "ERROR: el símbolo 'z' del nivel cerrado sigue siendo accesible.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El símbolo 'z' del nivel cerrado deja de estar disponible correctamente.\n");
    

    liberar_TS (ts);

    printf ("Tabla de símbolos liberada correctamente.\n");

    printf ("========== PRUEBA FINALIZADA ==========");

    return 0;
}
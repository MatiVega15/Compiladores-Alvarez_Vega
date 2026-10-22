#include "TS.h"

#include <stdio.h>

/**
 * Prueba la utilización de varios niveles de la tabla de símbolos (TS).
 * 
 * Verifica que:
 * - Se pueden abrir nuevos niveles.
 * - Se pueden declarar símbolos con el mismo nombre en niveles diferentes.
 * - La búsqueda priorice el nivel actual.
 * - Al cerrar el nivel, la búsqueda vuelve al nivel anterior.
 * - Los símbolos del nivel cerrado dejan de estar disponibles.
 * - Los símbolos del nivel cerrado conservan sus referencias mientras la tabla permanezca abierta.
 * - Los símbolos del nivel cerrado se liberan al liberar la tabla completa.
 * - Los símbolos del nivel interno no son marcados como globales.
 * - Los símbolos del nivel externo son marcados como globales.
 */
int main (void) {
    printf ("========== PRUEBA: NIVELES ==========\n");

    TablaSimbolos *ts = iniciar_TS ();

    if (ts == NULL) {
        fprintf (stderr, "ERROR: no se pudo inicializar la tabla de símbolos.\n");
        return 1;
    }
    
    printf ("Tabla de símbolos creada correctamente.\n");

    Simbolo *simbolo_externo = insertar_elemento (ts, "x", TIPO_INT, SIMBOLO_VARIABLE);

    if (simbolo_externo == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'x' en el nivel externo.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Símbolo 'x' del nivel externo insertado correctamente.\n");

    if (simbolo_externo -> es_global != 1) {
        fprintf (stderr, "ERROR: el símbolo del nivel externo no fue marcado como global.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El símbolo del nivel externo es correctamente global.\n");
    
    abrir_nivel (ts);

    if (ts -> nivel_actual == NULL) {
        fprintf (stderr, "ERROR: no se pudo abrir el nuevo nivel.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Nuevo nivel abierto correctamente.\n");

    if (ts -> nivel_actual -> anterior == NULL) {
        fprintf (stderr, "ERROR: el nuevo nivel no conserva el nivel anterior.\n");
        liberar_TS (ts);
        return 1;
    }
    
    printf ("La referencia al nivel anterior es correcta.\n");

    Simbolo *simbolo_interno = insertar_elemento (ts, "x", TIPO_FLOAT, SIMBOLO_VARIABLE);

    if (simbolo_interno == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'x' en el nivel interno.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Símbolo 'x' del nivel interno insertado correctamente.\n");

    if (simbolo_interno -> es_global != 0) {
        fprintf (stderr, "ERROR: el símbolo del nivel interno fue marcado incorrectamente como global.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El símbolo del nivel interno es correctamente local.\n");

    Simbolo *encontrado = buscar_elemento (ts, "x");

    if (encontrado == NULL) {
        fprintf (stderr, "ERROR: no se encontró el símbolo 'x'.\n");
        liberar_TS (ts);
        return 1;
    }

    if (encontrado != simbolo_interno) {
        fprintf (stderr, "ERROR: la búsqueda no priorizó el nivel actual.\n");
        liberar_TS (ts);
        return 1;
    }

    if (encontrado -> tipo != TIPO_FLOAT) {
        fprintf (stderr, "ERROR: se encontró un símbolo 'x' con un tipo incorrecto.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("La búsqueda prioriza correctamente el nivel actual.\n");

    cerrar_nivel (ts);

    if (ts -> nivel_actual == NULL) {
        fprintf (stderr, "ERROR: no se recuperó el nivel anterior al cerrar el nivel.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Nivel interno cerrado correctamente.\n");

    if (ts -> niveles_cerrados == NULL) {
        fprintf (stderr, "ERROR: el nivel cerrado no se conservó en la lista de niveles cerrados.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Lista de niveles cerrados correctamente actualizada.\n");

    if (ts -> niveles_cerrados -> simbolos != simbolo_interno) {
        fprintf (stderr, "ERROR: el nivel cerrado no conserva el símbolo esperado.\n");
        liberar_TS (ts);
        return 1;  
    }

    if (simbolo_interno -> tipo != TIPO_FLOAT) {
        fprintf (stderr, "ERROR: los datos del símbolo interno no se conservaron al cerrar el nivel.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El nivel cerrado conserva sus símbolos correctamente.\n");

    encontrado = buscar_elemento (ts, "x");

    if (encontrado == NULL) {
        fprintf (stderr, "ERROR: no se encontró el símbolo 'x' del nivel externo.\n");
        liberar_TS (ts);
        return 1;
    }

    if (encontrado != simbolo_externo) {
        fprintf (stderr, "ERROR: la búsqueda no recuperó el símbolo del nivel externo.\n");
        liberar_TS (ts);
        return 1;
    }

    if (encontrado -> tipo != TIPO_INT) {
        fprintf (stderr, "ERROR: el símbolo 'x' del nivel externo tiene un tipo incorrecto.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("La búsqueda recupera correctamente el nivel externo.\n");
    
    abrir_nivel (ts);

    if (ts -> nivel_actual == NULL) {
        fprintf (stderr, "ERROR: no se pudo abrir un segundo nivel interno.\n");
        liberar_TS (ts);
        return 1;
    }

    Simbolo *simbolo_temporal = insertar_elemento (ts, "temporal", TIPO_BOOLEAN, SIMBOLO_VARIABLE);

    if (simbolo_temporal == NULL) {
        fprintf (stderr, "ERROR: no se pudo insertar el símbolo 'temporal'.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Símbolo 'temporal' del nivel interno insertado correctamente.\n");

    if (simbolo_temporal -> es_global != 0) {
        fprintf (stderr, "ERROR: el símbolo del nivel interno fue marcado incorrectamente como global.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("El símbolo del nivel interno es correctamente local.\n");
    
    encontrado = buscar_elemento (ts, "temporal");
    
    if (encontrado != simbolo_temporal) {
        fprintf (stderr, "ERROR: no se encontró correctamente el símbolo del nivel interno.\n");
        liberar_TS (ts);
        return 1;
    }

    cerrar_nivel (ts);

    encontrado = buscar_elemento (ts, "temporal");
    
    if (encontrado != NULL) {
        fprintf (stderr, "ERROR: el símbolo del nivel cerrado sigue siendo accesible.\n");
        liberar_TS (ts);
        return 1;
    }

    printf ("Los símbolos del nivel cerrado dejan de estar disponibles correctamente.\n");

    liberar_TS (ts);

    printf ("Tabla de símbolos liberada correctamente.\n");
    
    printf ("========== PRUEBA FINALIZADA ==========");

    return 0;
}
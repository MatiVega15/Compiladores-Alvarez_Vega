#include "TS.h"
#include <stdlib.h>
#include <string.h>

/* Funciones auxiliares privadas. */
static Nivel *crear_nivel (void);
static void liberar_nivel (Nivel *nivel);
static Simbolo *buscar_en_nivel (Nivel *nivel, const char *nombre); 

TablaSimbolos *iniciar_TS (void) {
    // Se reserva memoria para la Tabla de Símbolos.
    TablaSimbolos *ts = malloc (sizeof (TablaSimbolos));

    if (ts == NULL) {
        fprintf (stderr, "Error: no se pudo reservar memoria para la Tabla de Símbolos.\n");
        exit (EXIT_FAILURE);
    }

    // La tabla comienza con un nivel abierto.
    ts -> nivel_actual = crear_nivel ();

    return ts;
}

void liberar_TS (TablaSimbolos *ts) {
    if (ts == NULL) {
        return;
    }

    // Se cierran y eliminan los niveles iterativamente.
    while (ts ->nivel_actual != NULL) {
        cerrar_nivel (ts);
    }

    // Se elimina la Tabla de Símbolos (TS).
    free (ts);
}

void abrir_nivel (TablaSimbolos *ts) {
    Nivel *nuevo_nivel;

    if (ts == NULL) {
        return;
    }

    nuevo_nivel = crear_nivel ();
    
    // El nivel actual pasa a ser el nivel anterior del nuevo nivel.
    nuevo_nivel -> anterior = ts -> nivel_actual;

    // El nuevo nivel pasa a ser el nivel actual.
    ts -> nivel_actual = nuevo_nivel;
}

void cerrar_nivel (TablaSimbolos *ts) {
    Nivel *nivel_a_cerrar;

    if (ts == NULL || ts -> nivel_actual == NULL) {
        return;
    }

    nivel_a_cerrar = ts -> nivel_actual;

    // El nivel anterior pasa a ser el nuevo nivel actual.
    ts -> nivel_actual = nivel_a_cerrar -> anterior;

    // Se liberan los símbolos y la estructura del nivel.
    liberar_nivel (nivel_a_cerrar);
}

Simbolo *insertar_elemento (TablaSimbolos *ts, const char *nombre, TipoDato tipo, ClaseSimbolo clase) {
    Simbolo *simbolo;

    if (ts == NULL || ts -> nivel_actual == NULL || nombre == NULL) {
        return NULL;
    }
    
    // No se permiten declaraciones duplicadas dentro del mismo nivel.
    if (buscar_en_nivel (ts -> nivel_actual, nombre) != NULL) {
        return NULL;
    }

    // Se reserva memoria para el nuevo símbolo.
    simbolo = malloc (sizeof (Simbolo));

    if (simbolo == NULL) {
        fprintf (stderr, "Error: no se pudo reservar memoria para un símbolo.\n");
        
        exit (EXIT_FAILURE);
    }

    // Se crea una copia independiente del nombre en el símbolo.
    simbolo -> nombre = malloc (strlen (nombre) + 1);

    if (simbolo -> nombre == NULL) {
        fprintf (stderr, "Error: no se pudo reservar memoria para el nombre del símbolo.\n");
        
        free (simbolo);
        exit (EXIT_FAILURE);
    }

    strcpy (simbolo -> nombre, nombre);

    simbolo -> tipo = tipo;
    simbolo -> clase = clase;

    // El símbolo se inserta al comienzo de la lista del nivel actual.
    simbolo -> siguiente = ts -> nivel_actual -> simbolos;
    ts -> nivel_actual -> simbolos = simbolo;

    return simbolo;
}

Simbolo *buscar_elemento (TablaSimbolos *ts, const char *nombre) {
    Nivel *nivel;
    Simbolo *simbolo;

    if (ts == NULL || nombre == NULL) {
        return NULL;
    }

    nivel = ts -> nivel_actual;

    // Se recorren los niveles desde el más interno hacia los niveles exteriores.
    while (nivel != NULL) {
        simbolo = buscar_en_nivel (nivel, nombre);

        if (simbolo != NULL) {
            return simbolo;
        }

        nivel = nivel -> anterior;
    }

    return NULL;
}

/* =========== Funciones auxiliares privadas =========== */

/**
 * Crea un nivel vacío en la Tabla de Símbolos (TS).
 */
static Nivel *crear_nivel (void) {
    Nivel *nivel = malloc (sizeof (Nivel));

    if (nivel == NULL) {
        fprintf (stderr, "Error: no se pudo reservar memoria para un nuevo nivel.\n");
        exit (EXIT_FAILURE);
    }

    // El nivel se crea vacío, sin símbolos ni referencia a un nivel anterior.
    nivel -> simbolos = NULL;
    nivel -> anterior = NULL;
    
    return nivel;
}

/**
 * Libera un nivel y todos los símbolos que contiene.
 */
static void liberar_nivel (Nivel *nivel) {
    Simbolo *actual;
    Simbolo *siguiente;

    if (nivel == NULL) {
        return;
    }

    actual = nivel -> simbolos;

    // Se recorre la lista de símbolos liberando cada elemento.
    while (actual != NULL) {
        siguiente = actual -> siguiente;

        free (actual -> nombre);
        free (actual);

        actual = siguiente;
    }

    free (nivel);
}

/**
 * Busca un símbolo únicamente dentro de un nivel.
 */
static Simbolo *buscar_en_nivel (Nivel *nivel, const char *nombre) {
    Simbolo *actual;

    if (nivel == NULL || nombre == NULL) {
        return NULL;
    }

    actual = nivel -> simbolos;

    // Se recorren los símbolos comparando con el nombre buscado.
    while (actual != NULL) {
        if (strcmp (actual -> nombre, nombre) == 0) {
            return actual;
        }

        actual = actual -> siguiente;
    }

    return NULL;
}
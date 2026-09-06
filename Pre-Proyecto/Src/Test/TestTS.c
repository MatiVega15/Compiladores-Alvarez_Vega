#include "TS.h"
#include <stdio.h>

/* Funciones auxiliares privadas. */
static const char *nombre_tipo (TipoDato tipo);
static const char *nombre_clase (ClaseSimbolo clase);
static void imprimir_simbolo (const Simbolo *simbolo);

/**
 * Pruebas para la Tabla de Símbolos (TS).
 */
int main (void) {
    /**
     * =================================================
     * Prueba 1: Inicialización de la Tabla de Símbolos. 
     * =================================================
     */

    printf ("\n========== PRUEBA 1: INICIALIZACIÓN ==========\n");

    TablaSimbolos *ts = iniciar_TS ();

    if (ts != NULL && ts ->nivel_actual != NULL) {
        printf ("Tabla de Símbolos inicializada correctamente.\n");
        printf ("Nivel inicial abierto correctamente.\n");
    }
    else {
        printf ("ERROR: no se pudo inicializar la Tabla de Símbolos.\n");
        return 1;
    }

    /**
     * ================================
     * Prueba 2: Inserción de símbolos. 
     * ================================
     */

    printf ("\n========== PRUEBA 2: INSERCIÓN ==========\n");

    Simbolo *simbolo_x = insertar_elemento (ts, "x", TIPO_INT, SIMBOLO_VARIABLE);
    Simbolo *simbolo_bandera = insertar_elemento (ts, "bandera", TIPO_BOOL, SIMBOLO_VARIABLE);
    Simbolo *simbolo_main = insertar_elemento (ts, "main", TIPO_VOID, SIMBOLO_FUNCION);

    if (simbolo_x != NULL) {
        printf ("Símbolo 'x' insertado correctamente.\n");
        imprimir_simbolo (simbolo_x);
    }
    else {
        printf ("ERROR: no se pudo insertar 'x'.\n");
    }

    if (simbolo_bandera != NULL) {
        printf ("Símbolo 'bandera' insertado correctamente.\n");
        imprimir_simbolo (simbolo_bandera);
    }
    else {
        printf ("ERROR: no se pudo insertar 'bandera'.\n");
    }

    if (simbolo_main != NULL) {
        printf ("Símbolo 'main' insertado correctamente. \n");
        imprimir_simbolo (simbolo_main);
    }
    else {
        printf ("ERROR: no se pudo insertar 'main'.\n");
    }

    /**
     * ===============================
     * Prueba 3: Búsqueda de símbolos. 
     * ===============================
     */

    printf ("\n========== PRUEBA 3: BÚSQUEDA ==========\n");

    Simbolo *busqueda_x = buscar_elemento (ts, "x");
    Simbolo *busqueda_bandera = buscar_elemento (ts, "bandera");
    Simbolo *busqueda_main = buscar_elemento (ts, "main");
    Simbolo *busqueda_inexistente = buscar_elemento (ts, "y");

    printf ("Búsqueda de 'x':\n");
    imprimir_simbolo (busqueda_x);

    printf ("Búsqueda de 'bandera':\n");
    imprimir_simbolo (busqueda_bandera);
    
    printf ("Búsqueda de 'main':\n");
    imprimir_simbolo (busqueda_main);

    printf ("Búsqueda de 'y':\n");
    imprimir_simbolo (busqueda_inexistente);
    
    /**
     * ============================
     * Prueba 4: Símbolo duplicado. 
     * ============================
     */

    printf ("\n========== PRUEBA 4: DUPLICADOS ==========\n");

    Simbolo *duplicado = insertar_elemento (ts, "x", TIPO_BOOL, SIMBOLO_VARIABLE);

    if (duplicado == NULL) {
        printf ("Correcto: no se permitió insertar nuevamente el símbolo 'x' en el mismo nivel.\n");
    }
    else {
        printf ("ERROR: se permitió insertar un símbolo duplicado.\n");
    }

    /**
     * =====================================
     * Prueba 5: Apertura de un nuevo nivel. 
     * =====================================
     */

    printf ("\n========== PRUEBA 5: NUEVO NIVEL ==========\n");

    abrir_nivel (ts);

    if (ts -> nivel_actual != NULL) {
        printf ("Nuevo nivel abierto correctamente.\n");
    }
    else {
        printf ("ERROR: no se pudo abrir el nuevo nivel.\n");
    }

    // Se inserta un símbolo propio del nuevo nivel.
    Simbolo *simbolo_y = insertar_elemento (ts, "y", TIPO_INT, SIMBOLO_VARIABLE);

    if (simbolo_y != NULL) {
        printf ("Símbolo 'y' insertado en el nivel interior.\n");
        imprimir_simbolo (simbolo_y);
    }

    /**
     * =================================
     * Prueba 6: Búsqueda entre niveles. 
     * =================================
     */

    printf ("\n========== PRUEBA 6: BÚSQUEDA ENTRE NIVELES ==========\n");

    // 'y' pertenece al nivel actual.
    printf ("Búsqueda de 'y' en el nivel actual: \n");
    imprimir_simbolo (buscar_elemento (ts, "y"));

    // 'x' pertenece al nivel anterior, pero debería poder encontrarse desde el actual.
    printf ("Búsqueda de 'x' desde el nivel interior: \n");
    imprimir_simbolo (buscar_elemento (ts, "x"));

    /**
     * ===================================
     * Prueba 7: Ocultamiento de símbolos. 
     * ===================================
     */

    printf ("\n========== PRUEBA 7: OCULTAMIENTO ==========\n");

    // Se declara nuevamente 'x' en el nivel interior, lo cual no es duplicado.
    Simbolo *nuevo_x = insertar_elemento (ts, "x", TIPO_BOOL, SIMBOLO_VARIABLE);

    if (nuevo_x != NULL) {
        printf ("Se permitió declarar 'x' en el nivel interior.\n");
        imprimir_simbolo (nuevo_x);
    }
    else {
        printf ("ERROR: no se pudo declarar 'x' en el nivel interior.\n");
    }

    // La búsqueda debe encontrar primero el 'x' del nivel interior.
    printf ("Búsqueda de 'x' desde el nivel interior.\n");
    imprimir_simbolo (buscar_elemento (ts, "x"));

    /**
     * ===========================
     * Prueba 8: Cierre del nivel. 
     * ===========================
     */

    printf ("\n========== PRUEBA 8: CIERRE DE NIVEL ==========\n");

    cerrar_nivel (ts);

    printf ("Nivel interior cerrado correctamente.\n");

    // Después de cerrar el nivel, 'y' y el segundo 'x' no deberían ser visibles.
    printf ("Búsqueda de 'y' después de cerrar el nivel:\n");
    imprimir_simbolo (buscar_elemento (ts, "y"));

    // El 'x' original del nivel exterior vuelve a ser el símbolo visible.
    printf ("Búsqueda de 'x' después de cerrar el nivel:\n");
    imprimir_simbolo (buscar_elemento (ts, "x"));

    /**
     * =============================================
     * Prueba 9: Liberación de la Tabla de Símbolos. 
     * =============================================
     */

    printf ("\n========== PRUEBA 9: LIBERACIÓN ==========\n");

    liberar_TS (ts);

    printf ("Tabla de Símbolos liberada correctamente.\n");

    printf ("\n========== PRUEBAS FINALIZADAS ==========\n\n");
    
    return 0;
}

/* =========== Funciones auxiliares privadas =========== */

/**
 * Convierte un TipoDato a una cadena para facilitar
 * la visualización de los resultados de las pruebas.
 */
static const char *nombre_tipo (TipoDato tipo) {
    switch (tipo) {
        case TIPO_INT:
            return "INT";
        case TIPO_BOOL:
            return "BOOL";
        case TIPO_VOID:
            return "VOID";
        default:
            return "DESCONOCIDO";
    }
}

/**
 * Convierte una ClaseSimbolo a una cadena para facilitar
 * la visualización de los resultados de las pruebas.
 */
static const char *nombre_clase (ClaseSimbolo clase) {
    switch (clase) {
        case SIMBOLO_VARIABLE:
            return "VARIABLE";
        case SIMBOLO_FUNCION:
            return "FUNCIÓN";
        default:
            return "DESCONOCIDA";
    }
}

/**
 * Imprime la información de un símbolo.
 */
static void imprimir_simbolo (const Simbolo *simbolo) {
    if (simbolo == NULL) {
        printf ("Símbolo no encontrado.\n");
        return;
    }

    printf ("Nombre: %s | Tipo: %s | Clase: %s\n", simbolo -> nombre, nombre_tipo (simbolo -> tipo), nombre_clase (simbolo -> clase));
}
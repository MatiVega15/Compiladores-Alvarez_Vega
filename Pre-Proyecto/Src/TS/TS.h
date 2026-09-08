#ifndef TS_H
#define TS_H

#include "Tipos.h"
#include <stdio.h>

/**
 * Representa las clases que puede tener un símbolo.
 */
typedef enum {
    SIMBOLO_VARIABLE,
    SIMBOLO_FUNCION
} ClaseSimbolo;

/**
 * Representa un elemento en la Tabla de Símbolos (TS).
 * 
 * Cada símbolo almacena:
 * 
 * - nombre: nombre del identificador.
 * - tipo: tipo de dato asociado al símbolo.
 * - clase: clase del símbolo.
 * 
 * Los símbolos de un mismo nivel se organizan mediante
 * una lista enlazada.
 */
typedef struct Simbolo {
    char *nombre;
    TipoDato tipo;
    ClaseSimbolo clase;

    struct Simbolo *siguiente;
} Simbolo;

/**
 * Representa un nivel de la Tabla de Símbolos (TS).
 * 
 * Cada nivel contiene una lista de símbolos y una
 * referencia al nivel anterior. De esta forma, los
 * niveles pueden organizarse como una pila.
 */
typedef struct Nivel {
    Simbolo *simbolos;
    
    struct Nivel *anterior;
} Nivel;

/**
 * Representa la Tabla de Símbolos (TS) completa.
 * 
 * El nivel_actual apunta al nivel que se encuentra
 * actualmente abierto.
 */
typedef struct {
    Nivel *nivel_actual;
} TablaSimbolos;

/**
 * Inicializa una nueva Tabla de Símbolos (TS).
 * 
 * La tabla se crea con un nivel abierto.
 */
TablaSimbolos *iniciar_TS (void);

/**
 * Libera toda la memoria utilizada por la Tabla de Símbolos (TS).
 * 
 * Se cierran todos los niveles que permanezcan abiertos,
 * comenzando desde el nivel actual.
 */
void liberar_TS (TablaSimbolos *ts);

/**
 * Abre un nuevo nivel de la Tabla de Símbolos (TS).
 * 
 * El nuevo nivel pasa a ser el nivel actual y conserva una
 * referencia al nivel que estaba abierto anteriormente.
 */
void abrir_nivel (TablaSimbolos *ts);

/**
 * Cierra el nivel actual de la Tabla de Símbolos (TS).
 * 
 * Se liberan todos los símbolos pertenecientes al nivel y
 * el nivel anterior pasa a ser nuevamente el nivel actual.
 */
void cerrar_nivel (TablaSimbolos *ts);

/**
 * Inserta un nuevo símbolo en el nivel actual.
 * 
 * No se permiten insertar dos símbolos con el mismo nombre en un mismo nivel.
 * 
 * Retorna un puntero al símbolo insertado si la operación fue exitosa,
 * o NULL si el símbolo ya existe o la tabla es inválida.
 */
Simbolo *insertar_elemento (TablaSimbolos *ts, const char *nombre, TipoDato tipo, ClaseSimbolo clase);

/**
 * Busca un símbolo en la Tabla de Símbolos (TS).
 * 
 * La búsqueda comienza en el nivel actual y continúa hacia los
 * niveles anteriores hasta encontrar el símbolo.
 * 
 * Retorna un puntero al símbolo encontrado, o NULL si el símbolo no existe.
 */
Simbolo *buscar_elemento (TablaSimbolos *ts, const char *nombre);

#endif
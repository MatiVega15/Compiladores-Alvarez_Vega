#ifndef TS_H
#define TS_H

#include "Tipos.h"

#include <stdio.h>

/**
 * Representa las clases que puede tener un símbolo.
 */
typedef enum {
    SIMBOLO_VARIABLE,
    SIMBOLO_FUNCION,
    SIMBOLO_PARAMETRO
} ClaseSimbolo;

/**
 * Representa un elemento en la Tabla de Símbolos (TS).
 * 
 * Cada símbolo almacena:
 * 
 * - nombre: nombre del identificador.
 * - tipo: tipo de dato asociado al símbolo.
 * - clase: clase del símbolo.
 * - inicializado: si el identificador tiene ya un valor asociado o no.
 * - direccion: ubicación asociada al símbolo.
 * 
 * Los símbolos de un mismo nivel se organizan mediante una lista enlazada.
 */
typedef struct Simbolo {
    char *nombre;
    TipoDato tipo;
    ClaseSimbolo clase;

    int inicializada;
    int direccion;

    struct Simbolo *siguiente;
} Simbolo;

/**
 * Representa un nivel de la Tabla de Símbolos (TS).
 * 
 * Cada nivel contiene una lista de símbolos y una
 * referencia al nivel anterior de la pila de
 * ámbitos activos.
 * 
 * Cuando un nivel se cierra, deja de formar parte de
 * la pila activa y se incorpora a la lista de niveles
 * cerrados para conservar sus símbolos mientras pueda
 * existir referencias a ellos desde el AST.
 */
typedef struct Nivel {
    Simbolo *simbolos;
    
    struct Nivel *anterior;
    struct Nivel *siguiente_cerrado;
} Nivel;

/**
 * Representa la Tabla de Símbolos (TS) completa.
 * 
 * El nivel_actual apunta al nivel que se encuentra
 * actualmente abierto y encabeza la pila de ámbitos
 * activos.
 * 
 * Por su parte, niveles_cerrados apunta al comienzo
 * de una lista con los niveles que ya fueron cerrados.
 * Estos niveles conservan sus símbolos hasta que se
 * libera la tabla completa.
 */
typedef struct {
    Nivel *nivel_actual;
    Nivel *niveles_cerrados;
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
 * Libera los niveles que permanecen abiertos y los que fueron
 * cerrados previamente, junto con todos los símbolos que contienen.
 * 
 * Debe invocarse cuando ya no sean necesarias las referencias a los
 * símbolos almacenados en el AST.
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
 * El nivel deja de participar en las búsquedas, pero sus
 * símbolos y su estructura permanecen en memoria hasta
 * que se libera la tabla completa.
 * 
 * Esto permite conservar los punteros a los símbolos que
 * fueron asociados previamente con los nodos del AST.
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
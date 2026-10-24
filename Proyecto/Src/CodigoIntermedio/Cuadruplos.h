#ifndef CUADRUPLOS_H
#define CUADRUPLOS_H

#include "TS.h"

#include <stdio.h>

/**
 * Representa las operaciones que pueden aparecer
 * en el código intermedio.
 * 
 * Cada operación determina cómo deben interpretarse
 * los operandos del cuádruplo.
 */
typedef enum {
    /* Asignación y operaciones aritméticas. */
    CI_ASIGNAR,
    CI_SUMAR,
    CI_RESTAR,
    CI_MULTIPLICAR,
    CI_DIVIDIR,
    CI_MODULO,
    CI_MENOS_UNARIO,

    /* Operaciones relacionales. */
    CI_MENOR,
    CI_MAYOR,
    CI_IGUAL,

    /* Control de flujo. */
    CI_ETIQUETA,
    CI_SALTO,
    CI_SALTO_SI_FALSO,

    /* Funciones y parámetros. */
    CI_INICIO_FUNCION,
    CI_PARAMETRO,
    CI_ARGUMENTO,
    CI_LLAMADA,
    CI_RETURN,
    CI_FIN_FUNCION
} CodigoOperacion;

/**
 * Representa la clase de un operando del código intermedio.
 * 
 * Permite distinguir constantes, temporales, símbolos de
 * la Tabla de Símbolos, etiquetas y operandos vacíos.
 */
typedef enum {
    OPERANDO_VACIO,
    OPERANDO_CONSTANTE,
    OPERANDO_TEMPORAL,
    OPERANDO_SIMBOLO,
    OPERANDO_ETIQUETA
} TipoOperando;

/**
 * Representa un operando de una instrucción de código intermedio.
 * 
 * Cada operando contiene:
 *  - tipo: clase del operando.
 *  - nombre: representación textual del operando.
 *  - tipo_dato: tipo de dato asociado, cuando corresponda.
 *  - simbolo: referencia al símbolo original de la TS, si corresponde.
 */
typedef struct {
    TipoOperando tipo;
    char *nombre;
    TipoDato tipo_dato;
    Simbolo *simbolo;
} Operando;

/**
 * Representa una instrucción de código intermedio mediante
 * un cuádruplo (operación, argumento 1, argumento 2, resultado).
 * 
 * Los operandos que no se utilizan en una instrucción se
 * representan mediante OPERANDO_VACIO.
 */
typedef struct {
    CodigoOperacion op;

    Operando arg1;
    Operando arg2;
    Operando resultado;
} Cuadruplo;

/**
 * Representa un nodo de la lista doblemente encadenada
 * de cuádruplos.
 * 
 * Cada nodo contiene una instrucción y referencias al
 * nodo anterior y al nodo siguiente.
 */
typedef struct NodoCuadruplo {
    Cuadruplo instruccion;

    struct NodoCuadruplo *anterior;
    struct NodoCuadruplo *siguiente;
} NodoCuadruplo;

/**
 * Representa una lista, doblemente encadenada y ordenada,
 * de instrucciones de código intermedio.
 */
typedef struct {
    NodoCuadruplo *primero;
    NodoCuadruplo *ultimo;
    
    int cantidad;
} ListaCuadruplos;

/**
 * Crea una lista vacía de cuádruplos.
 * 
 * Retorna un puntero a la lista creada o NULL si no se
 * pudo reservar la memoria necesaria.
 */
ListaCuadruplos *crear_lista_cuadruplos (void);

/**
 * Agrega un cuádruplo al final de la lista.
 * 
 * Retorna 1 si la instrucción se agregó correctamente
 * o 0 si ocurrió un error.
 */
int agregar_cuadruplo (ListaCuadruplos *lista, CodigoOperacion op, const Operando *arg1, const Operando *arg2, const Operando *resultado);

/**
 * Crea un operando vacío.
 * 
 * Sus campos indican que no contiene un valor,
 * un nombre ni una referencia a un símbolo.
 */
Operando crear_operando_vacio (void);

/**
 * Crea un operando constante.
 * 
 * El valor se almacena como una cadena independiente.
 * El tipo de dato conserva la información semántica
 * de la constante.
 */
Operando crear_operando_constante (const char *valor, TipoDato tipo_dato);

/**
 * Crea un operando temporal.
 * 
 * El identificador del temporal se almacena como
 * una cadena independiente.
 */
Operando crear_operando_temporal (const char *nombre, TipoDato tipo_dato);

/**
 * Crea un operando asociado a un símbolo de la TS.
 * 
 * La referencia al símbolo se conserva sin copiarlo.
 * El nombre se copia independientemente para permitir
 * utilizar identificadores internos únicos.
 */
Operando crear_operando_simbolo (Simbolo *simbolo, const char *nombre);

/**
 * Crea un operando que representa una etiqueta.
 * 
 * Las etiquetas se almacenan como cadenas independientes
 * de las instrucciones que las utilizan.
 */
Operando crear_operando_etiqueta (const char *nombre);

/**
 * Libera la memoria administrada por un operando.
 * 
 * El operando queda vacío después de la liberación.
 */
void liberar_operando (Operando *operando);

/**
 * Libera la lista y todos sus cuádruplos. 
 * 
 * También libera los nombres almacenados en los operandos.
 */
void liberar_lista_cuadruplos (ListaCuadruplos *lista);

/**
 * Retorna la cantidad de instrucciones almacenadas.
 * 
 * Si la lista es NULL, retorna cero.
 */
int cantidad_cuadruplos (const ListaCuadruplos *lista);

/**
 * Imprime la lista de cuádruplos en el archivo indicado.
 * 
 * Cada instrucción se escribe en una línea, utilizando
 * una representación textual uniforme:
 * 
 *          OPERACIÓN ARG1 ARG2 RESULTADO
 * 
 * Los campos vacíos se representan mediante '-'.
 * 
 * Retorna 1 si la escritura fue exitosa o 0 si ocurrió un error.
 */
int escribir_cuadruplos (const ListaCuadruplos *lista, FILE *archivo);

/**
 * Retorna el nombre textual asociado a una operación.
 * 
 * Retorna NULL si el código de operación no es válido.
 */
const char *nombre_operacion (CodigoOperacion op);

/**
 * Retorna el nombre textual asociado a un tipo de dato.
 * 
 * Retorna NULL si el tipo no es válido.
 */
const char *nombre_tipo_dato (TipoDato tipo);

#endif
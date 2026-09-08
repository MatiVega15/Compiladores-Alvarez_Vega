#ifndef TIPOS_H
#define TIPOS_H

/**
 * Representa los tipos de datos que admite el lenguaje.
 * 
 * TIPO_NO_DEFINIDO se utiliza cuando el tipo todavía no
 * fue determinado o debe inferirse posteriormente a
 * partir del árbol.
 */
typedef enum {
    TIPO_INT,
    TIPO_BOOL,
    TIPO_VOID,
    TIPO_NO_DEFINIDO
} TipoDato;

#endif
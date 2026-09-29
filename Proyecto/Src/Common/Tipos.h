#ifndef TIPOS_H
#define TIPOS_H

/**
 * Representa los tipos de datos que admite el lenguaje.
 * 
 * TIPO_NO_DEFINIDO se utiliza cuando el tipo todavía no
 * fue determinado o debe determinarse posteriormente a
 * durante el análisis semántico.
 */
typedef enum {
    TIPO_INT,
    TIPO_BOOLEAN,
    TIPO_FLOAT,
    TIPO_VOID,
    TIPO_NO_DEFINIDO
} TipoDato;

#endif
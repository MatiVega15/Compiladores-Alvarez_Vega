#ifndef INTERPRETE_H
#define INTERPRETE_H

#include "AST.h"

/**
 * Representa un valor durante la ejecución del programa.
 * 
 * Puede almacenar un valor entero o booleano.
 */
typedef struct {
    TipoDato tipo;

    union {
        int entero;
        int booleano;
    };
} ValorRuntime;

/**
 * Ejecuta el programa representado por el Árbol Sintáctico Abstracto (AST).
 * 
 * Se asume que el AST fue construido correctamente por el analizador
 * sintáctico y validado por el analizador semánctico.
 * 
 * Retorna 0 si la ejecución finalizó correctamente,
 * o un valor distinto de 0 si ocurrió un error.
 * 
 * Si el programa retorna un valor, este se almacena en valor_retorno.
 */
int interpretar_programa (NodoAST *arbol, ValorRuntime *valor_retorno);

#endif
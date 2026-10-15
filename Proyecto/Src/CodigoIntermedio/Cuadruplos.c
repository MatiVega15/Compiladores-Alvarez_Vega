#include "Cuadruplos.h"

#include <stdlib.h>
#include <string.h>

/* Funciones auxiliares privadas. */
static char *copiar_cadena (const char *cadena);
static int codigo_operacion_valido (CodigoOperacion op);
static int operando_valido (const Operando *operando);
static Operando copiar_operando (const Operando *operando);
static void liberar_cuadruplo (Cuadruplo *cuadruplo);

ListaCuadruplos *crear_lista_cuadruplos (void) {
    // Se reserva memoria para la estructura de la lista.
    ListaCuadruplos *lista = malloc (sizeof (ListaCuadruplos));

    if (lista == NULL) {
        return NULL;
    }

    // La lista comienza vacía, sin primer ni último nodo.
    lista -> primero = NULL;
    lista -> ultimo = NULL;
    lista -> cantidad = 0;

    return lista;
}

int agregar_cuadruplo (ListaCuadruplos *lista, CodigoOperacion op, const Operando *arg1, const Operando *arg2, const Operando *resultado) {
    // Se validan la lista, la operación y los tres operandos.
    if (lista == NULL || !codigo_operacion_valido (op) || !operando_valido (arg1) || !operando_valido (arg2) || !operando_valido (resultado)) {
        return 0;
    }

    // Se prepara una copia independiente de la instrucción.
    Cuadruplo instruccion;

    instruccion.op = op;
    instruccion.arg1 = copiar_operando (arg1);
    instruccion.arg2 = copiar_operando (arg2);
    instruccion.resultado = copiar_operando (resultado);

    // Si falla la copia de un operando, se liberan los recursos.
    if (arg1 -> tipo != OPERANDO_VACIO && instruccion.arg1.tipo == OPERANDO_VACIO) {
        liberar_cuadruplo (&instruccion);
        return 0;
    }
    if (arg2 -> tipo != OPERANDO_VACIO && instruccion.arg2.tipo == OPERANDO_VACIO) {
        liberar_cuadruplo (&instruccion);
        return 0;
    }
    if (resultado -> tipo != OPERANDO_VACIO && instruccion.resultado.tipo == OPERANDO_VACIO) {
        liberar_cuadruplo (&instruccion);
        return 0;
    }

    // Se reserva memoria para el nuevo nodo.
    NodoCuadruplo *nuevo = malloc (sizeof (NodoCuadruplo));

    if (nuevo == NULL) {
        liberar_cuadruplo (&instruccion);
        return 0;
    }

    // Se vincula el nodo con el último elemento existente.
    nuevo -> instruccion = instruccion;
    nuevo -> anterior = lista -> ultimo;
    nuevo -> siguiente = NULL;

    // Si no hay instrucciones, el nuevo nodo será el primero.
    if (lista -> primero == NULL) {
        lista -> primero = nuevo;
    }
    // Si la lista ya contiene instrucciones, se enlaza el último nodo con el nuevo.
    else {
        lista -> ultimo -> siguiente = nuevo;
    }

    // Se actualiza el último nodo y la cantidad de instrucciones.
    lista -> ultimo = nuevo;
    lista -> cantidad ++;

    return 1;
}

Operando crear_operando_vacio (void) {
    Operando operando;

    // Un operando vacío no tiene contenido ni tipo.
    operando.tipo = OPERANDO_VACIO;
    operando.nombre = NULL;
    operando.tipo_dato = TIPO_NO_DEFINIDO;
    operando.simbolo = NULL;

    return operando;
}

Operando crear_operando_constante (const char *valor, TipoDato tipo_dato) {
    Operando operando = crear_operando_vacio ();

    if (valor == NULL) {
        return operando;
    }

    // Se copia el valor para que el operando sea independiente del original.
    char *copia = copiar_cadena (valor);

    if (copia == NULL) {
        return operando;
    }

    operando.tipo = OPERANDO_CONSTANTE;
    operando.nombre = copia;
    operando.tipo_dato = tipo_dato;

    return operando;
}

Operando crear_operando_temporal (const char *nombre, TipoDato tipo_dato) {
    Operando operando = crear_operando_vacio ();

    if (nombre == NULL) {
        return operando;
    }

    // Se copia el nombre del temporal.
    char *copia = copiar_cadena (nombre);

    if (copia == NULL) {
        return operando;
    }

    operando.tipo = OPERANDO_TEMPORAL;
    operando.nombre = copia;
    operando.tipo_dato = tipo_dato;

    return operando;
}

Operando crear_operando_simbolo (Simbolo *simbolo, const char *nombre) {
    Operando operando = crear_operando_vacio ();

    // El símbolo y su representación interna son necesarios.
    if (simbolo == NULL || nombre == NULL) {
        return operando;
    }

    // El nombre interno se almacena independientemente del símbolo.
    char *copia = copiar_cadena (nombre);

    if (copia == NULL) {
        return operando;
    }

    operando.tipo = OPERANDO_SIMBOLO;
    operando.nombre = copia;

    // Se conserva el tipo y la referencia al símbolo original.
    operando.tipo_dato = simbolo -> tipo;
    operando.simbolo = simbolo;

    return operando;
}

Operando crear_operando_etiqueta (const char *nombre) {
    Operando operando = crear_operando_vacio ();

    if (nombre == NULL) {
        return operando;
    }

    // Se copia el nombre de la etiqueta.
    char *copia = copiar_cadena (nombre);

    if (copia == NULL) {
        return operando;
    }

    operando.tipo = OPERANDO_ETIQUETA;
    operando.nombre = copia;
    operando.tipo_dato = TIPO_NO_DEFINIDO;

    return operando;
}

void liberar_operando (Operando *operando) {
    if (operando == NULL) {
        return;
    }

    // Se libera únicamente la cadena propiedad del operando.
    free (operando -> nombre);

    // Se restablecen los campos para dejarlo en estado vacío.
    operando -> tipo = OPERANDO_VACIO;
    operando -> nombre = NULL;
    operando -> tipo_dato = TIPO_NO_DEFINIDO;
    operando -> simbolo = NULL;
}

void liberar_lista_cuadruplos (ListaCuadruplos *lista) {
    if (lista == NULL) {
        return;
    }

    NodoCuadruplo *actual = lista -> primero;

    // Se recorre la lista guardando el siguiente nodo antes de liberar el actual.
    while (actual != NULL) {
        NodoCuadruplo *siguiente = actual -> siguiente;

        // Se liberan los operandos de la instrucción.
        liberar_cuadruplo (&actual -> instruccion);

        // Se libera el nodo actual.
        free (actual);

        actual = siguiente;
    }

    // Finalmente, se libera la estructura lista.
    free (lista);
}

int cantidad_cuadruplos (const ListaCuadruplos *lista) {
    // Una lista inexistente se considera vacía.
    if (lista == NULL) {
        return 0;
    }

    return lista -> cantidad;
}

int escribir_cuadruplos (const ListaCuadruplos *lista, FILE *archivo) {
    // Se requiere una lista y un archivo válidos.
    if (lista == NULL || archivo == NULL) {
        return 0;
    }

    NodoCuadruplo *actual = lista -> primero;

    // Se recorre la lista desde el principio hasta el final.
    while (actual != NULL) {
        Cuadruplo *instruccion = &actual -> instruccion;

        // Se obtiene el nombre textual de la operación.
        const char *operacion = nombre_operacion (instruccion -> op);

        if (operacion == NULL) {
            return 0;
        }

        // Los operandos vacíos se representan con '-'.
        const char *arg1 = instruccion -> arg1.tipo == OPERANDO_VACIO ? "-" : instruccion -> arg1.nombre;
        const char *arg2 = instruccion -> arg2.tipo == OPERANDO_VACIO ? "-" : instruccion -> arg2.nombre;
        const char *resultado = instruccion -> resultado.tipo == OPERANDO_VACIO ? "-" : instruccion -> resultado.nombre;
    
        // Se escribe una instrucción por línea, con cuatro campos.
        if (fprintf (archivo, "%s %s %s %s\n", operacion, arg1, arg2, resultado) < 0) {
            return 0;
        }

        actual = actual -> siguiente;
    }

    // Se comprueba si ocurrió un error durante la escritura.
    if (ferror (archivo)) {
        return 0;
    }

    return 1;
}

const char *nombre_operacion (CodigoOperacion op) {
    // Se asocia cada código interno con su nombre textual.
    switch (op) {
        case CI_ASIGNAR:
            return "ASSIGN";
        case CI_SUMAR:
            return "ADD";
        case CI_RESTAR:
            return "SUB";
        case CI_MULTIPLICAR:
            return "MUL";
        case CI_DIVIDIR:
            return "DIV";
        case CI_MODULO:
            return "MOD";
        case CI_MENOS_UNARIO:
            return "NEG";
        case CI_MENOR:
            return "LT";
        case CI_MAYOR:
            return "GT";
        case CI_IGUAL:
            return "EQ";
        case CI_AND:
            return "AND";
        case CI_OR:
            return "OR";
        case CI_NEGACION:
            return "NOT";
        case CI_ETIQUETA:
            return "LABEL";
        case CI_SALTO:
            return "GOTO";
        case CI_SALTO_SI_FALSO:
            return "IF_FALSE";
        case CI_INICIO_FUNCION:
            return "FUNC_BEGIN";
        case CI_PARAMETRO:
            return "PARAM";
        case CI_ARGUMENTO:
            return "ARG";
        case CI_LLAMADA:
            return "CALL";
        case CI_RETURN:
            return "RETURN";
        case CI_FIN_FUNCION:
            return "FUNC_END";
        default:
            return NULL;
    }
}

const char *nombre_tipo_dato (TipoDato tipo) {
    switch (tipo) {
        case TIPO_INT:
            return "int";
        case TIPO_BOOLEAN:
            return "boolean";
        case TIPO_FLOAT:
            return "float";
        case TIPO_VOID:
            return "void";
        case TIPO_NO_DEFINIDO:
            return "no definido";
        default:
            return NULL;
    }
}

/* =========== Funciones auxiliares privadas =========== */

/**
 * Crea una copia independiente de una cadena.
 * 
 * Retorna NULL si la cadena recibida es NULL o si no
 * se pudo reservar la memoria necesaria.
 */
static char *copiar_cadena (const char *cadena) {
    // No se pudo copiar una cadena inexistente.
    if (cadena == NULL) {
        return NULL;
    }

    size_t longitud = strlen (cadena);

    // Se reserva espacio para los caracteres y el terminador '\0'.
    char *copia = malloc (longitud + 1);

    if (copia == NULL) {
        return NULL;
    }

    // Se copian todos los caracteres.
    memcpy (copia, cadena, longitud + 1);

    return copia;
}

/**
 * Determina si un código de operación es válido.
 */
static int codigo_operacion_valido (CodigoOperacion op) {
    // Los códigos válidos abarcan todos los valores del enum.
    return op >= CI_ASIGNAR && op <= CI_FIN_FUNCION;
}

/**
 * Verifica que un operando tenga una representación válida.
 * 
 * Los operandos vacíos no necesitan nombre ni símbolo.
 * Los restantes deben tener un nombre.
 * 
 * Un operando de símbolo también debe conservar una
 * referencia válida a su símbolo.
 */
static int operando_valido (const Operando *operando) {
    if (operando == NULL) {
        return 0;
    }

    // Se comprueba que el tipo de operando pertenezca al enum.
    if (operando -> tipo < OPERANDO_VACIO || operando -> tipo > OPERANDO_ETIQUETA) {
        return 0;
    }

    // Un operando vacío no debe contener nombre ni símbolo.
    if (operando -> tipo == OPERANDO_VACIO) {
        return operando -> nombre == NULL && operando -> simbolo == NULL;
    }

    // Los operandos con contenido necesitan un nombre.
    if (operando -> nombre == NULL) {
        return 0;
    }

    // Un operando de símbolo debe apuntar a un símbolo existente.
    if (operando -> tipo == OPERANDO_SIMBOLO && operando -> simbolo == NULL) {
        return 0;
    }

    return 1;
}

/**
 * Crea una copia independiente de un operando.
 * 
 * La cadena nombre se copia, mientras que la referencia al
 * símbolo se conserva sin modificar su propiedad.
 * 
 * Si no se puede copiar el nombre, retorna un operando vacío.
 */
static Operando copiar_operando (const Operando *operando) {
    if (!operando_valido (operando)) {
        return crear_operando_vacio ();
    }

    // No hace falta copiar cadenas para un operando vacío.
    if (operando -> tipo == OPERANDO_VACIO) {
        return crear_operando_vacio ();
    }

    // Cada instrucción tendrá su propia copia del nombre.
    char *nombre = copiar_cadena (operando -> nombre);

    if (nombre == NULL) {
        return crear_operando_vacio ();
    }

    Operando copia;

    copia.tipo = operando -> tipo;
    copia.nombre = nombre;
    copia.tipo_dato = operando -> tipo_dato;

    // El símbolo se referencia pero no se copia ni se libera.
    copia.simbolo = operando -> simbolo;

    return copia;
}

/**
 * Libera todos los recursos propios de un cuádruplo.
 * 
 * No libera los símbolos referenciados por sus operandos.
 */
static void liberar_cuadruplo (Cuadruplo *cuadruplo) {
    if (cuadruplo == NULL) {
        return;
    }

    // Cada operando libera su propia cadena.
    liberar_operando (&cuadruplo -> arg1);
    liberar_operando (&cuadruplo -> arg2);
    liberar_operando (&cuadruplo -> resultado);
}
#include "GeneradorCodigoAssembly.h"

#include <stdlib.h>
#include <string.h>

/* Límite arbitrario para la cantidad de temporales por función. */
#define MAX_TEMPORALES 10000

/* Límite arbitrario para la cantidad de argumentos en pila. */
#define MAX_ARGUMENTOS_PILA 1000

/* Búfer estático para los nombres de los registros de argumentos (Linux ABI). */
static const char *REGISTROS_ARG [] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

/**
 * Mantiene el estado utilizado durante la generación de código Assembly.
 */
typedef struct {
    FILE *archivo;
    int modo_debug;
    int error;

    // Arreglo para mapear el número de un temporal a su offset en la pila.
    int offset_temporales [MAX_TEMPORALES];

    // Búfer para retener los argumentos > 6 antes de apilarlos.
    Operando buffer_argumentos [MAX_ARGUMENTOS_PILA];
    int tope_argumentos;
} ContextoAssembly;

/* =========== Funciones auxiliares privadas =========== */

static void escribir_seccion_datos (TablaSimbolos *ts, ContextoAssembly *contexto);
static void procesar_funcion (NodoCuadruplo **actual, ContextoAssembly *contexto);
static void calcular_entorno_funcion (NodoCuadruplo *inicio, ContextoAssembly *contexto, int *bytes_locales, int *cantidad_parametros);
static void obtener_referencia_operando (Operando *op, ContextoAssembly *contexto, char *buffer);
static void debug_assembly (ContextoAssembly *contexto, const char *mensaje);
static void registrar_error (ContextoAssembly *contexto, const char *mensaje);

/* =========== Función principal =========== */

int generar_codigo_assembly (ListaCuadruplos *lista, TablaSimbolos *ts, FILE *archivo_salida, int modo_debug) {
    // Se validan los argumentos.
    if (lista == NULL || ts == NULL || archivo_salida == NULL) {
        return 0;
    }

    ContextoAssembly contexto;
    contexto.modo_debug = modo_debug;
    contexto.error = 0;
    contexto.archivo = archivo_salida;

    // Se inicializa en 0 los arreglos del contexto.
    memset (contexto.offset_temporales, 0, sizeof (contexto.offset_temporales));
    memset (contexto.buffer_argumentos, 0, sizeof (contexto.buffer_argumentos));

    // Se inicializa el tope del búfer de argumentos.
    contexto.tope_argumentos = 0;

    debug_assembly (&contexto, "Comenzando la generación de código Assembly x86-64.");

    // 1. Generar la sección de datos globales (.data).
    escribir_seccion_datos (ts, &contexto);

    // 2. Generar la sección de código ejecutable (.text).
    fprintf (contexto.archivo, "\n.text\n");

    NodoCuadruplo *actual = lista -> primero;
    while (actual != NULL && !contexto.error) {
        // En el nivel superior (global), solo se procesan las declaraciones de funciones.
        if (actual -> instruccion.op == CI_INICIO_FUNCION) {
            procesar_funcion (&actual, &contexto);
        }
        else {
            actual = actual -> siguiente;
        }
    }

    if (contexto.error) {
        debug_assembly (&contexto, "La generación finalizó con errores.");
        return 0;
    }

    // Directiva de seguridad para evitar warning de GNU-stack en Linux.
    fprintf (contexto.archivo, "\n.section .note.GNU-stack,\"\",@progbits\n");

    debug_assembly (&contexto, "Código Assembly generado correctamente.");
    return 1;
}

/* =========== Funciones auxiliares privadas =========== */

/**
 * Escribe la sección .data recorriendo el nivel más bajo (global) de la TS.
 * 
 * Se reserva un quadword (8 bytes) inicializado en 0 para cada variable global.
 */
static void escribir_seccion_datos (TablaSimbolos *ts, ContextoAssembly *contexto) {
    fprintf (contexto -> archivo, ".data\n");

    // Se retrocede hasta el nivel global de la TS.
    Nivel *nivel = ts -> nivel_actual;
    while (nivel != NULL && nivel -> anterior != NULL) {
        nivel = nivel -> anterior;
    }

    if (nivel != NULL) {
        Simbolo *actual = nivel -> simbolos;
        while (actual != NULL) {
            // Solamente las variables globales reservan espacio real en memoria (.data).
            if (actual -> es_global && actual -> clase == SIMBOLO_VARIABLE) {
                // .quad reserva 8 bytes, manteniendo la alineación x86-64.
                fprintf (contexto -> archivo, "%s:\n\t.quad 0\n", actual -> nombre);
            }

            actual = actual -> siguiente;
        }
    }
}

/**
 * Recorre y traduce los cuádruplos pertenecientes al cuerpo de una función.
 */
static void procesar_funcion (NodoCuadruplo **actual_ptr, ContextoAssembly *contexto) {
    NodoCuadruplo *actual = *actual_ptr;
    char ref1 [64], ref2 [64], res [64];

    char *nombre_func = actual -> instruccion.arg1.nombre;

    // Directiva para que el sistema operativo encuentre el punto de entrada 'main'.
    if (strcmp (nombre_func, "main") == 0) {
        fprintf (contexto -> archivo, ".globl main\n");
    }

    // Etiqueta de la función.
    fprintf (contexto -> archivo, "%s:\n", nombre_func);

    // Se calculan todos los offsets para dimensionar el Stack Frame.
    int bytes_locales = 0;
    int cantidad_parametros = 0;
    calcular_entorno_funcion (actual, contexto, &bytes_locales, &cantidad_parametros);

    // Prólogo: 'enter' guarda el %rbp viejo y resta 'bytes_locales' al %rsp.
    fprintf (contexto -> archivo, "\tenter $(%d), $0\n", bytes_locales);

    // Las funciones reciben los argumentos (los primeros 6) a través de registros.
    NodoCuadruplo *param_nodo = actual -> siguiente;
    for (int i = 0; i < cantidad_parametros && i < 6; i ++) {
        // Se pasan los parámetros a la pila.
        if (param_nodo -> instruccion.op == CI_PARAMETRO) {
            obtener_referencia_operando (&param_nodo -> instruccion.arg1, contexto, ref1);
            fprintf (contexto -> archivo, "\tmovq %s, %s\n", REGISTROS_ARG [i], ref1);
            param_nodo = param_nodo -> siguiente;
        }
    }

    actual = actual -> siguiente;

    // Bandera para evitar doble epílogo.
    int ultimo_fue_return = 0;

    // Generación instrucción por instrucción hasta encontrar CI_FIN_FUNCION.
    while (actual != NULL && actual -> instruccion.op != CI_FIN_FUNCION && !contexto -> error) {
        Cuadruplo inst = actual -> instruccion;

        // Actualmente no se genera código Assembly para las operaciones que involucren el tipo float.
        if ((inst.arg1.tipo != OPERANDO_VACIO && inst.arg1.tipo_dato == TIPO_FLOAT) ||
            (inst.arg2.tipo != OPERANDO_VACIO && inst.arg2.tipo_dato == TIPO_FLOAT) ||
            (inst.resultado.tipo != OPERANDO_VACIO && inst.resultado.tipo_dato == TIPO_FLOAT)) {
            
            fprintf (contexto -> archivo, "\t# TODO: Implementar coerción/operación float (%s).\n", nombre_operacion (inst.op));
            actual = actual -> siguiente;
            continue;
        }

        if (inst.arg1.tipo != OPERANDO_VACIO) {
            obtener_referencia_operando (&inst.arg1, contexto, ref1);
        }

        if (inst.arg2.tipo != OPERANDO_VACIO) {
            obtener_referencia_operando (&inst.arg2, contexto, ref2);
        }

        if (inst.resultado.tipo != OPERANDO_VACIO) {
            obtener_referencia_operando (&inst.resultado, contexto, res);
        }

        if (contexto -> modo_debug) {
            char msg_debug [128];
            sprintf (msg_debug, "Generando instrucción: %s", nombre_operacion (inst.op));
            debug_assembly (contexto, msg_debug);
        }

        switch (inst.op) {
            case CI_ASIGNAR:
                // No se puede hacer mov directo memoria-memoria. Se pasa por %r10.
                fprintf (contexto -> archivo, "\tmovq %s, %%r10\n", ref1);
                fprintf (contexto -> archivo, "\tmovq %%r10, %s\n", res);
                break;

            case CI_SUMAR:
                fprintf (contexto -> archivo, "\tmovq %s, %%r10\n", ref1);
                fprintf (contexto -> archivo, "\taddq %s, %%r10\n", ref2);
                fprintf (contexto -> archivo, "\tmovq %%r10, %s\n", res);
                break;

            case CI_RESTAR:
                fprintf (contexto -> archivo, "\tmovq %s, %%r10\n", ref1);
                // La instrucción SUB resta el origen al destino (destino = destino - origen).
                fprintf (contexto -> archivo, "\tsubq %s, %%r10\n", ref2);
                fprintf (contexto -> archivo, "\tmovq %%r10, %s\n", res);
                break;

            case CI_MULTIPLICAR:
                fprintf (contexto -> archivo, "\tmovq %s, %%r10\n", ref1);
                fprintf (contexto -> archivo, "\timulq %s, %%r10\n", ref2);
                fprintf (contexto -> archivo, "\tmovq %%r10, %s\n", res);
                break;

            case CI_DIVIDIR:
                // La división entera requiere que el dividendo esté en %rax (y su signo extendido en %rdx mediante 'cqo').
                fprintf (contexto -> archivo, "\tmovq %s, %%rax\n", ref1);
                fprintf (contexto -> archivo, "\tcqo\n");
                fprintf (contexto -> archivo, "\tmovq %s, %%r10\n", ref2);
                // idiv divide rdx:rax por r10. El cociente queda en rax.
                fprintf (contexto -> archivo, "\tidivq %%r10\n");
                fprintf (contexto -> archivo, "\tmovq %%rax, %s\n", res);
                break;

            case CI_MODULO:
                // Similar a la división, pero el resto de la operación queda almacenado en %rdx.
                fprintf (contexto -> archivo, "\tmovq %s, %%rax\n", ref1);
                fprintf (contexto -> archivo, "\tcqo\n");
                fprintf (contexto -> archivo, "\tmovq %s, %%r10\n", ref2);
                fprintf (contexto -> archivo, "\tidivq %%r10\n");
                fprintf (contexto -> archivo, "\tmovq %%rdx, %s\n", res);
                break;

            case CI_MENOS_UNARIO:
                fprintf (contexto -> archivo, "\tmovq %s, %%r10\n", ref1);
                // neg calcula el complemento a 2 (cambia el signo).
                fprintf (contexto -> archivo, "\tnegq %%r10\n");
                fprintf (contexto -> archivo, "\tmovq %%r10, %s\n", res);
                break;

            case CI_MENOR:
            case CI_MAYOR:
            case CI_IGUAL:
                // Las comparaciones requieren que ambos registros estén en operandos.
                fprintf (contexto -> archivo, "\tmovq %s, %%r10\n", ref1);
                fprintf (contexto -> archivo, "\tmovq %s, %%r11\n", ref2);
                // Compara r11 contra r10 y actualiza banderas.
                fprintf (contexto -> archivo, "\tcmpq %%r11, %%r10\n");

                // Se coloca 0 en r11 y 1 en r10 para hacer el movimiento condicional.
                fprintf (contexto -> archivo, "\tmovq $0, %%r11\n");
                fprintf (contexto -> archivo, "\tmovq $1, %%r10\n");

                if (inst.op == CI_MENOR) {
                    fprintf (contexto -> archivo, "\tcmovl %%r10, %%r11\n");
                }
                else if (inst.op == CI_MAYOR) {
                    fprintf (contexto -> archivo, "\tcmovg %%r10, %%r11\n");
                }
                else if (inst.op == CI_IGUAL) {
                    fprintf (contexto -> archivo, "\tcmove %%r10, %%r11\n");
                }

                fprintf (contexto -> archivo, "\tmovq %%r11, %s\n", res);
                break;

            case CI_ETIQUETA:
                fprintf (contexto -> archivo, "%s:\n", res);
                break;

            case CI_SALTO:
                fprintf (contexto -> archivo, "\tjmp %s\n", res);
                break;

            case CI_SALTO_SI_FALSO:
                fprintf (contexto -> archivo, "\tmovq %s, %%r10\n", ref1);
                fprintf (contexto -> archivo, "\tcmpq $0, %%r10\n");
                // Si es igual a 0, es falso.
                fprintf (contexto -> archivo, "\tje %s\n", res);
                break;

            case CI_ARGUMENTO:
                int arg_idx = atoi (inst.arg2.nombre);

                // Los primeros 6 argumentos se mandan a los registros correspondientes.
                if (arg_idx < 6) {
                    fprintf (contexto -> archivo, "\tmovq %s, %%r10\n", ref1);
                    fprintf (contexto -> archivo, "\tmovq %%r10, %s\n", REGISTROS_ARG [arg_idx]);
                }
                // Los argumentos del 7 en adelante se guardan temporalmente en el búfer.
                else {
                    contexto -> buffer_argumentos [contexto -> tope_argumentos ++] = inst.arg1;
                }
                break;

            case CI_LLAMADA: {
                int args_totales = atoi (inst.arg2.nombre);
                int args_en_pila = args_totales > 6 ? args_totales - 6 : 0;

                // Si hay argumentos en pila, se vacía el búfer en orden inverso (LIFO).
                for (int i = 1; i <= args_en_pila; i ++) {
                    Operando arg_op = contexto -> buffer_argumentos [contexto -> tope_argumentos - i];
                    obtener_referencia_operando (&arg_op, contexto, ref1);

                    fprintf (contexto -> archivo, "\tmovq %s, %%r10\n", ref1);
                    fprintf (contexto -> archivo, "\tpushq %%r10\n");
                }

                // Se actualiza el tope del búfer.
                contexto -> tope_argumentos -= args_en_pila;

                // Convención Linux ABI para funciones con cantidad de argumentos variables o externos.
                if (strcmp (inst.arg1.nombre, "printInt") == 0 || strcmp (inst.arg1.nombre, "get_int") == 0) {
                    fprintf (contexto -> archivo, "\tmovq $0, %%rax\n");
                }

                fprintf (contexto -> archivo, "\tcall %s\n", inst.arg1.nombre);

                // El llamador (caller) es responsable de limpiar los argumentos apilados.
                if (args_en_pila > 0) {
                    fprintf (contexto -> archivo, "\taddq $%d, %%rsp\n", args_en_pila * 8);
                }

                if (inst.resultado.tipo != OPERANDO_VACIO) {
                    fprintf (contexto -> archivo, "\tmovq %%rax, %s\n", res);
                }
                break;
            }

            case CI_RETURN:
                if (inst.arg1.tipo != OPERANDO_VACIO) {
                    fprintf (contexto -> archivo, "\tmovq %s, %%rax\n", ref1);
                }
                else if (strcmp (nombre_func, "main") == 0) {
                    // Por seguridad semántica, un return sin valor en main igual devuelve código de éxito (0).
                    fprintf (contexto -> archivo, "\tmovq $0, %%rax\n");
                }

                fprintf (contexto -> archivo, "\tleave\n\tret\n");
                break;

            default:
                break;
        }

        // Se actualiza la bandera si hubo un retorno.
        ultimo_fue_return = (inst.op == CI_RETURN);

        actual = actual -> siguiente;
    }

    // Epílogo si la función llegó al final sin un return explícito.
    if (!ultimo_fue_return) {
        if (strcmp (nombre_func, "main") == 0) {
            fprintf (contexto -> archivo, "\tmovq $0, %%rax\n");
        }

        fprintf (contexto -> archivo, "\tleave\n\tret\n");
    }
    
    // Se avanza el puntero externo al nodo que se procesó por último.
    *actual_ptr = actual;
}

/**
 * Realiza una pasada previa sobre los cuádruplos de una función para
 * asignar offsets a parámetros, variables locales y temporales, y
 * calcular el tamaño total del Stack Frame (entorno).
 */
static void calcular_entorno_funcion (NodoCuadruplo *inicio, ContextoAssembly *contexto, int *bytes_locales, int *cantidad_parametros) {
    // La pila crece hacia direcciones menores. El primer dato local se ubica en -8(%rbp).
    int offset_local = -8;
    // Los argumentos en pila comienzan en 16(%rbp).
    int offset_param_pila = 16;
    *cantidad_parametros = 0;

    // Se limpian los arreglos.
    memset (contexto -> offset_temporales, 0, sizeof (contexto -> offset_temporales));
    memset (contexto -> buffer_argumentos, 0, sizeof (contexto -> buffer_argumentos));

    NodoCuadruplo *actual = inicio -> siguiente;

    // 1. Asignar offset a los parámetros en el orden en que fueron declarados.
    while (actual != NULL && actual -> instruccion.op == CI_PARAMETRO) {
        Simbolo *simb = actual -> instruccion.arg1.simbolo;
        
        if (simb != NULL && simb -> offset == -1) {
            if (*cantidad_parametros < 6) {
                // Los primeros 6 bajan a la memoria local.
                simb -> offset = offset_local;
                offset_local -= 8;
            }
            else {
                // Del 7 en adelante, se leen directo desde la pila (frame anterior).
                simb -> offset = offset_param_pila;
                offset_param_pila += 8;
            }
            
            (*cantidad_parametros) ++;
        }

        actual = actual -> siguiente;
    }

    // 2. Asignar offset a las variables locales y a los temporales a medida que aparecen.
    actual = inicio -> siguiente;
    while (actual != NULL && actual -> instruccion.op != CI_FIN_FUNCION) {
        // Se revisa los tres posibles operandos del cuádruplo.
        Operando *ops [] = {&actual -> instruccion.arg1, &actual -> instruccion.arg2, &actual -> instruccion.resultado};

        for (int i = 0; i < 3; i ++) {
            if (ops [i] -> tipo == OPERANDO_SIMBOLO) {
                Simbolo *simb = ops [i] -> simbolo;

                // Si es local y aún no tiene offset, se le asigna.
                if (simb != NULL && !simb -> es_global && !simb -> es_externa && simb -> offset == -1) {
                    simb -> offset = offset_local;

                    offset_local -= 8;
                }
            }
            else if (ops [i] -> tipo == OPERANDO_TEMPORAL) {
                // Se extrae el número del temporal.
                int num = atoi (ops [i] -> nombre + 1);

                if (num < MAX_TEMPORALES && contexto -> offset_temporales [num] == 0) {
                    contexto -> offset_temporales [num] = offset_local;

                    offset_local -= 8;
                }
            }
        }

        actual = actual -> siguiente;
    }

    // Si no se asignó nada, offset_local será -8. El tamaño total debe ser positivo.
    if (offset_local == -8) {
        *bytes_locales = 0;
    }
    else {
        int tamano = -(offset_local + 8);
        // Alineación estricta a 16 bytes exigida por la System V ABI.
        if (tamano % 16 != 0) {
            tamano = ((tamano + 15) / 16) * 16;
        }
        *bytes_locales = tamano;
    }
}

/**
 * Traduce un Operando del código intermedio a su sintaxis x86-64 y la guarda en el buffer.
 */
static void obtener_referencia_operando (Operando *op, ContextoAssembly *contexto, char *buffer) {
    if (op -> tipo == OPERANDO_CONSTANTE) {
        // Los valores booleanos "true" y "false" se convierten a '1' y '0', respectivamente.
        if (strcasecmp (op -> nombre, "true") == 0) {
            sprintf (buffer, "$1");
        }
        else if (strcasecmp (op -> nombre, "false") == 0) {
            sprintf (buffer, "$0");
        }
        else {
            // Valores inmediatos enteros requieren el prefijo '$'.
            sprintf (buffer, "$%s", op -> nombre);
        }
        
    }
    else if (op -> tipo == OPERANDO_TEMPORAL) {
        int num = atoi (op -> nombre + 1);
        //Desplazamiento relativo al Base Pointer (%rbp).
        sprintf (buffer, "%d(%%rbp)", contexto -> offset_temporales [num]);
    }
    else if (op -> tipo == OPERANDO_SIMBOLO) {
        if (op -> simbolo -> es_global) {
            // Las variables globales usan direccionamiento relativo al Instruction Pointer (%rip).
            sprintf (buffer, "%s(%%rip)", op -> nombre);
        }
        else {
            // Las variables locales y los parámetros usan el offset de la pila (%rbp).
            sprintf (buffer, "%d(%%rbp)", op -> simbolo -> offset);
        }
    }
    else if (op -> tipo == OPERANDO_ETIQUETA) {
        // Las etiquetas locales inician con '.'.
        sprintf (buffer, ".%s", op -> nombre);
    }
}

/**
 * Imprime un mensaje de depuración cuando el modo debug está activo.
 */
static void debug_assembly (ContextoAssembly *contexto, const char *mensaje) {
    if (contexto -> modo_debug) {
        printf ("[ASSEMBLY] %s\n", mensaje);
    }
}

/**
 * Registra un error y marca el contexto para detener la generación.
 */
static void registrar_error (ContextoAssembly *contexto, const char *mensaje) {
    contexto -> error = 1;
    fprintf (stderr, "ERROR EN GENERACIÓN DE ASSEMBLY: %s\n", mensaje);
}
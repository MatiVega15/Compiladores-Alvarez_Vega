#include "OptimizadorAST.h"

#include <stdlib.h>
#include <stdio.h>

/* Funciones auxiliares privadas. */
static int pase_plegado_constantes (NodoAST *nodo, int modo_debug, int *total_transformaciones);
static int es_nodo_operacion (TipoNodo tipo);
static int es_literal_constante (const NodoAST *nodo);
static int calcular_resultado_operacion (NodoAST *nodo, TipoNodo *tipo_res, TipoDato *tipo_dato_res, ValorNodo *valor_res);
static void reemplazar_hijo_por_constante (NodoAST *padre, int indice_hijo, TipoNodo tipo_nuevo, TipoDato tipo_dato_nuevo, ValorNodo valor_nuevo, int modo_debug, int *total_transformaciones);
static const char *nombre_tipo_nodo (TipoNodo tipo);

/* =========== Función principal =========== */

NodoAST *optimizar_ast (NodoAST *ast, OpcionesOptimizacion opciones) {
    // Se validan los argumentos.
    if (ast == NULL || ast -> tipo != AST_PROGRAMA) {
        return ast;
    }

    if (opciones.modo_debug) {
        printf ("[OPTIMIZADOR] Comienza la optimización del AST.\n");
    }

    // Si el usuario activó la optimización de constantes.
    if (opciones.plegado_constantes) {
        int se_hizo_cambio = 1;
        int numero_pasada = 0;
        int total_transformaciones = 0;

        if (opciones.modo_debug) {
            printf ("[OPTIMIZADOR] Pase activado: Plegado de Constantes (Constant Folding).\n");
        }

        // Se repite el pase mientras se encuentren operaciones para plegar.
        while (se_hizo_cambio) {
            numero_pasada ++;
            if (opciones.modo_debug) {
                printf ("[OPTIMIZADOR] Iniciando pasada %d...\n", numero_pasada);
            }
            
            se_hizo_cambio = pase_plegado_constantes (ast, opciones.modo_debug, &total_transformaciones);
        }

        if (opciones.modo_debug) {
            printf ("[OPTIMIZADOR] Plegado de constantes finalizado.\n");
            printf ("[OPTIMIZADOR] Resumen: %d transformación(es) en %d pasada(s).\n", total_transformaciones, numero_pasada);
        }
    }

    if (opciones.modo_debug) {
        printf ("[OPTIMIZADOR] Optimización del AST finalizada correctamente.\n");
    }

    return ast;
}

/* =========== Funciones auxiliares privadas =========== */

/**
 * Recorre el AST de forma recursiva (Post-Order / Bottom-Up, de abajo hacia arriba).
 * 
 * Primero procesa todos los subárboles hijos de forma recursiva, luego revisa si los
 * hijos del nodo actual son operaciones con operandos constantes.
 * 
 * Retorna 1 si modificó al menos una operación en el árbol, o 0 si no hubo cambios.
 */
static int pase_plegado_constantes (NodoAST *nodo, int modo_debug, int *total_transformaciones) {
    // Caso base.
    if (nodo == NULL) {
        return 0;
    }

    int hubo_cambios = 0;

    // Se recorren todos los hijos de forma recursiva (Post-Order).
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        if (pase_plegado_constantes (nodo -> hijos [i], modo_debug, total_transformaciones)) {
            hubo_cambios = 1;
        }
    }

    // Se intenta plegar los hijos que representen una operación.
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        NodoAST *hijo = nodo -> hijos [i];

        if (hijo != NULL && es_nodo_operacion (hijo -> tipo)) {
            TipoNodo tipo_nuevo;
            TipoDato tipo_dato_nuevo;
            ValorNodo valor_nuevo;

            // Si la operación del hijo puede calcularse por completo.
            if (calcular_resultado_operacion (hijo, &tipo_nuevo, &tipo_dato_nuevo, &valor_nuevo)) {
                reemplazar_hijo_por_constante (nodo, i, tipo_nuevo, tipo_dato_nuevo, valor_nuevo, modo_debug, total_transformaciones);
                hubo_cambios = 1;
            }
        }
    }

    return hubo_cambios;
}

/**
 * Determina si un tipo de nodo representa una operación evaluable,
 * es decir, una operación aritmética, relacional o lógica.
 */
static int es_nodo_operacion (TipoNodo tipo) {
    switch (tipo) {
        case AST_SUMA:
        case AST_RESTA:
        case AST_MULTIPLICACION:
        case AST_DIVISION:
        case AST_MODULO:
        case AST_MENOS_UNARIO:
        case AST_MENOR:
        case AST_MAYOR:
        case AST_IGUAL:
        case AST_AND:
        case AST_OR:
        case AST_NEGACION:
            return 1;
        default:
            return 0;
    }
}

/**
 * Determina si un nodo del AST es un valor literal constante,
 * es decir, entero, real o booleano.
 */
static int es_literal_constante (const NodoAST *nodo) {
    // Se valida el argumento.
    if (nodo == NULL) {
        return 0;
    }

    switch (nodo -> tipo) {
        case AST_NUMERO:
        case AST_REAL:
        case AST_TRUE:
        case AST_FALSE:
            return 1;
        default:
            return 0;
    }
}

/**
 * Intenta calcular el resultado de una operación cuyo operando(s)
 * son literales constantes.
 * 
 * Si la operación se puede resolver:
 *  - Almacena las propiedades del resultado en las variables de salida.
 *  - Retorna 1 para indicar éxito.
 * 
 * Si no se puede resolver:
 *  - Retorna 0 sin modificar nada.
 */
static int calcular_resultado_operacion (NodoAST *nodo, TipoNodo *tipo_res, TipoDato *tipo_dato_res, ValorNodo *valor_res) {
    // Se valida el argumento.
    if (nodo == NULL) {
        return 0;
    }

    // --- OPERACIONES UNARIAS ---

    // Se procesa la inversión de signo numérico (- unario).
    if (nodo -> tipo == AST_MENOS_UNARIO) {
        NodoAST *op = nodo -> hijos [0];
        
        // El operando debe ser una constante literal.
        if (!es_literal_constante (op)) {
            return 0;
        }

        // Caso entero: se invierte el valor y se asigna el tipo INT.
        if (op -> tipo == AST_NUMERO) {
            *tipo_res = AST_NUMERO;
            *tipo_dato_res = TIPO_INT;
            valor_res -> numero = -(op -> valor.numero);

            return 1;
        }
        // Caso real: se invierte el valor flotante y se asigna el tipo FLOAT.
        else if (op -> tipo == AST_REAL) {
            *tipo_res = AST_REAL;
            *tipo_dato_res = TIPO_FLOAT;
            valor_res -> real = -(op -> valor.real);

            return 1;
        }

        return 0;
    }

    // Se procesa la negación lógica (! unario).
    if (nodo -> tipo == AST_NEGACION) {
        NodoAST *op = nodo -> hijos [0];
        
        // El operando debe ser un literal booleano.
        if (!es_literal_constante (op)) {
            return 0;
        }

        // Inversión booleana: !true -> false.
        if (op -> tipo == AST_TRUE) {
            *tipo_res = AST_FALSE;
            *tipo_dato_res = TIPO_BOOLEAN;

            return 1;
        }
        // Inversión booleana: !false -> true.
        else if (op -> tipo == AST_FALSE) {
            *tipo_res = AST_TRUE;
            *tipo_dato_res = TIPO_BOOLEAN;

            return 1;
        }

        return 0;
    }

    // --- OPERACIONES BINARIAS ---

    // Una operación binaria requiere dos hijos.
    if (nodo -> cantidad_hijos < 2) {
        return 0;
    }

    NodoAST *izq = nodo -> hijos [0];
    NodoAST *der = nodo -> hijos [1];

    // Ambos operandos deben ser literales constantes conocidos.
    if (!es_literal_constante (izq) || !es_literal_constante (der)) {
        return 0;
    }

    // --- Operaciones aritméticas ---
    if (nodo -> tipo == AST_SUMA ||
        nodo -> tipo == AST_RESTA ||
        nodo -> tipo == AST_MULTIPLICACION ||
        nodo -> tipo == AST_DIVISION ||
        nodo -> tipo == AST_MODULO) {
        
        // Ambos operandos deben ser numéricos (enteros o reales).
        if ((izq -> tipo != AST_NUMERO && izq -> tipo != AST_REAL) ||
            (der -> tipo != AST_NUMERO && der -> tipo != AST_REAL)) {
            
            return 0;
        }

        // Se extraen los valores representados como double para facilitar el cálculo unificado.
        double val_izq, val_der;
        
        if (izq -> tipo == AST_NUMERO) {
            val_izq = (double) izq -> valor.numero;
        }
        else {
            val_izq = izq -> valor.real;
        }

        if (der -> tipo == AST_NUMERO) {
            val_der = (double) der -> valor.numero;
        }
        else {
            val_der = der -> valor.real;
        }

        // Evitar división o módulo por cero en tiempo de compilación.
        if ((nodo -> tipo == AST_DIVISION || nodo -> tipo == AST_MODULO) && val_der == 0) {
            return 0;
        }

        // Caso A: Operación estrictamente entre enteros (excepto la división).
        if (izq -> tipo == AST_NUMERO && der -> tipo == AST_NUMERO && nodo -> tipo != AST_DIVISION) {
            int i_izq = izq -> valor.numero;
            int i_der = der -> valor.numero;
            int res = 0;

            if (nodo -> tipo == AST_SUMA) {
                res = i_izq + i_der;
            }
            else if (nodo -> tipo == AST_RESTA) {
                res = i_izq - i_der;
            }
            else if (nodo -> tipo == AST_MULTIPLICACION) {
                res = i_izq * i_der;
            }
            else if (nodo -> tipo == AST_MODULO) {
                res = i_izq % i_der;
            }

            // El resultado es un literal entero.
            *tipo_res = AST_NUMERO;
            *tipo_dato_res = TIPO_INT;
            valor_res -> numero = res;

            return 1;
        }
        // Caso B: Operación con números reales o división entre enteros.
        else {
            double res = 0.0;

            if (nodo -> tipo == AST_SUMA) {
                res = val_izq + val_der;
            }
            else if (nodo -> tipo == AST_RESTA) {
                res = val_izq - val_der;
            }
            else if (nodo -> tipo == AST_MULTIPLICACION) {
                res = val_izq * val_der;
            }
            else if (nodo -> tipo == AST_DIVISION) {
                // Si ambos operandos son enteros, se aplica división entera truncada.
                if (izq -> tipo == AST_NUMERO && der -> tipo == AST_NUMERO) {
                    *tipo_res = AST_NUMERO;
                    *tipo_dato_res = TIPO_INT;
                    valor_res -> numero = izq -> valor.numero / der -> valor.numero;

                    return 1;
                }

                // División en punto flotante si al menos uno es flotante.
                res = val_izq / val_der;
            }

            // El resultado es un literal flotante.
            *tipo_res = AST_REAL;
            *tipo_dato_res = TIPO_FLOAT;
            valor_res -> real = res;

            return 1;
        }
    }

    // --- Operaciones relacionales ---
    if (nodo -> tipo == AST_MENOR || nodo -> tipo == AST_MAYOR || nodo -> tipo == AST_IGUAL) {
        double val_izq = 0.0, val_der = 0.0;

        // Se obtiene el valor escalar del operando izquierdo.
        if (izq -> tipo == AST_NUMERO) {
            val_izq = (double) izq -> valor.numero;
        }
        else if (izq -> tipo == AST_REAL) {
            val_izq = izq -> valor.real;
        }
        else if (izq -> tipo == AST_TRUE) {
            val_izq = 1.0;
        }
        else if (izq -> tipo == AST_FALSE) {
            val_izq = 0.0;
        }

        // Se obtiene el valor escalar del operando derecho.
        if (der -> tipo == AST_NUMERO) {
            val_der = (double) der -> valor.numero;
        }
        else if (der -> tipo == AST_REAL) {
            val_der = der -> valor.real;
        }
        else if (der -> tipo == AST_TRUE) {
            val_der = 1.0;
        }
        else if (der -> tipo == AST_FALSE) {
            val_der = 0.0;
        }

        int cumple = 0;

        // Evaluación de la condición relacional.
        if (nodo -> tipo == AST_MENOR) {
            cumple = (val_izq < val_der);
        }
        else if (nodo -> tipo == AST_MAYOR) {
            cumple = (val_izq > val_der);
        }
        else if (nodo -> tipo == AST_IGUAL) {
            cumple = (val_izq == val_der);
        }

        // El resultado siempre es un literal booleano.
        if (cumple) {
            *tipo_res = AST_TRUE;
        }
        else {
            *tipo_res = AST_FALSE;
        }
        *tipo_dato_res = TIPO_BOOLEAN;

        return 1;
    }

    // --- Operaciones lógicas ---
    if (nodo -> tipo == AST_AND || nodo -> tipo == AST_OR) {
        // Ambos operandos deben ser estrictamente valores booleanos.
        if ((izq -> tipo != AST_TRUE && izq -> tipo != AST_FALSE) ||
            (der -> tipo != AST_TRUE && der -> tipo != AST_FALSE)) {
            
            return 0;
        }

        // Se convierten a 1 (verdadero) o 0 (falso).
        int b_izq = (izq -> tipo == AST_TRUE);
        int b_der = (der -> tipo == AST_TRUE);
        int cumple = 0;

        // Evaluación del operador lógico.
        if (nodo -> tipo == AST_AND) {
            cumple = (b_izq && b_der);
        }
        else if (nodo -> tipo == AST_OR) {
            cumple = (b_izq || b_der);
        }

        // Asignación del nodo booleano resultante.
        if (cumple) {
            *tipo_res = AST_TRUE;
        }
        else {
            *tipo_res = AST_FALSE;
        }
        *tipo_dato_res = TIPO_BOOLEAN;

        return 1;
    }

    // La operación no pertenece a ninguna categoría evaluable en compilación.
    return 0;
}

/**
 * Crea un nuevo nodo constante hoja con el resultado calculado, lo
 * coloca en la posición del hijo correspondiente en el padre y
 * libera la memoria del subárbol anterior.
 */
static void reemplazar_hijo_por_constante (NodoAST *padre, int indice_hijo, TipoNodo tipo_nuevo, TipoDato tipo_dato_nuevo, ValorNodo valor_nuevo, int modo_debug, int *total_transformaciones) {
    NodoAST *rama_vieja = padre -> hijos [indice_hijo];
    
    // Se crea el nuevo nodo hoja reutilizando la línea y columna original.
    NodoAST *nodo_nuevo = crear_nodo (tipo_nuevo, rama_vieja -> linea, rama_vieja -> columna);
    nodo_nuevo -> tipo_dato = tipo_dato_nuevo;
    nodo_nuevo -> valor = valor_nuevo;

    if (modo_debug) {
        printf ("[OPTIMIZADOR] Línea %d, Columna %d: Operación '%s' replegada a literal constante ", rama_vieja -> linea, rama_vieja -> columna, nombre_tipo_nodo (rama_vieja -> tipo));
    
        if (tipo_nuevo == AST_NUMERO) {
            printf ("(int : %d).\n", valor_nuevo.numero);
        }
        else if (tipo_nuevo == AST_REAL) {
            printf ("(float: %g).\n", valor_nuevo.real);
        }
        else if (tipo_nuevo == AST_TRUE) {
            printf ("(boolean: true).\n");
        }
        else if (tipo_nuevo == AST_FALSE) {
            printf ("(boolean: false).\n");
        }
    }

    // Se coloca el nuevo nodo en el arreglo del padre.
    padre -> hijos [indice_hijo] = nodo_nuevo;

    // Se destruye el subárbol anterior de la operación.
    liberar_arbol (rama_vieja);

    if (total_transformaciones != NULL) {
        (*total_transformaciones) ++;
    }
}

/**
 * Retorna una representación textual del tipo de nodo dado.
 */
static const char *nombre_tipo_nodo (TipoNodo tipo) {
    switch (tipo) {
        case AST_SUMA:
            return "+";
        case AST_RESTA:
            return "-";
        case AST_MULTIPLICACION:
            return "*";
        case AST_DIVISION:
            return "/";
        case AST_MODULO:
            return "%";
        case AST_MENOS_UNARIO:
            return "- (unario)";
        case AST_MENOR:
            return "<";
        case AST_MAYOR:
            return ">";
        case AST_IGUAL:
            return "==";
        case AST_AND:
            return "&&";
        case AST_OR:
            return "||";
        case AST_NEGACION:
            return "!";
        default:
            return "Operación desconocida";
    }
}
#include "OptimizadorAST.h"

#include <stdlib.h>
#include <stdio.h>

/* Funciones auxiliares privadas. */
static int pase_plegado_constantes (NodoAST *nodo, int modo_debug, int *total_transformaciones);
static int pase_eliminacion_codigo_muerto (NodoAST *nodo, int modo_debug, int *total_transformaciones);
static int es_nodo_operacion (TipoNodo tipo);
static int es_literal_constante (const NodoAST *nodo);
static int calcular_resultado_operacion (NodoAST *nodo, TipoNodo *tipo_res, TipoDato *tipo_dato_res, ValorNodo *valor_res);
static void reemplazar_hijo_por_constante (NodoAST *padre, int indice_hijo, TipoNodo tipo_nuevo, TipoDato tipo_dato_nuevo, ValorNodo valor_nuevo, int modo_debug, int *total_transformaciones);
static void reemplazar_hijo_por_subarbol (NodoAST *padre, int indice, NodoAST *subarbol_nuevo, int modo_debug, int *total_transformaciones, const char *motivo);
static void eliminar_hijo_y_desplazar (NodoAST *padre, int indice, int modo_debug, int *total_transformaciones, const char *motivo);
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

    // Si el usuario activó la eliminación de código muerto.
    if (opciones.eliminacion_codigo_muerto) {
        int se_hizo_cambio = 1;
        int numero_pasada = 0;
        int total_transformaciones = 0;

        if (opciones.modo_debug) {
            printf ("[OPTIMIZADOR] Pase activado: Eliminación de código muerto (Dead Code Elimination).\n");
        }

        // Se repite el pase mientras se encuentre código muerto para eliminar.
        while (se_hizo_cambio) {
            numero_pasada ++;
            if (opciones.modo_debug) {
                printf ("[OPTIMIZADOR] Iniciando pasada %d...\n", numero_pasada);
            }
            
            se_hizo_cambio = pase_eliminacion_codigo_muerto (ast, opciones.modo_debug, &total_transformaciones);
        }

        if (opciones.modo_debug) {
            printf ("[OPTIMIZADOR] Eliminación de código muerto finalizada.\n");
            printf ("[OPTIMIZADOR] Resumen: %d nodo(s) o rama(s) podada(s) en %d pasada(s).\n", total_transformaciones, numero_pasada);
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
 * Recorre el AST de forma recursiva (Post-Order / Bottom-Up, de abajo hacia arriba)
 * para identificar y eliminar patrones de código inalcanzable (Dead Code).
 * 
 * Primero procesa todos los subárboles hijos de forma recursiva. Luego, evalúa
 * los hijos del nodo actual buscando los siguientes patrones:
 *  - IF (true) -> Reemplaza el condicional entero por su bloque THEN.
 *  - IF (false) -> Elimina el condicional, o lo reemplaza por su bloque ELSE (si existe).
 *  - WHILE (false) -> Elimina el ciclo completo.
 *  - Código post-RETURN -> Elimina las sentencias que le siguen dentro de un bloque.
 */
static int pase_eliminacion_codigo_muerto (NodoAST *nodo, int modo_debug, int *total_transformaciones) {
    // Caso base.
    if (nodo == NULL) {
        return 0;
    }

    int hubo_cambios = 0;

    // Se recorren todos los hijos de forma recursiva (Post-Order).
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        if (pase_eliminacion_codigo_muerto (nodo -> hijos [i], modo_debug, total_transformaciones)) {
            hubo_cambios = 1;
        }
    }

    // Revisa si los hijos actuales son código muerto.
    for (int i = 0; i < nodo -> cantidad_hijos; i ++) {
        NodoAST *hijo = nodo -> hijos [i];

        if (hijo == NULL) {
            continue;
        }

        // Patrón A: Condicionales IF.
        if (hijo -> tipo == AST_IF) {
            NodoAST *condicion = hijo -> hijos [0];

            if (condicion -> tipo == AST_TRUE) {
                // IF (true) -> Reemplazar el IF entero por el bloque THEN (hijo 1).
                reemplazar_hijo_por_subarbol (nodo, i, hijo -> hijos [1], modo_debug, total_transformaciones, "IF (true) -> Reemplazado por rama THEN");
                hubo_cambios = 1;
            }
            else if (condicion -> tipo == AST_FALSE) {
                if (hijo -> cantidad_hijos > 2) {
                    // IF (false) con ELSE -> Reemplazar por bloque ELSE (hijo 2).
                    reemplazar_hijo_por_subarbol (nodo, i, hijo -> hijos [2], modo_debug, total_transformaciones, "IF (false) -> Reemplazado por rama ELSE");
                }
                else {
                    // IF (false) sin ELSE -> Eliminar IF por completo.
                    eliminar_hijo_y_desplazar (nodo, i, modo_debug, total_transformaciones, "IF (false) -> Rama eliminada");
                    
                    // Se desplazaron los índices, hay que retroceder i.
                    i --;
                }

                hubo_cambios = 1;
            }
        }
        // Patrón B: Ciclo WHILE.
        else if (hijo -> tipo == AST_WHILE) {
            NodoAST *condicion = hijo -> hijos [0];

            if (condicion -> tipo == AST_FALSE) {
                // WHILE (false) -> Eliminar WHILE por completo.
                eliminar_hijo_y_desplazar (nodo, i, modo_debug, total_transformaciones, "WHILE (false) -> Ciclo inalcanzable eliminado");
                    
                // Se desplazaron los índices, hay que retroceder i.
                i --;
            
                hubo_cambios = 1;
            }
        }
        // Patrón C: Código inalcanzable post-RETURN.
        else if (hijo -> tipo == AST_RETURN && nodo -> tipo == AST_BLOQUE) {
            if (i < nodo -> cantidad_hijos - 1) {
                if (modo_debug) {
                    printf ("[OPTIMIZADOR] Línea %d, Columna %d: Sentencia RETURN trunca %d sentencia(s) inalcanzable(s) posterior(es).\n", hijo -> linea, hijo -> columna, (nodo -> cantidad_hijos - 1 - i));
                }

                // Se liberan todos los nodos siguientes.
                for (int j = i + 1; j < nodo -> cantidad_hijos; j ++) {
                    liberar_arbol (nodo -> hijos [j]);
                }

                // Se trunca el bloque.
                nodo -> cantidad_hijos = i + 1;
                if (total_transformaciones != NULL) {
                    (*total_transformaciones) ++;
                }

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
 * Reemplaza un nodo padre por uno de sus propios subárboles.
 * 
 * Desvincula el subárbol preservado para evitar que sea
 * liberado al destruir el padre original.
 */
static void reemplazar_hijo_por_subarbol (NodoAST *padre, int indice, NodoAST *subarbol_nuevo, int modo_debug, int *total_transformaciones, const char *motivo) {
    NodoAST *viejo = padre -> hijos [indice];

    // Se coloca el subárbol preservado en el lugar del nodo viejo.
    padre -> hijos [indice] = subarbol_nuevo;

    // Se desvincula el subárbol nuevo del viejo.
    if (subarbol_nuevo != NULL) {
        for (int i = 0; i < viejo -> cantidad_hijos; i ++) {
            if (viejo -> hijos [i] == subarbol_nuevo) {
                viejo -> hijos [i] = NULL;
                break;
            }
        }
    }

    if (modo_debug) {
        printf ("[OPTIMIZADOR] Línea %d, Columna %d: %s.\n", viejo -> linea, viejo -> columna, motivo);
    }

    // Se destruye el nodo viejo y todo su contenido (excepto lo desvinculado).
    liberar_arbol (viejo);

    if (total_transformaciones != NULL) {
        (*total_transformaciones) ++;
    }
}

/**
 * Elimina un nodo por completo y compacta el arreglo de hijos
 * desplazándolo hacia la izquierda.
 */
static void eliminar_hijo_y_desplazar (NodoAST *padre, int indice, int modo_debug, int *total_transformaciones, const char *motivo) {
    NodoAST *viejo = padre -> hijos [indice];

    if (modo_debug) {
        printf ("[OPTIMIZADOR] Línea %d, Columna %d: %s.\n", viejo -> linea, viejo -> columna, motivo);
    }

    // Se destruye la rama muerta.
    liberar_arbol (viejo);

    // Se desplazan los hijos siguientes una posición hacia la izquierda.
    for (int i = indice; i < padre -> cantidad_hijos - 1; i ++) {
        padre -> hijos [i] = padre -> hijos [i + 1];
    }

    // Se limpia la última posición y se achica el contador.
    padre -> hijos [padre -> cantidad_hijos - 1] = NULL;
    padre -> cantidad_hijos --;

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
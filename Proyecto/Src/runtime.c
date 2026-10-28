#include <stdio.h>

/**
 * Lee un número entero desde la entrada estándar (terminal).
 * 
 * Implementa la función externa get_int () requerida por los programas C-TDS. 
 * 
 * Utiliza scanf para capturar el valor numérico y lo retorna directamente.
 */
int get_int (void) {
    int valor = 0;

    // Lee un entero de la entrada estándar.
    if (scanf ("%d", &valor) != 1) {
        // Valor por defecto ante un fallo de lectura.
        valor = 0;
    }

    return valor;
}

/**
 * Imprime un número entero seguido de un salto de línea.
 * 
 * Implementa la función externa printInt () requerida por los programas C-TDS.
 * 
 * Muestra el resultado formateado por la salida estándar (terminal).
 */
void printInt (int valor) {
    printf ("%d\n", valor);
}
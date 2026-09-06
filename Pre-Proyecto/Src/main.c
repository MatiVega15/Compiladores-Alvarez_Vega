#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AST.h"
#include "TS.h"
#include "AnalizadorSemantico.h"
#include "Interprete.h"

// Variables y funciones definidas por Bison.
extern FILE *yyin;
extern int yyparse (void);
extern NodoAST *arbol;
extern int error_lexico;

int main (int argc, char **argv) {
    TablaSimbolos *ts;
    ValorRuntime valor_retorno;
    int resultado;
    int errores_semanticos = 0;
    int resultado_interpretacion = 0;
    
    /* ========== Verificación de argumentos ========== */

    // Se requiere al menos el archivo fuente de entrada.
    if (argc < 2) {
        fprintf (stderr, "Uso: %s <archivo> [archivo_dot]\n", argv [0]);
        return 1;
    }
    
    /* ========== Apertura del archivo fuente ========== */

    // Se abre el archivo fuente para realizar el análisis.
    FILE *archivo = fopen (argv [1], "r");

    if (!archivo) {
        perror ("Error al abrir el archivo de entrada");
        return 1;
    }

    // Se indica a Flex qué archivo debe leer.
    yyin = archivo;

    /* ========== Análisis léxico y sintáctico ========== */

    // Se ejecuta el análisis léxico y sintáctico.
    resultado = yyparse ();

    if (error_lexico) {
        printf ("\n✗ El análisis léxico finalizó con errores.\n");

        fclose (archivo);

        return 1;
    }

    if (resultado != 0) {
        printf ("\n✗ El análisis sintáctico finalizó con errores.\n");

        fclose (archivo);

        return 1;
    }
    
    printf ("✓ Análisis léxico correcto.\n");
    printf ("✓ Análisis sintáctico correcto.\n");

    /* ========== Análisis semántico ========== */
    
    // Se inicializa la Tabla de Símbolos.
    ts = iniciar_TS ();

    // Se realiza el análisis semántico sobre el AST.
    errores_semanticos = analizar_semantica (arbol, ts);

    if (errores_semanticos != 0) {
        printf ("\n✗ El análisis semántico finalizó con %d error%s.", errores_semanticos, errores_semanticos == 1 ? "" : "es");
    }
    else {
        printf ("✓ Análisis semántico correcto.\n");

        /* ========== Interpretación ========== */
        
        // Se ejecuta el programa.
        resultado_interpretacion = interpretar_programa (arbol, &valor_retorno);

        if (resultado_interpretacion != 0) {
            printf ("\n✗ La interpretación finalizó con errores.");
        }
        else {
            printf ("✓ Interpretación finalizada correctamente.");

            // Se muestra el valor retornado por main solamente cuando lo tiene.
            if (arbol -> tipo_dato == TIPO_INT) {
                printf ("\n     Retorno: %d", valor_retorno.entero);
            }
            else if (arbol -> tipo_dato == TIPO_BOOL) {
                printf ("\n     Retorno: %s", valor_retorno.booleano ? "true" : "false");
            }
        }
    }

    printf ("\n");

    /* ========== Generación del archivo DOT ========== */
    
    // Se determina el nombre del archivo DOT a generar.
    char nombre_dot [1024];
        
    if (argc >= 3) {
        // Si se proporciona un nombre de archivo DOT, se utiliza.
        strcpy (nombre_dot, argv [2]);
    }
    else {
        // De lo contrario, se genera a partir del nombre del archivo fuente.
        strcpy (nombre_dot, argv [1]);
    }
        
    char *extension = strrchr (nombre_dot, '.');

    if (extension != NULL) {
        // Se reemplaza la extensión existente por ".dot".
        strcpy (extension, ".dot");
    }
    else {
        // Si no hay extensión, se agrega ".dot" al final.
        strcat (nombre_dot, ".dot");
    }

    // Se crea el archivo DOT.
    FILE *archivo_dot = fopen (nombre_dot, "w");
        
    if (!archivo_dot) {
        perror ("Error al crear el archivo DOT");
        liberar_arbol (arbol);
        liberar_TS (ts);
        fclose (archivo);
        return 1;
    }

    // Se genera la representación DOT del AST.
    generar_dot (arbol, archivo_dot);
    fclose (archivo_dot);

    liberar_arbol (arbol);
    liberar_TS (ts);
    fclose (archivo);

    if (errores_semanticos > 0 || resultado_interpretacion != 0) {
        return 1;
    }

    return 0;
}
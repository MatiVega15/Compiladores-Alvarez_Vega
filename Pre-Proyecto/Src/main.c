#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AST.h"

// Variables y funciones definidas por Bison.
extern FILE *yyin;
extern int yyparse (void);
extern NodoAST *arbol;

int main (int argc, char **argv) {
    // Se requiere al menos el archivo fuente de entrada.
    if (argc < 2) {
        fprintf (stderr, "Uso: %s <archivo> [archivo_dot]\n", argv [0]);
        return 1;
    }
    
    // Se abre el archivo fuente para realizar el análisis.
    FILE *archivo = fopen (argv [1], "r");

    if (!archivo) {
        perror ("Error al abrir el archivo de entrada");
        return 1;
    }

    // Se indica a Flex qué archivo debe leer.
    yyin = archivo;

    // Se ejecuta el análisis léxico y sintáctico.
    int resultado = yyparse ();

    if (resultado == 0) {
        printf ("\n¡Análisis sintáctico finalizado con éxito!\n\n");
        
        printf ("Árbol Sintáctico Abstracto (AST) del programa:\n\n");
        imprimir_arbol (arbol);

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
            fclose (archivo);
            return 1;
        }

        // Se genera la representación DOT del AST.
        generar_dot (arbol, archivo_dot);

        fclose (archivo_dot);

        printf ("\nArchivo DOT generado: %s\n", nombre_dot);

        liberar_arbol (arbol);
    }

    fclose (archivo);

    return resultado;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== Variables y funciones de Flex ========== */

extern FILE *yyin;
extern int yylex (void);
extern int error_lexico;
extern int modo_debug;
extern FILE *salida_lexico;

/* ========== Variables y funciones de Bison ========== */

extern int yyparse (void);
extern FILE *salida_sintactico;

/* ========== Etapas del compilador ========== */

typedef enum {
    ETAPA_SCAN,
    ETAPA_PARSE,
    ETAPA_CODINTER,
    ETAPA_ASSEMBLY
} Etapa;

/* ========== Configuración del compilador ========== */

typedef struct {
    const char *archivo_entrada;

    // Nombre solicitado mediante -o.
    const char *archivo_salida;

    // Última etapa que debe ejecutarse.
    Etapa etapa;

    int debug;
    int optimizar;
    int optimizar_todo;
} Configuracion;

/* ========== Prototipos de funciones auxiliares ========== */

static int procesar_argumentos (int argc, char *argv [], Configuracion *configuracion);
static int analizar_lexicamente (const Configuracion *configuracion, FILE *salida);
static int analizar_sintacticamente (const Configuracion *configuracion, FILE *salida);
static Etapa obtener_etapa (const char *nombre);
static const char *nombre_etapa (Etapa etapa);
static char *generar_nombre_salida (const char *archivo_entrada, Etapa etapa);
static int termina_con (const char *cadena, const char *sufijo);

/* ========== Función principal ========== */

int main (int argc, char *argv []) {
    Configuracion configuracion;
    FILE *salida;
    char *nombre_salida;
    int resultado;

    // Configuración por defecto.
    configuracion.archivo_entrada = NULL;
    configuracion.archivo_salida = NULL;
    configuracion.etapa = ETAPA_PARSE;
    configuracion.debug = 0;
    configuracion.optimizar = 0;
    configuracion.optimizar_todo = 0;

    // Se procesan los argumentos de la línea de comandos.
    if (procesar_argumentos (argc, argv, &configuracion) != 0) {
        return EXIT_FAILURE;
    }

    // Optimizaciones aún sin implementar.
    if (configuracion.optimizar) {
        fprintf (stderr, "ERROR: Las optimizaciones todavía no están implementadas.\n");

        return EXIT_FAILURE;
    }

    // Actualmente solo están implementadas las etapas scan y parse.
    if (configuracion.etapa > ETAPA_PARSE) {
        fprintf (stderr, "ERROR: La etapa '%s' todavía no está implementada.\n", nombre_etapa (configuracion.etapa));

        return EXIT_FAILURE;
    }

    // Se genera una única salida correspondiente a la última etapa que se debe ejecutar.
    if (configuracion.archivo_salida != NULL) {
        nombre_salida = malloc (strlen (configuracion.archivo_salida) + 1);

        if (nombre_salida != NULL) {
            strcpy (nombre_salida, configuracion.archivo_salida);
        }
    }
    else {
        nombre_salida = generar_nombre_salida (configuracion.archivo_entrada, configuracion.etapa);
    }

    if (nombre_salida == NULL) {
        fprintf (stderr, "ERROR: No se pudo generar el nombre del archivo de salida.\n");

        return EXIT_FAILURE;
    }

    // Se crea el archivo de salida.
    salida = fopen (nombre_salida, "w");

    if (salida == NULL) {
        fprintf (stderr, "ERROR: No se pudo crear el archivo '%s'.\n", nombre_salida);

        free (nombre_salida);
        return EXIT_FAILURE;
    }

    // Se ejecuta el análisis léxico.
    if (configuracion.etapa == ETAPA_SCAN) {
        resultado = analizar_lexicamente (&configuracion, salida);
    }

    // Se ejecuta el análisis sintáctico.
    else {
        resultado = analizar_sintacticamente (&configuracion, salida);
    }
    
    fclose (salida);
    free (nombre_salida);

    return resultado;
}

/* ========== Procesamiento de argumentos ========== */

/**
 * Procesa los argumentos de la línea de comandos.
 * 
 * Formato general:
 * 
 *      c-tds [opciones] archivo.ctds
 * 
 * Opciones reconocidas:
 * 
 *      -debug
 *      -o <salida>
 *      -target <etapa>
 *      -opt <optimizacion>
 */
static int procesar_argumentos (int argc, char *argv [], Configuracion *configuracion) {
    // Se verifica el número mínimo necesario de argumentos.
    if (argc < 2) {
        fprintf (stderr, "Uso: c-tds [opciones] archivo.ctds\n");

        return 1;
    }

    // Se recorren todos los argumentos.
    for (int i = 1; i < argc; i ++) {
        // Modo debug.
        if (strcmp (argv [i], "-debug") == 0) {
            configuracion -> debug = 1;
        }

        // Nombre de salida.
        else if (strcmp (argv [i], "-o") == 0) {
            // -o requiere argumento.
            if (i + 1 >= argc || argv [i + 1] [0] == '-') {
                fprintf (stderr, "ERROR: La opción -o requiere un nombre de salida.\n");

                return 1;
            }

            i ++;
            configuracion -> archivo_salida = argv [i];
        }

        // Etapa del compilador.
        else if (strcmp (argv [i], "-target") == 0) {
            Etapa etapa;

            // -target requiere el nombre de una etapa.
            if (i + 1 >= argc || argv [i + 1] [0] == '-') {
                fprintf (stderr, "ERROR: La opción -target requiere una etapa.\n");

                return 1;
            }

            i ++;
            etapa = obtener_etapa (argv [i]);

            // obtener_etapa devuelve -1 cuando la etapa no es reconocida.
            if (etapa < ETAPA_SCAN || etapa > ETAPA_ASSEMBLY) {
                fprintf (stderr, "ERROR: Etapa desconocida '%s'.\n", argv [i]);

                return 1;
            }

            configuracion -> etapa = etapa;
        }

        // Optimización.
        else if (strcmp (argv [i], "-opt") == 0) {
            configuracion -> optimizar = 1;
            
            // -opt requiere un argumento, que puede ser "all" o el nombre de una optimización específica.
            if (i + 1 >= argc || argv [i + 1] [0] == '-') {
                fprintf (stderr, "ERROR: La opción -opt requiere una optimización.\n");

                return 1;
            }

            // El archivo de entrada no se puede utilizar como optimización.
            if (termina_con (argv [i + 1], ".ctds")) {
                fprintf (stderr, "ERROR: La opción -opt requiere una optimización.\n");

                return 1;
            }
            
            i ++;

            // "all" activa todas las optimizaciones.
            if (strcmp (argv [i], "all") == 0) {
                configuracion -> optimizar_todo = 1;
            }
        }

        // Archivo de entrada.
        else {
            // Un archivo de entrada no puede comenzar con '-'.
            if (argv [i] [0] == '-') {
                fprintf (stderr, "ERROR: Opción desconocida '%s'.\n", argv [i]);

                return 1;
            }

            // Solo se permite un archivo de entrada.
            if (configuracion -> archivo_entrada != NULL) {
                fprintf (stderr, "ERROR: Se especificaron varios archivos de entrada.\n");

                return 1;
            }

            configuracion -> archivo_entrada = argv [i];
        }
    }

    // Debe existir un archivo de entrada.
    if (configuracion -> archivo_entrada == NULL) {
        fprintf (stderr, "ERROR: No se especificó un archivo de entrada.\n");

        return 1;
    }

    // El archivo de entrada debe tener extensión .ctds.
    if (!termina_con (configuracion -> archivo_entrada, ".ctds")) {
        fprintf (stderr, "ERROR: El archivo de entrada debe tener extensión '.ctds'.\n");

        return 1;
    }

    return 0;
}

/* ========== Análisis léxico ========== */

/**
 * Ejecuta únicamente la etapa de análisis léxico.
 * 
 * El resultado se escribe en el archivo de salida
 * proporcionado por main.
 */
static int analizar_lexicamente (const Configuracion *configuracion, FILE *salida) {
    FILE *entrada;

    // Se abre el archivo fuente.
    entrada = fopen (configuracion -> archivo_entrada, "r");

    if (entrada == NULL) {
        fprintf (stderr, "ERROR: No se pudo abrir el archivo '%s'.\n", configuracion -> archivo_entrada);

        return EXIT_FAILURE;
    }

    // Se configura la entrada utilizada por Flex.
    yyin = entrada;

    // Se configura el modo debug.
    modo_debug = configuracion -> debug;

    // Se reinicia el estado de error del analizador.
    error_lexico = 0;

    // Se indica al lexer dónde debe registrar los tokens reconocidos.
    salida_lexico = salida;

    // Se ejecuta el analizador léxico.
    while (yylex () != 0) {

    }

    // Se cierran los archivos y se liberan punteros.
    fclose (entrada);
    salida_lexico = NULL;

    // Si ocurrió un error léxico, la etapa termina indicando un resultado fallido.
    if (error_lexico) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* ========== Análisis sintáctico ========== */

/**
 * Ejecuta la etapa de análisis sintáctico.
 * 
 * Bison utiliza el analizador léxico de Flex
 * para obtener los tokens.
 * 
 * La salida contiene únicamente la información
 * generada por el análisis sintáctico.
 */
static int analizar_sintacticamente (const Configuracion *configuracion, FILE *salida) {
    FILE *entrada;
    int resultado_parser;

    // Se abre nuevamente el archivo fuente.
    entrada = fopen (configuracion -> archivo_entrada, "r");

    if (entrada == NULL) {
        fprintf (stderr, "ERROR: No se pudo abrir el archivo '%s'.\n", configuracion -> archivo_entrada);

        return EXIT_FAILURE;
    }

    // Se configura la entrada utilizada por Flex.
    yyin = entrada;

    // Se configura el modo debug.
    modo_debug = configuracion -> debug;

    // Se reinicia el estado de error del analizador.
    error_lexico = 0;

    // Se indica al parser dónde debe registrar los tokens reconocidos.
    salida_sintactico = salida;

    // Durante parse, el lexer no genera una salida .lex.
    salida_lexico = NULL;
    
    // Se ejecuta el analizador sintáctico.
    resultado_parser = yyparse ();

    // Se cierran los archivos y se liberan punteros.
    fclose (entrada);
    salida_sintactico = NULL;

    // Si ocurrió un error léxico, la etapa termina indicando un resultado fallido.
    if (error_lexico) {
        return EXIT_FAILURE;
    }

    // Si Bison informa errores sintácticos, la etapa termina indicando un resultado fallido.
    if (resultado_parser != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* ========== Conversión de etapas ========== */

/**
 * Convierte el nombre textual de una etapa
 * en el valor correspondiente del enumerado.
 */
static Etapa obtener_etapa (const char *nombre) {
    if (strcmp (nombre, "scan") == 0) {
        return ETAPA_SCAN;
    }
    if (strcmp (nombre, "parse") == 0) {
        return ETAPA_PARSE;
    }
    if (strcmp (nombre, "codinter") == 0) {
        return ETAPA_CODINTER;
    }
    if (strcmp (nombre, "assembly") == 0) {
        return ETAPA_ASSEMBLY;
    }

    return (Etapa) -1;
}

/**
 * Devuelve el nombre textual de una etapa.
 */
static const char *nombre_etapa (Etapa etapa) {
    switch (etapa) {
        case ETAPA_SCAN:
            return "scan";
        case ETAPA_PARSE:
            return "parse";
        case ETAPA_CODINTER:
            return "codinter";
        case ETAPA_ASSEMBLY:
            return "assembly";
        default:
            return "desconocida";
    }
}

/* ========== Generación de nombres de salida ========== */

/**
 * Genera el nombre del archivo de salida correspondiente
 * a la etapa del compilador.
 * 
 * Ejemplos:
 * 
 *      programa.ctds -> programa.lex
 *      programa.ctds -> programa.sint
 *      programa.ctds -> programa.ci
 *      programa.ctds -> programa.ass
 */
static char *generar_nombre_salida (const char *archivo_entrada, Etapa etapa) {
    const char *extension;
    const char *punto;
    size_t longitud;
    size_t longitud_extension;
    char *resultado;

    switch (etapa) {
        case ETAPA_SCAN:
            extension = ".lex";
            break;
        case ETAPA_PARSE:
            extension = ".sint";
            break;
        case ETAPA_CODINTER:
            extension = ".ci";
            break;
        case ETAPA_ASSEMBLY:
            extension = ".ass";
            break;
        default:
            return NULL;
    }

    // Se busca el último punto del nombre de archivo.
    punto = strrchr (archivo_entrada, '.');

    if (punto != NULL) {
        longitud = (size_t) (punto - archivo_entrada);
    }
    else {
        longitud = strlen (archivo_entrada);
    }

    longitud_extension = strlen (extension);

    resultado = malloc (longitud + longitud_extension + 1);

    if (resultado == NULL) {
        return NULL;
    }

    // Se copia el nombre sin la extensión original.
    memcpy (resultado, archivo_entrada, longitud);

    // Se agrega la nueva extensión.
    strcpy (resultado + longitud, extension);

    return resultado;
}

/* ========== Utilidades ========== */

/**
 * Indica si una cadena termina con un determinado sufijo.
 */
static int termina_con (const char *cadena, const char *sufijo) {
    size_t longitud_cadena;
    size_t longitud_sufijo;

    longitud_cadena = strlen (cadena);
    longitud_sufijo = strlen (sufijo);

    if (longitud_cadena < longitud_sufijo) {
        return 0;
    }

    return strcmp (cadena + longitud_cadena - longitud_sufijo, sufijo) == 0;
}
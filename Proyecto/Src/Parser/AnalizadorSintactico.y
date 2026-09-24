%{

#include <stdio.h>
#include <stdlib.h>

/*
 * Analizador sintáctico de C-TDS.
 *
 * Bison recibe los tokens generados por el analizador léxico
 * y verifica que la secuencia de tokens respete la gramática
 * definida para el lenguaje.
 *
 * En esta etapa no se construye todavía el AST ni se realizan
 * comprobaciones semánticas.
 */

/* Función proporcionada por el analizador léxico. */
extern int yylex (void);

/* Modo de depuración configurado por main. */
extern int modo_debug;

/* Indica si se produjo algún error léxico. */
extern int error_lexico;

/* Archivo de salida .sint. */
FILE *salida_sintactico = NULL;

/* Función utilizada por Bison para informar errores sintácticos. */
void yyerror (const char *mensaje);

/* Prototipos de funciones auxiliares. */
static void registrar_sintaxis (const char *mensaje, int linea, int columna);
static void registrar_debug (const char *mensaje);

%}

/* - DEFINICIONES - */

/* - Valores semánticos - */

%union {
    int numero;
    double real;
    char *identificador;
}

/* - Ubicaciones - */

%locations

/* - Mensajes de error - */

%define parse.error detailed

/* - Tokens - */

/* Palabras reservadas. */
%token INT BOOLEAN FLOAT VOID
%token IF ELSE WHILE RETURN
%token TRUE FALSE

/* Literales. */
%token <numero> NRO
%token <real> REAL

/* Identificadores. */
%token <identificador> ID

/* Operadores de más de un carácter. */
%token IGUAL "=="
%token AND "&&"
%token OR "||"

/* - Precedencia y asociatividad - */

/* De menor a mayor precedencia. */
%left OR
%left AND
%nonassoc IGUAL
%nonassoc '<' '>'
%left '+' '-'
%left '*' '/' '%'
%right '!'
%right UMINUS // Menos unario.

%%

    /* - REGLAS - */

    /* - Programa - */

program             :   lista_declaraciones                             { registrar_sintaxis ("Programa reconocido.", @$.first_line, @$.first_column); }
                    ;

    /* - Declaraciones - */

lista_declaraciones :   %empty
                    |   type id declaracion_tipo
                    |   VOID id method_decl_resto lista_method_decl     { registrar_sintaxis ("Declaración de función reconocida.", @$.first_line, @$.first_column); }
                    ;

declaracion_tipo    :   lista_id_resto ';' lista_declaraciones          { registrar_sintaxis ("Declaración de variable reconocida.", @$.first_line, @$.first_column); } 
                    |   method_decl_resto lista_method_decl             { registrar_sintaxis ("Declaración de función reconocida.", @$.first_line, @$.first_column); }
                    ;

lista_id_resto      :   %empty
                    |   lista_id_resto ',' id
                    ;

method_decl_resto   :   '(' lista_parametros ')' block
                    ;

lista_method_decl   :   %empty
                    |   method_decl lista_method_decl
                    ;

method_decl         :   tipo_method id '(' lista_parametros ')' block   { registrar_sintaxis ("Declaración de función reconocida.", @$.first_line, @$.first_column); }
                    ;

tipo_method         :   type
                    |   VOID
                    ;

    /* - Parámetros - */

lista_parametros    :   %empty
                    |   parametros
                    ;

parametros          :   type id
                    |   parametros ',' type id
                    ;

    /* - Bloques - */

block               :   '{' lista_var_decl lista_statement '}'          { registrar_sintaxis ("Bloque reconocido.", @$.first_line, @$.first_column); }
                    ;

lista_var_decl      :   %empty
                    |   lista_var_decl var_decl
                    ;

var_decl            :   type lista_id ';'                               { registrar_sintaxis ("Declaración de variable reconocida.", @$.first_line, @$.first_column); }
                    ;

lista_id            :   id
                    |   lista_id ',' id
                    ;

    /* - Sentencias - */

lista_statement     :   %empty
                    |   lista_statement statement
                    ;

statement           :   id '=' expr ';'                                 { registrar_sintaxis ("Asignación reconocida.", @$.first_line, @$.first_column); }
                    |   method_call ';'                                 { registrar_sintaxis ("Llamada a función reconocida.", @$.first_line, @$.first_column); }
                    |   IF '(' expr ')' block                           { registrar_sintaxis ("Sentencia if reconocida.", @$.first_line, @$.first_column); }
                    |   IF '(' expr ')' block ELSE block                { registrar_sintaxis ("Sentencia if-else reconocida.", @$.first_line, @$.first_column); }
                    |   WHILE '(' expr ')' block                        { registrar_sintaxis ("Sentencia while reconocida.", @$.first_line, @$.first_column); }
                    |   RETURN ';'                                      { registrar_sintaxis ("Return sin expresión reconocido.", @$.first_line, @$.first_column); }
                    |   RETURN expr ';'                                 { registrar_sintaxis ("Return con expresión reconocido.", @$.first_line, @$.first_column); }
                    |   ';'                                             { registrar_sintaxis ("Sentencia vacía reconocida.", @$.first_line, @$.first_column); }
                    |   block
                    ;

    /* - Llamadas a funciones - */

method_call         :   id '(' lista_argumentos ')'
                    ;

lista_argumentos    :   %empty
                    |   argumentos
                    ;

argumentos          :   expr
                    |   argumentos ',' expr
                    ;

    /* - Expresiones - */

expr                :   id
                    |   method_call
                    |   literal
                    |   expr '+' expr                                   { registrar_debug ("Reducción: expresión + expresión."); }
                    |   expr '-' expr                                   { registrar_debug ("Reducción: expresión - expresión."); }
                    |   expr '*' expr                                   { registrar_debug ("Reducción: expresión * expresión."); }
                    |   expr '/' expr                                   { registrar_debug ("Reducción: expresión / expresión."); }
                    |   expr '%' expr                                   { registrar_debug ("Reducción: expresión % expresión."); }
                    |   expr '<' expr                                   { registrar_debug ("Reducción: expresión < expresión."); }
                    |   expr '>' expr                                   { registrar_debug ("Reducción: expresión > expresión."); }
                    |   expr IGUAL expr                                 { registrar_debug ("Reducción: expresión == expresión."); }
                    |   expr AND expr                                   { registrar_debug ("Reducción: expresión && expresión."); }
                    |   expr OR expr                                    { registrar_debug ("Reducción: expresión || expresión."); }
                    |   '-' expr %prec UMINUS                           { registrar_debug ("Reducción: - expresión."); }
                    |   '!' expr                                        { registrar_debug ("Reducción: ! expresión."); }
                    |   '(' expr ')'                                    { registrar_debug ("Reducción: ( expresión )."); }
                    ;

    /* - Literales - */

literal             :   int_literal
                    |   bool_literal
                    |   float_literal
                    ;

int_literal         :   NRO
                    ;

bool_literal        :   TRUE
                    |   FALSE
                    ;

float_literal       :   REAL
                    ;

    /* - Identificadores - */

id                  :   ID
                    ;

    /* - Tipos - */

type                :   INT
                    |   BOOLEAN
                    |   FLOAT
                    ;

%%

/* - CÓDIGO DE USUARIO - */

/*
 * Registra una construcción sintáctica en el archivo .sint.
 *
 * Si -debug está activo, también muestra el mensaje por consola.
 */
static void registrar_sintaxis (const char *mensaje, int linea, int columna) {
    if (salida_sintactico != NULL) {
        fprintf (salida_sintactico, "%d:%d - %s\n", linea, columna, mensaje);
    }
    
    if (modo_debug) {
        printf ("[PARSE] %d:%d - %s\n", linea, columna, mensaje);
    }
}

/*
 * Registra una reducción específica para depurar precedencias. 
 */
static void registrar_debug (const char *mensaje) {
    if (modo_debug) {
        printf ("[PARSE] %s\n", mensaje);
    }
}

/*
 *  Informa un error sintáctico indicando su ubicación.
 */
void yyerror (const char *mensaje) {    
    fprintf (stderr, "ERROR SINTÁCTICO: línea %d, columna %d - %s.\n", yylloc.first_line, yylloc.first_column, mensaje);
}
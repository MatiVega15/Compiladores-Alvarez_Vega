%{

#include <stdio.h>
#include <stdlib.h>

/*
 * Analizador léxico de C-TDS.
 *
 * Reconoce los elementos léxicos definidos por la especificación
 * del lenguaje y registra los errores léxicos.
 *
 * La variable modo_debug permite controlar la salida del lexer.
 *  - Si vale 0, el lexer no muestra los tokens reconocidos.
 *  - Si vale 1, muestra información de cada token reconocido.
 */

/* Columna actual dentro de la línea. */
int yycolumn = 1;

/* Indica si ocurrió algún error léxico. */
int error_lexico = 0;

/* Indica si está activado el modo debug. */
int modo_debug = 0;

/* Archivo de salida .lex. */
FILE *salida_lexico = NULL;

/* Prototipos de funciones auxiliares. */
static void registrar_token (const char *tipo, const char *lexema);
static void mostrar_error_lexico (const char *lexema);

%}

/* - OPCIONES DE FLEX - */

%option noyywrap
%option yylineno

/* Estado especial del autómata para comentarios de más de una línea. */
%x COMENTARIO

/* - DEFINICIONES - */

LETRA       [a-zA-Z]
DIGITO      [0-9]

%%

    /* - REGLAS - */
    
    /* - Palabras reservadas - */

"int"                               { registrar_token ("PALABRA RESERVADA", yytext); yycolumn += yyleng; }
"boolean"                           { registrar_token ("PALABRA RESERVADA", yytext); yycolumn += yyleng; }
"float"                             { registrar_token ("PALABRA RESERVADA", yytext); yycolumn += yyleng; }
"void"                              { registrar_token ("PALABRA RESERVADA", yytext); yycolumn += yyleng; }

"if"                                { registrar_token ("PALABRA RESERVADA", yytext); yycolumn += yyleng; }
"else"                              { registrar_token ("PALABRA RESERVADA", yytext); yycolumn += yyleng; }
"while"                             { registrar_token ("PALABRA RESERVADA", yytext); yycolumn += yyleng; }
"return"                            { registrar_token ("PALABRA RESERVADA", yytext); yycolumn += yyleng; }

"true"                              { registrar_token ("PALABRA RESERVADA", yytext); yycolumn += yyleng; }
"false"                             { registrar_token ("PALABRA RESERVADA", yytext); yycolumn += yyleng; }

    /* - Literales - */

{DIGITO}+"."{DIGITO}+               { registrar_token ("LITERAL REAL", yytext); yycolumn += yyleng; }
{DIGITO}+                           { registrar_token ("LITERAL ENTERO", yytext); yycolumn += yyleng; }     

    /* - identificadores - */

{LETRA}({LETRA}|{DIGITO}|_)*        { registrar_token ("IDENTIFICADOR", yytext); yycolumn += yyleng; }

    /* - Operadores - */

"=="                                { registrar_token ("OPERADOR", yytext); yycolumn += yyleng; }
"<"                                 { registrar_token ("OPERADOR", yytext); yycolumn += yyleng; }
">"                                 { registrar_token ("OPERADOR", yytext); yycolumn += yyleng; }

"&&"                                { registrar_token ("OPERADOR", yytext); yycolumn += yyleng; }
"||"                                { registrar_token ("OPERADOR", yytext); yycolumn += yyleng; }
"!"                                 { registrar_token ("OPERADOR", yytext); yycolumn += yyleng; }

"="                                 { registrar_token ("OPERADOR", yytext); yycolumn += yyleng; }
"+"                                 { registrar_token ("OPERADOR", yytext); yycolumn += yyleng; }
"-"                                 { registrar_token ("OPERADOR", yytext); yycolumn += yyleng; }
"*"                                 { registrar_token ("OPERADOR", yytext); yycolumn += yyleng; }
"/"                                 { registrar_token ("OPERADOR", yytext); yycolumn += yyleng; }
"%"                                 { registrar_token ("OPERADOR", yytext); yycolumn += yyleng; }

    /* - Delimitadores - */

"("                                 { registrar_token ("DELIMITADOR", yytext); yycolumn += yyleng; }
")"                                 { registrar_token ("DELIMITADOR", yytext); yycolumn += yyleng; }

"{"                                 { registrar_token ("DELIMITADOR", yytext); yycolumn += yyleng; }
"}"                                 { registrar_token ("DELIMITADOR", yytext); yycolumn += yyleng; }

";"                                 { registrar_token ("DELIMITADOR", yytext); yycolumn += yyleng; }
","                                 { registrar_token ("DELIMITADOR", yytext); yycolumn += yyleng; }
"."                                 { registrar_token ("DELIMITADOR", yytext); yycolumn += yyleng; }

    /* - Comentarios - */

"//".*                              { yycolumn += yyleng; }

"/*"                                { BEGIN (COMENTARIO); yycolumn += yyleng; }
<COMENTARIO>"*/"                    { BEGIN (INITIAL); yycolumn += yyleng; }
<COMENTARIO>[^*\n]+                 { yycolumn += yyleng; }
<COMENTARIO>"*"                     { yycolumn += yyleng; }
<COMENTARIO>\n                      { yycolumn = 1; }
<COMENTARIO><<EOF>>                 { fprintf (stderr, "ERROR LÉXICO: Comentario multilínea sin cerrar en línea %d, columna %d.\n", yylineno, yycolumn); error_lexico = 1; BEGIN (INITIAL); }

    /* - Espacios en blanco - */

[ \t\r]+                            { yycolumn += yyleng; }
\n                                  { yycolumn = 1; }

    /* - Error léxico - */

.                                   { mostrar_error_lexico (yytext); error_lexico = 1; yycolumn += yyleng; }

%%

/* - CÓDIGO DE USUARIO - */

/**
 * Escribe en el archivo de salida los tokens reconocidos y su ubicación.
 * Muestra información sobre el token cuando el modo debug está activado.
 */
static void registrar_token (const char *tipo, const char *lexema) {
    if (salida_lexico != NULL) {
        fprintf (salida_lexico, "%s %s %d:%d\n", tipo, lexema, yylineno, yycolumn);
    }
    
    if (modo_debug) {
        printf ("[LEX] %s: %s (línea %d, columna %d)\n", tipo, lexema, yylineno, yycolumn);
    }
}

/*
 * Muestra un error léxico indicando su ubicación.
 */
static void mostrar_error_lexico (const char *lexema) {
    fprintf (stderr, "ERROR LÉXICO: Carácter no reconocido '%s' en línea %d, columna %d.\n", lexema, yylineno, yycolumn);
}
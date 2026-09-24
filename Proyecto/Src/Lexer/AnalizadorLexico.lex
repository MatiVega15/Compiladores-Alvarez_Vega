%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "AnalizadorSintactico.tab.h"

/*
 * Analizador léxico de C-TDS.
 *
 * Reconoce los elementos léxicos definidos por la especificación
 * del lenguaje y registra los errores léxicos.
 *
 * Además, entrega los tokens reconocidos al analizador sintáctico
 * de Bison junto con su ubicación y, cuando corresponde, su valor.
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

/*
 * Registra la ubicación del token actual para Bison
 * y avanza la columna hasta el final del lexema.
 */
#define REGISTRAR_UBICACION()                       \
    do {                                            \
        yylloc.first_line = yylineno;               \
        yylloc.last_line = yylineno;                \
        yylloc.first_column = yycolumn;             \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;                         \
    } while (0)

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

"int"                               { registrar_token ("PALABRA RESERVADA", yytext); REGISTRAR_UBICACION (); return INT; }
"boolean"                           { registrar_token ("PALABRA RESERVADA", yytext); REGISTRAR_UBICACION (); return BOOLEAN; }
"float"                             { registrar_token ("PALABRA RESERVADA", yytext); REGISTRAR_UBICACION (); return FLOAT; }
"void"                              { registrar_token ("PALABRA RESERVADA", yytext); REGISTRAR_UBICACION (); return VOID; }

"if"                                { registrar_token ("PALABRA RESERVADA", yytext); REGISTRAR_UBICACION (); return IF; }
"else"                              { registrar_token ("PALABRA RESERVADA", yytext); REGISTRAR_UBICACION (); return ELSE; }
"while"                             { registrar_token ("PALABRA RESERVADA", yytext); REGISTRAR_UBICACION (); return WHILE; }
"return"                            { registrar_token ("PALABRA RESERVADA", yytext); REGISTRAR_UBICACION (); return RETURN; }

"true"                              { registrar_token ("PALABRA RESERVADA", yytext); REGISTRAR_UBICACION (); return TRUE; }
"false"                             { registrar_token ("PALABRA RESERVADA", yytext); REGISTRAR_UBICACION (); return FALSE; }

    /* - Literales - */

{DIGITO}+"."{DIGITO}+               { registrar_token ("LITERAL REAL", yytext); REGISTRAR_UBICACION (); yylval.real = atof (yytext); return REAL; }
{DIGITO}+                           { registrar_token ("LITERAL ENTERO", yytext); REGISTRAR_UBICACION (); yylval.numero = atoi (yytext); return NRO; }

    /* - Identificadores - */

{LETRA}({LETRA}|{DIGITO}|_)*        { registrar_token ("IDENTIFICADOR", yytext); REGISTRAR_UBICACION (); yylval.identificador = strdup (yytext); return ID; }

    /* - Operadores - */

"=="                                { registrar_token ("OPERADOR", yytext); REGISTRAR_UBICACION (); return IGUAL; }
"<"                                 { registrar_token ("OPERADOR", yytext); REGISTRAR_UBICACION (); return '<'; }
">"                                 { registrar_token ("OPERADOR", yytext); REGISTRAR_UBICACION (); return '>'; }

"&&"                                { registrar_token ("OPERADOR", yytext); REGISTRAR_UBICACION (); return AND; }
"||"                                { registrar_token ("OPERADOR", yytext); REGISTRAR_UBICACION (); return OR; }
"!"                                 { registrar_token ("OPERADOR", yytext); REGISTRAR_UBICACION (); return '!'; }

"="                                 { registrar_token ("OPERADOR", yytext); REGISTRAR_UBICACION (); return '='; }
"+"                                 { registrar_token ("OPERADOR", yytext); REGISTRAR_UBICACION (); return '+'; }
"-"                                 { registrar_token ("OPERADOR", yytext); REGISTRAR_UBICACION (); return '-'; }
"*"                                 { registrar_token ("OPERADOR", yytext); REGISTRAR_UBICACION (); return '*'; }
"/"                                 { registrar_token ("OPERADOR", yytext); REGISTRAR_UBICACION (); return '/'; }
"%"                                 { registrar_token ("OPERADOR", yytext); REGISTRAR_UBICACION (); return '%'; }

    /* - Delimitadores - */

"("                                 { registrar_token ("DELIMITADOR", yytext); REGISTRAR_UBICACION (); return '('; }
")"                                 { registrar_token ("DELIMITADOR", yytext); REGISTRAR_UBICACION (); return ')'; }

"{"                                 { registrar_token ("DELIMITADOR", yytext); REGISTRAR_UBICACION (); return '{'; }
"}"                                 { registrar_token ("DELIMITADOR", yytext); REGISTRAR_UBICACION (); return '}'; }

";"                                 { registrar_token ("DELIMITADOR", yytext); REGISTRAR_UBICACION (); return ';'; }
","                                 { registrar_token ("DELIMITADOR", yytext); REGISTRAR_UBICACION (); return ','; }
"."                                 { registrar_token ("DELIMITADOR", yytext); REGISTRAR_UBICACION (); return '.'; }

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
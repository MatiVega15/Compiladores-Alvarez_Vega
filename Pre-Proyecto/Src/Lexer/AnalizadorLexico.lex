%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AnalizadorSintactico.tab.h"

/* Columna actual dentro de la línea. */

int yycolumn = 1;
int error_lexico = 0;

/* Registra la ubicación del token actual para Bison. */

#define REGISTRAR_UBICACION()                       \
    do {                                            \
        yylloc.first_line = yylineno;               \
        yylloc.last_line = yylineno;                \
        yylloc.first_column = yycolumn;             \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;                         \
    } while (0)

%}

%option noyywrap
%option yylineno

/* - DEFINICIONES - */
/* Abreviaturas utilizadas en las expresiones regulares. */

LETRA       [a-zA-Z]
DIGITO      [0-9]

%%

"int"                           { REGISTRAR_UBICACION(); return INT; }
"bool"                          { REGISTRAR_UBICACION(); return BOOL; }
"void"                          { REGISTRAR_UBICACION(); return VOID; }
"main"                          { REGISTRAR_UBICACION(); return MAIN; }
"return"                        { REGISTRAR_UBICACION(); return RETURN; }

{DIGITO}+                       { REGISTRAR_UBICACION(); yylval.numero = atoi (yytext); return NRO; }
"true"                          { REGISTRAR_UBICACION(); return TRUE; }
"false"                         { REGISTRAR_UBICACION(); return FALSE; }

{LETRA}({LETRA}|{DIGITO})*      { REGISTRAR_UBICACION(); yylval.identificador = strdup (yytext); return ID; }

"+"                             { REGISTRAR_UBICACION(); return '+'; }
"*"                             { REGISTRAR_UBICACION(); return '*'; }
"="                             { REGISTRAR_UBICACION(); return '='; }

"("                             { REGISTRAR_UBICACION(); return '('; }
")"                             { REGISTRAR_UBICACION(); return ')'; }
"{"                             { REGISTRAR_UBICACION(); return '{'; }
"}"                             { REGISTRAR_UBICACION(); return '}'; }
";"                             { REGISTRAR_UBICACION(); return ';'; }

[ \t\r]+                        { yycolumn += yyleng; }
\n                              { yycolumn = 1; }

.                               { fprintf (stderr, "ERROR LÉXICO: Caracter no reconocido '%s' en línea %d, columna %d.\n", yytext, yylineno, yycolumn); error_lexico = 1; return yytext [0]; }

%%
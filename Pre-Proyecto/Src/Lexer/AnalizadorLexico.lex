%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AnalizadorSintactico.tab.h"

%}

%option noyywrap
%option yylineno

/* - DEFINICIONES - */
/* Abreviaturas utilizadas en las expresiones regulares. */

LETRA       [a-zA-Z]
DIGITO      [0-9]

%%

"int"                           { return INT; }
"bool"                          { return BOOL; }
"void"                          { return VOID; }
"main"                          { return MAIN; }
"return"                        { return RETURN; }

{DIGITO}+                       { yylval.numero = atoi (yytext); return NRO; }
"true"                          { return TRUE; }
"false"                         { return FALSE; }

{LETRA}({LETRA}|{DIGITO})*      { yylval.identificador = strdup (yytext); return ID; }

"+"                             { return '+'; }
"*"                             { return '*'; }
"="                             { return '='; }

"("                             { return '('; }
")"                             { return ')'; }
"{"                             { return '{'; }
"}"                             { return '}'; }
";"                             { return ';'; }

[ \t\n\r]+                      { /* Ignorar espacios en blanco y saltos de línea. */ }

.                               { fprintf (stderr, "\nERROR LÉXICO: Caracter no reconocido '%s' en línea %d\n", yytext, yylineno); exit (1); }

%%
%{

#include <stdio.h>
#include <stdlib.h>
#include "AnalizadorSintactico.tab.h"

%}

%option noyywrap
%option yylineno

/* - DEFINICIONES - */
/* Abreviaturas utilizadas en las expresiones regulares. */

LETRA       [a-zA-Z]
DIGITO      [0-9]

%%

"int"                           {printf ("Una palabra reservada: %s\n", yytext); return INT;}
"bool"                          {printf ("Una palabra reservada: %s\n", yytext); return BOOL;}
"void"                          {printf ("Una palabra reservada: %s\n", yytext); return VOID;}
"main"                          {printf ("Una palabra reservada: %s\n", yytext); return MAIN;}
"return"                        {printf ("Una palabra reservada: %s\n", yytext); return RETURN;}

{DIGITO}+                       {printf ("Un valor constante: %s\n", yytext); return NRO;}
"true"                          {printf ("Un valor constante: %s\n", yytext); return TRUE;}
"false"                         {printf ("Un valor constante: %s\n", yytext); return FALSE;}

{LETRA}({LETRA}|{DIGITO})*      {printf ("Un identificador: %s\n", yytext); return ID;}

"+"                             {printf ("Un operador: %s\n", yytext); return '+';}
"*"                             {printf ("Un operador: %s\n", yytext); return '*';}
"="                             {printf ("Un operador: %s\n", yytext); return '=';}

"("                             {printf ("Un delimitador: %s\n", yytext); return '(';}
")"                             {printf ("Un delimitador: %s\n", yytext); return ')';}
"{"                             {printf ("Un delimitador: %s\n", yytext); return '{';}
"}"                             {printf ("Un delimitador: %s\n", yytext); return '}';}
";"                             {printf ("Un delimitador: %s\n", yytext); return ';';}

[ \t\n\r]+                      {/* Ignorar espacios en blanco y saltos de línea. */}

.                               {fprintf (stderr, "ERROR LÉXICO: Caracter no reconocido '%s' en línea %d\n", yytext, yylineno); exit (1);}

%%
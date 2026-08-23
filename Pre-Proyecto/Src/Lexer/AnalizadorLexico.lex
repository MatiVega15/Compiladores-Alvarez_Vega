%{

#include <stdio.h>

%}

%option noyywrap
%option yylineno

/* - DEFINICIONES - */
/* Abreviaturas utilizadas en las expresiones regulares. */

LETRA       [a-zA-Z]
DIGITO      [0-9]

%%

"int"|"bool"|"void"|"main"|"return"         {printf ("Una palabra reservada: %s\n", yytext);}

{DIGITO}+|"true"|"false"                    {printf ("Un valor constante: %s\n", yytext);}

{LETRA}({LETRA}|{DIGITO})*                  {printf ("Un identificador: %s\n", yytext);}

"+"|"*"|"="                                 {printf ("Un operador: %s\n", yytext);}

"("|")"|"{"|"}"|";"                         {printf ("Un delimitador: %s\n", yytext);}

[ \t\n\r]+                                  {/* Ignorar espacios en blanco y saltos de línea. */}

.                                           {printf ("ERROR LÉXICO: Caracter no reconocido '%s' en línea %d\n", yytext, yylineno);}

%%

/* - CÓDIGO DE USUARIO - */

int main (int argc, char **argv) {
    if (argc > 1) {
        FILE *archivo = fopen (argv [1], "r");

        if (!archivo) {
            perror ("Error al abrir el archivo de entrada.\n");
            return 1;
        }

        yyin = archivo;
    }

    yylex ();

    return 0;
}
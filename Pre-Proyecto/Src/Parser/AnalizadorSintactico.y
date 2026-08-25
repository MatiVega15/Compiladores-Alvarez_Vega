%{

#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
extern int yylineno;

int yylex (void);
void yyerror (const char *s);

%}

/* - DEFINICIONES - */
/* Tokens provenientes del analizador léxico. */

%token INT
%token BOOL
%token VOID
%token MAIN
%token RETURN
%token NRO
%token TRUE
%token FALSE
%token ID

/* Precedencia y asociatividad de operadores. */

%left '+'
%left '*'

%%

P               : TipoMain MAIN '(' ')' '{' Declaraciones Sentencias '}'
                ;

TipoMain        : INT
                | BOOL
                | VOID
                ;

Declaraciones   : Decl Declaraciones
                | Decl
                ;

Decl            : Tipo ID ';'
                ;

Tipo            : INT
                | BOOL
                ;

Sentencias      : Sent Sentencias
                | Sent
                ;

Sent            : ID '=' E ';'
                | RETURN E ';'
                | RETURN ';'
                ;

E               : E '+' E   
                | E '*' E   
                | '(' E ')'   
                | NRO       
                | ID        
                | TRUE      
                | FALSE
                ;   

%%

/* - CÓDIGO DE USUARIO - */

void yyerror (const char *s) {
    fprintf (stderr, "ERROR SINTÁCTICO: producción no reconocida en la línea %d: %s\n", yylineno, s);
}

int main (int argc, char **argv) {
    if (argc > 1) {
        FILE *archivo = fopen (argv [1], "r");

        if (!archivo) {
            perror ("Error al abrir el archivo de entrada.\n");
            return 1;
        }

        yyin = archivo;
    }

    if (yyparse () == 0) {
        printf ("\n¡Análisis sintáctico finalizado con éxito!\n");
    }

    return 0;
}
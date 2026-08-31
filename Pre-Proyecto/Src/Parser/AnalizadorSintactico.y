%{

#include <stdio.h>
#include "AST.h"

extern int yylineno;

int yylex (void);
void yyerror (const char *s);

NodoAST *arbol = NULL;

%}

%code requires {
    #include "AST.h"
}

/* Valores que pueden transportar los tokens y las producciones. */

%union {
    int numero;
    char *identificador;
    TipoDato tipo_dato;
    NodoAST *nodo;
}

/* - DEFINICIONES - */
/* Tokens provenientes del analizador léxico. */

%token INT
%token BOOL
%token VOID
%token MAIN
%token RETURN
%token <numero> NRO
%token TRUE
%token FALSE
%token <identificador> ID

/* Tipos de valores de las producciones que construirán el AST. */

%type <nodo> E
%type <nodo> Decl
%type <nodo> Declaraciones
%type <nodo> Sent
%type <nodo> Sentencias
%type <tipo_dato> TipoMain
%type <tipo_dato> Tipo

/* Precedencia y asociatividad de operadores. */

%left '+'
%left '*'

%%

P               : TipoMain MAIN '(' ')' '{' Declaraciones Sentencias '}'        {
                                                                                    arbol = crear_nodo (AST_PROGRAMA);
                                                                                    arbol -> tipo_dato = $1;

                                                                                    agregar_hijo (arbol, $6);
                                                                                    agregar_hijo (arbol, $7);
                                                                                }
                ;

TipoMain        : INT                                                           {
                                                                                    $$ = TIPO_INT;
                                                                                }
                | BOOL                                                          {
                                                                                    $$ = TIPO_BOOL;
                                                                                }
                | VOID                                                          {
                                                                                    $$ = TIPO_VOID;
                                                                                }
                ;

Declaraciones   : Decl Declaraciones                                            {
                                                                                    $$ = crear_nodo (AST_DECLARACIONES);

                                                                                    agregar_hijo ($$, $1);
                                                                                    agregar_hijo ($$, $2);
                                                                                }
                | Decl                                                          {
                                                                                    $$ = crear_nodo (AST_DECLARACIONES);

                                                                                    agregar_hijo ($$, $1);
                                                                                }
                ;

Decl            : Tipo ID ';'                                                   {
                                                                                    $$ = crear_nodo (AST_DECLARACION);
                                                                                    $$ -> tipo_dato = $1;

                                                                                    NodoAST *identificador = crear_nodo (AST_IDENTIFICADOR);
                                                                                    identificador -> valor.identificador = $2;

                                                                                    agregar_hijo ($$, identificador);
                                                                                }
                ;

Tipo            : INT                                                           {
                                                                                    $$ = TIPO_INT;
                                                                                }
                | BOOL                                                          {
                                                                                    $$ = TIPO_BOOL;
                                                                                }
                ;

Sentencias      : Sent Sentencias                                               {
                                                                                    $$ = crear_nodo (AST_SENTENCIAS);

                                                                                    agregar_hijo ($$, $1);
                                                                                    agregar_hijo ($$, $2);
                                                                                }
                | Sent                                                          {
                                                                                    $$ = crear_nodo (AST_SENTENCIAS);

                                                                                    agregar_hijo ($$, $1);
                                                                                }
                ;

Sent            : ID '=' E ';'                                                  {
                                                                                    $$ = crear_nodo (AST_ASIGNACION);

                                                                                    NodoAST *identificador = crear_nodo (AST_IDENTIFICADOR);
                                                                                    identificador -> valor.identificador = $1;

                                                                                    agregar_hijo ($$, identificador);
                                                                                    agregar_hijo ($$, $3);
                                                                                }
                | RETURN E ';'                                                  {
                                                                                    $$ = crear_nodo (AST_RETURN);
                                                                                    agregar_hijo ($$, $2);
                                                                                }
                | RETURN ';'                                                    {
                                                                                    $$ = crear_nodo (AST_RETURN);
                                                                                }
                ;

E               : E '+' E                                                       {
                                                                                    $$ = crear_nodo (AST_SUMA);
                                                                                    agregar_hijo ($$, $1);
                                                                                    agregar_hijo ($$, $3);
                                                                                }
                | E '*' E                                                       {
                                                                                    $$ = crear_nodo (AST_MULTIPLICACION);
                                                                                    agregar_hijo ($$, $1);
                                                                                    agregar_hijo ($$, $3);
                                                                                }
                | '(' E ')'                                                     {
                                                                                    $$ = $2;
                                                                                }
                | NRO                                                           {
                                                                                    $$ = crear_nodo (AST_NUMERO);
                                                                                    $$ -> valor.numero = $1;
                                                                                }
                | ID                                                            {
                                                                                    $$ = crear_nodo (AST_IDENTIFICADOR);
                                                                                    $$ -> valor.identificador = $1;
                                                                                }
                | TRUE                                                          {
                                                                                    $$ = crear_nodo (AST_TRUE);
                                                                                    $$ -> tipo_dato = TIPO_BOOL;
                                                                                }
                | FALSE                                                         {
                                                                                    $$ = crear_nodo (AST_FALSE);
                                                                                    $$ -> tipo_dato = TIPO_BOOL;
                                                                                }
                ;   

%%

/* - CÓDIGO DE USUARIO - */

void yyerror (const char *s) {
    fprintf (stderr, "\nERROR SINTÁCTICO: producción no reconocida en la línea %d: %s\n", yylineno, s);
}
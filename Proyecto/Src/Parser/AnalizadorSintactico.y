%{

#include "AST.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * Analizador sintáctico de C-TDS.
 *
 * Bison recibe los tokens generados por el analizador léxico
 * y verifica que la secuencia de tokens respete la gramática
 * definida para el lenguaje.
 *
 * Durante el análisis sintáctico también se construye el
 * Árbol Sintáctico Abstracto (AST) correspondiente al
 * programa reconocido.
 */

/* Función proporcionada por el analizador léxico. */
extern int yylex (void);

/* Modo de depuración configurado por main. */
extern int modo_debug;

/* Indica si se produjo algún error léxico. */
extern int error_lexico;

/* Archivo de salida .sint. */
FILE *salida_sintactico = NULL;

/* Árbol Sintáctico Abstracto (AST) construido por Bison. */
NodoAST *arbol = NULL;

/* Función utilizada por Bison para informar errores sintácticos. */
void yyerror (const char *mensaje);

/* Prototipos de funciones auxiliares. */
static void registrar_sintaxis (const char *mensaje, int linea, int columna);
static void registrar_debug (const char *mensaje);

/* Lista auxiliar utilizada durante la construcción del AST. */
typedef struct ListaNodos {
    NodoAST **nodos;
    int cantidad;
    int capacidad;
} ListaNodos;

/* Prototipos de funciones auxiliares para listas. */
static ListaNodos *crear_lista_nodos (void);
static void agregar_nodo_lista (ListaNodos *lista, NodoAST *nodo);
static void agregar_nodos_lista (ListaNodos *destino, ListaNodos *origen);
static void agregar_hijo_al_inicio (NodoAST *padre, NodoAST *hijo);
static void liberar_lista_nodos (ListaNodos *lista);

%}

/* - DEFINICIONES - */

/* Permite utilizar los tipos definidos en AST.h. */
%code requires {
    #include "AST.h"

    typedef struct ListaNodos ListaNodos;
}

/* - Valores semánticos - */

%union {
    int numero;
    double real;
    char *identificador;
    TipoDato tipo_dato;
    NodoAST *nodo;
    ListaNodos *lista;
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

/* Tipos semánticos de las producciones. */

%type <nodo> expr
%type <nodo> literal
%type <nodo> int_literal
%type <nodo> bool_literal
%type <nodo> float_literal
%type <nodo> id
%type <nodo> method_call
%type <nodo> var_decl
%type <nodo> statement
%type <nodo> block
%type <nodo> method_decl

%type <lista> lista_argumentos
%type <lista> argumentos
%type <lista> lista_id
%type <lista> lista_id_resto
%type <lista> lista_var_decl
%type <lista> lista_declaraciones
%type <lista> lista_statement
%type <lista> lista_method_decl
%type <lista> method_decl_resto
%type <lista> lista_parametros
%type <lista> parametros
%type <lista> declaracion_tipo

%type <tipo_dato> type
%type <tipo_dato> tipo_method

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

program             :   lista_declaraciones                             {
                                                                            arbol = crear_nodo (AST_PROGRAMA, @1.first_line, @1.first_column);

                                                                            // Se transfieren las declaraciones al programa.
                                                                            for (int i = 0; i < $1 -> cantidad; i ++) {
                                                                                agregar_hijo (arbol, $1 -> nodos [i]);
                                                                            }

                                                                            liberar_lista_nodos ($1);

                                                                            registrar_sintaxis ("Programa reconocido.", @$.first_line, @$.first_column);
                                                                        }
                    ;

    /* - Declaraciones - */

lista_declaraciones :   %empty                                          {
                                                                            $$ = crear_lista_nodos ();
                                                                        }
                    |   type id declaracion_tipo                        {
                                                                            $$ = $3;

                                                                            NodoAST *declaracion = $$ -> nodos [0];

                                                                            // Se completa el tipo de la primera declaración.
                                                                            declaracion -> tipo_dato = $1;

                                                                            // El identificador debe ser el primer hijo.
                                                                            agregar_hijo_al_inicio (declaracion, $2);
                                                                        }
                    |   VOID id method_decl_resto lista_method_decl     {
                                                                            $$ = crear_lista_nodos ();

                                                                            NodoAST *funcion = crear_nodo (AST_DECLARACION_FUNCION, @2.first_line, @2.first_column);
                                                                            funcion -> tipo_dato = TIPO_VOID;

                                                                            agregar_hijo (funcion, $2);

                                                                            // Se agregan parámetros y bloque.
                                                                            for (int i = 0; i < $3 -> cantidad; i ++) {
                                                                                agregar_hijo (funcion, $3 -> nodos [i]);
                                                                            }
                                                                            
                                                                            liberar_lista_nodos ($3);

                                                                            agregar_nodo_lista ($$, funcion);

                                                                            // Se agregan las funciones restantes.
                                                                            agregar_nodos_lista ($$, $4);

                                                                            registrar_sintaxis ("Declaración de función reconocida.", @$.first_line, @$.first_column);
                                                                        }
                    ;

declaracion_tipo    :   lista_id_resto ';' lista_declaraciones          {
                                                                            $$ = crear_lista_nodos ();

                                                                            NodoAST *declaracion = crear_nodo (AST_DECLARACION_VARIABLE, @1.first_line, @1.first_column);

                                                                            // Se agregan los identificadores posteriores al primero.
                                                                            for (int i = 0; i < $1 -> cantidad; i ++) {
                                                                                agregar_hijo (declaracion, $1 -> nodos [i]);
                                                                            }

                                                                            liberar_lista_nodos ($1);

                                                                            agregar_nodo_lista ($$, declaracion);

                                                                            // Se agregan las declaraciones siguientes.
                                                                            agregar_nodos_lista ($$, $3);

                                                                            registrar_sintaxis ("Declaración de variable reconocida.", @$.first_line, @$.first_column);
                                                                        }
                    |   method_decl_resto lista_method_decl             {
                                                                            $$ = crear_lista_nodos ();

                                                                            NodoAST *funcion = crear_nodo (AST_DECLARACION_FUNCION, @1.first_line, @1.first_column);

                                                                            // Se agregan parámetros y bloque.
                                                                            for (int i = 0; i < $1 -> cantidad; i ++) {
                                                                                agregar_hijo (funcion, $1 -> nodos [i]);
                                                                            }

                                                                            liberar_lista_nodos ($1);

                                                                            agregar_nodo_lista ($$, funcion);

                                                                            // Se agregan las funciones siguientes.
                                                                            agregar_nodos_lista ($$, $2);

                                                                            registrar_sintaxis ("Declaración de función reconocida.", @$.first_line, @$.first_column);
                                                                        }
                    ;

lista_id_resto      :   %empty                                          {
                                                                            $$ = crear_lista_nodos ();
                                                                        }
                    |   lista_id_resto ',' id                           {
                                                                            $$ = $1;

                                                                            agregar_nodo_lista ($$, $3);
                                                                        }
                    ;

method_decl_resto   :   '(' lista_parametros ')' block                  {
                                                                            $$ = crear_lista_nodos ();

                                                                            // Se agregan los parámetros.
                                                                            for (int i = 0; i < $2 -> cantidad; i ++) {
                                                                                agregar_nodo_lista ($$, $2 -> nodos [i]);
                                                                            }

                                                                            liberar_lista_nodos ($2);
                                                                            
                                                                            // El último elemento es el bloque de la función.
                                                                            agregar_nodo_lista ($$, $4);
                                                                        }
                    ;

lista_method_decl   :   %empty                                          {
                                                                            $$ = crear_lista_nodos ();
                                                                        }
                    |   method_decl lista_method_decl                   {
                                                                            $$ = crear_lista_nodos ();

                                                                            agregar_nodo_lista ($$, $1);

                                                                            // Se agregan las funciones restantes.
                                                                            agregar_nodos_lista ($$, $2);
                                                                        }
                    ;

method_decl         :   tipo_method id '(' lista_parametros ')' block   {
                                                                            $$ = crear_nodo (AST_DECLARACION_FUNCION, @2.first_line, @2.first_column);
                                                                            $$ -> tipo_dato = $1;

                                                                            agregar_hijo ($$, $2);

                                                                            // Se agregan los parámetros.
                                                                            for (int i = 0; i < $4 -> cantidad; i ++) {
                                                                                agregar_hijo ($$, $4 -> nodos [i]);
                                                                            }

                                                                            liberar_lista_nodos ($4);

                                                                            agregar_hijo ($$, $6);

                                                                            registrar_sintaxis ("Declaración de función reconocida.", @$.first_line, @$.first_column);
                                                                        }
                    ;

tipo_method         :   type                                            {
                                                                            $$ = $1;
                                                                        }
                    |   VOID                                            {
                                                                            $$ = TIPO_VOID;
                                                                        }
                    ;

    /* - Parámetros - */

lista_parametros    :   %empty                                          {
                                                                            $$ = crear_lista_nodos ();
                                                                        }
                    |   parametros                                      {
                                                                            $$ = $1;
                                                                        }
                    ;

parametros          :   type id                                         {
                                                                            $$ = crear_lista_nodos ();

                                                                            NodoAST *parametro = crear_nodo (AST_PARAMETRO, @2.first_line, @2.first_column);
                                                                            parametro -> tipo_dato = $1;

                                                                            agregar_hijo (parametro, $2);
                                                                            agregar_nodo_lista ($$, parametro);
                                                                        }
                    |   parametros ',' type id                          {
                                                                            $$ = $1;

                                                                            NodoAST *parametro = crear_nodo (AST_PARAMETRO, @4.first_line, @4.first_column);
                                                                            parametro -> tipo_dato = $3;

                                                                            agregar_hijo (parametro, $4);
                                                                            agregar_nodo_lista ($$, parametro);
                                                                        }
                    ;

    /* - Bloques - */

block               :   '{' lista_var_decl lista_statement '}'          {
                                                                            $$ = crear_nodo (AST_BLOQUE, @1.first_line, @1.first_column);

                                                                            // Las declaraciones aparecen antes que las sentencias.
                                                                            for (int i = 0; i < $2 -> cantidad; i ++) {
                                                                                agregar_hijo ($$, $2 -> nodos [i]);
                                                                            }

                                                                            liberar_lista_nodos ($2);

                                                                            // Se conservan las sentencias en orden.
                                                                            for (int i = 0; i < $3 -> cantidad; i ++) {
                                                                                agregar_hijo ($$, $3 -> nodos [i]);
                                                                            }

                                                                            liberar_lista_nodos ($3);

                                                                            registrar_sintaxis ("Bloque reconocido.", @$.first_line, @$.first_column);
                                                                        }
                    ;

lista_var_decl      :   %empty                                          {
                                                                            $$ = crear_lista_nodos ();
                                                                        }
                    |   lista_var_decl var_decl                         {
                                                                            $$ = $1;

                                                                            agregar_nodo_lista ($$, $2);
                                                                        }
                    ;

var_decl            :   type lista_id ';'                               {
                                                                            $$ = crear_nodo (AST_DECLARACION_VARIABLE, @2.first_line, @2.first_column);
                                                                            
                                                                            $$ -> tipo_dato = $1;

                                                                            // Se agregan todos los identificadores de la declaración.
                                                                            for (int i = 0; i < $2 -> cantidad; i ++) {
                                                                                agregar_hijo ($$, $2 -> nodos [i]);
                                                                            }

                                                                            liberar_lista_nodos ($2);

                                                                            registrar_sintaxis ("Declaración de variable reconocida.", @$.first_line, @$.first_column);
                                                                        }
                    ;

lista_id            :   id                                              {
                                                                            $$ = crear_lista_nodos ();

                                                                            agregar_nodo_lista ($$, $1);
                                                                        }
                    |   lista_id ',' id                                 {
                                                                            $$ = $1;

                                                                            agregar_nodo_lista ($$, $3);
                                                                        }
                    ;

    /* - Sentencias - */

lista_statement     :   %empty                                          {
                                                                            $$ = crear_lista_nodos ();
                                                                        }
                    |   lista_statement statement                       {
                                                                            $$ = $1;

                                                                            agregar_nodo_lista ($$, $2);
                                                                        }
                    ;

statement           :   id '=' expr ';'                                 { 
                                                                            $$ = crear_nodo (AST_ASIGNACION, @2.first_line, @2.first_column);

                                                                            agregar_hijo ($$, $1);
                                                                            agregar_hijo ($$, $3);

                                                                            registrar_sintaxis ("Asignación reconocida.", @$.first_line, @$.first_column);
                                                                        }
                    |   method_call ';'                                 {
                                                                            $$ = $1;

                                                                            registrar_sintaxis ("Llamada a función reconocida.", @$.first_line, @$.first_column);
                                                                        }
                    |   IF '(' expr ')' block                           {
                                                                            $$ = crear_nodo (AST_IF, @1.first_line, @1.first_column);

                                                                            agregar_hijo ($$, $3);
                                                                            agregar_hijo ($$, $5);    
                    
                                                                            registrar_sintaxis ("Sentencia if reconocida.", @$.first_line, @$.first_column);
                                                                        }
                    |   IF '(' expr ')' block ELSE block                {
                                                                            $$ = crear_nodo (AST_IF, @1.first_line, @1.first_column);

                                                                            agregar_hijo ($$, $3);
                                                                            agregar_hijo ($$, $5);
                                                                            agregar_hijo ($$, $7);
                        
                                                                            registrar_sintaxis ("Sentencia if-else reconocida.", @$.first_line, @$.first_column);
                                                                        }
                    |   WHILE '(' expr ')' block                        {
                                                                            $$ = crear_nodo (AST_WHILE, @1.first_line, @1.first_column);

                                                                            agregar_hijo ($$, $3);
                                                                            agregar_hijo ($$, $5);
                        
                                                                            registrar_sintaxis ("Sentencia while reconocida.", @$.first_line, @$.first_column);
                                                                        }
                    |   RETURN ';'                                      {
                                                                            $$ = crear_nodo (AST_RETURN, @1.first_line, @1.first_column);

                                                                            registrar_sintaxis ("Return sin expresión reconocido.", @$.first_line, @$.first_column);
                                                                        }
                    |   RETURN expr ';'                                 {
                                                                            $$ = crear_nodo (AST_RETURN, @1.first_line, @1.first_column);

                                                                            agregar_hijo ($$, $2);
                        
                                                                            registrar_sintaxis ("Return con expresión reconocido.", @$.first_line, @$.first_column);
                                                                        }
                    |   ';'                                             {
                                                                            $$ = crear_nodo (AST_SENTENCIA_VACIA, @1.first_line, @1.first_column);
                        
                                                                            registrar_sintaxis ("Sentencia vacía reconocida.", @$.first_line, @$.first_column);
                                                                        }
                    |   block                                           {
                                                                            $$ = $1;
                                                                        }
                    ;

    /* - Llamadas a funciones - */

method_call         :   id '(' lista_argumentos ')'                     {
                                                                            $$ = crear_nodo (AST_LLAMADA, @1.first_line, @1.first_column);

                                                                            $$ -> valor.identificador = $1 -> valor.identificador;

                                                                            // El nombre pasa a ser propiedad de la llamada.
                                                                            $1 -> valor.identificador = NULL;
                                                                            free ($1);

                                                                            // Se agregan los argumentos.
                                                                            for (int i = 0; i < $3 -> cantidad; i ++) {
                                                                                agregar_hijo ($$, $3 -> nodos [i]);
                                                                            }

                                                                            liberar_lista_nodos ($3);
                                                                        }
                    ;

lista_argumentos    :   %empty                                          {
                                                                            $$ = crear_lista_nodos ();
                                                                        }
                    |   argumentos                                      {
                                                                            $$ = $1;
                                                                        }
                    ;

argumentos          :   expr                                            {
                                                                            $$ = crear_lista_nodos ();

                                                                            agregar_nodo_lista ($$, $1);
                                                                        }
                    |   argumentos ',' expr                             {
                                                                            $$ = $1;

                                                                            agregar_nodo_lista ($$, $3);
                                                                        }
                    ;

    /* - Expresiones - */

expr                :   id                                              {
                                                                            $$ = $1;
                                                                        }
                    |   method_call                                     {
                                                                            $$ = $1;
                                                                        }
                    |   literal                                         {
                                                                            $$ = $1;
                                                                        }
                    |   expr '+' expr                                   {
                                                                            $$ = crear_nodo (AST_SUMA, @2.first_line, @2.first_column);

                                                                            agregar_hijo ($$, $1);
                                                                            agregar_hijo ($$, $3);

                                                                            registrar_debug ("Reducción: expresión + expresión.");
                                                                        }
                    |   expr '-' expr                                   {
                                                                            $$ = crear_nodo (AST_RESTA, @2.first_line, @2.first_column);

                                                                            agregar_hijo ($$, $1);
                                                                            agregar_hijo ($$, $3);

                                                                            registrar_debug ("Reducción: expresión - expresión.");
                                                                        }
                    |   expr '*' expr                                   {
                                                                            $$ = crear_nodo (AST_MULTIPLICACION, @2.first_line, @2.first_column);

                                                                            agregar_hijo ($$, $1);
                                                                            agregar_hijo ($$, $3);

                                                                            registrar_debug ("Reducción: expresión * expresión.");
                                                                        }
                    |   expr '/' expr                                   {
                                                                            $$ = crear_nodo (AST_DIVISION, @2.first_line, @2.first_column);

                                                                            agregar_hijo ($$, $1);
                                                                            agregar_hijo ($$, $3);

                                                                            registrar_debug ("Reducción: expresión / expresión.");
                                                                        }
                    |   expr '%' expr                                   {
                                                                            $$ = crear_nodo (AST_MODULO, @2.first_line, @2.first_column);

                                                                            agregar_hijo ($$, $1);
                                                                            agregar_hijo ($$, $3);

                                                                            registrar_debug ("Reducción: expresión % expresión.");
                                                                        }
                    |   expr '<' expr                                   {
                                                                            $$ = crear_nodo (AST_MENOR, @2.first_line, @2.first_column);

                                                                            agregar_hijo ($$, $1);
                                                                            agregar_hijo ($$, $3);

                                                                            registrar_debug ("Reducción: expresión < expresión.");
                                                                        }
                    |   expr '>' expr                                   {
                                                                            $$ = crear_nodo (AST_MAYOR, @2.first_line, @2.first_column);

                                                                            agregar_hijo ($$, $1);
                                                                            agregar_hijo ($$, $3);

                                                                            registrar_debug ("Reducción: expresión > expresión.");
                                                                        }
                    |   expr IGUAL expr                                 {
                                                                            $$ = crear_nodo (AST_IGUAL, @2.first_line, @2.first_column);

                                                                            agregar_hijo ($$, $1);
                                                                            agregar_hijo ($$, $3);

                                                                            registrar_debug ("Reducción: expresión == expresión.");
                                                                        }
                    |   expr AND expr                                   {
                                                                            $$ = crear_nodo (AST_AND, @2.first_line, @2.first_column);

                                                                            agregar_hijo ($$, $1);
                                                                            agregar_hijo ($$, $3);

                                                                            registrar_debug ("Reducción: expresión && expresión.");
                                                                        }
                    |   expr OR expr                                    {
                                                                            $$ = crear_nodo (AST_OR, @2.first_line, @2.first_column);

                                                                            agregar_hijo ($$, $1);
                                                                            agregar_hijo ($$, $3);

                                                                            registrar_debug ("Reducción: expresión || expresión.");
                                                                        }
                    |   '-' expr %prec UMINUS                           {
                                                                            $$ = crear_nodo (AST_MENOS_UNARIO, @1.first_line, @1.first_column);

                                                                            agregar_hijo ($$, $2);

                                                                            registrar_debug ("Reducción: - expresión.");
                                                                        }
                    |   '!' expr                                        {
                                                                            $$ = crear_nodo (AST_NEGACION, @1.first_line, @1.first_column);

                                                                            agregar_hijo ($$, $2);

                                                                            registrar_debug ("Reducción: ! expresión.");
                                                                        }
                    |   '(' expr ')'                                    {
                                                                            $$ = $2;

                                                                            registrar_debug ("Reducción: ( expresión ).");
                                                                        }
                    ;

    /* - Literales - */

literal             :   int_literal                                     {
                                                                            $$ = $1;
                                                                        }
                    |   bool_literal                                    {
                                                                            $$ = $1;
                                                                        }
                    |   float_literal                                   {
                                                                            $$ = $1;
                                                                        }
                    ;

int_literal         :   NRO                                             {
                                                                            $$ = crear_nodo (AST_NUMERO, @1.first_line, @1.first_column);
                                                                            
                                                                            $$ -> valor.numero = $1;
                                                                            $$ -> tipo_dato = TIPO_INT;
                                                                        }
                    ;

bool_literal        :   TRUE                                            {
                                                                            $$ = crear_nodo (AST_TRUE, @1.first_line, @1.first_column);

                                                                            $$ -> tipo_dato = TIPO_BOOLEAN;
                                                                        }
                    |   FALSE                                           {
                                                                            $$ = crear_nodo (AST_FALSE, @1.first_line, @1.first_column);

                                                                            $$ -> tipo_dato = TIPO_BOOLEAN;
                                                                        }
                    ;

float_literal       :   REAL                                            {
                                                                            $$ = crear_nodo (AST_REAL, @1.first_line, @1.first_column);

                                                                            $$ -> valor.real = $1;
                                                                            $$ -> tipo_dato = TIPO_FLOAT;
                                                                        }
                    ;

    /* - Identificadores - */

id                  :   ID                                              {
                                                                            $$ = crear_nodo (AST_IDENTIFICADOR, @1.first_line, @1.first_column);
                                                                            $$ -> valor.identificador = $1;
                                                                        }
                    ;

    /* - Tipos - */

type                :   INT                                             {
                                                                            $$ = TIPO_INT;
                                                                        }
                    |   BOOLEAN                                         {
                                                                            $$ = TIPO_BOOLEAN;
                                                                        }
                    |   FLOAT                                           {
                                                                            $$ = TIPO_FLOAT;
                                                                        }
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
 * Informa un error sintáctico indicando su ubicación.
 */
void yyerror (const char *mensaje) {    
    fprintf (stderr, "ERROR SINTÁCTICO: línea %d, columna %d - %s.\n", yylloc.first_line, yylloc.first_column, mensaje);
}

/*
 * Crea una lista auxiliar vacía.
 *
 * La lista solamente administra los punteros a los nodos;
 * no es propietaria de los nodos almacenados.
 */
static ListaNodos *crear_lista_nodos (void) {
    ListaNodos *lista = malloc (sizeof (ListaNodos));

    if (lista == NULL) {
        fprintf (stderr, "ERROR: No se pudo reservar memoria para una lista de nodos.\n");
        exit (EXIT_FAILURE);
    }

    lista -> nodos = NULL;
    lista -> cantidad = 0;
    lista -> capacidad = 0;

    return lista;
}

/*
 * Agrega un nodo al final de una lista auxiliar.
 */
static void agregar_nodo_lista (ListaNodos *lista, NodoAST *nodo) {
    NodoAST **nuevos_nodos;
    int nueva_capacidad;

    if (lista == NULL || nodo == NULL) {
        return;
    }

    if (lista -> cantidad >= lista -> capacidad) {
        if (lista -> capacidad == 0) {
            nueva_capacidad = 4;
        }
        else {
            nueva_capacidad = lista -> capacidad * 2;
        }

        nuevos_nodos = realloc (lista -> nodos, nueva_capacidad * sizeof (NodoAST *));

        if (nuevos_nodos == NULL) {
            fprintf (stderr, "ERROR: No se pudo ampliar la lista de nodos.\n");
            exit (EXIT_FAILURE);
        }

        lista -> nodos = nuevos_nodos;
        lista -> capacidad = nueva_capacidad;
    }

    lista -> nodos [lista -> cantidad] = nodo;
    lista -> cantidad ++;
}

/*
 * Transfiere todos los nodos de una lista a otra.
 *
 * La lista de origen se libera después de transferir
 * sus punteros, pero los nodos permanecen vivos.
 */
static void agregar_nodos_lista (ListaNodos *destino, ListaNodos *origen) {
    if (origen == NULL || destino == NULL) {
        return;
    }

    for (int i = 0; i < origen -> cantidad; i ++) {
        agregar_nodo_lista (destino, origen -> nodos [i]);
    }

    liberar_lista_nodos (origen);
}

/*
 * Agrega un hijo al principio de un nodo AST.
 */
static void agregar_hijo_al_inicio (NodoAST *padre, NodoAST *hijo) {
    if (padre == NULL || hijo == NULL) {
        return;
    }

    agregar_hijo (padre, hijo);

    for (int i = padre -> cantidad_hijos - 1; i > 0; i --) {
        padre -> hijos [i] = padre -> hijos [i - 1];
    }

    padre -> hijos [0] = hijo;
}

/*
 * Libera únicamente la estructura auxiliar y su arreglo 
 * de punteros. No libera los nodos almacenados.
 */
static void liberar_lista_nodos (ListaNodos *lista) {
    if (lista == NULL) {
        return;
    }

    free (lista -> nodos);
    free (lista);
}
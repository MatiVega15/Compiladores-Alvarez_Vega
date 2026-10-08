# ***Especificación del Lenguaje C-TDS***

---

## **1.1. Gramática**

La **especificación sintáctica del lenguaje C-TDS** es proporcionada por la cátedra. **La gramática define la estructura sintáctica de los programas**, incluyendo declaraciones de variables, declaraciones de funciones, bloques, sentencias, llamadas a funciones y expresiones.

**Toda ambigüedad presente en la gramática se resolverá posteriormente con la herramienta Bison, en el análisis sintáctico**, en caso de presentarse conflictos.

La **gramática especificada** se puede representar como:

$$G = \langle V_N,\ V_T,\ R,\ S \rangle$$

donde:

- **$V_N$**: conjunto de símbolos no terminales.
- **$V_T$**: conjunto de símbolos terminales.
- **$R$**: conjunto de reglas de producción.
- **$S$**: símbolo inicial de la gramática.

### *Notación utilizada*

| Notación | Significado |
|---|---|
| `<simb>` | `<simb>` es un no-terminal. |
| `simb` | `simb` es un terminal. |
| `[x]` | Cero o una ocurrencia de `x`, es decir, `x` es opcional. |
| `x*` | Cero o más ocurrencias de `x`. |
| `x⁺,` | Una lista de una o más ocurrencias de `x` separadas por coma. |
| `{ }` | Las llaves se utilizan para agrupar elementos. Notar que las llaves entre comillas `'{'` y `'}'` son terminales. |
| `\|` | Separa alternativas. |

### *Símbolos no terminales*

Los símbolos no terminales representan las diferentes **construcciones sintácticas** del lenguaje.

```math
V_N =
    \{
        \langle program \rangle,\ 
        \langle var\_decl \rangle,\ 
        \langle method\_decl \rangle,\ 
        \langle block \rangle,\ 
        \langle type \rangle,\ 
        \langle statement \rangle,\ 
        \langle method\_call \rangle,\ 
        \langle expr \rangle,\ 
        \langle bin\_op \rangle,\ 
        \langle arith\_op \rangle,\ 
        \langle rel\_op \rangle,\ 
        \langle cond\_op \rangle,\ 
        \langle literal \rangle,\ 
        \langle id \rangle,\ 
        \langle alpha\_num \rangle,\ 
        \langle alpha \rangle,\ 
        \langle digit \rangle,\ 
        \langle int\_literal \rangle,\ 
        \langle bool\_literal \rangle,\ 
        \langle float\_literal \rangle
    \}
```
### *Símbolos terminales*

Los símbolos terminales son los **elementos que aparecen directamente en los programas escritos en C-TDS**.

```math
V_T =
    \{
        int,\ 
        boolean,\ 
        float,\ 
        void,\ 
        if,\ 
        else,\ 
        while,\ 
        return,\ 
        true,\ 
        false,\ 
        +,\ 
        -,\ 
        *,\ 
        /,\ 
        \%,\ 
        <,\ 
        >,\ 
        ==,\ 
        \&\&,\ 
        ||,\ 
        !,\ 
        =,\ 
        (,\ 
        ),\ 
        \{,\ 
        \},\ 
        ;,\ 
        .,\ 
        ,
    \}
```

### *Símbolo inicial*

El **símbolo inicial** de la gramática es:

$$S = \langle program \rangle$$

### *Reglas de producción*

Las **reglas de producción especificadas** por la cátedra son las siguientes.

#### **- Programa**

La estructura general de un programa está formada por **cero o más declaraciones de variables globales**, seguidas de **cero o más declaraciones de funciones**. Sin embargo, semánticamente, se obliga a que todo programa tenga, **por lo menos, una función principal (main) que no recibe parámetros**.

```math
\langle program \rangle \rightarrow
\langle var\_decl \rangle^{*}\ 
\langle method\_decl \rangle^{*}
```

#### **- Declaración de variables**

Una declaración de variable comienza con un **tipo** y permite declarar **uno o más identificadores**, separados por **comas** y finalizados con **punto y coma**.

```math
\langle var\_decl \rangle \rightarrow
\langle type \rangle\ \{\langle id \rangle\}^+,\ ;
```

#### **- Declaración de funciones**

Una declaración de función puede **devolver un tipo** (`int`, `boolean` o `float`) o **no devolver ningún valor** (`void`). Luego se especifica el **identificador** de la función, su lista de **parámetros** (puede no tener) y finalmente su **bloque**.

```math
\langle method\_decl \rangle \rightarrow
\{ \langle type \rangle\ |\ void \}
\ \langle id \rangle
\ (\ 
[
\{ \langle type \rangle\ \langle id \rangle \}^{+},
]
\ )\ 
\ \langle block \rangle
```

#### **- Bloque**

Un bloque **comienza y termina con llaves**. Dentro de él pueden aparecer **cero o más declaraciones de variables** y **cero o más sentencias**.

Es decir, se permiten **bloques vacíos**, y también **bloques con declaraciones pero sin sentencias y viceversa**.

```math
\langle block \rangle \rightarrow
\text{'\{'}\ 
\langle var\_decl \rangle^*\ 
\langle statement \rangle^*\ 
\text{'\}'}
```

#### **- Tipo**

Los tipos disponibles para variables y funciones son **entero**, **booleano** y **real**.

```math
\langle type \rangle \rightarrow
int \ |\ boolean\ |\ float
```

#### **- Sentencias**

Las sentencias permiten realizar **asignaciones, llamadas a funciones, estructuras condicionales, estructuras repetitivas, retornos, sentencias vacías y bloques**.

```math
\begin{aligned}
\langle statement \rangle \rightarrow\ &
\langle id \rangle = \langle expr \rangle\ ; \\
|\ &\langle method\_call \rangle\ ; \\
|\ &\text{if}\ (\ \langle expr \rangle\ )\ \langle block \rangle\ 
[\text{else}\ \langle block \rangle] \\
|\ &\text{while}\ (\ \langle expr \rangle\ )\ \langle block \rangle \\
|\ &\text{return}\ [\langle expr \rangle]\ ; \\
|\ &; \\
|\ &\langle block \rangle
\end{aligned}
```

Las asignaciones se permiten sobre variables y parámetros de tipo `int`, `float` o `boolean`. El **tipo de la expresión asignada** debe ser compatible con el tipo de la ubicación de acuerdo con las reglas de compatibilidad y coerción del lenguaje.

Se permiten **conversiones implícitas** entre los tipos numéricos `int` y `float`. La conversión de `int` a `float` conserva el valor numérico, mientras que la conversión de `float` a `int` trunca la parte decimal.

No se permiten conversiones implícitas entre `boolean` y los tipos numéricos.

#### **- Llamada a funciones**

Una llamada a una función está formada por el **identificador** de la función, seguido de una lista, posiblemente vacía, de expresiones que representan los **argumentos**.

```math
\langle method\_call \rangle \rightarrow
\langle id \rangle
\ (\ 
[\langle expr \rangle^{+},]\ 
)
```

#### **- Expresiones**

Las expresiones pueden estar formadas por **identificadores, llamadas a funciones, literales, operaciones binarias, operaciones unarias o expresiones entre paréntesis**.

```math
\begin{aligned}
\langle expr \rangle \rightarrow\ &
\langle id \rangle \\
|\ &\langle method\_call \rangle \\
|\ &\langle literal \rangle \\
|\ &\langle expr \rangle\ \langle bin\_op \rangle\ \langle expr \rangle \\
|\ &-\langle expr \rangle \\
|\ &!\langle expr \rangle \\
|\ &(\ \langle expr \rangle\ )
\end{aligned}
```

#### **- Operadores binarios**

Los operadores binarios se clasifican en **operadores aritméticos, relacionales y condicionales**.

```math
\langle bin\_op \rangle \rightarrow
\langle arith\_op \rangle
\ |\ 
\langle rel\_op \rangle
\ |\ 
\langle cond\_op \rangle
```

#### **- Operadores aritméticos**

Los operadores aritméticos disponibles son **suma, resta, multiplicación, división y módulo**.

```math
\langle arith\_op \rangle \rightarrow
+\ |\ -\ |\ *\ |\ /\ |\ \%
```

#### **- Operadores relacionales**

Los operadores relacionales permiten **comparar valores**.

```math
\langle rel\_op \rangle \rightarrow
\ <\ |\ >\ |\ ==
```

#### **- Operadores condicionales**

Los operadores condicionales permiten realizar **operaciones lógicas AND y OR**.

```math
\langle cond\_op \rangle \rightarrow
\&\&\ |\ ||
```

#### **- Literales**

Los literales pueden ser **enteros, booleanos o reales**.

```math
\langle literal \rangle \rightarrow
\langle int\_literal \rangle
\ |\ 
\langle bool\_literal \rangle
\ |\ 
\langle float\_literal \rangle
```

#### **- Identificadores**

Un identificador **comienza con una letra y puede continuar con cero o más letras, dígitos o guiones bajos**. Las letras pueden ser **mayúsculas o minúsculas** en cualquier posición.

```math
\langle id \rangle \rightarrow
\langle alpha \rangle\ \langle alpha\_num \rangle^*
```

#### **- Caracteres alfanuméricos**

Un caracter alfanumérico puede ser **una letra, un dígito o un '_'**.

```math
\langle alpha\_num \rangle \rightarrow
\langle alpha \rangle
\ |\ 
\langle digit \rangle
\ |\ 
\_
```

#### **- Letras**

Las letras pueden ser **mayúsculas o minúsculas.**

```math
\langle alpha \rangle \rightarrow
a\ |\ b\ |\ \cdots\ |\ z
\ |\ 
A\ |\ B\ |\ \cdots\ |\ Z
```

#### **- Dígitos**

Los dígitos disponibles son los comprendidos entre **0 y 9**.

```math
\langle digit \rangle \rightarrow
0\ |\ 1\ |\ \cdots\ |\ 9
```

#### **- Literales enteros**

Un literal entero está formado por **uno o más dígitos**. Se permite la presencia de ceros no significativos, por ejemplo, números del estilo `0240`.

```math
\langle int\_literal \rangle \rightarrow
\langle digit \rangle\ \langle digit \rangle^*
```

Los valores enteros representables por el lenguaje utilizan una representación **con signo de 32 bits**, por lo que sus valores deben encontrarse dentro del rango `-2147483648` a `2147483647`. El signo negativo se interpreta mediante el operador unario `-` y no forma parte del literal entero.

#### **- Literales booleanos**

Los literales booleanos pueden tomar los valores `true` o `false`.

```math
\langle bool\_literal \rangle \rightarrow
true\ |\ false
```

#### **- Literales reales**

Un literal real está formado por **una o más cifras, seguido de un punto y de una o más cifras**. Se permite la presencia de ceros no significativos, por ejemplo, números del estilo `00005.2500`. 

```math
\langle float\_literal \rangle \rightarrow
\langle digit \rangle\ \langle digit \rangle^*
\ \text{.}\ 
\langle digit \rangle\ \langle digit \rangle^*
```

Los literales reales siguen la **notación utilizada en C**.

---

## **1.2. Gramática transformada para Bison**

Para implementar el análisis sintáctico mediante **Bison**, se realiza una **transformación de la gramática** especificada anteriormente. Esta transformación permite expresar explícitamente las listas de elementos y separar determinadas construcciones que en la gramática original se presentan mediante los operadores de notación `*`, `+` y `[]`.

La transformación **no modifica el lenguaje generado por la gramática original**, sino que introduce símbolos no terminales auxiliares para facilitar su implementación mediante Bison.

Además, la **precedencia y la asociatividad** de los operadores de las expresiones se especifican mediante las declaraciones de Bison, evitando la necesidad de introducir diferentes niveles de no terminales para cada precedencia.

Las **reglas** utilizadas en el analizador sintáctico son las siguientes.

### *- Programa y declaraciones*

```text
<program> → <lista_declaraciones>

<lista_declaraciones> → λ
                      | <type> <id> <declaracion_tipo>
                      | void <id> <method_decl_resto> <lista_method_decl>

<declaracion_tipo> → <lista_id_resto> ; <lista_declaraciones>
                   | <method_decl_resto> <lista_method_decl>

<lista_id_resto> → λ
                 | <lista_id_resto> , <id>

<lista_method_decl> → λ
                    | <method_decl> <lista_method_decl>
```

Estas producciones permiten representar la **secuencia de declaraciones globales** y **distinguir entre declaraciones de variables y declaraciones de funciones**.

### *- Declaraciones de funciones*

```text
<method_decl> → <tipo_method> <id> ( <lista_parametros> ) <block>

<tipo_method> → <type>
              | void

<method_decl_resto> → ( <lista_parametros> ) <block>
``` 

El no terminal `<method_decl_resto>` permite **reutilizar la parte común de una declaración de función** una vez reconocidos su tipo y su identificador.

### *- Parámetros*

```text
<lista_parametros> → λ
                   | <parametros>

<parametros> → <type> <id>
             | <parametros> , <type> <id>
```

Estas producciones permiten **representar tanto funciones sin parámetros como funciones con uno o más parámetros** separados por comas.

### *- Bloques*

```text
<block> → { <lista_var_decl> <lista_statement> }

<lista_var_decl> → λ
                 | <lista_var_decl> <var_decl>

<var_decl> → <type> <lista_id> ;

<lista_id> → <id>
           | <lista_id> , <id>
```

La separación entre `<lista_var_decl>` y `<lista_statement>` permite mantener la **restricción de que las declaraciones de variables aparecen antes de las sentencias dentro de un bloque**.

Las producciones $\lambda$ permiten representar **bloques vacíos**.

### *- Sentencias*

```text
<lista_statement> → λ
                   | <lista_statement> <statement>

<statement> → <id> = <expr> ;
            | <method_call> ;
            | if ( <expr> ) <block>
            | if ( <expr> ) <block> else <block>
            | while ( <expr> ) <block>
            | return ;
            | return <expr> ;
            | ;
            | <block>
```

Estas producciones representan las diferentes **formas de sentencia** definidas por la especificación del lenguaje.

### *- Llamadas a funciones*

```text
<method_call> → <id> ( <lista_argumentos> )

<lista_argumentos> → λ
                   | <argumentos>

<argumentos> → <expr>
            | <argumentos> , <expr>
```

Estas producciones permiten representar **llamadas a funciones tanto sin argumentos como con uno o más argumentos** separados por comas.

### *- Expresiones*

```text
<expr> → <id>
       | <method_call>
       | <literal>
       | <expr> + <expr>
       | <expr> - <expr>
       | <expr> * <expr>
       | <expr> / <expr>
       | <expr> % <expr>
       | <expr> < <expr>
       | <expr> > <expr>
       | <expr> == <expr>
       | <expr> && <expr>
       | <expr> || <expr>
       | - <expr>
       | ! <expr>
       | ( <expr> )
```

Para establecer **la precedencia y la asociatividad de los operadores** y resolver los posibles **conflictos** derivados de las expresiones ambiguas, **Bison** utiliza las siguientes declaraciones, ordenadas **de menor a mayor precedencia**:

```text
%left OR
%left AND
%nonassoc IGUAL
%nonassoc '<' '>'
%left '+' '-'
%left '*' '/' '%'
%right '!'
%right UMINUS
```

El símbolo `UMINUS` es un **símbolo auxiliar** utilizado por **Bison** para asignar una precedencia específica al operador `-` cuando se utiliza como operador unario.

Los operadores `==`, `<` y `>` se declaran como **no asociativos**. Por lo tanto, expresiones como `x == y == z`, `x < y < z` o `x < y > z` no son aceptadas por el analizador sintáctico.

### *- Literales*

```text
<literal> → <int_literal>
           | <bool_literal>
           | <float_literal>

<int_literal> → NRO

<bool_literal> → TRUE
               | FALSE

<float_literal> → REAL
```

Los símbolos `NRO`, `REAL`, `TRUE` y `FALSE` corresponden a los **tokens producidos por el analizador léxico**.

### *- Identificadores y tipos*

```text
<id> → ID

<type> → INT
       | BOOLEAN
       | FLOAT
```

El símbolo `<id>` representa un **identificador reconocido por el analizador léxico**, mientras que `<type>` agrupa los **tres tipos disponibles para variables y funciones**.

La gramática transformada introduce únicamente los **símbolos auxiliares** necesarios para la implementación del analizador sintáctico.

---

## **2. Expresiones regulares**

### *Palabras reservadas*

```math
int \ |\ boolean \ |\ float \ |\ void \ |\ if \ |\ else \ |\ while \ |\ return \ |\ true \ |\ false
```

**Las palabras reservadas y los identificadores deben estar separados** por un espacio o por un símbolo que no sea ni una palabra reservada ni un identificador. Por ejemplo, `whiletrue` se considera un identificador y no la combinación de las palabras reservadas `while` y `true`.

### *Variables o identificadores*

```math
[a-zA-Z]\ (\ [a-zA-Z] \ |\ [0-9] \ |\ \_\ )^*
```

Se considera que el conjunto de letras **comprende tanto las mayúsculas como las minúsculas**. Los identificadores **comienzan obligatoriamente con una letra y pueden continuar con letras, dígitos o guiones bajos**.

El lenguaje C-TDS es **case-sensitive**, por lo que las palabras reservadas y los identificadores distinguen entre mayúsculas y minúsculas. Por ejemplo, `while` es una palabra reservada, mientras que `WHILE` es un identificador. De la misma forma, `cont` y `Cont` corresponden a identificadores diferentes.

### *Literales o valores constantes*

```math
[0-9]^+ \ |\ true \ |\ false \ |\ [0-9]^+.[0-9]^+
```

Los literales pueden ser **enteros, booleanos o reales**. Se permiten **ceros a la izquierda** en los valores enteros y reales.

### *Operadores*

```math
+ \ |\ - \ |\ * \ |\ / \ |\ \% \ |\ < \ |\ > \ |\ == \ |\ \&\& \ |\ || \ |\ ! \ |\ =
```

### *Delimitadores*

```math
( \ |\ ) \ |\ \{ \ |\ \} \ |\ ; \ |\ , \ |\ .
```

### *Comentarios*

Los **comentarios de una línea** comienzan con `//` y se extienden hasta el final de la línea. Pueden reconocerse mediante la expresión regular `//.*`.

Los **comentarios multilínea** comienzan con `/*` y finalizan con `*/`, pudiendo abarcar varias líneas. Debido a su complejidad, se implementan en Flex mediante un estado especial del autómata, en lugar de utilizar una única expresión regular.

Ambos tipos de comentarios **son ignorados por el analizador léxico**.

### *Espacios*

Los **espacios, tabulaciones, comentarios y saltos de línea** se consideran separadores y son ignorados por el analizador léxico, por medio de las expresiones `[ \t\r]+` y `\n`.

---

## **3. Restricciones semánticas**

Las reglas sintácticas definidas anteriormente determinan si un programa está correctamente formado desde el punto de vista de la estructura del lenguaje. Sin embargo, **un programa puede ser sintácticamente válido y no cumplir determinadas condiciones necesarias para que su significado sea correcto**.

Las **restricciones semánticas** complementan las reglas sintácticas y deben ser verificadas por el **analizador semántico** utilizando la información del árbol sintáctico abstracto (AST) y de la tabla de símbolos (TS).

Si se detecta una violación de alguna de estas reglas, el compilador deberá generar un **mensaje de error semántico** que describa el problema detectado. Si no se detecta ninguna violación, no deberá generarse ningún informe de error semántico.

### *3.1 Declaraciones y ámbitos*

1. Ningún identificador puede ser declarado **más de una vez** dentro de un **mismo ámbito**.
2. Los identificadores deben ser **declarados antes de ser utilizados**.
3. Una función únicamente puede ser invocada desde **código ubicado después de su declaración**.
4. Los distintos ámbitos se encuentran **anidados**. Un identificador declarado en un ámbito interno puede **ocultar** a otro identificador con el mismo nombre perteneciente a un ámbito exterior.
5. Un identificador utilizado como `location` debe corresponder a una **variable global**, una **variable local** o un **parámetro** de la función actual.
6. Una función **no puede utilizarse como `location`** de una asignación.

### *3.2 Función `main`*

Todo programa **debe contener** una función denominada `main`.

La función `main` **no puede recibir parámetros**.

La **ejecución** del programa comienza en la función `main`.

### *3.3 Asignaciones*

Una asignación tiene la **forma**:

```text
<location> = <expr>
```

La `location` debe corresponder a una **variable o parámetro declarado** y visible en el punto donde se realiza la asignación.

El tipo de la `location` y el tipo de la expresión asignada deben ser **compatibles** de acuerdo con las reglas de tipos del lenguaje.

Los **parámetros** pueden utilizarse como `location` de una asignación. Las modificaciones realizadas sobre un parámetro tienen efecto únicamente dentro del **ámbito** de la función correspondiente, debido a que los parámetros son pasados por valor.

### *3.4 Tipos de expresiones*

Cada expresión posee un **tipo de dato** determinado por sus operandos y por el operador utilizado.

Los **literales** tienen los siguientes tipos:

- Los literales **enteros** tienen tipo `int`.
- Los literales **booleanos** tienen tipo `boolean`.
- Los literales **reales** tienen tipo `float`.

Una **referencia** a una variable o parámetro tiene el tipo declarado para dicho identificador.

Una **llamada a una función** tiene como tipo el tipo de retorno de la función invocada. Las funciones `void` no producen un valor y, por lo tanto, una llamada a una función `void` no puede utilizarse como una expresión.

### *3.5 Operadores*

Los operadores **aritméticos** requieren operandos de tipo numérico (`int` o `float`).

Los operadores **relacionales** requieren operandos de tipo numérico (`int` o `float`) y producen un resultado de tipo `boolean`.

El operador de **igualdad** `==` requiere operandos del mismo tipo y produce un resultado de tipo `boolean`.

Los operadores **lógicos** `&&` y `||` requieren operandos de tipo `boolean` y producen un resultado de tipo `boolean`.

El operador de **negación** `!` requiere un operando de tipo `boolean` y produce un resultado de tipo `boolean`.

El operador **menos unario** requiere un operando de tipo numérico (`int` o `float`) y produce un resultado numérico.

### *3.6 Compatibilidad entre tipos numéricos*

Las **operaciones aritméticas** permiten operandos de tipo `int` o `float`.

Cuando una **operación aritmética** **combina un operando `int` y un operando `float`**, el operando `int` se convierte implícitamente a `float` y el resultado de la operación es de tipo `float`.

Cuando **ambos operandos son de tipo `int`**, el resultado es de tipo `int`, excepto en aquellos casos en los que la operación produzca un tipo diferente según las reglas específicas del operador.

La **división entre dos operandos `int`** corresponde a una división entera. Si **al menos uno de los operandos es de tipo `float`**, la división produce un tipo `float`.

El **operador módulo `%`** solamente admite operandos de tipo `int` y produce un resultado de tipo `int`.

En las **operaciones relacionales** `<` y `>`, se permite combinar operandos `int` y `float`. En caso de que **los operandos sean de tipos diferentes**, el operando `int` se convierte implícitamente a `float`.

El operador de **igualdad** `==` requiere que ambos operandos sean del mismo tipo. **No se realiza coerción implícita** entre `int` y `float` para esta operación.

### *3.7 Sentencias condicionales y repetitivas*

La **expresión** asociada a una sentencia `if` debe tener tipo `boolean`.

La **expresión** asociada a una sentencia `while` debe tener tipo `boolean`.

### *3.8 Llamadas a funciones*

En una invocación a una función, la **cantidad de argumentos** debe coincidir con la cantidad de parámetros declarados por la función.

El **tipo de cada argumento** debe ser compatible con el tipo del parámetro formal correspondiente.

Los **argumentos** se evalúan de izquierda a derecha y sus valores son pasados por valor a los parámetros formales.

Una **función `void`** únicamente puede ser **invocada como una sentencia** y no puede utilizarse como parte de una expresión.

Una **función que retorna un valor** puede ser **invocada tanto como una expresión como una sentencia**. Cuando se utiliza como una sentencia, el valor retornado es ignorado.

### *3.9 Sentencias `return`*

Una sentencia `return` debe ser **compatible con el tipo de retorno de la función** en la que aparece.

En una función `void`, la sentencia `return` **no puede contener una expresión**.

En una función que retorna un valor, la sentencia `return` **debe contener una expresión**.

El tipo de la expresión retornada debe ser **compatible con el tipo de retorno** declarado por la función.

### *3.10 Retorno obligatorio de funciones*

Una función que retorna un valor **no puede alcanzar el final de su cuerpo sin ejecutar una sentencia `return`** válida.

Por lo tanto, todos los **caminos posibles** de ejecución de una función que retorna un valor deben finalizar mediante una sentencia `return` que contenga una expresión compatible con el tipo de retorno declarado.

Las funciones `void`, en cambio, **pueden alcanzar el final de su cuerpo sin ejecutar una sentencia `return`**.

### *3.11 Coerciones entre tipos numéricos*

El lenguaje permite **conversiones implícitas** entre los tipos numéricos `int` y `float`.

La **conversión de `int` a `float`** conserva el valor numérico. La **conversión de `float` a `int`** trunca la parte decimal.

Estas conversiones **se permiten en**:

- Asignaciones.
- Argumentos de funciones.
- Valores de retorno.
- Operaciones aritméticas.
- Operaciones relacionales `<` y `>`.

**No se realizan conversiones implícitas** entre `boolean` y los tipos numéricos.

El operador de **igualdad** `==` requiere operandos del mismo tipo y **no aplica coerciones** entre `int` y `float`.

### *3.12 División y módulo por cero*

El divisor de una operación de división `/` **no puede ser cero**.

El segundo operando de una operación módulo `%` **tampoco puede ser cero**.

Cuando el divisor es una expresión cuyo valor puede determinarse en tiempo de compilación y dicho valor es cero, **el compilador deberá informar un error semántico**.

Cuando el divisor depende de valores que solamente pueden conocerse durante la ejecución, **la condición deberá ser detectada durante la etapa de ejecución correspondiente**.

La comprobación de división o módulo por cero **no modifica las reglas de tipos de los operadores**.
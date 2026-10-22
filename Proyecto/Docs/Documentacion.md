# **Documentación del Proyecto**

El Proyecto consiste en el **diseño e implementación de un compilador para un lenguaje de programación simple, denominado C-TDS**, similar a C o Pascal.

El trabajo se aborda de una manera incremental, mediante las siguientes etapas:

1. Análisis **léxico** y **sintáctico**.
2. Generación del **árbol sintáctico abstracto (AST)** y la **tabla de símbolos (TS)**.
3. Análisis **semántico**.
4. Generación de **código intermedio**.
5. Generación de **código objeto**.
6. **Optimizador** y extensiones.

## *1. Análisis léxico y análisis sintáctico*

Previo a comenzar con esta etapa, se dejó descripta la **gramática del lenguaje**, así como también sus **expresiones regulares** asociadas.

*Ver [Especificación del Proyecto](Especificacion.md) para más detalles.*

### *1.1 Análisis léxico*

El análisis léxico fue implementado utilizando **Flex**, a partir de las expresiones regulares definidas previamente en la especificación del lenguaje.

Su función es **reconocer los distintos componentes léxicos de C-TDS**, ignorar los espacios y comentarios y detectar los caracteres que no pertenecen al lenguaje.

Además de reconocer los componentes léxicos, el analizador se encuentra **integrado con Bison** para proporcionar los tokens al analizador sintáctico junto con su ubicación y, cuando corresponde, su valor asociado.

#### *1.1.1 Diseño y decisiones*

El lexer reconoce las **palabras reservadas, identificadores, literales enteros y reales, operadores y delimitadores** definidos en la especificación.

El lenguaje es **sensible a mayúsculas y minúsculas (case-sensitive)**. Los identificadores pueden comenzar con una letra y continuar con letras, dígitos o `_`.

El carácter `-` se reconoce siempre como un operador. La distinción entre **resta binaria** y **menos unario** se deja para el análisis sintáctico.

Los **comentarios de una línea y multilínea son ignorados**. Para los comentarios multilínea se utiliza un **estado exclusivo del autómata de Flex**, lo que permite detectar además el caso en que el archivo finaliza antes de encontrar el cierre `*/`.

Las reglas fueron escritas en un orden tal que Flex dé prioridad a las coincidencias más específicas. Ante varias reglas que pueden reconocer una misma secuencia, **Flex selecciona la coincidencia más larga** y, en caso de empate, **la regla que aparece primero**.

El analizador léxico se encuentra integrado con el analizador sintáctico mediante la **interfaz generada por Bison**. Para ello, el archivo generado `AnalizadorSintactico.tab.h` define los tokens utilizados por el lexer.

Los **operadores de más de un carácter** se representan mediante tokens específicos:

- `==` → `IGUAL`.
- `&&` → `AND`.
- `||` → `OR`.

Los **operadores y delimitadores de un único carácter** se devuelven directamente mediante su carácter correspondiente.

Los identificadores y literales proporcionan además **valores semánticos** al parser mediante `yylval`. Los identificadores almacenan su lexema, mientras que los literales enteros y reales almacenan su valor numérico.

La **ubicación de cada token** se proporciona mediante `yylloc`. La macro `REGISTRAR_UBICACION()` establece la línea y columna inicial y final del token y actualiza la columna actual del analizador.

La implementación del lexer se mantiene **separada de la interfaz de línea de comandos**. El archivo `Src/main.c` actúa como punto de entrada del programa y es responsable de coordinar la ejecución del análisis léxico y las demás etapas del compilador.

*La implementación puede consultarse en [Analizador Léxico](../Src/Lexer/AnalizadorLexico.lex).*

#### *1.1.2 Salida del análisis léxico*

Como resultado de la etapa de análisis léxico, se **genera un archivo con extensión `.lex`** únicamente si la etapa se solicita de forma explícita mediante la opción `-target scan`. En caso contrario, el análisis léxico se ejecuta de manera silenciosa como soporte fundamental para las etapas posteriores del compilador, sin generar archivos de resultados.

El archivo contiene la **secuencia de tokens reconocidos** por el analizador, indicando el tipo de token, su lexema y su ubicación dentro del archivo fuente.

Por ejemplo:

```text
PALABRA RESERVADA int 1:1
IDENTIFICADOR x 1:5
OPERADOR = 2:3
LITERAL ENTERO 10 2:5
DELIMITADOR ; 2:7
```

**La generación de este archivo es coordinada por `main.c`**, mientras que el reconocimiento de los tokens y el registro de su representación son responsabilidad del analizador léxico.

El registro utilizado para generar el archivo `.lex` es **independiente** de los valores semánticos que el lexer proporciona a Bison. De esta manera, la salida de la etapa léxica puede mantenerse aunque el analizador sea utilizado como parte de una etapa posterior.

En caso de producirse **errores léxicos**, el análisis continúa para permitir detectar múltiples errores en una misma ejecución. En ese caso, **el archivo `.lex` puede contener los tokens reconocidos correctamente**, mientras que los errores se informan por la salida estándar de errores y el compilador finaliza indicando que el análisis léxico no fue exitoso.

Este formato fue definido como una **decisión de diseño** debido a que la especificación establece la generación de un archivo `.lex`, pero no determina un formato específico para su contenido.

#### *1.1.3 Ubicación de errores*

El analizador mantiene el **número de línea y columna** de la entrada para poder informar tanto la ubicación de los errores léxicos como la posición de los tokens entregados al analizador sintáctico.

El número de línea es gestionado mediante `yylineno`, mientras que la columna se mantiene mediante la variable `yycolumn`.

Para cada **token reconocido**, la macro `REGISTRAR_UBICACION()` establece en `yyloc`:

- **Línea** inicial y final.
- **Columna** inicial y final.

La **columna inicial** corresponde a la posición en la que comienza el lexema y la **columna final** a la posición de su último carácter.

Cuando se encuentra un **carácter no reconocido**, se informa el error indicando el carácter, la línea y la columna correspondiente.

**El análisis no se detiene al primer error léxico**. Se registra la existencia del error mediante `error_lexico` y se continúa procesando la entrada, permitiendo **detectar múltiples errores** en una misma ejecución.

**Los errores léxicos no se consideran tokens y, por lo tanto, no se incorporan al archivo `.lex`**. Se informan mediante la salida estándar de errores (`stderr`).

#### *1.1.4 Modo de depuración*

Se incorporó un mecanismo de **modo debug** mediante la variable `modo_debug`, que es configurada por `main.c` a partir de la opción `-debug`.

Cuando se encuentra **desactivado**, el analizador no produce salida para los tokens reconocidos. Cuando se **activa**, muestra información sobre cada token junto con su línea y columna.

**El modo debug se mantiene separado de la generación del archivo `.lex`**. De esta manera, la salida destinada al usuario durante la ejecución y la salida correspondiente al resultado de una etapa de compilación no se mezclan.

Esta decisión permite utilizar el mismo lexer tanto en la **ejecución normal** como durante las **tareas de desarrollo y pruebas**.

#### *1.1.5 Integración con el análisis sintáctico*

El analizador léxico proporciona los tokens necesarios para el análisis sintáctico mediante la interfaz generada por **Bison**.

Para cada elemento léxico reconocido, el lexer realiza las siguientes operaciones cuando corresponde:

1. **Registra el token** para la salida de la etapa léxica.
2. **Registra su ubicación** en `yylloc`.
3. **Carga su valor semántico** en `yylval`, cuando el token posee información asociada.
4. **Devuelve el token** mediante `return` para que Bison pueda procesarlo.

Los **tokens que no poseen un valor semántico específico**, como las palabras reservadas y los operadores, solamente requieren su código de token.

Esta integración permite que **Flex sea responsable del reconocimiento léxico** y que **Bison sea responsable del análisis sintáctico**, manteniendo una separación clara entre ambas etapas.

#### *1.1.6 Cambios respecto del Pre-Proyecto*

El analizador léxico fue **ampliado para adaptarse a la especificación de C-TDS**. Entre los principales cambios se incorporaron:

- Nuevos **tipos** y **palabras reservadas**, como `boolean`, `float`, `extern` `if`, `else` y `while`.
- **Identificadores** que permiten `_`.
- **Literales** reales.
- **Operadores** relacionales y lógicos.
- **Comentarios** de una y varias líneas.
- **Mecanismo de depuración** controlado mediante `modo_debug`.
- **Registro de los tokens reconocidos** para generar la salida de la etapa léxica.
- Vinculación con la **interfaz de línea de comandos**.

La **integración con Bison** permite que el mismo analizador léxico sea utilizado durante la etapa de análisis léxico y, a su vez, como componente del análisis sintáctico.

#### *1.1.7 Pruebas*

Se incorporaron **16 pruebas** para el análisis léxico, divididas en:

- **12 pruebas válidas**, destinadas a verificar el reconocimiento correcto de los elementos léxicos.
- **4 pruebas inválidas**, destinadas a verificar la detección de errores léxicos.

Las **pruebas válidas** cubren:

- Palabras reservadas.
- Identificadores.
- Literales enteros.
- Literales reales.
- Operadores.
- Delimitadores.
- Comentarios.
- Espacios y líneas.
- Casos ambiguos y coincidencias de mayor longitud.
- Programas completos.

Las **pruebas inválidas** cubren:

- Caracteres no reconocidos.
- Operadores no reconocidos.
- Comentarios multilínea sin cerrar.
- Múltiples errores léxicos en una misma entrada.

Las pruebas se encuentran **organizadas** en:

```text
Src/Test/Lexico/
├── Validas/
└── Invalidas/
```

Los **resultados** generados durante la ejecución se almacenan en:

```text
Src/Test/Resultados/Lexico/
├── Validas/
└── Invalidas/
```

Los resultados de las pruebas **no forman parte del repositorio** y se encuentran excluidos mediante [.gitignore](../../.gitignore).

Para cada prueba se genera el **archivo `.lex`** correspondiente. En las pruebas que producen errores léxicos se conserva además un **archivo `.err`** con los mensajes enviados por `stderr`.

Los archivos `.err` **solamente se conservan cuando contienen información**. De esta manera, las pruebas válidas que no producen errores no generan archivos de error vacíos.

### *1.2 Análisis sintáctico*

El análisis sintáctico fue implementado utilizando **Bison**, a partir de la gramática del lenguaje C-TDS definida previamente en la especificación.

Su función es **verificar que la secuencia de tokens producida por el analizador léxico respete la estructura sintáctica del lenguaje** y, como resultado de las acciones semánticas asociadas a las producciones, **construir el árbol sintáctico abstracto (AST)** correspondiente al programa reconocido.

El analizador sintáctico se encuentra **integrado con el analizador léxico**, recibiendo los tokens generados por **Flex** junto con sus valores semánticos y su ubicación dentro del archivo fuente.

#### *1.2.1 Diseño y decisiones*

La implementación del analizador sintáctico utiliza **Bison** para generar un parser basado en la gramática definida para C-TDS.

La gramática utilizada en Bison corresponde a una **transformación de la gramática especificada** por la cátedra, en la que se introducen símbolos no terminales auxiliares para representar explícitamente listas de elementos y construcciones opcionales.

Por ejemplo, las **listas de declaraciones, parámetros, argumentos y sentencias** se representan mediante producciones auxiliares como `<lista_declaraciones>`, `<lista_parametros>`, `<lista_argumentos>` y `<lista_statement>`.

Esta transformación **no modifica las construcciones sintácticas permitidas por el lenguaje**, sino que adapta su representación para facilitar la implementación del analizador mediante Bison.

Se tomó esta **decisión de diseño** por los siguientes motivos:

- Bison **no utiliza directamente la notación de cardinalidad de la especificación**. Por ejemplo, `<var_decl>*` debe representarse mediante producciones auxiliares que permitan reconocer cero o más ocurrencias. 
- Se presentaban dificultades para **diferenciar la declaración de una variable y la declaración de una función**. Por ejemplo, para distinguir `int contador;` de `int contador () {}`.
- La utilización de una producción general del estilo `<expr> <bin_op> <expr>` producía **conflictos shift/reduce**. Por tal motivo, se decidió eliminar los no terminales `<bin_op>`, `<arith_op>`, `<cond_op>` y `<rel_op>`, incorporando una producción específica para cada operador y utilizando las declaraciones de precedencia y asociatividad de Bison.

*Para revisar a detalle la gramática transformada que se utilizó y la manera de afrontar las precedencias y asociatividades, consultar [Gramática transformada](Especificacion.md#12-gramática-transformada-para-bison).*

Además de las listas representadas mediante producciones auxiliares de la gramática, las acciones semánticas del parser requieren **listas temporales de nodos del AST** para construcciones cuya cantidad de elementos puede variar, como declaraciones, parámetros y argumentos.

Para resolver esta necesidad, se **implementó una estructura auxiliar privada denominada `ListaNodos`**. Esta estructura almacena temporalmente punteros a nodos y permite construir las listas de manera incremental antes de incorporarlas al AST definitivo.

Se tomó la decisión de mantener `ListaNodos` **fuera del módulo AST** porque se trata de una estructura propia del proceso de reconocimiento sintáctico y no de una construcción del lenguaje. De esta manera, **el AST mantiene únicamente nodos que representan construcciones reales del programa**, mientras que las estructuras utilizadas para facilitar las acciones de Bison permanecen encapsuladas dentro del parser.

Las listas utilizan **crecimiento dinámico**, aumentando su capacidad cuando se alcanza el límite actual. Además, las operaciones de transferencia de elementos permiten incorporar los nodos de una lista temporal a otra sin duplicar los nodos ni modificar su propiedad.

Esta separación también permite simplificar el **manejo de memoria**: `ListaNodos` almacena punteros, pero no es propietaria de los nodos que contiene. Al liberar una lista temporal, se libera únicamente la estructura auxiliar, mientras que los nodos son liberados posteriormente junto con el AST al que fueron incorporados.

Las acciones semánticas de Bison utilizan estas listas para **construir progresivamente el árbol**. Una vez que una construcción sintáctica está completa, los nodos temporales son transferidos al nodo correspondiente del AST.

El analizador sintáctico utiliza además `%locations` para conservar la **ubicación de los elementos sintácticos** y `%define parse.error detailed` para obtener **mensajes de error sintáctico** más descriptivos.

En esta etapa, **`main` se reconoce léxicamente como un identificador (`ID`)**. La comprobación de que exista una función `main` y que cumpla las restricciones semánticas correspondientes se realizará durante el análisis semántico.

*La implementación puede consultarse en [Analizador Sintáctico](../Src/Parser/AnalizadorSintactico.y).*

#### *1.2.2 Salida del análisis sintáctico*

Como resultado de la etapa de análisis sintáctico, **se genera un archivo con extensión `.sint`** únicamente si la etapa se solicita de forma explícita mediante la opción `-target parse`. En caso contrario, el análisis sintáctico se ejecuta de manera silenciosa como soporte fundamental para las etapas posteriores del compilador (construyendo el AST temporal en memoria), sin generar archivos de resultados. Naturalmente, esta etapa no se ejecuta en absoluto si el usuario solicitó un objetivo estrictamente anterior, como `-target scan`.

El archivo `.sint` contiene **información sobre las construcciones sintácticas reconocidas**, indicando su **ubicación** dentro del archivo fuente.

Por ejemplo, para un programa que contiene una declaración de variable, una asignación y un `return`, pueden registrarse mensajes como:

```text
2:5 - Declaración de variable reconocida.
3:5 - Asignación reconocida.
4:5 - Return con expresión reconocido.
5:1 - Bloque reconocido.
1:1 - Declaración de función reconocida.
1:1 - Programa reconocido.
```

Este formato fue definido como una **decisión de diseño** debido a que la especificación establece la generación de un archivo `.sint`, pero no determina un formato específico para su contenido.

La **generación de estos mensajes** es realizada mediante la función `registrar_sintaxis ()`.

La función **escribe** la información en el archivo `.sint` y, cuando el **modo de depuración** se encuentra activo, también muestra el mensaje por consola.

El **orden en que se registran las construcciones** en el archivo `.sint` no necesariamente coincide con el orden en que aparecen en el código fuente. Esto se debe a que las acciones semánticas asociadas a las producciones se ejecutan cuando Bison realiza las correspondientes reducciones durante el análisis. Por este motivo, algunas construcciones pueden registrarse en un orden diferente al orden en que aparecen en el código fuente.

La salida `.sint` es **independiente de los valores semánticos utilizados internamente por Bison**. Su objetivo es documentar el resultado de esta etapa de compilación sin interferir con el funcionamiento del parser.

Además, si el **modo de depuración** se encuentra activo, la ejecución exitosa muestra por pantalla una **representación textual del AST** asociado al programa, y genera un **archivo .dot con la estructura del AST**. Así, puede obtenerse una **representación visual usando Graphviz** con el comando:

```bash
dot -Tpng -o salida.png archivo.dot
```

En caso de producirse un **error sintáctico**, el mensaje de error se informa mediante la salida estándar de errores (`stderr`) y la ejecución de esta etapa finaliza indicando que el análisis no fue exitoso.

#### *1.2.3 Ubicación de errores*

El analizador sintáctico utiliza las ubicaciones proporcionadas por **Bison** mediante `%locations`.

La información de ubicación se obtiene a través de `yylloc`, que contiene la **posición del elemento sintáctico** actualmente analizado.

Además de utilizarse para informar errores, **esta información se conserva al crear los nodos del AST**, almacenando en cada nodo la línea y columna correspondientes a la construcción reconocida.

Cuando se produce un **error sintáctico**, `yyerror ()` informa:

- **Línea** en la que se detectó el error.
- **Columna** en la que se detectó el error.
- **Descripción** proporcionada por Bison.

El formato utilizado es:

```text
ERROR SINTÁCTICO: línea X, columna Y - mensaje.
```

Por ejemplo:

```text
ERROR SINTÁCTICO: línea 3, columna 5 - syntax error, unexpected ID, expecting ';' or ','.
```

A diferencia del análisis léxico, el analizador sintáctico **no implementa recuperación de errores mediante producciones especiales de Bison**.

Por este motivo, **ante un error sintáctico, el parser informa el primer error detectado y finaliza el análisis de esa entrada**.

Esta **decisión** permite mantener una **implementación sencilla** del parser durante la etapa. La recuperación de errores no resulta necesaria para validar si un caso de prueba es sintácticamente correcto o incorrecto.

#### *1.2.4 Modo de depuración*

Se incorporó un mecanismo de **modo debug** mediante la variable `modo_debug` configurada por `main.c` a partir de la opción `-debug`.

Cuando el modo de depuración se encuentra **desactivado**, el analizador sintáctico no muestra por consola las reducciones realizadas durante el análisis.

Cuando se encuentra **activado**, se muestran mensajes asociados a determinadas reducciones de expresiones.

Por ejemplo:

```text
[PARSE] Reducción: expresión * expresión.
[PARSE] Reducción: expresión + expresión.
```

Estos mensajes permiten observar el orden en que Bison reduce las expresiones y comprobar el comportamiento de las **reglas de precedencia y asociatividad**.

Además, en el **modo depuración** se genera una **representación textual** de la estructura del programa reconocido, correspondiente al AST. 

Por ejemplo:

```text
PROGRAMA
   DECLARACION FUNCIÓN [BOOLEAN]
      IDENTIFICADOR: main
      BLOQUE
         DECLARACION VARIABLE [BOOLEAN]
            IDENTIFICADOR: resultado
         =
            IDENTIFICADOR: resultado
            ||
               &&
                  TRUE
                  FALSE
               !
                  TRUE
```

Por su parte, el **modo depuración** deja preparado un **archivo .dot** con la estructura del AST, que posteriormente puede ser convertido a **.png** con **Graphviz**.

Por ejemplo:

```text
digraph AST {
    nodo0 [label="PROGRAMA"];
    nodo1 [label="DECLARACION FUNCIÓN [BOOLEAN]"];
    nodo2 [label="IDENTIFICADOR: main"];
    nodo1 -> nodo2;
    nodo3 [label="BLOQUE"];
    nodo4 [label="DECLARACION VARIABLE [BOOLEAN]"];
    nodo5 [label="IDENTIFICADOR: resultado"];
    nodo4 -> nodo5;
    nodo3 -> nodo4;
    nodo6 [label="="];
    nodo7 [label="IDENTIFICADOR: resultado"];
    nodo6 -> nodo7;
    nodo8 [label="||"];
    nodo9 [label="&&"];
    nodo10 [label="TRUE"];
    nodo9 -> nodo10;
    nodo11 [label="FALSE"];
    nodo9 -> nodo11;
    nodo8 -> nodo9;
    nodo12 [label="!"];
    nodo13 [label="TRUE"];
    nodo12 -> nodo13;
    nodo8 -> nodo12;
    nodo6 -> nodo8;
    nodo3 -> nodo6;
    nodo1 -> nodo3;
    nodo0 -> nodo1;
}
```

El modo de depuración se mantiene **separado de la generación del archivo `.sint`**. De esta manera, la salida de depuración destinada al desarrollador no se mezcla con la salida correspondiente al resultado de la etapa.

#### *1.2.5 Integración con el análisis léxico*

El analizador sintáctico recibe los tokens generados por el analizador léxico mediante la función `yylex ()`.

La interfaz entre **Flex** y **Bison** se establece mediante el archivo generado `AnalizadorSintactico.tab.h`.

Este archivo contiene las **definiciones de los tokens** que utiliza el parser y es incluido por el analizador léxico.

Para cada elemento reconocido, **el lexer puede proporcionar**:

1. El **token** que representa el elemento léxico.
2. Su **ubicación**, mediante `yylloc`.
3. Su **valor semántico**, mediante `yylval`, cuando corresponde.

Los identificadores utilizan el tipo semántico `identificador`, mientras que los literales enteros y reales utilizan los tipos `numero` y `real`, respectivamente.

Los **operadores de más de un carácter** se representan mediante tokens específicos:

- `==` → `IGUAL`.
- `&&` → `AND`.
- `||` → `OR`.

Los **operadores y delimitadores de un único carácter** se reciben mediante el carácter correspondiente.

Las **constantes lógicas** `TRUE` y `FALSE` no llevan un valor semántico propio en el parser actual.

La integración permite mantener una **separación clara de responsabilidades**:

- **Flex** reconoce los componentes léxicos.
- **Bison** verifica la estructura sintáctica y construye el AST.
- **main.c** coordina la ejecución de ambas etapas.

Los **valores semánticos** proporcionados por el lexer son utilizados por las acciones del parser para construir los nodos correspondientes. Por ejemplo, los identificadores y literales reconocidos por Flex proporcionan la información necesaria para crear los nodos hoja del AST.

#### *1.2.6 Cambios respecto del Pre-Proyecto*

El analizador sintáctico fue **ampliado considerablemente** respecto de la versión desarrollada durante el Pre-Proyecto.

En el **Pre-Proyecto**, la gramática se encontraba centrada principalmente en:

- **Declaraciones** de variables.
- **Asignaciones**.
- **Sentencias** `return`.
- **Expresiones** con suma y multiplicación.
- **Constantes** enteras.
- **Constantes** booleanas.
- **Identificadores**.

En la versión actual se incorporaron las **construcciones necesarias** para soportar la especificación completa de C-TDS.

Entre los **principales cambios** se encuentran:

- Incorporación de **funciones** con tipos `int`, `boolean`, `float` y `void`.
- Incorporación de **parámetros** en las funciones.
- Incorporación de **declaraciones de funciones externas (`extern`)**, garantizando su sintaxis sin bloque y validando su orden estricto dentro del nivel global del programa.
- Incorporación de **múltiples variables** en una misma declaración, separadas por comas.
- Incorporación de **declaraciones locales** dentro de bloques.
- Incorporación de **llamadas a funciones**.
- Incorporación de **argumentos** en las llamadas a funciones.
- Incorporación de los **condicionales** `if` e `if-else`.
- Incorporación de la sentencia `while`.
- Incorporación de **sentencias vacías**.
- Incorporación de **bloques como sentencias**.
- Incorporación de **operadores de resta, división y módulo**.
- Incorporación de **operadores relacionales** `<`, `>` y `==`.
- Incorporación de **operadores condicionales** `&&`, `||`.
- Incorporación del **operador lógico** `!`.
- Incorporación del **menos unario**.
- Incorporación de **literales reales**.
- Incorporación de las **reglas de precedencia y asociatividad** correspondientes a los operadores.
- Incorporación de **mensajes de depuración** para observar las reducciones de expresiones.

Además de ampliar las construcciones reconocidas, la implementación actual incorpora la **construcción del AST durante el análisis sintáctico**. Las acciones semánticas asociadas a las producciones crean los nodos correspondientes y los relacionan mediante sus hijos.

Para manejar construcciones con una cantidad variable de elementos se incorporó la **estructura auxiliar `ListaNodos`**. Esta estructura permite construir temporalmente listas de declaraciones, parámetros y argumentos y transferir posteriormente sus nodos al AST.

A diferencia del Pre-Proyecto, la implementación actual separa explícitamente las **estructuras temporales utilizadas por el parser**, como las listas de declaraciones, parámetros y argumentos, de la representación definitiva del programa. **Esta decisión evita utilizar nodos del AST como simples contenedores auxiliares y permite mantener una representación coherente del árbol**.

#### *1.2.7 Pruebas*

Se incorporaron **133 pruebas** para el análisis sintáctico, divididas en:

- **60 pruebas válidas**, destinadas a verificar que programas sintácticamente correctos sean aceptados.
- **73 pruebas inválidas**, destinadas a verificar que construcciones que no pertenecen a la gramática sean rechazadas.

Además de verificar la aceptación o rechazo de cada entrada, las pruebas válidas permiten comprobar la **estructura del AST generado** mediante los archivos correspondientes y la inspección de los árboles obtenidos.

Las **pruebas válidas** cubren:

- Programas vacíos.
- Declaraciones de variables globales.
- Declaraciones con múltiples identificadores.
- Declaraciones de funciones.
- Declaraciones de funciones externas (`extern`).
- Orden estricto de declaraciones globales (variables seguidas de externas y luego funciones).
- Funciones con y sin parámetros.
- Funciones con parámetros de distintos tipos.
- Funciones `void`.
- Declaraciones locales.
- Asignaciones.
- Expresiones aritméticas.
- Expresiones relacionales.
- Expresiones lógicas.
- Operadores unarios.
- Expresiones parentizadas.
- Llamadas a funciones con y sin argumentos.
- Llamadas con expresiones como argumentos.
- Llamadas a funciones utilizadas como sentencias.
- Sentencias vacías.
- Sentencias `if`.
- Sentencias `if-else`.
- Sentencias `while`.
- Bloques anidados.
- Precedencia de operadores.
- Asociatividad de operadores.
- Combinaciones de operadores aritméticos, relacionales y lógicos.
- Operaciones mixtas entre enteros y reales desde el punto de vista sintáctico.

Las **pruebas inválidas** cubren:

- Declaraciones incompletas.
- Declaraciones sin identificadores.
- Declaraciones sin punto y coma.
- Declaraciones de funciones externas con bloque de código.
- Alteración del orden global (por ejemplo, declarar variables globales después de una función externa o convencional).
- Separadores incorrectos.
- Parámetros incompletos o incorrectamente separados.
- Funciones sin paréntesis o sin bloque.
- Asignaciones incompletas.
- Llamadas a funciones incorrectas.
- Operadores sin operandos.
- Expresiones parentizadas incompletas.
- Condiciones incompletas en `if` y `while`.
- Sentencias `return` incorrectas.
- Uso incorrecto de `else`.
- Declaraciones después de sentencias.
- Sentencias sin separadores.
- Encadenamiento de operadores relacionales no asociativos.
- Múltiples errores sintácticos en una misma entrada.

Las pruebas se encuentran **organizadas** en:

```text
Src/Test/Sintactico/
├── Validas/
└── Invalidas/
```

Los **resultados** generados durante la ejecución se almacenan en:

```text
Src/Test/Resultados/Sintactico/
├── Validas/
└── Invalidas/
```

Los resultados de las pruebas **no forman parte del repositorio** y se encuentran excluidos mediante [.gitignore](../../.gitignore).

Para cada prueba se genera el **archivo `.sint`** correspondiente. En las pruebas inválidas se conserva además un **archivo `.err`** con los mensajes de error enviados por `stderr`.

Los archivos `.err` **solamente se conservan cuando contienen información**. De esta manera, las pruebas válidas que no producen errores no generan archivos de error vacíos.

Por otro lado, las pruebas válidas también generan **archivos .dot y .png** con el AST generado, para poder verificar visualmente que el parser está construyendo los árboles esperados para las etapas posteriores.

## *2. Generación del árbol sintáctico abstracto (AST) y generación de la tabla de símbolos (TS)*

Luego del análisis léxico y sintáctico, el proyecto incorpora la representación estructurada del programa mediante un **árbol sintáctico abstracto (AST)**.

El AST es construido durante el **análisis sintáctico**, a partir de las acciones semánticas asociadas a las producciones de **Bison**. Cada construcción reconocida por el parser genera los nodos correspondientes y establece las relaciones entre ellos.

En paralelo, el proyecto incorpora una **tabla de símbolos (TS)** que permite registrar y organizar la información asociada a los identificadores y demás entidades declaradas en el programa, como variables, funciones y parámetros.

La TS organiza estos elementos de acuerdo con los distintos **niveles de ámbito** del programa y permite realizar búsquedas sobre ellos durante etapas posteriores del compilador.

El AST mantiene **referencias** que permiten vincular, durante el análisis semántico, los **nodos que correspondan con los elementos de la tabla de símbolos**.

### *2.1 Árbol sintáctico abstracto (AST)*

El AST fue implementado como una **estructura jerárquica de nodos** que representa las construcciones relevantes del programa, eliminando información sintáctica que no resulta necesaria para las etapas posteriores del compilador.

La **implementación** se encuentra separada en un módulo propio:

- [`AST.h`](../Src/AST/AST.h).
- [`AST.c`](../Src/AST/AST.c).
- [`Tipos.h`](../Src/Common/Tipos.h).

#### *2.1.1 Diseño de los nodos*

Cada nodo del AST se representa mediante la estructura `NodoAST`, que contiene:

- El **tipo de nodo**, representado mediante el enumerado `TipoNodo`.
- El **tipo de dato asociado**, representado mediante `TipoDato`.
- Un **valor**, cuando la construcción necesita almacenar información adicional.
- Un arreglo dinámico de **hijos**.
- La **cantidad** actual de hijos.
- La **capacidad** disponible del arreglo de hijos.
- La **línea y columna** asociadas a la construcción dentro del código fuente.
- Una **referencia al símbolo** correspondiente de la tabla de símbolos (TS), cuando exista.

El **tipo de nodo** permite distinguir las distintas construcciones del lenguaje. La implementación actual contempla nodos para:

- El **programa** y las **declaraciones**.
- Las **funciones (tanto regulares como externas)** y sus **parámetros**.
- Los **bloques** y las **sentencias**.
- Las **asignaciones** y **llamadas** a funciones.
- Las sentencias **`if`, `while`, `return`** y **sentencias vacías**.
- Los operadores **aritméticos**.
- Los operadores **relacionales**.
- Los operadores **lógicos**.
- Los operadores **unarios**.
- Los **identificadores** y **literales**.

#### *2.1.2 Representación de valores*

Para almacenar **información** asociada a determinados nodos se utiliza la unión `ValorNodo`.

Actualmente se contemplan:

- `numero`, para los literales **enteros**.
- `real`, para los literales **reales**.
- `identificador`, para los **identificadores** y nombres de **llamadas** a funciones.

Los nodos que no necesitan almacenar un valor adicional no utilizan estos campos.

Esta separación permite que la estructura del nodo sea común par todas las construcciones del lenguaje, **manteniendo únicamente la información adicional necesaria en cada caso**.

Las cadenas correspondientes a identificadores se **almacenan dinámicamente**. Esto permite que cada nodo mantenga su propio lexema y que la memoria pueda ser liberada correctamente al destruir el árbol. 

#### *2.1.3 Tipos de datos*

Los **tipos de datos** del lenguaje se representan mediante el enumerado `TipoDato`, definido en `Tipos.h`.

Actualmente se contemplan:

- `TIPO_INT`.
- `TIPO_BOOLEAN`.
- `TIPO_FLOAT`.
- `TIPO_VOID`.
- `TIPO_NO_DEFINIDO`.

Cuando se crea un nodo, su tipo de dato se establece **inicialmente como `TIPO_NO_DEFINIDO`**.

**El tipo puede establecerse durante la construcción sintáctica cuando la información ya se encuentra disponible**, por ejemplo en declaraciones de variables, funciones y parámetros.

En las construcciones cuyo tipo depende de **información que todavía no fue determinada**, se mantiene `TIPO_NO_DEFINIDO` para que pueda ser resuelto durante las etapas posteriores.

Esta decisión permite **separar la construcción sintáctica del análisis semántico**, evitando realizar durante la creación del AST comprobaciones que corresponden a etapas posteriores.

#### *2.1.4 Representación n-aria*

El AST utiliza una **representación n-aria**, es decir, un nodo puede tener una **cantidad variable de hijos**.

Esta decisión resulta adecuada para **construcciones que pueden contener una cantidad variable de elementos**, como:

- **Programas** con múltiples declaraciones.
- **Bloques** con múltiples declaraciones y sentencias.
- **Declaraciones** de variables con múltiples identificadores.
- **Funciones** con múltiples parámetros.
- **Llamadas** con múltiples argumentos.

Por otro lado, las **operaciones binarias** tienen exactamente dos hijos y las **operaciones unarias** tienen un único hijo.

Esta representación **evita introducir nodos intermedios que no representan construcciones reales del lenguaje** únicamente para poder almacenar listas de elementos.

#### *2.1.5 Arreglo dinámico de hijos*

Los hijos de cada nodo se almacenan mediante un **arreglo dinámico de punteros** a `NodoAST`.

Al crear un nodo, el arreglo **comienza sin capacidad reservada**. Cuando se agrega el primer hijo, se reserva espacio y, cuando la capacidad se alcanza, esta **se incrementa geométricamente**.

La capacidad comienza en `2` elementos y posteriormente se duplica:

```text
2 → 4 → 8 → 16 → ...
```

Esta estrategia **evita realizar una operación de `realloc` por cada nuevo hijo** y permite que las construcciones con una cantidad variable de elementos puedan **crecer de manera eficiente**.

La **operación** responsable de esta tarea es `agregar_hijo ()`.

#### *2.1.6 Construcción del AST durante el análisis sintáctico*

Los nodos del AST se construyen directamente desde las **acciones semánticas** del parser.

Cada producción relevante de la gramática **crea el nodo correspondiente y agrega como hijos los nodos que representan sus componentes**.

Por ejemplo:

- Una **asignación** genera un nodo `AST_ASIGNACION` con el identificador destino y la expresión como hijos.
- Una **operación binaria** genera un nodo correspondiente al operador con sus dos operandos como hijos.
- Una **operación unaria** genera un nodo con un único hijo.
- Un **`return`** puede tener cero o un hijo dependiendo de si posee expresión.
- Un **`if`** posee la condición y el bloque correspondiente, y agrega un tercer hijo cuando existe `else`.
- Una **llamada a función** almacena el nombre de la función y sus argumentos.
- Una **función convencional** contiene su identificador, sus parámetros y su bloque.
- Una **función externa** genera un nodo `AST_DECLARACION_FUNCION_EXTERNA` que contiene su identificador y sus parámetros como hijos, pero a diferencia de las funciones convencionales, no posee un hijo correspondiente al bloque.

#### *2.1.7 Ubicación de los nodos*

Cada nodo del AST **almacena la línea y columna** asociada a la construcción sintáctica que representa.

Esta información se obtiene mediante las **ubicaciones proporcionadas por Bison** y se conserva al momento de crear los nodos mediante `crear_nodo ()`.

La ubicación permite **mantener información del código fuente dentro del árbol** y constituye una base para las etapas posteriores, especialmente para la generación de mensajes de error durante el análisis semántico.

#### *2.1.8 Manejo de memoria*

La implementación del AST incluye **mecanismos específicos para administrar la memoria** utilizada por los nodos y sus hijos.

La función `crear_nodo ()` **reserva memoria** para cada nodo.

La función `agregar_hijo ()` **administra dinámicamente el arreglo** de hijos.

La función `liberar_arbol ()` **recorre recursivamente el árbol** y libera:

1. Los **subárboles** correspondientes a cada hijo.
2. Las **cadenas dinámicas** almacenadas en los nodos que corresponda.
3. El **arreglo** de hijos.
4. El propio **nodo**.

Esta estrategia **permite liberar el árbol completo desde su raíz** sin requerir que cada etapa conozca individualmente todos los nodos que fueron creados.

Las referencias a símbolos de la TS **no se liberan junto con el AST**, ya que los símbolos son administrados por la tabla de símbolos.

La implementación también verifica los **errores de reserva de memoria** y finaliza la ejecución cuando no es posible reservar o ampliar correctamente las estructuras necesarias.

#### *2.1.9 Representación textual del AST*

Se implementó la función `imprimir_arbol ()` para obtener una **representación jerárquica del AST** mediante la salida estándar.

La función recorre recursivamente el árbol e **imprime cada nodo con una indentación correspondiente a su nivel**.

Cuando corresponde, también muestra:

- El **valor** del nodo.
- El **tipo** de dato asociado.

Esta representación se utiliza principalmente durante el **modo de depuración**, permitiendo inspeccionar rápidamente la estructura generada por el parser sin necesidad de utilizar una herramienta gráfica.

#### *2.1.10 Generación de archivos DOT*

Además de la representación textual, el módulo AST permite generar una **representación del árbol en formato DOT**, mediante la función `generar_dot ()`.

A cada nodo se le asigna un **identificador numérico** y se genera una relación dirigida entre cada nodo padre y sus hijos.

El archivo resultante puede procesarse con **Graphviz** para obtener una representación gráfica del árbol:

```bash
dot -Tpng -o salida.png archivo.dot
```

La generación de DOT se utiliza actualmente durante el **modo depuración** del análisis sintáctico.

Esta funcionalidad permite **comprobar visualmente** que la estructura construida por las acciones semánticas de Bison coincide con la estructura esperada para el programa analizado.

#### *2.1.11 Relación con la tabla de símbolos (TS)*

El AST mantiene una **referencia a la entrada correspondiente a la tabla de símbolos (TS)** mediante el campo `Simbolo *simbolo` de cada nodo.

Esta referencia permite que, en las etapas posteriores del compilador, **los nodos que representan declaraciones, funciones, parmámetros, identificadores y llamadas a funciones puedan asociarse a la información almacenada en la tabla de símbolos**.

Al crear un nodo del AST, **esta referencia se inicializa como `NULL`**. La asociación efectiva entre los nodos del AST y las entradas de la TS se realiza durante el **análisis semántico**.

El AST **no es propietario de los símbolos**. La memoria de las estructuras `Simbolo` pertenece a la tabla de símbolos, por lo que `liberar_arbol ()` no libera las referencias almacenadas en este campo.

De esta manera, se mantiene una **separación clara de responsabilidades**: el AST conserva la representación estructurada del programa y referencias a información semántica, mientras que la TS administra las entradas y su memoria.

#### *2.1.12 Cambios respecto del Pre-Proyecto*

La implementación actual del AST **amplía considerablemente** la representación utilizada durante el Pre-Proyecto.

Entre los principales cambios se encuentran:

- Incorporación de nodos para **funciones** y sus **parámetros**.
- Incorporación de nodos específicos para la representación de firmas de **funciones externas**.
- Incorporación de **bloques**.
- Incorporación de **llamadas** a funciones.
- Incorporación de **`if`, `else` y `while`**.
- Incorporación de **sentencias vacías**.
- Incorporación de todos los **operadores** definidos por C-TDS.
- Incorporación de operadores **unarios**.
- Incorporación de literales **reales**.
- Incorporación de información de **columna**.
- Incorporación de una **capacidad dinámica** para el arreglo de hijos.
- Incorporación de **reales** en el almacenamiento `union`.
- Incorporación de la representación **`TIPO_FLOAT`**.
- **Separación** entre el AST definitivo y las listas temporales utilizadas por el parser.

En particular, el uso de un **arreglo de hijos con capacidad dinámica** reemplaza la estrategia utilizada en el Pre-Proyecto, donde el arreglo se redimensionaba en cada incorporación de un nuevo hijo.

La implementación actual proporciona una **representación más general para las construcciones del lenguaje C-TDS**, manteniendo al mismo tiempo separadas las responsabilidades del análisis sintáctico, la representación sintáctica y las futuras etapas semánticas.

#### *2.1.13 Pruebas*

Se incorporaron **8 pruebas independientes** para verificar el funcionamiento del módulo AST sin depender del analizador sintáctico.

Estas pruebas permiten **verificar el comportamiento de operaciones fundamentales del módulo**, incluyendo:

- Creación de **nodos**.
- Asociación de **tipos** de datos.
- Incorporación de **hijos**.
- Construcción de **árboles** con diferentes cantidades de hijos.
- **Impresión jerárquica**.
- Generación de **archivos DOT**.
- **Liberación** recursiva de la memoria utilizada por el árbol.

Las pruebas se encuentran **organizadas** en:

```text
Src/Test/AST/
```

Los **resultados** generados durante su ejecución se almacenan en:

```text
Src/Test/Resultados/AST/
```

Los resultados de las pruebas **no forman parte del repositorio** y se encuentran excluidos mediante [.gitignore](../../.gitignore).

Además de estas pruebas independientes, el AST se **verifica directamente mediante las pruebas válidas del análisis sintáctico**, ya que el parser construye un árbol durante su ejecución.

En dichas pruebas, el **modo de depuración** permite generar los archivos `.dot` y posteriormente convertirlos a `.png`, facilitando la inspección visual de la estructura obtenida.

### *2.2 Tabla de símbolos (TS)*

La tabla de símbolos (TS) fue implementada como una estructura que permite **registrar, organizar y consultar la información asociada a las entidades declaradas en el programa**.

La **implementación** se encuentra separada en un módulo propio:

- [`TS.h`](../Src/TS/TS.h).
- [`TS.c`](../Src/TS/TS.c).
- [`Tipos.h`](../Src/Common/Tipos.h).

La TS permite almacenar información de **variables, funciones y parámetros**, asociando a cada símbolo un nombre, un tipo de dato y una clase. Además, mantiene información relacionada con su estado de inicialización, banderas que indican su alcance (global o externo) y un offset en memoria, utilizado en etapas posteriores del compilador.

La tabla organiza los símbolos mediante **niveles de ámbito**, permitiendo representar distintos contextos de declaración y realizar búsquedas desde el nivel más interno hacia los niveles exteriores.

#### *2.2.1 Diseño de los símbolos*

Cada símbolo de la TS se representa mediante la **estructura `Simbolo`**, que contiene:

- El **nombre** del identificador.
- El **tipo de dato** asociado, representado mediante `TipoDato`.
- La **clase del símbolo**, representada mediante `ClaseSimbolo`.
- El estado de **inicialización** del símbolo.
- El **offset** asociado al símbolo en la pila o memoria, cuando corresponda.
- **Banderas de estado** `es_externa` y `es_global` para identificar la procedencia y el alcance del símbolo.
- Un puntero al **siguiente símbolo** del mismo nivel.

Las **clases de símbolos** contempladas actualmente son:

- `SIMBOLO_VARIABLE`, para **variables** declaradas en el programa.
- `SIMBOLO_FUNCION`, para **funciones**.
- `SIMBOLO_PARAMETRO`, para **parámetros** de funciones.

La incorporación de `SIMBOLO_PARAMETRO` permite **distinguir los parámetros de las variables y funciones**, manteniendo esta información disponible para las etapas posteriores del compilador.

#### *2.2.2 Representación de los niveles de ámbito*

Los símbolos se organizan mediante **estructuras `Nivel`**.

Cada nivel contiene:

- Una **lista enlazada de símbolos** pertenecientes al nivel.
- Una **referencia al nivel anterior de la pila de ámbitos activos**.
- Una **referencia al siguiente nivel de la lista de niveles cerrados**, cuando corresponda.

La tabla de símbolos completa se representa mediante `TablaSimbolos`, que mantiene un puntero al **nivel actualmente abierto** y otro al comienzo de la **lista de niveles cerrados**.

Los niveles activos se organizan como una **pila**. Al abrir un nuevo nivel, este pasa a ser el nivel actual y conserva una referencia al nivel que se encontraba abierto anteriormente.

Esta organización permite representar **ámbitos anidados** y facilita la búsqueda de identificadores respetando la visibilidad determinada por el nivel en el que fueron declarados.

Cuando un nivel se cierra, deja de formar parte de la pila activa y se incorpora a la **lista de niveles cerrados**. Esta organización permite conservar los símbolos de los ámbitos finalizados sin que continúen participando en las búsquedas.

#### *2.2.3 Inicialización de la tabla de símbolos*

La función `iniciar_TS ()` crea una **nueva tabla de símbolos** y genera inicialmente un **nivel vacío**.

De esta forma, la tabla siempre comienza con un **nivel abierto**, sobre el cual pueden realizarse las primeras inserciones.

La función **reserva dinámicamente la memoria** necesaria para la estructura `TablaSimbolos` y para su nivel actual.

#### *2.2.4 Apertura y cierre de niveles*

La función `abrir_nivel ()` permite **crear un nuevo nivel** de la tabla de símbolos.

El nuevo nivel se establece como **nivel actual** y mantiene una referencia al **nivel anterior**.

La función `cerrar_nivel ()` **retira el nivel actual de la pila de ámbitos activos** y restaura como nivel actual al nivel anterior.

El nivel cerrado se incorpora a una lista independiente y **sus símbolos dejan de participar en las búsquedas**. Sin embargo, sus estructuras **permanecen en memoria** hasta que se libera la tabla completa.

Esta **decisión** permite **mantener válidas las referencias** a símbolos almacenadas previamente en los nodos del AST, incluso después de cerrar el ámbito al que pertenecen.

#### *2.2.5 Inserción de símbolos*

La función `insertar_elemento ()` permite incorporar un **nuevo símbolo al nivel actual**.

Los símbolos se almacenan mediante una **lista enlazada**, insertándose al comienzo de la lista correspondiente al nivel actual.

La función **retorna** un puntero al símbolo insertado cuando la operación es exitosa, o `NULL` cuando no puede realizarse la inserción.

**No se permiten dos símbolos con el mismo nombre dentro de un mismo nivel**. Esta restricción permite detectar declaraciones duplicadas dentro de un mismo ámbito.

#### *2.2.6 Búsqueda de símbolos*

La función `buscar_elemento ()` permite **localizar un símbolo** a partir de su nombre.

La búsqueda comienza en el **nivel actual** y continúa hacia los niveles exteriores hasta encontrar una coincidencia.

Esta estrategia permite implementar el comportamiento habitual de los **ámbitos anidados**.

Cuando existe un símbolo con el mismo nombre en un nivel interno y en uno externo, la búsqueda encuentra primero el símbolo del nivel interno. De esta forma, **los símbolos de niveles internos pueden ocultar a símbolos con el mismo nombre pertenecientes a niveles exteriores**.

Si el símbolo no se encuentra en niguno de los niveles abiertos, la función retorna `NULL`.

#### *2.2.7 Estado de inicialización*

Cada símbolo mantiene un campo `inicializada` que permite **registrar si ya posee un valor asociado**.

Actualmente, los símbolos correspondientes a **variables** y **funciones** comienzan con estado `inicializada = 0` indicando que todavía no fueron inicializados.

Los **parámetros** comienzan con estado `inicializada = 1` ya que reciben su valor mediante los argumentos de la función al momento de su invocación.

Esta información resulta de utilidad durante el **análisis semántico** para detectar uso de variables que todavía no fueron inicializadas.

#### *2.2.8 Offset asociado al símbolo y banderas de alcance*

Cada símbolo contiene un campo `offset` destinado a almacenar la **ubicación relativa o desplazamiento** asociado al símbolo en la memoria (respecto al puntero base en la pila).

Al momento de insertar un nuevo símbolo, este campo se **inicializa** con `offset = -1`. El valor indica que todavía no existe un desplazamiento calculado.

Adicionalmente, cada símbolo incluye los campos `es_global` y `es_externa`. El valor de `es_global` **se calcula de forma dinámica al momento de la inserción**, determinando si el identificador pertenece al ámbito global (evaluando si el nivel actual carece de un nivel anterior). Por su parte, `es_externa` **se inicializa con `0`** y se configura específicamente al procesar las declaraciones de funciones externas en el analizador semántico.

Estas banderas resultan fundamentales para las etapas de generación de código, ya que permiten **distinguir rápidamente si un identificador debe ser declarado en la sección de datos globales, manejado en la pila local, o referenciado mediante directivas de enlazado al sistema operativo**. El cálculo efectivo de los offsets y la correcta evaluación de estas banderas corresponden entonces a etapas posteriores de código.

#### *2.2.9 Manejo de memoria*

La implementación de la TS **administra dinámicamente la memoria** utilizada por la tabla, sus niveles y sus símbolos.

La función `iniciar_TS ()` **reserva la memoria** correspondiente a la tabla y crea su nivel inicial.

La función `insertar_elemento ()` **reserva memoria** para cada nuevo símbolo y realiza una copia independiente del nombre.

La función `cerrar_nivel ()` **retira el nivel actual de la pila de ámbitos activos** y lo incorpora a la **lista de niveles cerrados**. Los símbolos y la estructura del nivel se conservan en memoria para mantener válidas las referencias que puedan existir desde el AST.

La función `liberar_TS ()` **libera todos los niveles que permanecen abiertos y todos los niveles cerrados que se hayan conservado**. Para cada nivel, libera los nombres de sus símbolos, las estructuras `Simbolo` y la estructura `Nivel`. Finalmente, libera la estructura principal de la tabla.

De esta manera, la TS mantiene la propiedad y la responsabilidad de **liberar la memoria de sus símbolos**, mientras que el AST conserva únicamente referencias a ellos.

#### *2.2.10 Cambios respecto del Pre-Proyecto*

La implementación actual de la tabla de símbolos **amplía la versión desarrollada durante el Pre-Proyecto** para adaptarla a las nuevas construcciones del lenguaje C-TDS.

Entre los principales **cambios** se encuentran:

- Incorporación de la **clase** `SIMBOLO_PARAMETRO`.
- Incorporación del **estado de inicialización** mediante el campo `inicializada`.
- Incorporación del **campo** `offset` para representar el desplazamiento del símbolo en memoria.
- Incorporación de las **banderas** `es_externa` y `es_global` para el correcto soporte de funciones de bibliotecas externas y variables globales durante la generación de código.
- Incorporación de **validaciones** sobre los tipos de datos y las clases de símbolos durante la inserción.
- Conservación de **niveles cerrados** en memoria, para no perder las referencias a sus símbolos.

La **organización general mediante niveles, listas enlazadas y búsqueda desde el nivel más interno hacia los niveles exteriores** se mantiene respecto de la implementación del Pre-Proyecto.

#### *2.2.11 Pruebas*

Se incorporaron **8 pruebas independientes** para verificar el funcionamiento del módulo de la tabla de símbolos.

Estas pruebas permiten **verificar el comportamiento de las operaciones fundamentales del módulo**, incluyendo:

- **Inicialización** de la tabla.
- **Inserción** de uno y varios símbolos.
- Detección de **símbolos duplicados** dentro de un mismo nivel.
- Apertura y cierre de **niveles**.
- Búsqueda de **símbolos inexistentes**.
- **Ocultamiento** de símbolos de niveles exteriores.
- **Conservación** de símbolos pertenecientes a **niveles cerrados**.
- **Liberación de la memoria** correspondiente a niveles activos y cerrados.

Las pruebas se encuentran **organizadas** en:

```text
Src/Test/TS/
```

Los **resultados** generados durante su ejecución se almacenan en:

```text
Src/Test/Resultados/TS/
```

Los resultados de las pruebas **no forman parte del repositorio** y se encuentran excluidos mediante [.gitignore](../../.gitignore).

## *3. Análisis semántico*

El análisis semántico fue implementado en lenguaje **C**, utilizando el **arbol sintáctico abstracto (AST)** construido durante el análisis sintáctico y la **tabla de símbolos (TS)** para verificar la información asociada a los identificadores y las declaraciones del programa.

Su función es **analizar semánticamente el AST generado por el parser**, asociando los identificadores con sus símbolos, determinando los tipos de las expresiones y verificando las restricciones semánticas definidas para el lenguaje C-TDS.

*Las restricciones semánticas que debe cumplir un programa se encuentran definidas en la sección [Restricciones semánticas](Especificacion.md#3-restricciones-semánticas) de la especificación del lenguaje.*

En esta sección se documentan las **decisiones de implementación y los mecanismos utilizados para realizar dichas verificaciones**, sin repetir las reglas ya especificadas.

El analizador semántico se encuentra implementado en:

- [AnalizadorSemantico.h](../Src/Semantico/AnalizadorSemantico.h).
- [AnalizadorSemantico.c](../Src/Semantico/AnalizadorSemantico.c).

La implementación se **integra** con el AST, la TS y el programa principal para realizar el análisis después de que la entrada haya sido reconocida sintácticamente.

Cuando el análisis semántico **finaliza correctamente**, la tabla de símbolos se **conserva** para utilizarla durante la generación de código intermedio. En el **modo de pruebas semánticas**, en cambio, se **libera** la tabla al finalizar el análisis, sin ejecutar la generación de cuádruplos.

### *3.1 Diseño y decisiones*

La implementación del análisis semántico utiliza una **estructura de contexto** denominada `ContextoSemantico`.

Esta estructura concentra la **información** necesaria durante el recorrido del AST:

- La **Tabla de Símbolos** utilizada durante el análisis.
- La cantidad de **errores semánticos** encontrados.
- El archivo de salida `.sem`.
- El estado del **modo de depuración**.
- El nodo correspondiente al programa.
- El tipo de retorno de la función que se está analizando.

Esta **decisión de diseño** permite evitar que las distintas funciones auxiliares deban recibir individualmente toda la información necesaria para realizar el análisis.

El análisis se encuentra dividido en **funciones auxiliares** especializadas según el tipo de construcción del AST. De esta manera, el recorrido general del árbol se mantiene separado de las verificaciones específicas de cada construcción.

Por ejemplo, existen **funciones específicas** para analizar:

- Declaraciones de **variables**.
- Declaraciones de **funciones convencionales**.
- Declaraciones de **funciones externas**.
- **Parámetros**.
- **Bloques**.
- **Asignaciones**.
- **Condicionales**.
- **Sentencias** `while`.
- **Sentencias** `return`.
- **Identificadores**.
- Operaciones **aritméticas**.
- Operaciones **lógicas**.
- **Comparaciones**.
- **Igualdades**.
- **Llamadas** a funciones.

Esta separación permite mantener cada comprobación localizada y facilita la **extensión** del analizador.

#### *3.1.1 Integración con la tabla de símbolos*

El análisis semántico utiliza la tabla de símbolos como **estructura central para resolver identificadores y administrar los ámbitos**.

Al analizar una declaración, **se crea el símbolo** correspondiente y se lo incorpora al **nivel actual** de la tabla. Al encontrar posteriormente un identificador, se realiza una **búsqueda** desde el nivel actual hacia los niveles exteriores.

Cuando se encuentra el símbolo correspondiente, el nodo `AST_IDENTIFICADOR` queda **asociado** directamente mediante su campo `simbolo`.

Esta asociación permite que las etapas posteriores puedan **acceder a la información de la declaración** sin tener que volver a resolver el nombre del identificador.

Los ámbitos de funciones y bloques se administran mediante las operaciones de **apertura y cierre de niveles** de la tabla de símbolos.

Los **bloques anidados** pueden crear nuevos niveles, permitiendo mantener separados los identificadores declarados en distintos ámbitos.

#### *3.1.2 Análisis de funciones*

Las **funciones** se registran en la tabla de símbolos antes de analizar su cuerpo.

Esta decisión permite que el **análisis del cuerpo** pueda resolver correctamente el símbolo correspondiente a la propia función.

Al analizar una función se guarda temporalmente el **tipo de retorno** anterior y se establece el tipo correspondiente a la nueva función en `tipo_retorno_actual`.

Luego se abre el **ámbito de la función**, se analizan sus **parámetros** y se procesa el **bloque** de la función.

Una vez finalizado el análisis, se **cierra el ámbito** y se restaura el tipo de retorno anterior.

Además, se implementó una **búsqueda** sobre el AST mediante `buscar_declaracion_funcion ()`. Esta función permite recuperar la declaración de una función a partir de su nombre, principalmente para acceder a la **información de sus parámetros** al analizar una llamada.

Adicionalmente, el analizador semántico procesa las declaraciones de **funciones externas (`extern`)**. A diferencia de las funciones regulares, estas se registran en la tabla de símbolos con la **bandera `es_externa` activada**, abriendo un ámbito temporal para validar semánticamente sus parámetros pero sin requerir un bloque de código ni la verificación obligatoria de una sentencia `return`. Asimismo, se valida que no existan colisiones de nombres entre funciones regulares y externas dentro del ámbito global.

#### *3.1.3 Determinación de tipos*

Cada expresión analizada obtiene un **tipo** que se almacena en el campo `tipo_dato` del nodo AST.

El **análisis de expresiones** se encuentra centralizado en `analizar_expresion ()`, que deriva el procesamiento hacia funciones específicas según el tipo de nodo.

El uso de `TIPO_NO_DEFINIDO` permite **continuar el análisis cuando una expresión contiene un error semántico**, evitando propagar innecesariamente el mismo error hacia las expresiones que dependen de ella.

Las **verificaciones** relacionadas con los tipos se implementan mediante funciones auxiliares como `es_tipo_numerico ()`, `tipos_compatibles ()` y `tipo_resultado_aritmetico ()`.

Las **reglas** concretas de compatibilidad y determinación de tipos se encuentran documentadas en la especificación del lenguaje.

#### *3.1.4 Inicialización de variables*

La información sobre la **inicialización** se almacena en cada `Simbolo` mediante el campo `inicializada`.

Las **variables** se incorporan inicialmente como no inicializadas, mientras que los **parámetros** se consideran inicializados al ingresar al ámbito de una función.

Cuando una **asignación** válida modifica una variable o parámetro, el símbolo correspondiente se marca como inicializado.

Al utilizar un **identificador** dentro de una expresión, `analizar_identificador ()` consulta este estado antes de permitir su utilización.

De esta manera, la **tabla de símbolos** no solamente permite resolver el nombre y tipo de un identificador, sino que también **conserva información necesaria para las verificaciones semánticas posteriores**.

#### *3.1.5 Llamadas a funciones*

Las **llamadas** a funciones se analizan mediante `analizar_llamada ()`.

Primero se busca el **símbolo** correspondiente a la función en la tabla de símbolos. Posteriormente se recupera su **declaración** desde el AST y se comparan los **argumentos** proporcionados con los **parámetros** declarados.

El análisis de los argumentos se encuentra separado en `analizar_argumentos ()`, lo que permite mantener independiente la **resolución de la función** y la **verificación de sus argumentos**.

El **tipo de retorno de la función** se asigna al nodo `AST_LLAMADA`, permitiendo utilizar posteriormente la llamada como parte de una expresión cuando corresponda.

Las **llamadas a funciones** `void` utilizadas como expresiones son detectadas desde `analizar_expresion ()`.

Las reglas de validación de argumentos y tipos aplican de forma idéntica tanto para **funciones regulares** como **funciones externas**, asegurando que la cantidad y los tipos de las expresiones pasadas como argumentos coincidan con los parámetros declarados en la firma externa.

#### *3.1.6 Análisis de retornos*

El **tipo de retorno de la función actual** se mantiene en `tipo_retorno_actual`.

Al encontrar un `AST_RETURN`, el analizador utiliza esta información para **verificar el retorno** correspondiente.

Además de comprobar cada sentencia `return`, se incorporó un **análisis estructural** mediante las funciones `garantiza_retorno_bloque ()` y `garantiza_retorno_sentencia ()`.

Estas funciones recorren la estructura de los bloques, condicionales y demás sentencias para **determinar si una función no `void` posee un retorno garantizado en todos los caminos de ejecución que pueden finalizar la función**.

#### *3.1.7 Evaluación de expresiones constantes*

Se incorporó una **función auxiliar** denominada `evaluar_constante_numerica ()` para determinar el **valor** de determinadas expresiones numéricas cuando sus operandos pueden conocerse durante el análisis semántico.

La evaluación contempla constantes **enteras y reales, menos unario y operaciones aritméticas** entre constantes.

Esta funcionalidad se utiliza principalmente para **detectar divisiones y operaciones módulo cuyo divisor puede determinarse estáticamente como cero**.

Cuando el valor de una expresión depende de una variable o de información que no puede determinarse durante el análisis semántico, **la evaluación constante no produce un resultado**.

#### *3.1.8 Errores y advertencias*

Los **errores semánticos** se centralizan mediante `error_semantico ()`.

Esta función informa la ubicación del **error**, escribe el mensaje correspondiente y aumenta el **contador de errores** del contexto semántico.

También se implementó `warning_semantico ()` para **situaciones que no impiden continuar el análisis**.

**Los errores y advertencias se mantienen separados de los mensajes de depuración**, evitando mezclar información destinada al usuario con información utilizada para observar el funcionamiento interno del analizador.

### *3.2 Salida del análisis semántico*

Como resultado de la etapa de análisis semántico, **se genera un archivo con extensión `.sem`**, únicamente si la etapa se solicita de forma explícita mediante la opción `-test-sem`. En caso contrario, el análisis semántico se ejecuta de manera silenciosa como soporte fundamental para las etapas posteriores del compilador (asociando símbolos y validando restricciones), sin generar archivos de resultados. Naturalmente, esta etapa no se ejecuta en absoluto si el usuario solicitó un objetivo estrictamente anterior, como `-target scan` o `-target parse`.

El archivo contiene información relacionada con las **verificaciones realizadas durante el análisis**, incluyendo las declaraciones procesadas, los identificadores asociados y otras operaciones semánticas relevantes.

La **generación de estos mensajes** se encuentra centralizada mediante la función `registrar_semantica ()`.

La función escribe la información en el archivo `.sem` y, cuando corresponde, el **modo de depuración** permite mostrar información adicional por consola.

La salida `.sem` se mantiene separada de la información utilizada internamente por el analizador. Su objetivo es **registrar el resultado de la etapa semántica** sin modificar el funcionamiento del AST ni de la tabla de símbolos.

Si el análisis encuentra **uno o más errores semánticos**, la etapa finaliza indicando que la entrada no superó el análisis.

Cuando el análisis es exitoso y se encuentra habilitado el modo de depuración, también puede generarse el **AST enriquecido** con la información obtenida durante esta etapa.

En este caso, los nodos pueden mostrar información que no estaba disponible durante la construcción sintáctica, ya que fueron **propagados** durante el análisis semántico.

La salida del análisis semántico depende del **modo de ejecución**. Durante la **ejecución habitual** del compilador, la información semántica no se escribe directamente en el archivo de salida principal, que se reserva para las etapas posteriores de generación de código. En cambio, al utilizar la **opción `-test-sem`**, el archivo de salida se proporciona al analizador semántico para registrar los resultados de las verificaciones realizadas.

De esta manera, las **pruebas semánticas** pueden generar archivos `.sem` sin incorporar esos mensajes a los archivos producidos por las etapas posteriores durante la compilación habitual.

### *3.3 Ubicación de errores*

La información de **ubicación** utilizada por el análisis semántico proviene de las posiciones almacenadas previamente en los nodos del AST durante el análisis sintáctico.

Cada nodo contiene su **línea y columna de origen**, por lo que el analizador semántico puede utilizar directamente esta información al detectar un error.

Los **errores** se informan utilizando el siguiente formato:

```text
ERROR SEMÁNTICO: línea X, columna Y - mensaje.
```

Por ejemplo:

```text
ERROR SEMÁNTICO: línea 4, columna 5 - La variable 'x' no fue declarada.
```

La utilización de las posiciones almacenadas en el AST permite que las funciones semánticas puedan **informar el lugar correspondiente** sin depender nuevamente del analizador léxico.

Cuando una verificación utiliza un nodo específico, se toma la **ubicación** de ese nodo para señalar de forma más precisa el origen del problema.

### *3.4 Modo de depuración*

El análisis semántico utiliza para **depuración** la variable `modo_debug`, configurada por `main.c` mediante la opción `-debug`.

Cuando el **modo de depuración** se encuentra activo, se muestran mensajes asociados al recorrido y procesamiento del AST.

Estos **mensajes** permiten observar, entre otras cuestiones:

- Entrada y salida de bloques.
- Apertura y cierre de ámbitos.
- Declaraciones procesadas.
- Tipos determinados para expresiones.
- Análisis de asignaciones.
- Análisis de llamadas.
- Análisis de retornos.

Por ejemplo:

```text
[SEM] Entrando al programa.
[SEM] Declaración de variable analizada.
[SEM] Abriendo ámbito de bloque.
[SEM] Condición de if analizada.
[SEM] Cerrando ámbito de bloque.
```

El **modo de depuración** también permite generar una **representación del AST** después del análisis semántico.

A diferencia de la representación generada inmediatamente después del parser, esta versión del árbol puede incluir la **información obtenida durante el análisis semántico**.

Cuando se activa el **modo de depuración** durante el procesamiento que incluye el análisis semántico, `main.c` genera un archivo `.dot` después de ejecutar esta etapa. Por lo tanto, la representación corresponde al **AST enriquecido** con la información obtenida durante el análisis semántico. En el **modo de pruebas semánticas** también puede generarse esta representación, aunque no se ejecuta la generación de código intermedio.

### *3.5 Integración con el análisis sintáctico*

El análisis semántico se ejecuta sobre el **AST generado por el analizador sintáctico**.

La separación entre ambas etapas permite mantener una **división clara de responsabilidades**:

* **Flex** reconoce los componentes léxicos.
* **Bison** verifica la estructura sintáctica y construye el AST.
* **El analizador semántico** interpreta la información del AST y verifica las restricciones semánticas.
* **La tabla de símbolos** conserva la información de declaraciones y ámbitos.

El **parser** no realiza búsquedas en la tabla de símbolos ni realiza comprobaciones semánticas. Su responsabilidad termina con la **construcción del AST**.

Posteriormente, `main.c` **crea o inicializa la tabla de símbolos y ejecuta `analizar_semantica ()`** sobre el árbol obtenido.

Esta separación permite que el AST funcione como **punto de comunicación** entre las etapas sintáctica y semántica.

La **información de línea y columna** construida durante el análisis sintáctico también es reutilizada directamente por el análisis semántico para informar errores.

### *3.6 Cambios respecto del Pre-Proyecto*

El análisis semántico actual representa una **ampliación considerable** respecto de la implementación realizada durante el Pre-Proyecto.

La versión inicial estaba orientada principalmente a las construcciones básicas del lenguaje, mientras que la implementación actual incorpora el análisis de las construcciones necesarias para el **lenguaje C-TDS completo**.

Entre los principales **cambios** se encuentran:

- Incorporación de **ámbitos anidados**.
- Análisis de **declaraciones globales y locales**.
- Análisis de **funciones y parámetros**.
- Análisis de **funciones externas (`extern`)**, su validación de parámetros y la ausencia de un bloque.
- Control del **tipo de retorno de las funciones**.
- Análisis de **llamadas y argumentos**.
- Control del estado de **inicialización de variables**.
- Análisis de **retornos obligatorios**.
- Detección de determinadas operaciones con **divisor constante igual a cero**.
- Incorporación de ubicación de columna en los **errores semánticos**.
- Incorporación de **advertencias semánticas**.
- Generación de información específica en archivos `.sem`.
- Incorporación de un **modo de depuración** para observar el recorrido semántico.

### *3.7 Pruebas*

Se incorporó un conjunto de **146 pruebas** para el análisis semántico, dividido en:

* **65 pruebas válidas**, destinadas a comprobar que programas que cumplen las restricciones semánticas sean aceptados.
* **81 pruebas inválidas**, destinadas a comprobar que programas que violan alguna restricción semántica sean rechazados.

Las pruebas se encuentran **organizadas** en:

```text
Src/Test/Semantico/
├── Validas/
└── Invalidas/
```

Los **resultados** generados durante la ejecución se almacenan en:

```text
Src/Test/Resultados/Semantico/
├── Validas/
└── Invalidas/
```

Los resultados generados durante las pruebas **no forman parte del repositorio** y se encuentran excluidos mediante [.gitignore](../../.gitignore).

Las **pruebas semánticas** se ejecutan mediante la **opción interna `test-sem`**, que permite realizar el análisis semántico sin continuar con la generación de código intermedio. El archivo de salida indicado para cada prueba se utiliza para registrar los resultados del análisis.

La ejecución de las pruebas **no requiere seleccionar un nuevo `target`**, ya que el modo de pruebas semánticas se controla mediante esta opción específica.

Las **pruebas válidas** permiten verificar, entre otras cuestiones:

- Declaraciones y usos de **identificadores**.
- Distintos **niveles de ámbito**.
- **Declaraciones locales**.
- **Funciones y parámetros**.
- **Asignaciones**.
- **Expresiones** de distintos tipos.
- **Operaciones** aritméticas y lógicas.
- **Comparaciones**.
- **Llamadas** a funciones.
- **Retornos**.
- **Inicialización** de variables.
- **Combinaciones** de las construcciones anteriores.
- Declaraciones y llamadas a **funciones externas (`extern`)** válidas.

Las **pruebas inválidas** permiten verificar situaciones como:

- Uso de identificadores **no declarados**.
- Declaraciones **duplicadas**.
- **Incompatibilidades** de tipos.
- Operaciones con operandos **incorrectos**.
- Llamadas con argumentos **incorrectos**.
- Uso **incorrecto** de funciones `void`.
- Retornos **incompatibles**.
- Funciones **sin retorno garantizado**.
- Uso de variables **sin inicializar**.
- Operaciones con **divisor constante igual a cero**.
- **Combinaciones** que producen múltiples errores semánticos.
- Declaración de funciones externas con **conflictos de nombres**.
- **Redeclaración** de funciones externas.

Para cada prueba se genera el **archivo `.sem`** correspondiente.

En las pruebas inválidas se conserva además un **archivo `.err`** cuando se producen mensajes por `stderr`. Este archivo también contiene las **advertencias semánticas**, por lo que pueden generarse incluso en pruebas válidas.

Los archivos `.err` solamente se conservan cuando contienen información. De esta manera, **las pruebas que no producen errores no generan archivos de error vacíos**.

Cuando se ejecutan las pruebas en **modo `-debug`**, también pueden generarse los archivos `.dot` y `.png` correspondientes al AST enriquecido con la información semántica.

## *4. Código intermedio*

La generación de código intermedio fue implementada en lenguaje **C**, utilizando el **árbol sintáctico abstracto (AST)** construido durante el análisis sintáctico y enriquecido durante el análisis semántico.

Esta etapa se encarga de **traducir las construcciones del lenguaje C-TDS a una representación intermedia basada en cuádruplos**, que permite expresar las operaciones, asignaciones, llamadas a funciones y estructuras de control del programa mediante instrucciones de tres direcciones.

A diferencia de la implementación del Pre-Proyecto, que generaba pseudo-assembly directamente en un archivo, la versión actual introduce una **representación intermedia estructurada**, independiente del formato textual utilizado para mostrar o almacenar sus instrucciones.

En esta sección se documentan las **decisiones de implementación y los mecanismos utilizados para construir y generar el código intermedio**.

La implementación se encuentra dividida en dos componentes:

- [Cuadruplos.h](../Src/CodigoIntermedio/Cuadruplos.h) y [Cuadruplos.c](../Src/CodigoIntermedio/Cuadruplos.c).
- [GeneradorCodigoIntermedio.h](../Src/CodigoIntermedio/GeneradorCodigoIntermedio.h) y [GeneradorCodigoIntermedio.c](../Src/CodigoIntermedio/GeneradorCodigoIntermedio.c).

El primer componente define la **representación de las instrucciones y las operaciones para administrar la lista de cuádruplos**. El segundo se encarga de **recorrer el AST y traducir sus construcciones a dicha representación**.

La generación se ejecuta después de que el programa haya superado el **análisis semántico**. De esta manera, el generador puede utilizar la información asociada a los nodos del AST y a los símbolos para producir las instrucciones correspondientes.

### *4.1 Cuádruplos*

La representación intermedia utiliza **cuádruplos** como estructura básica para expresar las instrucciones generadas a partir del AST.

Cada cuádruplo contiene un **código de operación** y **tres operandos**, denominados `arg1`, `arg2` y `resultado`.

Esta representación permite expresar operaciones binarias, asignaciones y otras instrucciones mediante una **estructura uniforme**, incluso cuando algunas de ellas no requieren los tres operandos.

El uso de una representación estructurada permite **separar la construcción del código intermedio de su serialización textual**. De esta manera, el generador no necesita escribir directamente cada instrucción en el archivo de salida, sino que construye una lista de cuádruplos que puede utilizarse posteriormente.

#### *4.1.1 Diseño y estructuras de datos*

La implementación define las **estructuras** necesarias para representar los operandos, las instrucciones y la lista que los contiene.

La **estructura `Operando`** almacena la información correspondiente a cada operando:

- El **tipo de operando**, que permite distinguir entre operandos vacíos, constantes, temporales, símbolos y etiquetas.
- El **nombre** utilizado para identificarlo en la representación textual.
- El **tipo de dato** asociado al operando.
- Un **puntero al símbolo** correspondiente, cuando el operando representa un identificador asociado a la tabla de símbolos.

La utilización de `TipoOperando` permite **distinguir la naturaleza de cada operando sin depender exclusivamente de su nombre**. Por ejemplo, un temporal y un símbolo pueden tener nombres textuales similares, pero corresponden a categorías diferentes dentro de la representación intermedia.

Por su parte, `CodigoOperacion` define las **operaciones que pueden representarse mediante un cuádruplo**. Cada instrucción almacena su código de operación y los tres operandos correspondientes.

La estructura `Cuadruplo` concentra esta información en un único elemento, manteniendo un **formato uniforme** para las distintas clases de instrucciones.

Finalmente, la estructura `NodoCuadruplo` permite **enlazar las instrucciones** mediante referencias al nodo anterior y al siguiente. La estructura `ListaCuadruplos` mantiene las referencias al primer y al último nodo, además de la cantidad total de instrucciones.

Esta organización permite **recorrer la lista en orden, agregar instrucciones al final y conocer la cantidad de cuádruplos generados** sin tener que recorrer toda la estructura cada vez que se incorpora una nueva instrucción.

#### *4.1.2 Tipos de operandos*

Los operandos se clasifican mediante el **enumerado `TipoOperando`**, que contempla las siguientes categorías:

- **`OPERANDO_VACIO`**: representa la ausencia de un operando.
- **`OPERANDO_CONSTANTE`**: representa un valor constante.
- **`OPERANDO_TEMPORAL`**: representa un valor intermedio almacenado en un temporal.
- **`OPERANDO_SIMBOLO`**: representa una variable u otro símbolo asociado a la tabla de símbolos.
- **`OPERANDO_ETIQUETA`**: representa una etiqueta utilizada para identificar un punto del código intermedio.

Los **operandos vacíos** se utilizan cuando una instrucción no necesita alguno de sus campos. En la representación textual, esta ausencia se muestra mediante el carácter `-`.

Los **operandos constantes** conservan el nombre textual del valor y su tipo de dato. Esto permite representar constantes enteras, reales y booleanas.

Los **operandos temporales** se utilizan para almacenar resultados intermedios de operaciones y expresiones. Cada temporal posee un nombre que permite referenciar el valor producido por una instrucción anterior.

Los **operandos asociados a símbolos** conservan tanto el nombre utilizado en el código como una referencia al símbolo correspondiente. Esta asociación permite mantener el vínculo con la información obtenida durante el análisis semántico.

Por último, los **operandos etiqueta** permiten representar destinos de saltos y puntos de entrada a determinadas partes del código.

Esta clasificación permite que los operandos contengan **información adicional** a su representación textual, facilitando su utilización durante las etapas posteriores.

#### *4.1.3 Operaciones disponibles*

El **enumerado `CodigoOperacion`** define las operaciones que pueden representarse mediante cuádruplos.

Las operaciones se encuentran **organizadas** según la clase de instrucción que representan.

Las **operaciones aritméticas** incluyen:

- `CI_SUMAR`.
- `CI_RESTAR`.
- `CI_MULTIPLICAR`.
- `CI_DIVIDIR`.
- `CI_MODULO`.
- `CI_MENOS_UNARIO`.

Las **operaciones relacionales y de igualdad** incluyen:

- `CI_MENOR`.
- `CI_MAYOR`.
- `CI_IGUAL`.

Las **operaciones lógicas** incluyen:

- `CI_AND`.
- `CI_OR`.
- `CI_NEGACION`.

Además, se define `CI_ASIGNAR` para representar las asignaciones.

Las **operaciones de control de flujo** incluyen:

- `CI_ETIQUETA`.
- `CI_SALTO`.
- `CI_SALTO_SI_FALSO`.

Por último, las operaciones relacionadas con funciones incluyen:

- `CI_INICIO_FUNCION`.
- `CI_PARAMETRO`.
- `CI_ARGUMENTO`.
- `CI_LLAMADA`.
- `CI_RETURN`.
- `CI_FIN_FUNCION`.

Esta organización permite representar tanto las expresiones como las estructuras de control y las llamadas a funciones mediante una **interfaz común de instrucciones**.

Los **nombres textuales** de las operaciones se encuentran centralizados en `nombre_operacion ()`. De esta manera, la correspondencia entre cada código de operación y su representación textual se mantiene en un único lugar.

El código intermedio define un **conjunto de operaciones** que permite representar asignaciones, expresiones aritméticas y lógicas, comparaciones, instrucciones de control de flujo y llamadas a funciones.

Cada operación se representa mediante un **cuádruplo compuesto por un código de operación y tres operandos**: `arg1`, `arg2` y `resultado`. Los operandos que no se utilizan en una instrucción se representan con `-` al serializar el cuádruplo.

La siguiente tabla resume las **operaciones definidas** en `Cuadruplos.h`.

| **Operación** | **Descripción** | **`arg1`** | **`arg2`** | **`resultado`** | **Ejemplo** |
|---|---|---|---|---|---|
| `ASSIGN` | Asigna un valor a una variable. | Valor de origen | No se utiliza | Variable destino | `ASSIGN 10 - x` |
| `ADD` | Suma dos operandos. | Primer operando | Segundo operando | Temporal con el resultado | `ADD a b T1` |
| `SUB` | Resta el segundo operando al primero. | Minuendo | Sustraendo | Temporal con el resultado | `SUB a b T1` |
| `MUL` | Multiplica dos operandos. | Primer operando | Segundo operando | Temporal con el resultado | `MUL a b T1` |
| `DIV` | Divide el primer operando por el segundo. | Dividendo | Divisor | Temporal con el resultado | `DIV a b T1` |
| `MOD` | Calcula el resto de la división entera. | Dividendo | Divisor | Temporal con el resto | `MOD a b T1` |
| `NEG` | Calcula el opuesto de un valor numérico. | Operando | No se utiliza | Temporal con el resultado | `NEG a - T1` |
| `LT` | Comprueba si el primer operando es menor que el segundo. | Primer operando | Segundo operando | Temporal booleano | `LT a b T1` |
| `GT` | Comprueba si el primer operando es mayor que el segundo. | Primer operando | Segundo operando | Temporal booleano | `GT a b T1` |
| `EQ` | Comprueba si dos operandos son iguales. | Primer operando | Segundo operando | Temporal booleano | `EQ a b T1` |
| `LABEL` | Marca una posición del código mediante una etiqueta. | No se utiliza | No se utiliza | Etiqueta | `LABEL - - L1` |
| `GOTO` | Transfiere incondicionalmente la ejecución a una etiqueta. | No se utiliza | No se utiliza | Etiqueta destino | `GOTO - - L1` |
| `IF_FALSE` | Transfiere la ejecución a una etiqueta si la condición es falsa. | Condición booleana | No se utiliza | Etiqueta destino | `IF_FALSE a - L1` |
| `FUNC_BEGIN` | Marca el comienzo de una función. | No se utiliza | No se utiliza | Nombre de la función | `FUNC_BEGIN calcular - -` |
| `PARAM` | Representa un parámetro de una función. | Nombre del parámetro | No se utiliza | No se utiliza | `PARAM x - -` |
| `ARG` | Registra un argumento de una llamada a función. | Valor del argumento | Índice del argumento | No se utiliza | `ARG a 0 -` |
| `CALL` | Invoca una función con una cantidad determinada de argumentos. | Nombre de la función | Cantidad de argumentos | Temporal para el valor retornado, si corresponde | `CALL sumar 2 T1` |
| `RETURN` | Devuelve un valor y finaliza la ejecución de la función. También admite una devolución vacía. | Valor retornado o `-` | No se utiliza | No se utiliza | `RETURN a - -` |
| `FUNC_END` | Marca el final de una función. | No se utiliza | No se utiliza | Nombre de la función | `FUNC_END calcular - -` |

**Consideraciones sobre las operaciones:**

- Los **temporales** se identifican mediante nombres como `T1`, `T2` y `T3`. Se utilizan para almacenar resultados intermedios de las expresiones.
- Las **etiquetas** se identifican mediante nombres como `L1`, `L2` y `L3`. Permiten representar los destinos de los saltos.
- En `ARG`, el **índice** del argumento comienza en cero.
- En `CALL`, el segundo operando indica la **cantidad de argumentos**. Si la llamada se utiliza como expresión, el resultado se almacena en un temporal; si se utiliza como sentencia, no se necesita un resultado.
- `RETURN` admite tanto **un valor** como una **devolución vacía**.

#### *4.1.4 Creación y administración de operandos*

La implementación proporciona **funciones específicas para crear cada clase de operando**.

Entre ellas se encuentran:

- `crear_operando_vacio ()`.
- `crear_operando_constante ()`.
- `crear_operando_temporal ()`.
- `crear_operando_simbolo ()`.
- `crear_operando_etiqueta ()`.

Estas funciones permiten **construir los operandos de manera uniforme**, asignando la información correspondiente a cada categoría.

Cuando un operando contiene un nombre textual, la implementación realiza una **copia de la cadena**. Esto evita que el operando dependa de la validez o del tiempo de vida de la cadena original utilizada durante su creación.

En el caso de los operandos asociados a símbolos, se conserva el **puntero al símbolo original**, mientras que el nombre textual se copia. La estructura de cuádruplos no pasa a ser propietaria del símbolo ni se encarga de liberarlo.

La función `liberar_operando ()` permite **liberar la memoria** asociada al nombre de un operando y restablecer sus campos.

Esta separación entre la administración del nombre y la referencia al símbolo permite **evitar liberaciones indebidas de memoria** y mantener la relación con la tabla de símbolos.

#### *4.1.5 Construcción de la lista de cuádruplos*

La lista se **inicializa** mediante `crear_lista_cuadruplos ()`.

Las **instrucciones** se incorporan utilizando `agregar_cuadruplo ()`, que recibe el código de operación y los tres operandos correspondientes.

La función verifica que la lista y los operandos sean válidos antes de agregar la instrucción. Luego construye una nueva **representación del cuádruplo**, realiza las copias necesarias de los nombres y enlaza el nodo al final de la lista.

La incorporación de instrucciones actualiza tanto las **referencias** al primer y al último nodo como el **contador** de cuádruplos.

La función devuelve un resultado que permite comprobar si la operación se realizó correctamente. De esta manera, el generador puede **detectar errores durante la construcción de la representación intermedia** y evitar continuar utilizando una lista incompleta como si fuera válida.

La copia de los nombres de los operandos permite que el cuádruplo mantenga su **propia representación textual**, independientemente de las cadenas temporales utilizadas durante la generación.

Por su parte, `cantidad_cuadruplos ()` permite consultar la **cantidad de instrucciones almacenadas** sin tener que recorrer la lista completa.

#### *4.1.6 Liberación de memoria*

La **liberación de memoria** se encuentra centralizada mediante `liberar_lista_cuadruplos ()`.

Esta función recorre la lista y libera los **nombres** almacenados en los operandos de cada instrucción, así como los **nodos** que contienen los cuádruplos. Finalmente, libera la **estructura** de la lista.

La liberación de los nombres no implica liberar los símbolos asociados a los operandos, ya que estos pertenecen a la **tabla de símbolos**.

Esta decisión permite mantener **separadas** las responsabilidades de administración de memoria de ambas estructuras.

La implementación también contempla **errores** durante la creación o copia de los operandos. Si no puede construirse correctamente una instrucción, se libera la memoria reservada para sus componentes antes de informar el fallo.

De esta manera, se busca **mantener la consistencia de la lista y evitar pérdidas de memoria durante su construcción**.

#### *4.1.7 Representación textual de los cuádruplos*

La función `escribir_cuadruplos ()` permite **escribir la lista en un archivo** mediante una representación textual.

Cada instrucción se serializa utilizando el siguiente **formato**:

```text
OPERACION ARG1 ARG2 RESULTADO
```

Cada cuádruplo ocupa **una línea** y los campos se separan mediante **espacios**. Cuando alguno de los operandos no se utiliza, se representa mediante `-`.

Por ejemplo, una **asignación** puede representarse de la siguiente manera:

```text
ASSIGN 10 - x
```

Una **operación aritmética** puede representarse así:

```text
ADD x 5 T1
```

En el primer caso, el valor `10` se asigna a `x`. En el segundo, se suman los operandos `x` y `5`, y el resultado se almacena en `T1`.

Los **nombres textuales** de las operaciones se obtienen mediante `nombre_operacion ()`, mientras que la función `nombre_tipo_dato ()` permite obtener la representación textual de los tipos de datos.

La serialización mantiene un **formato uniforme** para las distintas clases de instrucciones y facilita la inspección de los resultados durante las pruebas.

Sin embargo, la representación textual no incluye todos los campos internos de cada operando. En particular, el **tipo de dato y el puntero al símbolo no se imprimen como campos independientes**. Por lo tanto, el archivo serializado constituye una representación de las instrucciones, pero no contiene necesariamente toda la información disponible en las estructuras internas.

### *4.2 Generación del código intermedio*

La generación del código intermedio se encuentra implementada en `GeneradorCodigoIntermedio.c` y expuesta mediante la interfaz definida en `GeneradorCodigoIntermedio.h`.

Su función es **recorrer el AST y construir una lista de cuádruplos que represente las instrucciones del programa**.

La **función principal** recibe el AST y un indicador que permite habilitar o deshabilitar los mensajes de **depuración**. Cuando la generación finaliza correctamente, devuelve la **lista de cuádruplos** construida. Si se detecta un error que impide continuar, libera la lista parcial y devuelve `NULL`.

El generador presupone que el AST ha sido **validado previamente por el análisis semántico**. Por ello, utiliza la información asociada a los nodos y símbolos sin volver a implementar las verificaciones semánticas propias de la etapa anterior.

#### *4.2.1 Diseño y contexto de generación*

La generación utiliza una **estructura de contexto** que concentra la información necesaria durante el recorrido del AST.

Este contexto mantiene la **lista de cuádruplos** que se está construyendo, los **contadores** utilizados para generar temporales y etiquetas, y la información necesaria para registrar errores y mostrar mensajes de depuración.

La utilización de un contexto permite que las funciones auxiliares **compartan el estado de la generación** sin tener que recibir individualmente todos sus componentes.

El generador se encuentra dividido en **funciones auxiliares especializadas según la construcción del AST**. De esta manera, el recorrido general del árbol se mantiene separado de la traducción específica de las expresiones, asignaciones, sentencias, funciones y estructuras de control.

#### *4.2.2 Recorrido del AST*

La generación comienza verificando que el AST recibido sea válido y que su nodo raíz corresponda a un programa.

A continuación, el generador recorre las declaraciones globales en el orden en que aparecen en el AST. **Las declaraciones de variables globales y las firmas de funciones externas (`AST_DECLARACION_FUNCION_EXTERNA`) no generan instrucciones ejecutables**, mientras que las declaraciones de funciones convencionales se procesan mediante la función encargada de generar su código.

Dentro de cada función, **las declaraciones locales se omiten** durante la generación de instrucciones. Estas declaraciones ya fueron procesadas en las etapas anteriores y no producen por sí mismas operaciones ejecutables en la representación intermedia actual.

Las **sentencias**, en cambio, se recorren respetando su orden en el AST. Cada construcción se deriva hacia la función auxiliar que corresponde a su tipo.

Esta decisión permite **mantener el orden del programa fuente en el código intermedio**, sin incorporar instrucciones adicionales para construcciones que no requieren una operación propia.

#### *4.2.3 Generación de temporales y etiquetas*

Durante la generación se utilizan **temporales** para almacenar los resultados intermedios de las expresiones.

Los temporales se identifican mediante **nombres como `T1`, `T2` y `T3`**. El **contador** correspondiente se incrementa a medida que se necesitan nuevos temporales.

Cada operación que requiere almacenar un resultado genera un **temporal** y lo utiliza como operando de resultado del cuádruplo correspondiente.

Por ejemplo, la expresión `(2 + 3) * 4` puede representarse mediante:

```text
ADD 2 3 T1
MUL T1 4 T2
```

De esta manera, el resultado de la suma se conserva en `T1` y se utiliza como operando de la multiplicación, cuyo resultado queda almacenado en `T2`.

Las **etiquetas** se utilizan para identificar destinos de saltos y puntos de entrada a distintas partes del código. Se generan mediante un contador independiente, utilizando **nombres como `L1`, `L2` y `L3`**.

Mantener contadores separados permite que la generación de temporales no interfiera con la numeración de las etiquetas.

#### *4.2.4 Generación de expresiones*

La **generación de expresiones** se encuentra centralizada en funciones auxiliares que identifican el tipo de nodo y producen los cuádruplos correspondientes.

Las **constantes enteras** se representan utilizando su valor numérico. Las **constantes booleanas** se representan mediante los nombres `TRUE` y `FALSE`.

Las **constantes reales** se convierten a una representación textual utilizando el formato `%g`. Esta decisión permite generar una representación compacta de los valores, aunque puede eliminar la parte decimal cuando no es necesaria para expresar el valor numérico. Por ejemplo, un literal escrito como `2.0` puede aparecer serializado como `2`.

Este comportamiento no modifica el valor numérico, pero significa que **la representación textual no conserva necesariamente la escritura original del literal ni permite determinar su tipo únicamente a partir de la presencia de un punto decimal**.

Los **identificadores** se representan mediante el nombre asociado a su símbolo. El generador utiliza la asociación establecida durante el análisis semántico para recuperar la información correspondiente.

Las **operaciones aritméticas binarias** generan primero los cuádruplos necesarios para obtener sus operandos y luego incorporan la instrucción correspondiente a la operación. El resultado se almacena en un temporal.

Este mecanismo se aplica a la suma, resta, multiplicación, división y módulo. El **menos unario** se representa mediante su operación específica.

El **recorrido recursivo** de las expresiones permite conservar su estructura y respetar el orden de evaluación establecido por el AST.

#### *4.2.5 Generación de operaciones booleanas*

Las operaciones booleanas requieren un tratamiento particular debido a la utilización de **cortocircuito** en los operadores lógicos.

En lugar de traducir directamente todas las operaciones lógicas a una instrucción binaria, el generador utiliza **saltos condicionales y etiquetas** para controlar qué partes de la expresión deben evaluarse.

En el caso del **operador `&&`**, si el primer operando resulta falso, no es necesario evaluar el segundo para determinar el resultado de la expresión.

De manera equivalente, para el **operador `||`**, si el primer operando resulta verdadero, no es necesario evaluar el segundo.

El generador utiliza estas propiedades para construir el flujo de control correspondiente a cada expresión lógica.

La **negación lógica** también se resuelve mediante el control de flujo. Para ello, se intercambian los destinos asociados a los resultados verdadero y falso de la condición, evitando tener que generar una operación de negación independiente para controlar el flujo.

Cuando una expresión booleana necesita producir un valor que pueda utilizarse en una asignación, una operación o un retorno, el generador construye una **secuencia de saltos y etiquetas** que permite asignar `TRUE` o `FALSE` a un temporal booleano.

De esta manera, la implementación permite **utilizar expresiones booleanas tanto como condiciones de control como valores dentro de otras expresiones**.

#### *4.2.6 Generación de asignaciones y sentencias*

Las **asignaciones** se representan mediante la operación `CI_ASIGNAR`, cuyo nombre textual es `ASSIGN`.

El generador obtiene primero el operando correspondiente a la expresión ubicada a la derecha de la asignación y luego construye el **cuádruplo** que almacena ese valor en la variable de destino.

Por ejemplo:

```text
ASSIGN T1 - x
```

representa la **asignación** del valor almacenado en `T1` a la variable `x`.

Las **sentencias `return`** se representan mediante `CI_RETURN`. Cuando existe una expresión de retorno, el generador obtiene su operando y lo incorpora al cuádruplo. Cuando no existe una expresión, se utilizan operandos vacíos.

Por ejemplo:

```text
RETURN T1 - -
RETURN - - -
```

El primer caso representa un **retorno con valor** y el segundo, un **retorno sin expresión**.

El generador no vuelve a verificar si el tipo de retorno es compatible con el tipo declarado para la función, ya que esa comprobación corresponde al análisis semántico.

#### *4.2.7 Generación de estructuras de control*

Las estructuras de control se traducen mediante **etiquetas y saltos**, que permiten representar los distintos caminos de ejecución del programa.

Para las **sentencias `if`**, el generador obtiene la condición y construye el flujo de control correspondiente.

Cuando existe una **rama `else`**, se incorporan los saltos necesarios para evitar ejecutar ambas ramas consecutivamente. Las etiquetas delimitan los puntos de entrada y salida de cada parte de la estructura.

Las **sentencias `while`** se representan mediante una etiqueta que identifica el comienzo del ciclo, la evaluación de la condición, un salto de salida cuando la condición resulta falsa y un salto de retorno al comienzo al finalizar el cuerpo.

Este mecanismo permite representar tanto **estructuras de control simples como estructuras anidadas**, reutilizando las mismas operaciones de salto y etiquetado.

Las expresiones booleanas utilizadas en las condiciones se procesan mediante los **mecanismos de cortocircuito y control de flujo** descritos anteriormente.

#### *4.2.8 Generación de funciones y llamadas*

Las **declaraciones de funciones** se representan mediante las instrucciones `FUNC_BEGIN` y `FUNC_END`, que delimitan el comienzo y el final de cada función.

Al generar una función, el generador incorpora primero la **instrucción de inicio**, luego procesa sus **parámetros** mediante instrucciones `PARAM` y, finalmente, genera el código correspondiente al **cuerpo de la función** antes de incorporar la **instrucción de finalización.**

Los parámetros se representan en el **orden** en que aparecen en la declaración.

Las **llamadas a funciones** se generan mediante la función auxiliar encargada de procesar los nodos `AST_LLAMADA`.

Para cada **argumento**, el generador obtiene primero el operando correspondiente a su expresión y luego incorpora una instrucción `ARG` que identifica el argumento y su índice.

Los **índices** de los argumentos comienzan en **cero** y se asignan respetando el orden de la llamada en el programa fuente.

Una vez generados todos los argumentos, se incorpora una instrucción `CALL`, que identifica la **función invocada** y la **cantidad de argumentos proporcionados**. Cuando la llamada produce un valor utilizado por una expresión, este se almacena en un temporal. Cuando la llamada se utiliza como sentencia y su resultado no se necesita, el operando de resultado queda vacío.

Por ejemplo, una llamada con dos argumentos puede representarse mediante:

```text
ARG 3 0 -
ARG 4 1 -
CALL sumar 2 T1
```

Las llamadas anidadas se procesan **recursivamente**. Por ello, los cuádruplos correspondientes a una llamada interna pueden aparecer antes de que se genere la instrucción `ARG` que utiliza su resultado como argumento de una llamada externa.

Esta organización permite **representar llamadas con expresiones como argumentos, múltiples llamadas dentro de una misma expresión y llamadas a funciones desde otras funciones**.

#### *4.2.9 Manejo de errores durante la generación*

El generador presupone que el AST ha superado el análisis semántico, pero **contempla errores** que pueden impedir la construcción correcta del código intermedio.

Cuando se detecta una situación que impide continuar, la generación **informa el problema** mediante los mecanismos internos correspondientes y evita devolver una lista parcial como resultado válido.

Para ello, si la generación falla después de haber incorporado instrucciones, **se libera la lista de cuádruplos construida hasta ese momento** y la función principal devuelve `NULL`.

Esta decisión permite que el programa principal distinga entre una **generación exitosa** y una **generación fallida** sin tener que comprobar individualmente el estado de cada instrucción producida.

Los **errores de generación** se mantienen separados de los errores léxicos, sintácticos y semánticos. Cada etapa conserva la responsabilidad sobre las operaciones que le corresponden.

#### *4.2.10 Salida del código intermedio*

El resultado principal de esta etapa es una **lista de cuádruplos** que representa las instrucciones generadas a partir del programa fuente.

La lista **puede escribirse en un archivo con extensión `.ci`**, únicamente si la etapa se solicita de forma explícita mediante la opción `-target codinter`. En caso contrario, el generador de código intermedio se ejecuta de manera silenciosa como soporte fundamental para las etapas posteriores del compilador (construyendo los cuádruplos en memoria), sin generar archivos de resultados textuales. Naturalmente, esta etapa no se ejecuta en absoluto si el usuario solicitó un objetivo estrictamente anterior, como `-target scan` o `-target parse`.

Cuando el archivo de salida se genera explícitamente, `escribir_cuadruplos ()` **serializa las instrucciones en el orden en que fueron incorporadas**.

La **salida textual** permite observar las operaciones, los operandos, los temporales, las etiquetas y las instrucciones relacionadas con las funciones.

Por ejemplo, una asignación seguida de una operación aritmética puede representarse mediante:

```text
ASSIGN 10 - x
ADD x 5 T1
```

La representación textual se mantiene **separada** de las estructuras utilizadas internamente durante la generación. Por ello, el archivo permite inspeccionar las instrucciones, mientras que la lista conserva la información adicional de los operandos y sus asociaciones con los símbolos.

La **cantidad total de cuádruplos** generados puede consultarse mediante `cantidad_cuadruplos ()`. Esta información también se utiliza durante la depuración para indicar cuántas instrucciones se incorporaron a la lista.

Cuando la generación finaliza correctamente, la **lista** se devuelve al programa principal para que pueda utilizarse en las etapas posteriores.

#### *4.2.11 Modo de depuración*

El generador de código intermedio incorpora un **modo de depuración** controlado mediante el parámetro `modo_debug`.

Cuando se encuentra habilitado, se muestran **mensajes** que permiten observar el recorrido del AST y el procesamiento de las distintas construcciones del programa.

Estos mensajes utilizan el **prefijo `[CODINT]`** y permiten identificar, entre otras cuestiones:

- El comienzo y la finalización de la generación.
- La generación de declaraciones de funciones.
- El procesamiento de asignaciones.
- La generación de expresiones y operaciones binarias.
- El procesamiento de sentencias `if` y `while`.
- La generación de llamadas a funciones.
- La generación de sentencias `return`.
- La finalización correcta de las distintas construcciones.
- La cantidad total de cuádruplos generados.

Por ejemplo:

```text
[CODINT] Comienza la generación del código intermedio.
[CODINT] Generando una asignación.
[CODINT] Generando una operación binaria.
[CODINT] Operación binaria generada correctamente.
[CODINT] Asignación generada correctamente.
[CODINT] La generación del código intermedio finalizó correctamente.
[CODINT] Total de cuádruplos generados: 8
```

Estos mensajes permiten **seguir el proceso de generación** y detectar en qué construcción se produce un error.

Los **mensajes de depuración se mantienen separados de los cuádruplos**. Por lo tanto, no forman parte de la representación intermedia ni deben confundirse con las instrucciones que se almacenan en la lista.

#### *4.2.12 Integración con el análisis semántico*

La generación de código intermedio se ejecuta después de que el programa haya superado el **análisis semántico**.

Esta separación permite mantener una **división clara** de responsabilidades:

- **El analizador sintáctico** verifica la estructura del programa y construye el AST.
- **El analizador semántico** verifica las restricciones semánticas y asocia los identificadores con sus símbolos.
- **La tabla de símbolos** conserva la información de las declaraciones, los tipos y los ámbitos.
- **El generador de código intermedio** recorre el AST validado y construye la lista de cuádruplos.
- **La representación de cuádruplos** administra las instrucciones y permite serializarlas en un formato textual.

El generador utiliza la **información** de tipo almacenada en los nodos del AST y las referencias a símbolos establecidas durante el análisis semántico. De esta manera, no necesita volver a resolver los identificadores ni repetir las comprobaciones de compatibilidad de tipos.

La generación también se encuentra separada de la **ejecución del programa**. Su responsabilidad consiste en producir la representación intermedia, mientras que la interpretación de esas instrucciones corresponde a la etapa posterior que las consume.

Esta separación permite que las distintas etapas del compilador se desarrollen y verifiquen de manera independiente.

#### *4.2.13 Cambios respecto del Pre-Proyecto*

La implementación actual representa una **ampliación importante** respecto del generador de pseudo-assembly desarrollado durante el Pre-Proyecto.

En aquella versión, la función `generar_pseudo_assembly ()` recibía el AST y un archivo de salida, y **escribía directamente las instrucciones** mediante operaciones como `fprintf ()`.

**La implementación del Pre-Proyecto no contaba con una estructura de cuádruplos**. Las instrucciones se escribían directamente en el archivo, sin construir una representación intermedia estructurada que permitiera administrar por separado los operandos y las operaciones.

En la **implementación actual**, en cambio, se incorporaron los siguientes cambios:

- **Introducción de cuádruplos** para representar las instrucciones mediante una estructura uniforme.
- **Separación entre la generación y la serialización** del código intermedio.
- **Administración dinámica de una lista de cuádruplos**.
- **Clasificación de operandos** según su naturaleza.
- **Asociación de operandos con símbolos** de la tabla de símbolos.
- **Incorporación de tipos de datos** en la representación interna de los operandos.
- **Incorporación de operaciones aritméticas adicionales**, como resta, división, módulo y menos unario.
- **Incorporación de operaciones relacionales y lógicas**.
- **Generación de saltos y etiquetas** para representar estructuras de control y cortocircuito.
- **Representación de funciones (convencionales y externas), parámetros, argumentos, llamadas y retornos**.
- **Manejo de errores y liberación de la lista parcial** cuando la generación falla.
- **Incorporación de mensajes de depuración** para observar el recorrido y la construcción de las instrucciones.

De esta manera, la implementación actual deja de depender de la escritura directa de instrucciones y pasa a construir una **representación intermedia estructurada**, adecuada para su utilización por las etapas posteriores del compilador.

#### *4.2.14 Pruebas*

Se incorporó un conjunto de **48 pruebas** destinadas a verificar la generación del código intermedio.

Las pruebas utilizan programas fuente con extensión `.ctds` y se encuentran **organizadas** en el directorio:

```text
Src/Test/CodigoIntermedio/
```

Los **resultados** generados durante su ejecución se almacenan en:

```text
Src/Test/Resultados/CodigoIntermedio/
```

Los resultados generados durante las pruebas **no forman parte del repositorio** y se encuentran excluidos mediante [.gitignore](../../.gitignore).

Los archivos de resultados permiten **inspeccionar los cuádruplos generados** y, cuando corresponde, las representaciones adicionales producidas durante las pruebas.

Las **pruebas** cubren las principales construcciones que debe traducir el generador:

- **Asignaciones** de constantes enteras, reales y booleanas.
- **Asignaciones entre variables** y múltiples asignaciones en un mismo programa.
- **Operaciones aritméticas** y expresiones anidadas.
- **Precedencia de operadores** y expresiones con múltiples operaciones.
- **Operaciones con variables** y combinación de tipos numéricos.
- **Comparaciones** de menor, mayor e igualdad.
- **Operaciones booleanas**, incluyendo negación, conjunción, disyunción y expresiones complejas.
- **Condicionales** simples, con rama alternativa y anidados.
- **Ciclos** `while`, tanto simples como combinados con condicionales.
- **Funciones** sin parámetros, con uno o varios parámetros y con parámetros de distintos tipos.
- **Llamadas a funciones** utilizadas como sentencias o como parte de expresiones.
- **Llamadas anidadas** y múltiples llamadas dentro de una misma expresión.
- **Funciones con retorno booleano** y funciones que devuelven valores reales.
- **Sentencias return** con valores, expresiones complejas y sin expresión.
- **Programas completos** que combinan funciones, asignaciones, estructuras de control y expresiones.
- **Código muerto**, para verificar que las sentencias inalcanzables también se traducen.
- Declaraciones de **funciones externas (`extern`)**, verificando que el generador las omita correctamente al no requerir código ejecutable en la representación intermedia.

La prueba de código muerto verifica específicamente que el generador **no elimina sentencias por encontrarse después de un retorno o dentro de una rama cuya condición es constante falsa**. La generación conserva esas instrucciones y los saltos correspondientes, ya que la implementación actual no incorpora una etapa de optimización que elimine código inalcanzable.

En conjunto, estas pruebas permiten comprobar que las distintas construcciones del AST se traducen a **instrucciones coherentes** y que la representación intermedia conserva el orden y la estructura necesarios para las etapas posteriores.

---

## *Programa principal*

Se incorporó [main.c](../Src/main.c), encargado de **coordinar la ejecución del compilador y de gestionar la interfaz de línea de comandos**.

En la etapa actual, el programa principal permite ejecutar el **análisis léxico**, el **análisis sintáctico**, el **análisis semántico** y la **generación de código intermedio** del archivo fuente. Las etapas posteriores, correspondientes a la generación de código objeto y a las optimizaciones, se encuentran contempladas en la interfaz, pero todavía no están implementadas.

El **procesamiento** actual consiste en:

1. Lectura y validación de los **argumentos** de la línea de comandos.
2. Determinación de la **etapa de compilación** que debe ejecutarse.
3. Generación del **nombre del archivo de salida**.
4. Creación del **archivo de salida** correspondiente.
5. Ejecución de la **etapa seleccionada**:
   - **Análisis léxico** mediante Flex, para la etapa `scan`.
   - **Análisis sintáctico** mediante Bison, utilizando los tokens proporcionados por Flex, para la etapa `parse`.
   - **Análisis semántico** mediante el AST y la tabla de símbolos, como parte del procesamiento completo.
   - **Generación de código intermedio** a partir del AST validado semánticamente, cuando el análisis semántico finaliza correctamente y no se está ejecutando el modo de pruebas semánticas.
6. Generación de los **archivos auxiliares** correspondientes cuando se encuentra activado el **modo de depuración**.
7. **Liberación** de las estructuras utilizadas durante el análisis y finalización indicando, mediante el código de retorno, si la ejecución fue **exitosa** o si se produjeron **errores**.

El análisis léxico continúa procesando la entrada cuando encuentra errores, permitiendo detectar **múltiples errores** en una misma ejecución. Al finalizar, el programa indica mediante su **código de retorno** si el análisis fue exitoso.

Por su parte, el análisis sintáctico finaliza el procesamiento tras encontrar el **primer error sintáctico**, también con un código de retorno establecido.

El análisis semántico continúa procesando la entrada cuando encuentra errores, permitiendo detectar **múltiples errores** en una misma ejecución. Al finalizar, el programa indica mediante su **código de retorno** si el análisis fue exitoso.

De esta manera, no es necesario corregir un error para que aparezca automáticamente otro. **Se brindan todos los errores encontrados de una vez**. Sin embargo, hay que ser cuidadosos al analizarlos, ya que un error detectado puede ser únicamente causa de otro error previo propagado.

Durante la generación de código intermedio, la ocurrencia de un error provoca que **se cancele la ejecución de la etapa** sin producir ninguna secuencia como salida.

Cuando el análisis sintáctico se ejecuta como parte del **procesamiento completo**, el AST generado se conserva para realizar posteriormente el análisis semántico.

En cambio, cuando se solicita explícitamente **`-target parse`**, el AST solamente se utiliza para completar esa etapa y posteriormente es liberado.

La **generación del archivo de salida** y la configuración del modo de depuración son coordinadas por `main.c`. Dependiendo de la etapa seleccionada, la salida generada corresponde a un archivo `.lex`, `.sint` o `.ci`.

En el caso del **análisis sintáctico**, cuando se activa el modo de depuración, también se genera un archivo `.dot` con la representación del AST.

Cuando se ejecuta el **análisis semántico** con -debug, se genera el archivo .dot después del análisis semántico, por lo que representa el AST enriquecido con la información obtenida durante esta etapa.

El **reconocimiento de los tokens** es responsabilidad del analizador léxico, mientras que la **validación de la estructura sintáctica** y la **construcción del AST** corresponden al analizador sintáctico. El **análisis semántico** utiliza posteriormente ese AST junto con la tabla de símbolos propia de la etapa.

La **generación de código intermedio** se realiza a partir del AST validado semánticamente. Si el análisis sintáctico o semántico detecta errores, se **cancela el procesamiento** y no se genera una secuencia de código intermedio como salida. En caso contrario, se recorre el AST para generar las **instrucciones** correspondientes a las sentencias del programa, escribiéndolas en el archivo de salida con **extensión `.ci`**. Esta etapa se ejecuta **por defecto** cuando no se especifica un objetivo mediante `-target`, mientras que el **modo de pruebas semánticas** permite ejecutar el análisis semántico sin continuar con la generación de código intermedio.

---

## *Interfaz de línea de comandos*

El compilador se **ejecuta** mediante:

```bash
c-tds [opciones] nombreArchivo.ctds
```

El archivo de entrada debe tener extensión `.ctds` y no puede comenzar con `-`.

### *Ejecución por defecto*

Si no se especifica una etapa mediante `target`, el compilador ejecuta el **análisis léxico**, el **análisis sintáctico**, el **análisis semántico** y, si no se producen errores, la **generación de código intermedio**. Esta secuencia constituye el procesamiento completo implementado actualmente.

```bash
c-tds programa.ctds
```

En este caso se genera automáticamente:

```text
programa.ci
```

El archivo `.ci` corresponde al resultado de la última etapa ejecutada, es decir, el **código intermedio**.

### *Selección de etapa*

La **etapa de compilación** puede seleccionarse mediante:

```bash
-target <etapa>
```

Las **etapas reconocidas** son las siguientes:

| **Etapa** | **Descripción** | **Estado** |
| :--- | :--- | :--- |
| `scan` | Análisis léxico | Implementada |
| `parse` | Análisis sintáctico y construcción del AST | Implementada |
| `codinter` | Análisis semántico y generación de código intermedio | Implementada |
| `assembly` | Generación de código objeto | No implementada |

Por ejemplo:

```bash
c-tds -target scan programa.ctds
```

O:

```bash
c-tds -target parse programa.ctds
```

O:

```bash
c-tds -target codinter programa.ctds
```

Actualmente no existe un `-target sem`. El **análisis semántico** se ejecuta automáticamente cuando no se especifica un `-target`, o bien cuando se llega hasta la etapa `target codinter`.

La **etapa todavía no implementada (assembly)** es reconocida por la interfaz y produce un mensaje indicando que la etapa aún no se encuentra disponible.

### *Nombre del archivo de salida*

La opción:

```bash
-o <salida>
```

permite indicar explícitamente **el nombre y la ubicación del archivo de salida**.

Por ejemplo:

```bash
c-tds -o Resultados/prog.ci programa.ctds
```

La opción `-o` utiliza exactamente el nombre proporcionado por el usuario, **no agrega automáticamente una extensión**.

Cuando no se utiliza `-o`, **el nombre de salida se genera automáticamente** a partir del archivo de entrada y de la etapa seleccionada.

Actualmente las **extensiones generadas automáticamente** son las siguientes:

| **Etapa** | **Extensión** |
| :--- | :--- |
| `scan` | `.lex` |
| `parse` | `.sint` |
| `codinter` | `.ci` |
| `assembly` | `.ass` |

La extensión correspondiente a `assembly` se encuentra contemplada por la interfaz, aunque **dicha etapa todavía no está implementada**.

Con la implementación actual, si no se aclara una etapa previa, el compilador llega hasta la **generación de código intermedio**, produciendo un archivo de extensión `.ci`.

Las **pruebas semánticas** utilizan la **opción `-test-sem`** y especifican el archivo de salida mediante `-o`. En este modo, el análisis semántico se ejecuta sin generar código intermedio y el archivo indicado se utiliza para registrar sus resultados. La **opción `-test-sem`** no modifica la extensión generada automáticamente en la ejecución habitual.

### *Modo de depuración*

La opción:

```bash
-debug
```

activa la **salida de información de depuración** durante el análisis.

Por ejemplo:

```bash
c-tds -debug programa.ctds
```

La información mostrada depende de la etapa seleccionada. En el análisis léxico se muestran los **tokens reconocidos junto con su línea y columna**, mientras que en el análisis sintáctico también pueden mostrarse las **reducciones y construcciones reconocidas** por el parser.

Además, durante el análisis sintáctico, el modo de depuración permite **mostrar el AST por consola y generar un archivo `.dot` con su representación**, a partir del cual puede obtenerse una representación gráfica mediante Graphviz.

Cuando se ejecuta el **procesamiento completo** con `-debug`, el archivo `.dot` se genera después del análisis semántico, por lo que contiene el **AST enriquecido** con la información semántica obtenida durante esa etapa.

El modo de depuración es **independiente** del archivo de salida principal del compilador.

### *Optimizaciones*

La opción:

```bash
-opt <optimización>
```

se encuentra contemplada por la interfaz de línea de comandos para las **etapas posteriores** del proyecto.

Actualmente **las optimizaciones todavía no están implementadas**, por lo que su utilización informa esta situación y finaliza la ejecución.

También se contempla:

```bash
-opt all
```

para solicitar la **aplicación de todas las optimizaciones disponibles** cuando estas sean incorporadas.

### *Combinaciones*

La interfaz permite **combinar las opciones** de compilación. Por ejemplo:

```bash
c-tds -target scan -o resultado.lex -debug archivo.ctds
```

En este caso se selecciona el análisis léxico, se especifica el nombre del archivo de salida y se activa el modo de depuración.

---

## *Compilación y ejecución*

El [Makefile](../Makefile) permite **centralizar la compilación del compilador y la ejecución de las pruebas**.

Desde la raíz del repositorio:

### *Compilar*

```bash
cd Proyecto
make
```

El ejecutable generado se encuentra en:

```bash
build/c-tds
```

Durante la compilación se generan los archivos intermedios de Flex y Bison dentro de la carpeta `build/`, que no se encuentra versionada.

### *Ejecutar las pruebas léxicas*

Para **ejecutar todas las pruebas del análisis léxico**:

```bash
make tests-lexico
```

El objetivo **ejecuta las 16 pruebas** y muestra por consola el resultado de cada una.

Al finalizar, **se informa la cantidad de pruebas correctas y fallidas**.

Los **archivos de salida** se generan en:

```text
Src/Test/Resultados/Lexico/
```

### *Ejecutar las pruebas sintácticas*

Para **ejecutar todas las pruebas del análisis sintáctico**:

```bash
make tests-sintactico
```

El objetivo **ejecuta las 133 pruebas** y muestra por consola el resultado de cada una.

Al finalizar, **se informa la cantidad de pruebas correctas y fallidas**.

Las **pruebas válidas** generan archivos `.sint` y, al ejecutarse en modo de depuración, también archivos `.dot`. Estos últimos se convierten a imágenes `.png` mediante Graphviz.

Los **archivos de salida** se generan en:

```text
Src/Test/Resultados/Sintactico/
```

### *Ejecutar las pruebas del AST*

Para **ejecutar todas las pruebas independientes del árbol sintáctico abstracto (AST)**:

```bash
make tests-ast
```

El objetivo compila y ejecuta las **8 pruebas** ubicadas en:

```text
Src/Test/AST/
```

Los **resultados** de estas pruebas se generan en:

```text
Src/Test/Resultados/AST/
```

Al finalizar, **se informa la cantidad de pruebas correctas y fallidas**.

### *Ejecutar las pruebas de la TS*

Para **ejecutar todas las pruebas independientes de la tabla de símbolos (TS)**:

```bash
make tests-ts
```

El objetivo compila y ejecuta las **8 pruebas** ubicadas en:

```text
Src/Test/TS/
```

Los **resultados** de estas pruebas se generan en:

```text
Src/Test/Resultados/TS/
```

Al finalizar, **se informa la cantidad de pruebas correctas y fallidas**.

### *Ejecutar las pruebas semánticas*

Para **ejecutar todas las pruebas del análisis semántico**:

```bash
make tests-semantico
```

El objetivo **ejecuta las 146 pruebas** y muestra por consola el resultado de cada una.

Al finalizar, **se informa la cantidad de pruebas correctas y fallidas**.

Tanto las **pruebas válidas como inválidas** generan archivos `.sem` y, al ejecutarse en modo de depuración, también archivos `.dot`. Estos últimos se convierten a imágenes `.png` mediante Graphviz.

Los **archivos de salida** se generan en:

```text
Src/Test/Resultados/Semantico/
```

### *Ejecutar las pruebas de generación de código intermedio*

Para **ejecutar todas las pruebas del generador de código intermedio**:

```bash
make tests-codint
```

El objetivo compila y ejecuta las **48 pruebas** ubicadas en:

```text
Src/Test/CodigoIntermedio/
```

Los **resultados** de estas pruebas se generan en:

```text
Src/Test/Resultados/CodigoIntermedio/
```

Al finalizar, **se informa la cantidad de pruebas correctas y fallidas**, generando un archivo `.ci` con las instrucciones generadas en la etapa.

### *Ejecutar todas las pruebas*

El objetivo:

```bash
make tests
```

**ejecuta todas las pruebas correspondientes a las etapas implementadas**.

En el estado actual del proyecto, esto equivale a ejecutar:

1. Las pruebas del **análisis léxico**.
2. Las pruebas del **análisis sintáctico**.
3. Las pruebas independientes del **AST**.
4. Las pruebas independientes de la **TS**.
5. Las pruebas del **análisis semántico**.
6. Las pruebas de **código intermedio**.

Este objetivo se encuentra **preparado para incorporar las pruebas de las etapas posteriores** a medida que sean implementadas.

### *Ejecutar el compilador manualmente*

Una vez compilado, puede ejecutarse directamente:

```bash
./build/c-tds archivo.ctds
```

siguiendo la **interfaz de la línea de comandos** presentada previamente.

### *Limpiar archivos generados*

Para **eliminar los archivos generados** durante la compilación y las pruebas:

```bash
make clean
```

Actualmente, este comando elimina:

```text
build/
Src/Test/Resultados/
```

De esta manera, es posible volver a realizar una compilación y ejecución de las pruebas desde un **estado limpio**.

---
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

Como resultado de la etapa de análisis léxico se **genera un archivo con extensión `.lex`**. Esta etapa se ejecuta por defecto y también puede seleccionarse explícitamente mediante `-target scan`.

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

- Nuevos **tipos** y **palabras reservadas**, como `boolean`, `float`, `if`, `else` y `while`.
- **Identificadores** que permiten `_`.
- **Literales** reales.
- **Operadores** relacionales y lógicos.
- **Comentarios** de una y varias líneas.
- **Mecanismo de depuración** controlado mediante `modo_debug`.
- **Registro de los tokens reconocidos** para generar la salida de la etapa léxica.
- Vinculación con la **interfaz de línea de comandos**.

La **integración con Bison** permite que el mismo analizador léxico sea utilizado durante la etapa de análisis léxico y, a su vez, como componente del análisis sintáctico.

#### *1.1.7 Pruebas*

Se incorporaron **14 pruebas** para el análisis léxico, divididas en:

- **10 pruebas válidas**, destinadas a verificar el reconocimiento correcto de los elementos léxicos.
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

Como resultado de la etapa de análisis sintáctico se genera un archivo con extensión `.sint`.

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

Se incorporaron **120 pruebas** para el análisis sintáctico, divididas en:

- **55 pruebas válidas**, destinadas a verificar que programas sintácticamente correctos sean aceptados.
- **65 pruebas inválidas**, destinadas a verificar que construcciones que no pertenecen a la gramática sean rechazadas.

Además de verificar la aceptación o rechazo de cada entrada, las pruebas válidas permiten comprobar la **estructura del AST generado** mediante los archivos correspondientes y la inspección de los árboles obtenidos.

Las **pruebas válidas** cubren:

- Programas vacíos.
- Declaraciones de variables globales.
- Declaraciones con múltiples identificadores.
- Declaraciones de funciones.
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

El AST mantiene **referencias** que permiten vincular, en etapas posteriores, los **nodos que correspondan con los elementos de la tabla de símbolos**.

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
- Las **funciones** y sus **parámetros**.
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
- Una **función** contiene su identificador, sus parámetros y su bloque.

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

La TS permite almacenar información de **variables, funciones y parámetros**, asociando a cada símbolo un nombre, un tipo de dato y una clase. Además, mantiene información relacionada con su estado de inicialización y una dirección para poder utilizar en etapas posteriores del compilador.

La tabla organiza los símbolos mediante **niveles de ámbito**, permitiendo representar distintos contextos de declaración y realizar búsquedas desde el nivel más interno hacia los niveles exteriores.

#### *2.2.1 Diseño de los símbolos*

Cada símbolo de la TS se representa mediante la **estructura `Simbolo`**, que contiene:

- El **nombre** del identificador.
- El **tipo de dato** asociado, representado mediante `TipoDato`.
- La **clase del símbolo**, representada mediante `ClaseSimbolo`.
- El estado de **inicialización** del símbolo.
- La **dirección** asociada al símbolo, cuando corresponda.
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
- Una **referencia al nivel anterior**.

La tabla de símbolos completa se representa mediante `TablaSimbolos`, que mantiene un puntero al **nivel actualmente abierto**.

Los niveles se organizan como una **pila**. Al abrir un nuevo nivel, este pasa a ser el nivel actual y conserva una referencia al nivel que se encontraba abierto anteriormente.

Esta organización permite representar **ámbitos anidados** y facilita la búsqueda de identificadores respetando la visibilidad determinada por el nivel en el que fueron declarados.

#### *2.2.3 Inicialización de la tabla de símbolos*

La función `iniciar_TS ()` crea una **nueva tabla de símbolos** y genera inicialmente un **nivel vacío**.

De esta forma, la tabla siempre comienza con un **nivel abierto**, sobre el cual pueden realizarse las primeras inserciones.

La función **reserva dinámicamente la memoria** necesaria para la estructura `TablaSimbolos` y para su nivel actual.

#### *2.2.4 Apertura y cierre de niveles*

La función `abrir_nivel ()` permite **crear un nuevo nivel** de la tabla de símbolos.

El nuevo nivel se establece como **nivel actual** y mantiene una referencia al **nivel anterior**.

La función `cerrar_nivel ()` elimina el nivel actualmente abierto y todos los símbolos que contiene. Luego, **el nivel anterior pasa a ser el nivel actual**.

Esta organización permite que los **símbolos declarados dentro de un ámbito** dejen de estar disponibles cuando dicho ámbito se cierra.

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

#### *2.2.8 Dirección asociada al símbolo*

Cada símbolo contiene un campo `direccion` destinado a almacenar una **ubicación asociada al símbolo**.

Al momento de insertar un nuevo símbolo, este campo se **inicializa** con `direccion = -1`. El valor indica que todavía no existe una dirección asignada.

La asignación efectiva de direcciones corresponde a las etapas posteriores relacionadas con la **representación y generación de código**.

#### *2.2.9 Manejo de memoria*

La implementación de la TS **administra dinámicamente la memoria** utilizada por la tabla, sus niveles y sus símbolos.

La función `iniciar_TS ()` **reserva la memoria** correspondiente a la tabla y crea su nivel inicial.

La función `insertar_elemento ()` **reserva memoria** para cada nuevo símbolo y realiza una copia independiente del nombre.

La función `cerrar_nivel ()` utiliza **`liberar_nivel ()` para liberar**:

1. Los **nombres** almacenados dinámicamente a los símbolos.
2. Las **estructuras `Simbolo`** pertenecientes al nivel.
3. La **estructura `Nivel`** correspondiente.

La función `liberar_TS ()` **cierra todos los niveles** que permanezcan abiertos y finalmente **libera la estructura principal de la tabla**. De esta manera, la TS es responsable de administrar la memoria correspondiente a sus símbolos.

#### *2.2.10 Cambios respecto del Pre-Proyecto*

La implementación actual de la tabla de símbolos **amplía la versión desarrollada durante el Pre-Proyecto** para adaptarla a las nuevas construcciones del lenguaje C-TDS.

Entre los principales **cambios** se encuentran:

- Incorporación de la **clase** `SIMBOLO_PARAMETRO`.
- Incorporación del **estado de inicialización** mediante el campo `inicializada`.
- Incorporación del **campo** `direccion` para almacenar la ubicación asociada al símbolo.
- Incorporación de **validaciones** sobre los tipos de datos y las clases de símbolos durante la inserción.

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
- **Liberación de la memoria** utilizada por la tabla.

Las pruebas se encuentran **organizadas** en:

```text
Src/Test/TS/
```

Los **resultados** generados durante su ejecución se almacenan en:

```text
Src/Test/Resultados/TS/
```

Los resultados de las pruebas **no forman parte del repositorio** y se encuentran excluidos mediante [.gitignore](../../.gitignore).

---

## *Programa principal*

Se incorporó [main.c](../Src/main.c), encargado de **coordinar la ejecución del compilador y de gestionar la interfaz de línea de comandos**.

En la etapa actual, el programa principal permite ejecutar el **análisis léxico** y el **análisis sintáctico** del archivo fuente. Las etapas posteriores se encuentran contempladas en la interfaz, pero todavía no están implementadas.

El **procesamiento** actual consiste en:

1. Lectura y validación de los **argumentos** de la línea de comandos.
2. Determinación de la **etapa de compilación** que debe ejecutarse.
3. Apertura del **archivo fuente**.
4. Ejecución de la **etapa seleccionada**:
   - **Análisis léxico** mediante Flex, para la etapa `scan`.
   - **Análisis sintáctico** mediante Bison, utilizando los tokens proporcionados por Flex, para la etapa `parse`.
5. Generación del **archivo de salida** correspondiente a la etapa seleccionada.
6. Finalización indicando, mediante el código de retorno, si el análisis fue **exitoso** o si se produjeron **errores**.

El análisis léxico continúa procesando la entrada cuando encuentra errores, permitiendo detectar **múltiples errores** en una misma ejecución. Al finalizar, el programa indica mediante su **código de retorno** si el análisis fue exitoso.

Por su parte, el análisis sintáctico finaliza el procesamiento tras encontrar el **primer error sintáctico**, también con un código de retorno establecido.

La **generación del archivo de salida** y la configuración del modo de depuración son coordinadas por `main.c`. Dependiendo de la etapa seleccionada, la salida generada corresponde a un archivo `.lex` o `.sint`. En el caso del análisis sintáctico, cuando se activa el modo de depuración, también se genera un archivo `.dot` con la representación del AST.

El **reconocimiento de los tokens** es responsabilidad del analizador léxico, mientras que la **validación de la estructura sintáctica** y la **construcción del AST** corresponden al analizador sintáctico.

---

## *Interfaz de línea de comandos*

El compilador se **ejecuta** mediante:

```bash
c-tds [opciones] nombreArchivo.ctds
```

El archivo de entrada debe tener extensión `.ctds` y no puede comenzar con `-`.

### *Ejecución por defecto*

Si no se especifica una etapa, **se ejecuta el análisis sintáctico**, que constituye la última etapa actualmente implementada.

```bash
c-tds programa.ctds
```

En este caso se genera automáticamente:

```text
programa.sint
```

### *Selección de etapa*

La **etapa de compilación** puede seleccionarse mediante:

```bash
-target <etapa>
```

Las **etapas reconocidas** son las siguientes:

| **Etapa** | **Descripción** | **Estado** |
| :--- | :--- | :--- |
| `scan` | Análisis léxico | Implementada |
| `parse` | Análisis sintáctico | Implementada |
| `codinter` | Generación de código intermedio | No implementada |
| `assembly` | Generación de código objeto | No implementada |

Por ejemplo:

```bash
c-tds -target scan programa.ctds
```

O:

```bash
c-tds -target parse programa.ctds
```

Las **etapas todavía no implementadas** son reconocidas por la interfaz y producen un mensaje indicando que la etapa aún no se encuentra disponible.

### *Nombre del archivo de salida*

La opción:

```bash
-o <salida>
```

permite indicar explícitamente **el nombre y la ubicación del archivo de salida**.

Por ejemplo:

```bash
c-tds -o Resultados/prog.sint programa.ctds
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

Las extensiones correspondientes a `codinter` y `assembly` se encuentran contempladas por la interfaz, aunque **dichas etapas todavía no están implementadas**.

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

El objetivo **ejecuta las 14 pruebas** y muestra por consola el resultado de cada una.

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

El objetivo **ejecuta las 120 pruebas** y muestra por consola el resultado de cada una.

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
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

Su función es **verificar que la secuencia de tokens producida por el analizador léxico respete la estructura sintáctica del lenguaje**.

En esta etapa no se construye todavía el árbol sintáctico abstracto (AST) ni se realizan comprobaciones semánticas.

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

En caso de producirse un **error sintáctico**, el mensaje de error se informa mediante la salida estándar de errores (`stderr`) y la ejecución de esta etapa finaliza indicando que el análisis no fue exitoso.

#### *1.2.3 Ubicación de errores*

El analizador sintáctico utiliza las ubicaciones proporcionadas por **Bison** mediante `%locations`.

La información de ubicación se obtiene a través de `yylloc`, que contiene la **posición del elemento sintáctico** actualmente analizado.

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
- **Bison** verifica la estructura sintáctica.
- **main.c** coordina la ejecución de ambas etapas.

En esta etapa, el analizador sintáctico utiliza los **valores semánticos** únicamente para permitir el reconocimiento de los tokens correspondientes. La construcción del AST y el procesamiento semántico serán incorporados en etapas posteriores.

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

A diferencia del Pre-Proyecto, donde las acciones del parser estaban orientadas a la **construcción del AST**, en la implementación actual estas acciones todavía no construyen el AST.

En esta etapa, las acciones se utilizan principalmente para **registrar las construcciones sintácticas reconocidas** y generar la salida `.sint`.

#### *1.2.7 Pruebas*

Se incorporaron **120 pruebas** para el análisis sintáctico, divididas en:

- **55 pruebas válidas**, destinadas a verificar que programas sintácticamente correctos sean aceptados.
- **65 pruebas inválidas**, destinadas a verificar que construcciones que no pertenecen a la gramática sean rechazadas.

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

La **generación del archivo de salida** y la configuración del modo de depuración son coordinadas por `main.c`. Dependiendo de la etapa seleccionada, la salida generada corresponde a un archivo `.lex` o `.sint`.

El **reconocimiento de los tokens** es responsabilidad del analizador léxico, mientras que la **validación de la estructura sintáctica** corresponde al analizador sintáctico.

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

El modo de depuración es **independiente** de los archivos de salida generados por el compilador.

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

Los **archivos de salida** se generan en:

```text
Src/Test/Resultados/Sintactico/
```

### *Ejecutar todas las pruebas*

El objetivo:

```bash
make tests
```

**ejecuta todas las pruebas correspondientes a las etapas implementadas**.

En el estado actual del proyecto, esto equivale a ejecutar las pruebas del **análisis léxico**, seguidas de las pruebas del **análisis sintáctico**. Este objetivo se encuentra **preparado para incorporar las pruebas de las etapas posteriores** a medida que sean implementadas.

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
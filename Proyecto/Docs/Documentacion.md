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

#### *1.1.1 Diseño y decisiones*

El lexer reconoce las **palabras reservadas, identificadores, literales enteros y reales, operadores y delimitadores** definidos en la especificación.

El lenguaje es **sensible a mayúsculas y minúsculas (case-sensitive)**. Los identificadores pueden comenzar con una letra y continuar con letras, dígitos o `_`.

El carácter `-` se reconoce siempre como un operador. La distinción entre **resta binaria** y **menos unario** se deja para el análisis sintáctico.

Los **comentarios de una línea y multilínea son ignorados**. Para los comentarios multilínea se utiliza un **estado exclusivo del autómata de Flex**, lo que permite detectar además el caso en que el archivo finaliza antes de encontrar el cierre `*/`.

Las reglas fueron escritas en un orden tal que Flex dé prioridad a las coincidencias más específicas. Ante varias reglas que pueden reconocer una misma secuencia, **Flex selecciona la coincidencia más larga** y, en caso de empate, **la regla que aparece primero**.

La implementación del lexer se mantiene **independiente de la interfaz de línea de comandos y de las etapas posteriores del compilador**. El archivo `Src/main.c` actúa como punto de entrada del programa y es responsable de coordinar la ejecución del análisis léxico.

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

En caso de producirse **errores léxicos**, el análisis continúa para permitir detectar múltiples errores en una misma ejecución. En ese caso, **el archivo `.lex` puede contener los tokens reconocidos correctamente**, mientras que los errores se informan por la salida estándar de errores y el compilador finaliza indicando que el análisis léxico no fue exitoso.

Este formato fue definido como una **decisión de diseño** debido a que la especificación establece la generación de un archivo `.lex`, pero no determina un formato específico para su contenido.

#### *1.1.3 Ubicación de errores*

El analizador mantiene el **número de línea y columna** de la entrada para poder informar la ubicación de los errores léxicos.

El número de línea es gestionado mediante `yylineno`, mientras que la columna se mantiene mediante la variable `yycolumn`.

Cuando se encuentra un **carácter no reconocido**, se informa el error indicando el carácter, la línea y la columna correspondiente.

**El análisis no se detiene al primer error léxico**. Se registra la existencia del error mediante `error_lexico` y se continúa procesando la entrada, permitiendo **detectar múltiples errores** en una misma ejecución.

**Los errores léxicos no se consideran tokens y, por lo tanto, no se incorporan al archivo `.lex`**. Se informan mediante la salida estándar de errores (`stderr`).

#### *1.1.4 Modo de depuración*

Se incorporó un mecanismo de **modo debug** mediante la variable `modo_debug`, que es configurada por `main.c` a partir de la opción `-debug`.

Cuando se encuentra **desactivado**, el analizador no produce salida para los tokens reconocidos. Cuando se **activa**, muestra información sobre cada token junto con su línea y columna.

**El modo debug se mantiene separado de la generación del archivo `.lex`**. De esta manera, la salida destinada al usuario durante la ejecución y la salida correspondiente al resultado de una etapa de compilación no se mezclan.

Esta decisión permite utilizar el mismo lexer tanto en la **ejecución normal** como durante las **tareas de desarrollo y pruebas**.

#### *1.1.5 Cambios respecto del Pre-Proyecto*

El analizador léxico fue **ampliado para adaptarse a la especificación de C-TDS**. Entre los principales cambios se incorporaron:

- Nuevos **tipos** y **palabras reservadas**, como `boolean`, `float`, `if`, `else` y `while`.
- **Identificadores** que permiten `_`.
- **Literales** reales.
- **Operadores** relacionales y lógicos.
- **Comentarios** de una y varias líneas.
- **Mecanismo de depuración** controlado mediante `modo_debug`.
- **Registro de los tokens reconocidos** para generar la salida de la etapa léxica.
- Vinculación con la **interfaz de línea de comandos**.

La implementación se mantiene independiente de **Bison**, que será incorporado posteriormente.

#### *1.1.6 Pruebas*

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

---

## *Programa principal*

Se incorporó [main.c](../Src/main.c), encargado de **coordinar la ejecución del compilador y de gestionar la interfaz de línea de comandos**.

En la etapa actual, el programa principal permite ejecutar el **análisis léxico** del archivo fuente. Las etapas posteriores se encuentran contempladas en la interfaz, pero todavía no están implementadas.

El **procesamiento** actual consiste en:

1. Lectura y validación de los **argumentos** de la línea de comandos.
2. Apertura del **archivo fuente**.
3. Ejecución del **análisis léxico** mediante Flex.
4. Generación del **archivo de salida** correspondiente a la etapa léxica.
5. Finalización indicando si el análisis fue **exitoso** o si se produjeron **errores léxicos**.

El análisis léxico continúa procesando la entrada cuando encuentra errores, permitiendo detectar **múltiples errores** en una misma ejecución. Al finalizar, el programa indica mediante su **código de retorno** si el análisis fue exitoso.

La **generación del archivo `.lex`** y la configuración del modo de depuración son coordinadas por `main.c`, mientras que el **reconocimiento de los tokens** es responsabilidad del analizador léxico.

---

## *Interfaz de línea de comandos*

El compilador se **ejecuta** mediante:

```bash
c-tds [opcion] nombreArchivo.ctds
```

El archivo de entrada debe tener extensión `.ctds` y no puede comenzar con `-`.

### *Ejecución por defecto*

Si no se especifica una etapa, **se ejecuta el análisis léxico**, que constituye la etapa actualmente implementada.

```bash
c-tds programa.ctds
```

En este caso se genera automáticamente:

```text
programa.lex
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
| `parse` | Análisis sintáctico | No implementada |
| `codinter` | Generación de código intermedio | No implementada |
| `assembly` | Generación de código objeto | No implementada |

Por ejemplo:

```bash
c-tds -target scan programa.ctds
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
c-tds -o Resultados/programa.lex programa.ctds
```

La opción `-o` utiliza exactamente el nombre proporcionado por el usuario, **no agrega automáticamente una extensión**.

Cuando no se utiliza `-o`, **el nombre de salida se genera automáticamente** a partir del archivo de entrada y de la etapa seleccionada.

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

En este modo **se muestran por consola los tokens reconocidos junto con su línea y columna**.

El modo de depuración es **independiente** del archivo `.lex` generado por el compilador.

### *Optimizaciones*

La opción:

```bash
-opt <optimizacion>
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

Los archivos generados se almacenan en la carpeta `build/`, que no se encuentra versionada.

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

### *Ejecutar todas las pruebas*

El objetivo:

```bash
make tests
```

**ejecuta todas las pruebas correspondientes a las etapas implementadas**.

En el estado actual del proyecto, esto equivale a ejecutar las pruebas del análisis léxico. Este objetivo se encuentra **preparado para incorporar las pruebas de las etapas posteriores** a medida que sean implementadas.

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
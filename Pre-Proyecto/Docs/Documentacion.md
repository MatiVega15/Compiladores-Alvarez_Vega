# **Documentación del Pre-Proyecto**

El Pre-Proyecto consiste en el **desarrollo progresivo de un pequeño lenguaje de programación** y las herramientas necesarias para su análisis y procesamiento.

Las etapas previstas son:

1. Extensión de la **gramática**.
2. Definición de **expresiones regulares**.
3. Implementación de un **analizador léxico**.
4. Implementación de un **analizador sintáctico**.
5. Generación de un **AST (Árbol Sintáctico Abstracto)**.
6. Implementación de la **Tabla de Símbolos (TS)**, **análisis semántico** e **intérprete**.
7. Generación de **seudo-assembly**.

## *1. Gramática*

La gramática fue extendida según los requerimientos del enunciado para incorporar las distintas características del lenguaje, incluyendo **declaraciones de variables, asignaciones, sentencias de retorno y constantes booleanas**.

Las declaraciones de variables se realizan al comienzo del bloque y pueden corresponder a los tipos `int` y `bool`. El programa puede definir una función `main` con retorno `int`, `bool` o `void`. De manera obligatoria, debe definirse **al menos una declaración y al menos una sentencia por programa**.

*Ver [Especificación del Pre-Proyecto](Especificacion.md#punto-1-gramática) para más detalles.*

## *2. Expresiones Regulares*

Las expresiones regulares fueron definidas para reconocer los distintos componentes léxicos del lenguaje, incluyendo **identificadores, constantes enteras y booleanas, operadores, delimitadores y palabras reservadas**.

Los identificadores comienzan con una letra y pueden continuar con letras o dígitos.

*Ver [Especificación del Pre-Proyecto](Especificacion.md#punto-2-expresiones-regulares) para más detalles.*

## *3. Analizador Léxico*

El analizador léxico fue implementado utilizando la herramienta **Flex**, a partir de las expresiones regulares definidas para el lenguaje.

Además de reconocer los distintos tokens, el analizador registra la **ubicación de cada token mediante su número de línea y columna**. Esta información es utilizada posteriormente por el analizador sintáctico y el análisis semántico para reportar errores indicando su ubicación en el código fuente.

*Para revisar su implementación, ver [Analizador Léxico](../Src/Lexer/AnalizadorLexico.lex).*

## *4. Analizador Sintáctico*

El analizador sintáctico fue implementado utilizando la herramienta **Bison** a partir de la gramática extendida.

Se definieron las reglas sintácticas correspondientes a **declaraciones, asignaciones, sentencias de retorno y expresiones**. También se establecieron las precedencias de los operadores `'+'` y `'*'` para resolver las ambigüedades propias de las expresiones aritméticas.

Durante el análisis sintáctico se construye el **AST correspondiente al programa de entrada**.

El parser utiliza además la información de ubicación proporcionada por el analizador léxico para conservar el **número de línea asociado a los nodos construidos**.

*Para revisar su implementación, ver [Analizador Sintáctico](../Src/Parser/AnalizadorSintactico.y).*

## *5. Árbol Sintáctico Abstracto (AST)*

Se incorporó la construcción de un **Árbol Sintáctico Abstracto (AST)** durante el análisis sintáctico. El árbol representa la estructura esencial del programa mediante nodos que corresponden a sus principales construcciones: **programa, declaraciones, sentencias, asignaciones, retornos, expresiones, identificadores, números y valores booleanos.**

El AST permite trabajar con una representación estructurada del programa, independiente del texto original y de los detalles propios de la gramática.

Cada nodo del AST conserva, además de su tipo y la información correspondiente a su contenido, el **número de línea en el que se encuentra**. Esta información permite reportar posteriormente errores indicando su ubicación en el programa fuente.

Luego de incorporar la **Tabla de Símbolos (TS)**, los nodos del AST fueron modificados para poder mantener una **referencia al símbolo correspondiente de la TS**. De esta manera, los identificadores presentes en el árbol pueden quedar asociados con la información registrada para ellos durante el análisis semántico.

El módulo `AST.c` / `AST.h` proporciona **operaciones** para:

- Crear **nodos** y establecer **relaciones** entre ellos.
- Asociar **información** a los nodos.
- **Imprimir** el árbol por consola.
- Generar una **representación** en formato DOT.
- **Liberar la memoria** utilizada por el árbol.

*Para revisar su implementación, ver [AST.h](../Src/AST/AST.h) y [AST.c](../Src/AST/AST.c).*

Los árboles de los programas aceptados pueden exportarse a formato **DOT** y convertirse a **PNG** mediante **Graphviz**. Los resultados se almacenan en `Src/Test/Resultados`, carpeta que no se encuentra versionada.

Se incorporó [TestAST.c](../Src/Test/TestAST.c), una **prueba independiente destinada a verificar el funcionamiento del Árbol Sintáctico Abstracto**.

## *6. Tabla de Símbolos (TS), Análisis Semántico e Intérprete*

Como continuación del procesamiento del lenguaje, se incorporaron una **Tabla de Símbolos (TS)**, un **Analizador Semántico** y un **Intérprete**.

### *6.1 Tabla de Símbolos (TS)*

Se implementó una **Tabla de Símbolos (TS)** que permite registrar y consultar los identificadores declarados en el programa.

La TS utiliza una estructura de niveles, permitiendo administrar el **alcance y ámbito de los símbolos** y realizar búsquedas desde el nivel actual hacia niveles anteriores.

Cada símbolo almacena **información estática** sobre el elemento declarado, incluyendo:

- **Nombre**.
- **Tipo** de dato.
- **Clase** del símbolo.

Las variables declaradas son incorporadas a la TS durante el análisis semántico, etapa donde también se realizan búsquedas de identificadores para determinar si fueron declarados previamente.

Se incorporó [TestTS.c](../Src/Test/TestTS.c), una **prueba independiente destinada a verificar el funcionamiento de la TS**, incluyendo la inserción, búsqueda y manejo de símbolos.

*Para revisar su implementación, ver [TS.h](../Src/TS/TS.h) y [TS.c](../Src/TS/TS.c).*

### *6.2 Analizador Semántico*

Se implementó un **analizador semántico** encargado de verificar que los programas que superaron el análisis léxico y sintáctico sean semánticamente válidos.

Entre las **verificaciones** realizadas se encuentran:

- Detección de **variables no declaradas**.
- Detección de **declaraciones duplicadas**.
- Verificación de **compatibilidad de tipos** en las asignaciones.
- Verificación de los **tipos de los operandos** de las operaciones `+` y `*`.
- Verificación de **compatibilidad entre el tipo de retorno y el tipo declarado para `main`**.
- Verificación de las distintas **formas de la sentencia `return`**.

Durante este análisis, los nodos identificadores del AST se asocian a sus correspondientes símbolos de la TS y se propaga la **información de tipos** necesaria para realizar las comprobaciones semánticas.

Los errores semánticos se informan indicando la **línea correspondiente del programa fuente**.

*Para revisar su implementación, ver [AnalizadorSemantico.h](../Src/Semantico/AnalizadorSemantico.h) y [AnalizadorSemantico.c](../Src/Semantico/AnalizadorSemantico.c).*

### *6.3 Intérprete*

Se implementó un **intérprete** encargado de ejecutar los programas que superaron correctamente las etapas de análisis léxico, sintáctico y semántico.

El intérprete recorre el AST y evalúa las expresiones y sentencias del programa. Para mantener el **estado de ejecución de las variables** se utiliza una estructura independiente de la TS, que almacena el valor actual de cada variable y si esta fue inicializada.

Entre las **operaciones** soportadas se encuentran:

- **Asignación** de valores enteros y booleanos.
- **Reasignación** de variables.
- **Evaluación de expresiones** con `+` y `*`.
- **Evaluación de identificadores** utilizando su valor actual.
- **Ejecución de sentencias** `return`.
- **Detención de la ejecución** al encontrar el primer `return`.
- **Detección de uso de variables declaradas pero no inicializadas**.

Los errores que dependen del **estado de ejecución**, como el uso de una variable sin inicializar, son informados durante la interpretación.

*Para revisar su implementación, ver [Interprete.h](../Src/Interprete/Interprete.h) y [Interprete.c](../Src/Interprete/Interprete.c).*

## *7. Generador de Seudo-Assembly*

Se implementó un **generador de seudo-assembly** encargado de producir una representación similar a un **código de tres direcciones** a partir del AST validado semánticamente.

El generador utiliza **temporales** (`T1`, `T2`, `T3`, etc.) para almacenar los resultados intermedios de las expresiones.

Las instrucciones utilizadas son:

- **SUM** *operador1 operador2 resultado*
- **MUL** *operador1 operador2 resultado*
- **MOV** *operador resultado*
- **RETURN** *operador*
- **RETURN**

Las operaciones se generan respetando la estructura del AST y, por lo tanto, las precedencias y los paréntesis establecidos durante el análisis sintáctico.

Las **asignaciones** simples se representan mediante la instrucción `MOV`, mientras que las **operaciones** `+` y `*` generan un temporal para almacenar su resultado.

Las **constantes booleanas** se representan como `TRUE` y `FALSE` dentro del seudo-assembly.

Las **declaraciones** no generan instrucciones, ya que la información correspondiente a las variables ya fue registrada durante el análisis semántico en la TS.

El generador utiliza la información de los símbolos asociada a los identificadores del AST para obtener los **nombres de las variables** durante la generación del código.

El generador produce archivos con extensión `.asm`. Estos archivos contienen **seudo-assembly y no constituyen código assembly real destinado a ser ensamblado o ejecutado directamente**.

*Para revisar su implementación, ver [GeneradorPseudoAssembly.h](../Src/Generador/GeneradorPseudoAssembly.h) y [GeneradorPseudoAssembly.c](../Src/Generador/GeneradorPseudoAssembly.c).*

---

## *Programa principal*

Se incorporó [main.c](../Src/main.c), encargado de **coordinar las distintas etapas de procesamiento del programa**.

El programa principal recibe como primer argumento un archivo fuente y, opcionalmente un segundo argumento para el archivo de salida en formato DOT y un tercero para el archivo de salida de seudo-assembly.

Por ejemplo:

```bash
./build/Interprete archivo.txt archivo.dot archivo.asm
```

El **procesamiento** se realiza de manera progresiva:

1. Análisis **léxico**.
2. Análisis **sintáctico** y construcción del **AST**.
3. Análisis **semántico** y construcción de la **TS**.
4. Generación del **seudo-assembly**.
5. **Interpretación** del programa.

Si se detectan **errores léxicos o sintácticos**, el procesamiento se detiene y no se generan las salidas correspondientes al AST ni al seudo-assembly. La ejecución de cualquiera de estos dos análisis finaliza tras encontrar el primer error.

Si se detectan **errores semánticos**, se informa la totalidad de los errores encontrados y no se realiza la generación de seudo-assembly ni la interpretación. Es decir, este análisis no se detiene tras el primer error. Además, se genera el AST en formato DOT y PNG.

Si la interpretación produce un **error de ejecución**, este se informa y se conserva la posibilidad de generar la representación gráfica del AST y el seudo-assembly. Esta etapa tampoco se detiene al encontrar el primer error.

## *Pruebas*

Se incorporaron **36 pruebas** que cubren las distintas etapas del procesamiento del lenguaje.

Las **pruebas** incluyen:

- **Programas válidos**.
- **Errores léxicos**.
- **Errores sintácticos**.
- **Errores semánticos**.
- **Errores detectados durante la interpretación**.

Las pruebas se encuentran en `Src/Test/`.

Las pruebas válidas que superan el análisis semántico permiten comprobar, además, la correcta generación del **seudo-assembly**.

También se mantienen **pruebas independientes para los módulos AST y TS**:

- [TestAST.c](../Src/Test/TestAST.c), para verificar la construcción manual y generación del AST.
- [TestTS.c](../Src/Test/TestTS.c), para verificar las operaciones de la TS.

## *Compilación y Ejecución*

El [Makefile](../Makefile) permite **centralizar la compilación y ejecución** de las distintas herramientas y pruebas.

Desde la raíz del repositorio:

### **Compilar**

```bash
cd Pre-Proyecto
make
```

El ejecutable generado se encuentra en:

```bash
build/Interprete
```

Los archivos generados se almacenan en la carpeta `build/`, que no se encuentra versionada.

### **Ejecutar todas las pruebas**

```bash
make tests
```

Este comando ejecuta las **36 pruebas disponibles**.

Para cada prueba se genera, cuando corresponde, su representación en formato **DOT** y su correspondiente imagen **PNG**, así como también el archivo de **seudo-assembly** en formato **ASM**.

Los resultados se muestran por consola y se almacenan en:

```bash
Src/Test/Resultados
```

Dicha carpeta se encuentra excluida del repositorio mediante [.gitignore](../../.gitignore).

### **Ejecutar una prueba individual**

Se puede ejecutar una prueba específica mediante:

```bash
make prueba PRUEBA=<nombre_prueba>
```

Por ejemplo:

```bash
make prueba PRUEBA=Prueba1_Valida_AsignacionesYExpresiones
```

No es necesario incluir la extensión `.txt`, ya que `Makefile` la agrega automáticamente.

El comando ejecuta la prueba, la muestra por consola y genera, cuando corresponde, el **DOT** y el **PNG** del AST, así como el **seudo-assembly** en formato **ASM**. Los resultados se encuentran en la carpeta `Src/Test/Resultados`.

También es posible utilizar este objetivo para analizar/interpretar un archivo de entrada que no se encuentre incluido en la lista de las 36 pruebas. Por ejemplo, si existe:

```bash
Src/Test/programa.txt
```

Puede ejecutarse mediante:

```bash
make prueba PRUEBA=programa
```

### **Probar el AST**

Para compilar y ejecutar la prueba independiente del AST:

```bash
make test-ast
```

Esta prueba construye manualmente un AST y genera su representación gráfica mediante **Graphviz**.

### **Probar la TS**

Para compilar y ejecutar la prueba independiente de la TS:

```bash
make test-ts
```

Esta prueba informa los resultados por consola.

### **Limpiar archivos generados**

Para eliminar los archivos generados durante la compilación y las pruebas:

```bash
make clean
```

Esto elimina las carpetas:

```bash
build/
Src/Test/Resultados/
```

---
***Universidad Nacional de Río Cuarto.***  
***Facultad de Ciencias Exactas, Físico-Químicas y Naturales.***  
***Departamento de Computación.***  

---

# ***Taller de Diseño de Software (Código 3306) - Año 2026***

*Repositorio para el desarrollo del Pre-Proyecto y Proyecto de la materia.*

## *Integrantes*

- ÁLVAREZ, Joel Facundo.
- VEGA, Matías Thomas.

## *Tecnologías y requisitos*

- GCC, para compilar programas de Lenguaje C.
- Flex, para el desarrollo del analizador léxico.
- Bison, para el desarrollo del analizador sintáctico.
- GNU Make, para automatizar la compilación.
- Graphviz, para la generación de representaciones gráficas.

## *Estructura del repositorio*

```text
.
├── Pre-Proyecto
│   ├── Docs
│   │   ├── Especificacion.md
│   │   └── Pre-Proyecto.pdf
│   ├── Makefile
│   └── Src
│       ├── AST
│       │   ├── AST.c
│       │   └── AST.h
│       ├── Lexer
│       │   └── AnalizadorLexico.lex
│       ├── Parser
│       │   └── AnalizadorSintactico.y
│       ├── Test
│       │   ├── Prueba1.txt
│       │   ├── Prueba2.txt
│       │   ├── Prueba3.txt
│       │   ├── Prueba4.txt
│       │   ├── Prueba5.txt
│       │   ├── Prueba6.txt
│       │   ├── Prueba7.txt
│       │   ├── Prueba8.txt
│       │   ├── Prueba9.txt
│       │   ├── Prueba10.txt
│       │   └── TestAST.c
│       └── main.c
├── .gitignore
└── README.md
```

Las carpetas `build/` y `Src/Test/Resultados` se generan automáticamente mediante make y se encuentran excluidas del repositorio.

---

# **- Pre-Proyecto**

El Pre-Proyecto consiste en el desarrollo progresivo de un pequeño lenguaje de programación y las herramientas necesarias para su análisis y procesamiento.

Las etapas previstas son:

1. Extensión de la gramática.
2. Definición de expresiones regulares.
3. Implementación de un analizador léxico.
4. Implementación de un analizador sintáctico.
5. Generación de un AST (Árbol Sintáctico Abstracto).
6. Implementación de un intérprete/evaluador para el lenguaje.
7. Generación de seudo-assembly.

## *1. Gramática*

La gramática fue extendida según los requerimientos del enunciado para incorporar las distintas características del lenguaje.

*Ver [Especificación del Pre-Proyecto](Pre-Proyecto/Docs/Especificacion.md#punto-1-gramática) para más detalles.*

## *2. Expresiones Regulares*

Las expresiones regulares fueron definidas para reconocer los distintos componentes léxicos del lenguaje, incluyendo identificadores, constantes, operadores, delimitadores y palabras reservadas.

*Ver [Especificación del Pre-Proyecto](Pre-Proyecto/Docs/Especificacion.md#punto-2-expresiones-regulares) para más detalles.*

## *3. Analizador Léxico*

El analizador léxico fue implementado utilizando la herramienta Flex, a partir de las expresiones regulares definidas para el lenguaje.

*Para revisar su implementación, ver [Analizador Léxico](Pre-Proyecto/Src/Lexer/AnalizadorLexico.lex).*

## *4. Analizador Sintáctico*

El analizador sintáctico fue implementado utilizando la herramienta Bison a partir de la gramática extendida. Se definieron las reglas sintácticas y las precedencias de operadores `'+'` y `'*'` para resolver ambigüedades en las expresiones.

*Para revisar su implementación, ver [Analizador Sintáctico](Pre-Proyecto/Src/Parser/AnalizadorSintactico.y).*

## *5. Árbol Sintáctico Abstracto (AST)*

Se incorporó la construcción de un Árbol Sintáctico Abstracto (AST) durante el análisis sintáctico. El árbol representa la estructura esencial del programa mediante nodos que corresponden a sus principales construcciones: programa, declaraciones, sentencias, asignaciones, retornos, expresiones, identificadores, números y valores booleanos.

El AST permite trabajar con una representación estructurada del programa, independiente del texto original y de los detalles propios de la gramática. Esto proporciona una base para las etapas posteriores, como el análisis semántico, la construcción de la tabla de símbolos, la verificación de tipos y la generación de código.

El módulo `AST.c` / `AST.h` proporciona operaciones para:

- Crear nodos y establecer relaciones entre ellos.
- Imprimir el árbol por consola.
- Generar una representación en formato DOT.
- Liberar la memoria utilizada por el árbol.

*Para revisar su implementación, ver [AST.h](Pre-Proyecto/Src/AST/AST.h) y [AST.c](Pre-Proyecto/Src/AST/AST.c).*

Los árboles de los programas aceptados pueden exportarse a formato DOT y convertirse a PNG mediante Graphviz. Los resultados se almacenan en `Src/Test/Resultados`, carpeta que no se encuentra versionada.

---

## *Programa principal*

Se incorporó `Src/main.c`, encargado de recibir un archivo fuente, ejecutar el análisis léxico y sintáctico y, si el programa es válido, mostrar y generar su AST.

El archivo de salida DOT puede especificarse como segundo argumento:

```bash
./build/Analizador archivo.txt archivo.dot
```

Los programas que presentan errores léxicos o sintácticos informan el error correspondiente y no generan un AST.

## *Pruebas*

Se incorporaron diez archivos de prueba (`Prueba1.txt` a `Prueba10.txt`) que incluyen tanto programas válidos como entradas con errores léxicos y sintácticos.

Las pruebas pueden ejecutarse conjuntamente mediante:

```bash
make ejemplos
```

Los programas válidos generan sus archivos `.dot` y `.png` en `Src/Test/Resultados/`.

También se mantiene `TestAST.c`, una prueba independiente que construye manualmente un AST para verificar el funcionamiento del módulo sin depender del lexer ni del parser. Se ejecuta mediante:

```bash
make test-ast
```

## *Compilación y Ejecución*

El `Makefile` permite centralizar la compilación y ejecución de las distintas herramientas y pruebas.

Desde la raíz del repositorio, ejecutar los siguientes comandos para compilar el analizador completo:

```bash
cd Pre-Proyecto
make
```

Los archivos generados se almacenan en la carpeta `build/`, que no se encuentra versionada.

Para ejecutar las pruebas y generar las representaciones gráficas de los AST válidos:

```bash
make ejemplos
```

Por su parte, para compilar y ejecutar la prueba independiente del AST:

```bash
make test-ast
```

Las representaciones generadas para todas las pruebas se almacenan en la carpeta `Src/Test/Resultados/`, que no se encuentra versionada.

Por último, para limpiar todos los archivos generados:

```bash
make clean
```

---
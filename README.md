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

- GCC para compilar programas de Lenguaje C.
- Flex.
- Bison.
- GNU Make.

## *Estructura del repositorio*

```text
.
├── Pre-Proyecto/
│   ├── Makefile
│   ├── Docs/
│   │   ├── Especificacion.md
│   │   └── Pre-Proyecto.pdf
│   └── Src/
│       ├── Lexer/
│       │   └── AnalizadorLexico.lex
│       ├── Parser/
│       │   └── AnalizadorSintactico.y
│       └── Test/
│           ├── Prueba1.txt
│           ├── Prueba2.txt
│           ├── Prueba3.txt
│           ├── Prueba4.txt
│           ├── Prueba5.txt
│           ├── Prueba6.txt
│           ├── Prueba7.txt
│           ├── Prueba8.txt
│           ├── Prueba9.txt
│           └── Prueba10.txt
├── .gitignore
└── README.md
```

La carpeta `build/` se genera automáticamente mediante make y se encuentra excluida del repositorio.

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

### **Compilación y Ejecución**

Desde la raíz del repositorio, ejecutar los siguientes comandos:

```bash
cd Pre-Proyecto
make
```

Los archivos generados se almacenan en la carpeta `build/`, que no se encuentra versionada.

Para ejecutar el analizador sintáctico con los archivos de prueba:

```bash
./build/AnalizadorSintactico Src/Test/Prueba1.txt
./build/AnalizadorSintactico Src/Test/Prueba2.txt
./build/AnalizadorSintactico Src/Test/Prueba3.txt
./build/AnalizadorSintactico Src/Test/Prueba4.txt
./build/AnalizadorSintactico Src/Test/Prueba5.txt
./build/AnalizadorSintactico Src/Test/Prueba6.txt
./build/AnalizadorSintactico Src/Test/Prueba7.txt
./build/AnalizadorSintactico Src/Test/Prueba8.txt
./build/AnalizadorSintactico Src/Test/Prueba9.txt
./build/AnalizadorSintactico Src/Test/Prueba10.txt
```

Por último, para limpiar los archivos generados:

```bash
make clean
```

---
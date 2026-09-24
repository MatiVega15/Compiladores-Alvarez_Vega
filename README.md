***Universidad Nacional de Río Cuarto.***  
***Facultad de Ciencias Exactas, Físico-Químicas y Naturales.***  
***Departamento de Computación.***  

---

# ***Taller de Diseño de Software (Código 3306) - Año 2026***

*Repositorio para el desarrollo del Pre-Proyecto y Proyecto de la materia.*

## *Integrantes*

- **ÁLVAREZ**, Joel Facundo.
- **VEGA**, Matías Thomas.

## *Tecnologías y requisitos*

- **GCC**, para compilar los programas desarrollados en el lenguaje C.
- **Flex**, para el desarrollo del analizador léxico.
- **Bison**, para el desarrollo del analizador sintáctico.
- **GNU Make**, para automatizar la compilación.
- **Graphviz**, para la generación de representaciones gráficas.

## *Estructura del repositorio*

```text
.
├── Pre-Proyecto/
│   ├── Docs/
│   │   ├── Documentacion.md
│   │   ├── Especificacion.md
│   │   └── Pre-Proyecto.pdf
│   ├── Makefile
│   └── Src/
│       ├── AST/
│       ├── Common/
│       ├── Generador/
│       ├── Interprete/
│       ├── Lexer/
│       ├── Parser/
│       ├── Semantico/
│       ├── TS/
│       ├── Test/
│       └── main.c
├── Proyecto/
│   ├── Docs/
│   │   ├── Documentacion.md
│   │   ├── Especificacion.md
│   │   ├── Proyecto - Descripción y Requisitos.pdf
│   │   └── Proyecto - Especificación del Lenguaje.pdf
│   ├── Makefile
│   └── Src/
│       ├── Lexer/
│       ├── Parser/
│       ├── Test/
│       │   ├── Lexico/
│       │   │   ├── Invalidas/
│       │   │   └── Validas/
│       │   └── Sintactico/
│       │       ├── Invalidas/
│       │       └── Validas/
│       └── main.c
├── .gitignore
└── README.md
```

Las carpetas `Pre-Proyecto/build/`, `Pre-Proyecto/Src/Test/Resultados/`, `Proyecto/build/` y `Proyecto/Src/Test/Resultados/` se generan durante la compilación y ejecución de las pruebas y se encuentran excluidas del repositorio.

---

# **Pre-Proyecto**

## *Descripción*

El Pre-Proyecto consiste en el **desarrollo progresivo de un pequeño lenguaje de programación** y las herramientas necesarias para su análisis y procesamiento.

Las etapas previstas son:

1. Extensión de la **gramática**.
2. Definición de **expresiones regulares**.
3. Implementación de un **analizador léxico**.
4. Implementación de un **analizador sintáctico**.
5. Generación de un **AST (Árbol Sintáctico Abstracto)**.
6. Implementación de la **Tabla de Símbolos (TS)**, **análisis semántico** e **intérprete**.
7. Generación de **seudo-assembly**.

## *Compilación y ejecución*

Para **compilar** el Pre-Proyecto, desde la raíz del repositorio:

```bash
cd Pre-Proyecto
make
```

Para ejecutar las **pruebas**:

```bash
make tests
```

Para **limpiar** los archivos generados:

```bash
make clean
```

## *Documentación*

***La documentación completa de cada etapa junto con información adicional de compilación y ejecución se encuentra en [Documentación del Pre-Proyecto](Pre-Proyecto/Docs/Documentacion.md).***

---

# **Proyecto**

## *Descripción*

El Proyecto consiste en el **diseño e implementación de un compilador para un lenguaje de programación simple, denominado C-TDS**, similar a C o Pascal.

El trabajo se aborda de una manera incremental, mediante las siguientes etapas:

1. Análisis **léxico** y **sintáctico**.
2. Generación del **árbol sintáctico abstracto (AST)** y la **tabla de símbolos (TS)**.
3. Análisis **semántico**.
4. Generación de **código intermedio**.
5. Generación de **código objeto**.
6. **Optimizador** y extensiones.

Actualmente se encuentran implementadas las etapas de **análisis léxico** y **análisis sintáctico**.

## *Compilación y ejecución*

Para **compilar el Proyecto**, desde la raíz del repositorio:

```bash
cd Proyecto
make
```

El **ejecutable** generado se encuentra en:

```text
Proyecto/build/c-tds
```

Para **ejecutar el compilador**:

```bash
./build/c-tds archivo.ctds
```

Por defecto, se ejecuta la última etapa implementada, actualmente el **análisis sintáctico**, generando un archivo `.sint`.

Las etapas también pueden seleccionarse mediante `-target`. Por ejemplo, para el **análisis léxico**:

```bash
./build/c-tds -target scan archivo.ctds
```

Para activar el **modo depuración**:

```bash
./build/c-tds -debug archivo.ctds
```

El modo de depuración muestra **información adicional** durante la etapa de análisis que se esté ejecutando.

El **objetivo `tests`** permite ejecutar todas las pruebas correspondientes a las etapas implementadas:

```bash
make tests
```

Actualmente, esto equivale a ejecutar las pruebas del **análisis léxico** y del **análisis sintáctico**.

Para **ejecutar solamente las pruebas del análisis léxico**:

```bash
make tests-lexico
```
Para **ejecutar solamente las pruebas del análisis sintáctico**:

```bash
make tests-sintactico
```

Para **limpiar** los archivos generados:

```bash
make clean
```

## *Documentación*

***La documentación completa de cada etapa junto con información detallada sobre implementación, interfaz de línea de comandos, pruebas, compilación y ejecución, se encuentra en [Documentación del Proyecto](Proyecto/Docs/Documentacion.md).***

---
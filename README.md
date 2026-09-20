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
│   ├── Docs/
│   │   ├── Documentacion.md
│   │   ├── Especificacion.md
│   │   └── Pre-Proyecto.pdf
│   ├── Makefile
│   └── Src/
│       ├── AST/
│       ├── Common/
│       ├── Generador/
│       ├── Interprete/
│       ├── Lexer/
│       ├── Parser/
│       ├── Semantico/
│       ├── TS/
│       ├── Test/
│       └── main.c
├── Proyecto/
│   └── Docs/
│       ├── Proyecto - Descripción y Requisitos.pdf
│       └── Proyecto - Especificación del Lenguaje.pdf
├── .gitignore
└── README.md
```

Las carpetas `Pre-Proyecto/build/` y `Pre-Proyecto/Src/Test/Resultados/` se generan durante la compilación y ejecución de las pruebas y se encuentran excluidas del repositorio.

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
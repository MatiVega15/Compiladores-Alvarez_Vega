---

# Especificación del Lenguaje

---

## Punto 1: Gramática

### - Gramática Original:

- `P` $\rightarrow$ E;
- `E` $\rightarrow$ E + E | E * E | (E) | nro

### - Gramática Extendida:

La gramática extendida se define como:

$$G = \langle V_N,\ V_T,\ R,\ S \rangle$$

donde:

- $V_N$: conjunto de símbolos no terminales:

```math
V_N = \{ P,\ TipoMain,\ Declaraciones,\ Decl,\ Tipo,\ Sentencias,\ Sent,\ E\}
```

- $V_T$: conjunto de símbolos terminales:

```math
V_T = \{int,\ bool,\ void,\ main,\ id,\ nro,\ true,\ false,\ +,\ *,\ =,\ (,\ ),\ \{,\ \},\ ;,\ return\}
```

- $S$: símbolo inicial:

$$S = P$$

- $R$: las reglas o producciones presentadas a continuación:

***Programa:***

- `P` $\rightarrow$ TipoMain main () {Declaraciones Sentencias}
- `TipoMain` $\rightarrow$ int | bool | void

Se establece que el lenguaje no permite la combinación arbitraria de declaraciones y asignaciones intercaladas; las declaraciones deben figurar obligatoriamente al inicio, antes de cualquier sentencia ejecutable.

***Declaraciones:***

- `Declaraciones` $\rightarrow$ Decl Declaraciones | Decl
- `Decl` $\rightarrow$ Tipo id;
- `Tipo` $\rightarrow$ int | bool

De acuerdo con la interpretación de la consigna, se exige al menos una declaración de variable al inicio del bloque.

***Sentencias:***

- `Sentencias` $\rightarrow$ Sent Sentencias | Sent
- `Sent` $\rightarrow$ id = E; | return E; | return;

Se asume que toda función contiene una secuencia de una o más sentencias.

***Expresiones:***

- `E` $\rightarrow$ E + E | E * E | (E) | nro | id | true | false

Se extienden las expresiones sin quitar la ambigüedad ni establecer la precedencia de la multiplicación sobre la suma. Este aspecto se resolverá posteriormente con la herramienta Bison, a la hora de desarrollar el analizador sintáctico.

---

## Punto 2: Expresiones Regulares

### - Palabras reservadas:

```math
"int" | "bool" | "void" | "main" | "return"
```

### - Variables o identificadores:

```math
[a-zA-Z]([a-zA-Z] | [0-9])^*
```

Se considera que el conjunto de letras comprende tanto las mayúsculas como las minúsculas.

### - Valores constantes:

```math
[0-9]^+ | "true" | "false"
```

Se permiten ceros a la izquierda en los valores enteros.

### - Operadores:

```math
"+" | "*" | "="
```

### - Delimitadores:

```math
"(" | ")" | "\{" | "\}" | ";"
```

---
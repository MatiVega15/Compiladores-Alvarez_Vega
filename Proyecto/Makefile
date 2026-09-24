# ========== Directorios ==========

BUILD_DIR = build

TEST_DIR = Src/Test
RESULTADOS_DIR = $(TEST_DIR)/Resultados

LEXICO_TEST_DIR = $(TEST_DIR)/Lexico
LEXICO_VALIDAS_DIR = $(LEXICO_TEST_DIR)/Validas
LEXICO_INVALIDAS_DIR = $(LEXICO_TEST_DIR)/Invalidas

RESULTADOS_LEXICO_DIR = $(RESULTADOS_DIR)/Lexico
RESULTADOS_LEXICO_VALIDAS_DIR = $(RESULTADOS_LEXICO_DIR)/Validas
RESULTADOS_LEXICO_INVALIDAS_DIR = $(RESULTADOS_LEXICO_DIR)/Invalidas

SINTACTICO_TEST_DIR = $(TEST_DIR)/Sintactico
SINTACTICO_VALIDAS_DIR = $(SINTACTICO_TEST_DIR)/Validas
SINTACTICO_INVALIDAS_DIR = $(SINTACTICO_TEST_DIR)/Invalidas

RESULTADOS_SINTACTICO_DIR = $(RESULTADOS_DIR)/Sintactico
RESULTADOS_SINTACTICO_VALIDAS_DIR = $(RESULTADOS_SINTACTICO_DIR)/Validas
RESULTADOS_SINTACTICO_INVALIDAS_DIR = $(RESULTADOS_SINTACTICO_DIR)/Invalidas

# ========== Archivos fuente ==========

LEXER_SRC = Src/Lexer/AnalizadorLexico.lex
PARSER_SRC = Src/Parser/AnalizadorSintactico.y
MAIN_SRC = Src/main.c

# ========== Archivos de cabecera ==========

PARSER_H = $(BUILD_DIR)/AnalizadorSintactico.tab.h

# ========== Archivos generados ==========

LEXER_C = $(BUILD_DIR)/lex.yy.c
PARSER_C = $(BUILD_DIR)/AnalizadorSintactico.tab.c

# ========== Ejecutables ==========

BIN = $(BUILD_DIR)/c-tds

# ========== Archivos de prueba ==========

PRUEBAS_LEXICO_VALIDAS = $(wildcard $(LEXICO_VALIDAS_DIR)/*.ctds)
PRUEBAS_LEXICO_INVALIDAS = $(wildcard $(LEXICO_INVALIDAS_DIR)/*.ctds)

PRUEBAS_SINTACTICO_VALIDAS = $(wildcard $(SINTACTICO_VALIDAS_DIR)/*.ctds)
PRUEBAS_SINTACTICO_INVALIDAS = $(wildcard $(SINTACTICO_INVALIDAS_DIR)/*.ctds)

# ========== Objetivos ==========

.PHONY: all tests tests-lexico tests-sintactico clean

# ========== Compilación del compilador ==========

# Compila el compilador C-TDS.
all: $(BIN)

# Enlazado final.
$(BIN): $(MAIN_SRC) $(PARSER_C) $(LEXER_C)
	@mkdir -p $(BUILD_DIR)
	@gcc -I$(BUILD_DIR) $(MAIN_SRC) $(PARSER_C) $(LEXER_C) -o $(BIN)

# ========== Generación del analizador sintáctico ==========

# Genera los archivos C y H del Parser con Bison.
$(PARSER_C): $(PARSER_SRC)
	@mkdir -p $(BUILD_DIR)
	@bison -d -o $(PARSER_C) $(PARSER_SRC)

# ========== Generación del analizador léxico ==========

# Genera el código C del Lexer con Flex.
$(LEXER_C): $(LEXER_SRC) $(PARSER_H)
	@mkdir -p $(BUILD_DIR)
	@flex -o $(LEXER_C) $(LEXER_SRC)

# ========== Ejecución de todas las pruebas ==========

# Ejecuta todas las pruebas disponibles.
tests: tests-lexico tests-sintactico
	@echo "========================================="
	@echo "      TODAS LAS PRUEBAS FINALIZADAS"
	@echo "========================================="
	@echo ""

# ========== Pruebas del análisis léxico ==========

# Ejecuta todas las pruebas del análisis léxico.
tests-lexico: $(BIN)
	@mkdir -p $(RESULTADOS_LEXICO_VALIDAS_DIR)
	@mkdir -p $(RESULTADOS_LEXICO_INVALIDAS_DIR)
	
	@fallidas=0; \
	ok=0; \
	echo ""; \
	echo "========================================="; \
	echo "       PRUEBAS DEL ANÁLISIS LÉXICO"; \
	echo "========================================="; \
	echo ""; \
	echo "========== PRUEBAS VÁLIDAS =========="; \
	echo ""; \
	for prueba in $(PRUEBAS_LEXICO_VALIDAS); do \
		nombre=$$(basename $$prueba .ctds); \
		salida=$(RESULTADOS_LEXICO_VALIDAS_DIR)/$$nombre.lex; \
		error=$(RESULTADOS_LEXICO_VALIDAS_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -target scan -o $$salida $$prueba \
			2> $$error; \
		resultado=$$?; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if [ $$resultado -eq 0 ]; then \
			echo "RESULTADO: OK"; \
			ok=$$((ok + 1)); \
		else \
			echo "RESULTADO: FALLIDA (se esperaba éxito)"; \
			fallidas=$$((fallidas + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "========== PRUEBAS INVÁLIDAS =========="; \
	echo ""; \
	for prueba in $(PRUEBAS_LEXICO_INVALIDAS); do \
		nombre=$$(basename $$prueba .ctds); \
		salida=$(RESULTADOS_LEXICO_INVALIDAS_DIR)/$$nombre.lex; \
		error=$(RESULTADOS_LEXICO_INVALIDAS_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -target scan -o $$salida $$prueba \
			2> $$error; \
		resultado=$$?; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if [ $$resultado -ne 0 ]; then \
			echo "RESULTADO: OK"; \
			ok=$$((ok + 1)); \
		else \
			echo "RESULTADO: FALLIDA (se esperaba error)"; \
			fallidas=$$((fallidas + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "======================"; \
	echo "Pruebas correctas: $$ok"; \
	echo "Pruebas fallidas: $$fallidas"; \
	echo "======================"; \
	echo "";

# ========== Pruebas del análisis sintáctico ==========

# Ejecuta todas las pruebas del análisis sintáctico.
tests-sintactico: $(BIN)
	@mkdir -p $(RESULTADOS_SINTACTICO_VALIDAS_DIR)
	@mkdir -p $(RESULTADOS_SINTACTICO_INVALIDAS_DIR)

	@fallidas=0; \
	ok=0; \
	echo ""; \
	echo "==========================================="; \
	echo "      PRUEBAS DEL ANÁLISIS SINTÁCTICO"; \
	echo "==========================================="; \
	echo ""; \
	echo "========== PRUEBAS VÁLIDAS =========="; \
	echo ""; \
	for prueba in $(PRUEBAS_SINTACTICO_VALIDAS); do \
		nombre=$$(basename $$prueba .ctds); \
		salida=$(RESULTADOS_SINTACTICO_VALIDAS_DIR)/$$nombre.sint; \
		error=$(RESULTADOS_SINTACTICO_VALIDAS_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -target parse -o $$salida $$prueba \
			2> $$error; \
		resultado=$$?; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if [ $$resultado -eq 0 ]; then \
			echo "RESULTADO: OK"; \
			ok=$$((ok + 1)); \
		else \
			echo "RESULTADO: FALLIDA (se esperaba éxito)"; \
			fallidas=$$((fallidas + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "========== PRUEBAS INVÁLIDAS =========="; \
	echo ""; \
	for prueba in $(PRUEBAS_SINTACTICO_INVALIDAS); do \
		nombre=$$(basename $$prueba .ctds); \
		salida=$(RESULTADOS_SINTACTICO_INVALIDAS_DIR)/$$nombre.sint; \
		error=$(RESULTADOS_SINTACTICO_INVALIDAS_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -target parse -o $$salida $$prueba \
			2> $$error; \
		resultado=$$?; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if [ $$resultado -ne 0 ]; then \
			echo "RESULTADO: OK"; \
			ok=$$((ok + 1)); \
		else \
			echo "RESULTADO: FALLIDA (se esperaba error)"; \
			fallidas=$$((fallidas + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "======================"; \
	echo "Pruebas correctas: $$ok"; \
	echo "Pruebas fallidas: $$fallidas"; \
	echo "======================"; \
	echo "";

# ========== Limpieza ==========

# Elimina todos los archivos generados.
clean:
	@rm -rf $(BUILD_DIR) $(RESULTADOS_DIR)
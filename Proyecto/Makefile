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

AST_TEST_DIR = $(TEST_DIR)/AST
RESULTADOS_AST_DIR = $(RESULTADOS_DIR)/AST

TS_TEST_DIR = $(TEST_DIR)/TS
RESULTADOS_TS_DIR = $(RESULTADOS_DIR)/TS

SEMANTICO_TEST_DIR = $(TEST_DIR)/Semantico
SEMANTICO_VALIDAS_DIR = $(SEMANTICO_TEST_DIR)/Validas
SEMANTICO_INVALIDAS_DIR = $(SEMANTICO_TEST_DIR)/Invalidas

RESULTADOS_SEMANTICO_DIR = $(RESULTADOS_DIR)/Semantico
RESULTADOS_SEMANTICO_VALIDAS_DIR = $(RESULTADOS_SEMANTICO_DIR)/Validas
RESULTADOS_SEMANTICO_INVALIDAS_DIR = $(RESULTADOS_SEMANTICO_DIR)/Invalidas

CODINT_TEST_DIR = $(TEST_DIR)/CodigoIntermedio
RESULTADOS_CODINT_DIR = $(RESULTADOS_DIR)/CodigoIntermedio

ASSEMBLY_TEST_DIR = $(TEST_DIR)/CodigoAssembly
RESULTADOS_ASSEMBLY_DIR = $(RESULTADOS_DIR)/CodigoAssembly

OPTIMIZADOR_TEST_DIR = $(TEST_DIR)/Optimizador
PLEGADO_CONSTANTES_TEST_DIR = $(OPTIMIZADOR_TEST_DIR)/PlegadoConstantes
CODIGO_MUERTO_TEST_DIR = $(OPTIMIZADOR_TEST_DIR)/CodigoMuerto
REUTILIZACION_OFFSETS_TEST_DIR = $(OPTIMIZADOR_TEST_DIR)/ReutilizacionOffsets
REDUCCION_FUERZA_TEST_DIR = $(OPTIMIZADOR_TEST_DIR)/ReduccionFuerza
INTEGRALES_TEST_DIR = $(OPTIMIZADOR_TEST_DIR)

RESULTADOS_OPTIMIZADOR_DIR = $(RESULTADOS_DIR)/Optimizador
RESULTADOS_PLEGADO_CONSTANTES_DIR = $(RESULTADOS_OPTIMIZADOR_DIR)/PlegadoConstantes
RESULTADOS_CODIGO_MUERTO_DIR = $(RESULTADOS_OPTIMIZADOR_DIR)/CodigoMuerto
RESULTADOS_REUTILIZACION_OFFSETS_DIR = $(RESULTADOS_OPTIMIZADOR_DIR)/ReutilizacionOffsets
RESULTADOS_REDUCCION_FUERZA_DIR = $(RESULTADOS_OPTIMIZADOR_DIR)/ReduccionFuerza
RESULTADOS_INTEGRALES_DIR = $(RESULTADOS_OPTIMIZADOR_DIR)

# ========== Archivos fuente ==========

LEXER_SRC = Src/Lexer/AnalizadorLexico.lex
PARSER_SRC = Src/Parser/AnalizadorSintactico.y

AST_SRC = Src/AST/AST.c
TS_SRC = Src/TS/TS.c
SEMANTICO_SRC = Src/Semantico/AnalizadorSemantico.c

CUADRUPLOS_SRC = Src/CodigoIntermedio/Cuadruplos.c
GENERADOR_CI_SRC = Src/CodigoIntermedio/GeneradorCodigoIntermedio.c
GENERADOR_ASS_SRC = Src/CodigoAssembly/GeneradorCodigoAssembly.c

OPTIMIZADOR_SRC = Src/Optimizador/OptimizadorAST.c

MAIN_SRC = Src/main.c
RUNTIME_SRC = Src/runtime.c

# ========== Archivos de cabecera ==========

PARSER_H = $(BUILD_DIR)/AnalizadorSintactico.tab.h

TIPOS_H = Src/Common/Tipos.h
AST_H = Src/AST/AST.h
TS_H = Src/TS/TS.h
SEMANTICO_H = Src/Semantico/AnalizadorSemantico.h

CUADRUPLOS_H = Src/CodigoIntermedio/Cuadruplos.h
GENERADOR_CI_H = Src/CodigoIntermedio/GeneradorCodigoIntermedio.h
GENERADOR_ASS_H = Src/CodigoAssembly/GeneradorCodigoAssembly.h

OPTIMIZADOR_H = Src/Optimizador/OptimizadorAST.h

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

PRUEBAS_AST = $(wildcard $(AST_TEST_DIR)/*.c)

PRUEBAS_TS = $(wildcard $(TS_TEST_DIR)/*.c)

PRUEBAS_SEMANTICO_VALIDAS = $(wildcard $(SEMANTICO_VALIDAS_DIR)/*.ctds)
PRUEBAS_SEMANTICO_INVALIDAS = $(wildcard $(SEMANTICO_INVALIDAS_DIR)/*.ctds)

PRUEBAS_CODINT = $(wildcard $(CODINT_TEST_DIR)/*.ctds)

PRUEBAS_ASSEMBLY = $(wildcard $(ASSEMBLY_TEST_DIR)/*.ctds)

PRUEBAS_PLEGADO_CONSTANTES = $(wildcard $(PLEGADO_CONSTANTES_TEST_DIR)/*.ctds)
PRUEBAS_CODIGO_MUERTO = $(wildcard $(CODIGO_MUERTO_TEST_DIR)/*.ctds)
PRUEBAS_REUTILIZACION_OFFSETS = $(wildcard $(REUTILIZACION_OFFSETS_TEST_DIR)/*.ctds)
PRUEBAS_REDUCCION_FUERZA = $(wildcard $(REDUCCION_FUERZA_TEST_DIR)/*.ctds)
PRUEBAS_INTEGRALES = $(wildcard $(INTEGRALES_TEST_DIR)/*.ctds)

# ========== Objetivos ==========

.PHONY: all tests tests-lexico tests-sintactico tests-ast tests-ts tests-semantico tests-codint tests-assembly tests-optimizador clean

# ========== Compilación del compilador ==========

# Compila el compilador C-TDS.
all: $(BIN)

# Enlazado final.
$(BIN): $(MAIN_SRC) $(RUNTIME_SRC) $(PARSER_C) \
		$(LEXER_C) $(AST_SRC) $(TS_SRC) \
		$(SEMANTICO_SRC) $(CUADRUPLOS_SRC) \
		$(GENERADOR_CI_SRC) $(GENERADOR_ASS_SRC) \
		$(OPTIMIZADOR_SRC) $(AST_H) $(TS_H) \
		$(SEMANTICO_H) $(CUADRUPLOS_H) \
		$(GENERADOR_CI_H) $(TIPOS_H) \
		$(GENERADOR_ASS_H) $(OPTIMIZADOR_H)

	@mkdir -p $(BUILD_DIR)
	@gcc -I$(BUILD_DIR) \
		-ISrc/AST \
		-ISrc/Common \
		-ISrc/TS \
		-ISrc/Semantico \
		-ISrc/CodigoIntermedio \
		-ISrc/CodigoAssembly \
		-ISrc/Optimizador \
		$(MAIN_SRC) \
		$(RUNTIME_SRC) \
		$(PARSER_C) \
		$(LEXER_C) \
		$(AST_SRC) \
		$(TS_SRC) \
		$(SEMANTICO_SRC) \
		$(CUADRUPLOS_SRC) \
		$(GENERADOR_CI_SRC) \
		$(GENERADOR_ASS_SRC) \
		$(OPTIMIZADOR_SRC) \
		-o $(BIN)

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
tests: tests-lexico tests-sintactico tests-ast tests-ts tests-semantico tests-codint tests-assembly tests-optimizador
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
		dot=$(RESULTADOS_SINTACTICO_VALIDAS_DIR)/$$nombre.dot; \
		png=$(RESULTADOS_SINTACTICO_VALIDAS_DIR)/$$nombre.png; \
		error=$(RESULTADOS_SINTACTICO_VALIDAS_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -debug -target parse -o $$salida $$prueba \
			> /dev/null \
			2> $$error; \
		resultado=$$?; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if [ $$resultado -eq 0 ]; then \
			if [ -f $$dot ]; then \
				dot -Tpng $$dot -o $$png; \
			fi; \
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

# ========== Pruebas del AST ==========

# Ejecuta todas las pruebas independientes del AST.
tests-ast:
	@mkdir -p $(BUILD_DIR) $(RESULTADOS_AST_DIR)

	@fallidas=0; \
	ok=0; \
	echo ""; \
	echo "========================================="; \
	echo "             PRUEBAS DEL AST"; \
	echo "========================================="; \
	echo ""; \
	for prueba in $(PRUEBAS_AST); do \
		nombre=$$(basename $$prueba .c); \
		ejecutable=$(BUILD_DIR)/$$nombre; \
		salida=$(RESULTADOS_AST_DIR)/$$nombre.txt; \
		dot=$(RESULTADOS_AST_DIR)/$$nombre.dot; \
		png=$(RESULTADOS_AST_DIR)/$$nombre.png; \
		echo "---- $$nombre ----"; \
		gcc -ISrc/AST -ISrc/Common $(AST_SRC) $$prueba -o $$ejecutable; \
		resultado=$$?; \
		if [ $$resultado -eq 0 ]; then \
			$$ejecutable > $$salida; \
			resultado=$$?; \
		fi; \
		if [ $$resultado -eq 0 ]; then \
			if [ -f $$dot ]; then \
				dot -Tpng $$dot -o $$png; \
			fi; \
			echo "RESULTADO: OK"; \
			ok=$$((ok + 1)); \
		else \
			echo "RESULTADO: FALLIDA"; \
			fallidas=$$((fallidas + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "======================"; \
	echo "Pruebas correctas: $$ok"; \
	echo "Pruebas fallidas: $$fallidas"; \
	echo "======================"; \
	echo "";

# ========== Pruebas de la tabla de símbolos ==========

# Ejecuta todas las pruebas independientes de la tabla de símbolos.
tests-ts:
	@mkdir -p $(BUILD_DIR) $(RESULTADOS_TS_DIR)

	@fallidas=0; \
	ok=0; \
	echo ""; \
	echo "============================================="; \
	echo "       PRUEBAS DE LA TABLA DE SÍMBOLOS"; \
	echo "============================================="; \
	echo ""; \
	for prueba in $(PRUEBAS_TS); do \
		nombre=$$(basename $$prueba .c); \
		ejecutable=$(BUILD_DIR)/$$nombre; \
		salida=$(RESULTADOS_TS_DIR)/$$nombre.txt; \
		echo "---- $$nombre ----"; \
		gcc -ISrc/TS -ISrc/Common $(TS_SRC) $$prueba -o $$ejecutable; \
		resultado=$$?; \
		if [ $$resultado -eq 0 ]; then \
			$$ejecutable > $$salida; \
			resultado=$$?; \
		fi; \
		if [ $$resultado -eq 0 ]; then \
			echo "RESULTADO: OK"; \
			ok=$$((ok + 1)); \
		else \
			echo "RESULTADO: FALLIDA"; \
			fallidas=$$((fallidas + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "======================"; \
	echo "Pruebas correctas: $$ok"; \
	echo "Pruebas fallidas: $$fallidas"; \
	echo "======================"; \
	echo "";

# ========== Pruebas del análisis semántico ==========

# Ejecuta todas las pruebas del análisis semántico.
tests-semantico: $(BIN)
	@mkdir -p $(RESULTADOS_SEMANTICO_VALIDAS_DIR)
	@mkdir -p $(RESULTADOS_SEMANTICO_INVALIDAS_DIR)

	@fallidas=0; \
	ok=0; \
	echo ""; \
	echo "==========================================="; \
	echo "      PRUEBAS DEL ANÁLISIS SEMÁNTICO"; \
	echo "==========================================="; \
	echo ""; \
	echo "========== PRUEBAS VÁLIDAS =========="; \
	echo ""; \
	for prueba in $(PRUEBAS_SEMANTICO_VALIDAS); do \
		nombre=$$(basename $$prueba .ctds); \
		salida=$(RESULTADOS_SEMANTICO_VALIDAS_DIR)/$$nombre.sem; \
		dot=$(RESULTADOS_SEMANTICO_VALIDAS_DIR)/$$nombre.dot; \
		png=$(RESULTADOS_SEMANTICO_VALIDAS_DIR)/$$nombre.png; \
		error=$(RESULTADOS_SEMANTICO_VALIDAS_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -debug -test-sem -o $$salida $$prueba \
			> /dev/null \
			2> $$error; \
		resultado=$$?; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if [ $$resultado -eq 0 ]; then \
			if [ -f $$dot ]; then \
				dot -Tpng $$dot -o $$png; \
			fi; \
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
	for prueba in $(PRUEBAS_SEMANTICO_INVALIDAS); do \
		nombre=$$(basename $$prueba .ctds); \
		salida=$(RESULTADOS_SEMANTICO_INVALIDAS_DIR)/$$nombre.sem; \
		dot=$(RESULTADOS_SEMANTICO_INVALIDAS_DIR)/$$nombre.dot; \
		png=$(RESULTADOS_SEMANTICO_INVALIDAS_DIR)/$$nombre.png; \
		error=$(RESULTADOS_SEMANTICO_INVALIDAS_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -debug -test-sem -o $$salida $$prueba \
			> /dev/null \
			2> $$error; \
		resultado=$$?; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if [ $$resultado -ne 0 ]; then \
			if [ -f $$dot ]; then \
				dot -Tpng $$dot -o $$png; \
			fi; \
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

# ========== Pruebas del generador de código intermedio ==========

# Ejecuta todas las pruebas del generador de código intermedio.
tests-codint: $(BIN)
	@mkdir -p $(BUILD_DIR) $(RESULTADOS_CODINT_DIR)

	@fallidas=0; \
	ok=0; \
	echo ""; \
	echo "==========================================="; \
	echo "       PRUEBAS DEL CÓDIGO INTERMEDIO"; \
	echo "==========================================="; \
	echo ""; \
	for prueba in $(PRUEBAS_CODINT); do \
		nombre=$$(basename $$prueba .ctds); \
		salida=$(RESULTADOS_CODINT_DIR)/$$nombre.ci; \
		error=$(RESULTADOS_CODINT_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -target codinter -o $$salida $$prueba \
			> /dev/null \
			2> $$error; \
		resultado=$$?; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if [ $$resultado -eq 0 ]; then \
			echo "RESULTADO: OK"; \
			ok=$$((ok + 1)); \
		else \
			echo "RESULTADO: FALLIDA"; \
			fallidas=$$((fallidas + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "======================"; \
	echo "Pruebas correctas: $$ok"; \
	echo "Pruebas fallidas: $$fallidas"; \
	echo "======================"; \
	echo ""

# ========== Pruebas de código assembly y ejecutables ==========

# Ejecuta todas las pruebas de la etapa de assembly y ejecución.
tests-assembly: $(BIN)
	@mkdir -p $(RESULTADOS_ASSEMBLY_DIR)
	
	@fallidas=0; \
	ok=0; \
	echo ""; \
	echo "=================================================="; \
	echo "     PRUEBAS DE CÓDIGO ASSEMBLY Y EJECUTABLES"; \
	echo "=================================================="; \
	echo ""; \
	for prueba in $(PRUEBAS_ASSEMBLY); do \
		nombre=$$(basename $$prueba .ctds); \
		archivo_ass=$(RESULTADOS_ASSEMBLY_DIR)/$$nombre.ass; \
		ejecutable=$(RESULTADOS_ASSEMBLY_DIR)/$$nombre.out; \
		salida_actual=$(RESULTADOS_ASSEMBLY_DIR)/$$nombre.actual; \
		archivo_esperado=$(ASSEMBLY_TEST_DIR)/$$nombre.expected; \
		archivo_in=$(ASSEMBLY_TEST_DIR)/$$nombre.in; \
		error=$(RESULTADOS_ASSEMBLY_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		\
		$(BIN) -target assembly -o $$archivo_ass $$prueba 2>> $$error; \
		\
		$(BIN) -o $$ejecutable $$prueba 2>> $$error; \
		resultado_compilacion=$$?; \
		\
		if [ $$resultado_compilacion -ne 0 ]; then \
			echo "RESULTADO: FALLIDA (error de compilación/ensamblado con GCC)"; \
			fallidas=$$((fallidas + 1)); \
			continue; \
		fi; \
		\
		if [ -f $$archivo_in ]; then \
			$$ejecutable < $$archivo_in > $$salida_actual; \
		else \
			$$ejecutable > $$salida_actual; \
		fi; \
		\
		if [ -f $$archivo_esperado ]; then \
			if diff -u --strip-trailing-cr $$salida_actual $$archivo_esperado > /dev/null; then \
				echo "RESULTADO: OK"; \
				ok=$$((ok + 1)); \
				rm -f $$error; \
			else \
				echo "RESULTADO: FALLIDA (la salida no coincide con el .expected)"; \
				diff -u --strip-trailing-cr $$salida_actual $$archivo_esperado; \
				fallidas=$$((fallidas + 1)); \
			fi; \
		else \
			echo "ADVERTENCIA: No se encontró archivo .expected para $$nombre"; \
			echo "RESULTADO: OK (compilación exitosa)"; \
			ok=$$((ok + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "======================"; \
	echo "Pruebas correctas: $$ok"; \
	echo "Pruebas fallidas: $$fallidas"; \
	echo "======================"; \
	echo "";

# ========== Pruebas de optimizaciones ==========

# Ejecuta todas las pruebas del optimizador (comparando versión normal y optimizada).
tests-optimizador: $(BIN)
	@mkdir -p $(RESULTADOS_PLEGADO_CONSTANTES_DIR)
	@mkdir -p $(RESULTADOS_CODIGO_MUERTO_DIR)
	@mkdir -p $(RESULTADOS_REUTILIZACION_OFFSETS_DIR)
	@mkdir -p $(RESULTADOS_REDUCCION_FUERZA_DIR)
	@mkdir -p $(RESULTADOS_INTEGRALES_DIR)

	@fallidas=0; \
	ok=0; \
	echo ""; \
	echo "==========================================="; \
	echo "          PRUEBAS DE OPTIMIZACIÓN"; \
	echo "==========================================="; \
	echo ""; \
	echo "========== PLEGADO DE CONSTANTES =========="; \
	echo ""; \
	for prueba in $(PRUEBAS_PLEGADO_CONSTANTES); do \
		nombre=$$(basename $$prueba .ctds); \
		salida_norm=$(RESULTADOS_PLEGADO_CONSTANTES_DIR)/$$nombre.ci; \
		dot_norm=$(RESULTADOS_PLEGADO_CONSTANTES_DIR)/$$nombre.dot; \
		png_norm=$(RESULTADOS_PLEGADO_CONSTANTES_DIR)/$$nombre.png; \
		salida_opt=$(RESULTADOS_PLEGADO_CONSTANTES_DIR)/$${nombre}_opt.ci; \
		dot_opt=$(RESULTADOS_PLEGADO_CONSTANTES_DIR)/$${nombre}_opt.dot; \
		png_opt=$(RESULTADOS_PLEGADO_CONSTANTES_DIR)/$${nombre}_opt.png; \
		error=$(RESULTADOS_PLEGADO_CONSTANTES_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -debug -target codinter -o $$salida_norm $$prueba > /dev/null 2> $$error; \
		res_norm=$$?; \
		if [ $$res_norm -eq 0 ] && [ -f $$dot_norm ]; then \
			dot -Tpng $$dot_norm -o $$png_norm 2> /dev/null; \
		fi; \
		$(BIN) -debug -opt plegado-constantes -target codinter -o $$salida_opt $$prueba > /dev/null 2>> $$error; \
		res_opt=$$?; \
		if [ $$res_opt -eq 0 ] && [ -f $$dot_opt ]; then \
			dot -Tpng $$dot_opt -o $$png_opt 2> /dev/null; \
		fi; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if [ $$res_norm -eq 0 ] && [ $$res_opt -eq 0 ]; then \
			echo "RESULTADO: OK"; \
			ok=$$((ok + 1)); \
		else \
			echo "RESULTADO: FALLIDA"; \
			fallidas=$$((fallidas + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "======= ELIMINACIÓN DE CÓDIGO MUERTO ======="; \
	echo ""; \
	for prueba in $(PRUEBAS_CODIGO_MUERTO); do \
		nombre=$$(basename $$prueba .ctds); \
		salida_norm=$(RESULTADOS_CODIGO_MUERTO_DIR)/$$nombre.ci; \
		dot_norm=$(RESULTADOS_CODIGO_MUERTO_DIR)/$$nombre.dot; \
		png_norm=$(RESULTADOS_CODIGO_MUERTO_DIR)/$$nombre.png; \
		salida_opt=$(RESULTADOS_CODIGO_MUERTO_DIR)/$${nombre}_opt.ci; \
		dot_opt=$(RESULTADOS_CODIGO_MUERTO_DIR)/$${nombre}_opt.dot; \
		png_opt=$(RESULTADOS_CODIGO_MUERTO_DIR)/$${nombre}_opt.png; \
		error=$(RESULTADOS_CODIGO_MUERTO_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -debug -target codinter -o $$salida_norm $$prueba > /dev/null 2> $$error; \
		res_norm=$$?; \
		if [ $$res_norm -eq 0 ] && [ -f $$dot_norm ]; then \
			dot -Tpng $$dot_norm -o $$png_norm 2> /dev/null; \
		fi; \
		$(BIN) -debug -opt codigo-muerto -target codinter -o $$salida_opt $$prueba > /dev/null 2>> $$error; \
		res_opt=$$?; \
		if [ $$res_opt -eq 0 ] && [ -f $$dot_opt ]; then \
			dot -Tpng $$dot_opt -o $$png_opt 2> /dev/null; \
		fi; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if [ $$res_norm -eq 0 ] && [ $$res_opt -eq 0 ]; then \
			echo "RESULTADO: OK"; \
			ok=$$((ok + 1)); \
		else \
			echo "RESULTADO: FALLIDA"; \
			fallidas=$$((fallidas + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "========= REUTILIZACIÓN DE OFFSETS ========="; \
	echo ""; \
	for prueba in $(PRUEBAS_REUTILIZACION_OFFSETS); do \
		nombre=$$(basename $$prueba .ctds); \
		salida_ass=$(RESULTADOS_REUTILIZACION_OFFSETS_DIR)/$$nombre.ass; \
		salida_ass_opt=$(RESULTADOS_REUTILIZACION_OFFSETS_DIR)/$${nombre}_opt.ass; \
		ejecutable=$(RESULTADOS_REUTILIZACION_OFFSETS_DIR)/$$nombre.out; \
		ejecutable_opt=$(RESULTADOS_REUTILIZACION_OFFSETS_DIR)/$${nombre}_opt.out; \
		salida_actual=$(RESULTADOS_REUTILIZACION_OFFSETS_DIR)/$$nombre.actual; \
		salida_actual_opt=$(RESULTADOS_REUTILIZACION_OFFSETS_DIR)/$${nombre}_opt.actual; \
		archivo_esperado=$(REUTILIZACION_OFFSETS_TEST_DIR)/$$nombre.expected; \
		archivo_in=$(REUTILIZACION_OFFSETS_TEST_DIR)/$$nombre.in; \
		error=$(RESULTADOS_REUTILIZACION_OFFSETS_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -target assembly -o $$salida_ass $$prueba 2> $$error; \
		res_ass=$$?; \
		$(BIN) -opt reutilizacion-offsets -target assembly -o $$salida_ass_opt $$prueba 2>> $$error; \
		res_ass_opt=$$?; \
		$(BIN) -o $$ejecutable $$prueba 2>> $$error; \
		res_bin=$$?; \
		$(BIN) -opt reutilizacion-offsets -o $$ejecutable_opt $$prueba 2>> $$error; \
		res_bin_opt=$$?; \
		if [ $$res_ass -ne 0 ] || [ $$res_ass_opt -ne 0 ] || [ $$res_bin -ne 0 ] || [ $$res_bin_opt -ne 0 ]; then \
			echo "RESULTADO: FALLIDA (error de compilación/ensamblado)"; \
			fallidas=$$((fallidas + 1)); \
			continue; \
		fi; \
		if [ -f $$archivo_in ]; then \
			$$ejecutable < $$archivo_in > $$salida_actual 2>> $$error; \
			$$ejecutable_opt < $$archivo_in > $$salida_actual_opt 2>> $$error; \
		else \
			$$ejecutable > $$salida_actual 2>> $$error; \
			$$ejecutable_opt > $$salida_actual_opt 2>> $$error; \
		fi; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if ! diff -u --strip-trailing-cr $$salida_actual $$salida_actual_opt > /dev/null; then \
			echo "RESULTADO: FALLIDA (salidas de ejecutable normal y optimizado difieren)"; \
			fallidas=$$((fallidas + 1)); \
			continue; \
		fi; \
		if [ -f $$archivo_esperado ]; then \
			if diff -u --strip-trailing-cr $$salida_actual $$archivo_esperado > /dev/null; then \
				echo "RESULTADO: OK"; \
				ok=$$((ok + 1)); \
			else \
				echo "RESULTADO: FALLIDA (la salida no coincide con el .expected)"; \
				diff -u --strip-trailing-cr $$salida_actual $$archivo_esperado; \
				fallidas=$$((fallidas + 1)); \
			fi; \
		else \
			echo "RESULTADO: OK (compilación y ejecuciones equivalentes)"; \
			ok=$$((ok + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "=========== REDUCCIÓN DE FUERZA ==========="; \
	echo ""; \
	for prueba in $(PRUEBAS_REDUCCION_FUERZA); do \
		nombre=$$(basename $$prueba .ctds); \
		salida_ass=$(RESULTADOS_REDUCCION_FUERZA_DIR)/$$nombre.ass; \
		salida_ass_opt=$(RESULTADOS_REDUCCION_FUERZA_DIR)/$${nombre}_opt.ass; \
		ejecutable=$(RESULTADOS_REDUCCION_FUERZA_DIR)/$$nombre.out; \
		ejecutable_opt=$(RESULTADOS_REDUCCION_FUERZA_DIR)/$${nombre}_opt.out; \
		salida_actual=$(RESULTADOS_REDUCCION_FUERZA_DIR)/$$nombre.actual; \
		salida_actual_opt=$(RESULTADOS_REDUCCION_FUERZA_DIR)/$${nombre}_opt.actual; \
		archivo_esperado=$(REDUCCION_FUERZA_TEST_DIR)/$$nombre.expected; \
		archivo_in=$(REDUCCION_FUERZA_TEST_DIR)/$$nombre.in; \
		error=$(RESULTADOS_REDUCCION_FUERZA_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -target assembly -o $$salida_ass $$prueba 2> $$error; \
		res_ass=$$?; \
		$(BIN) -opt reduccion-fuerza -target assembly -o $$salida_ass_opt $$prueba 2>> $$error; \
		res_ass_opt=$$?; \
		$(BIN) -o $$ejecutable $$prueba 2>> $$error; \
		res_bin=$$?; \
		$(BIN) -opt reduccion-fuerza -o $$ejecutable_opt $$prueba 2>> $$error; \
		res_bin_opt=$$?; \
		if [ $$res_ass -ne 0 ] || [ $$res_ass_opt -ne 0 ] || [ $$res_bin -ne 0 ] || [ $$res_bin_opt -ne 0 ]; then \
			echo "RESULTADO: FALLIDA (error de compilación/ensamblado)"; \
			fallidas=$$((fallidas + 1)); \
			continue; \
		fi; \
		if [ -f $$archivo_in ]; then \
			$$ejecutable < $$archivo_in > $$salida_actual 2>> $$error; \
			$$ejecutable_opt < $$archivo_in > $$salida_actual_opt 2>> $$error; \
		else \
			$$ejecutable > $$salida_actual 2>> $$error; \
			$$ejecutable_opt > $$salida_actual_opt 2>> $$error; \
		fi; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if ! diff -u --strip-trailing-cr $$salida_actual $$salida_actual_opt > /dev/null; then \
			echo "RESULTADO: FALLIDA (salidas de ejecutable normal y optimizado difieren)"; \
			fallidas=$$((fallidas + 1)); \
			continue; \
		fi; \
		if [ -f $$archivo_esperado ]; then \
			if diff -u --strip-trailing-cr $$salida_actual $$archivo_esperado > /dev/null; then \
				echo "RESULTADO: OK"; \
				ok=$$((ok + 1)); \
			else \
				echo "RESULTADO: FALLIDA (la salida no coincide con el .expected)"; \
				diff -u --strip-trailing-cr $$salida_actual $$archivo_esperado; \
				fallidas=$$((fallidas + 1)); \
			fi; \
		else \
			echo "RESULTADO: OK (compilación y ejecuciones equivalentes)"; \
			ok=$$((ok + 1)); \
		fi; \
		echo ""; \
	done; \
	echo "=========== PRUEBAS INTEGRALES ==========="; \
	echo ""; \
	for prueba in $(PRUEBAS_INTEGRALES); do \
		nombre=$$(basename $$prueba .ctds); \
		salida_ass=$(RESULTADOS_INTEGRALES_DIR)/$$nombre.ass; \
		salida_ass_opt=$(RESULTADOS_INTEGRALES_DIR)/$${nombre}_opt.ass; \
		ejecutable=$(RESULTADOS_INTEGRALES_DIR)/$$nombre.out; \
		ejecutable_opt=$(RESULTADOS_INTEGRALES_DIR)/$${nombre}_opt.out; \
		salida_actual=$(RESULTADOS_INTEGRALES_DIR)/$$nombre.actual; \
		salida_actual_opt=$(RESULTADOS_INTEGRALES_DIR)/$${nombre}_opt.actual; \
		archivo_esperado=$(INTEGRALES_TEST_DIR)/$$nombre.expected; \
		error=$(RESULTADOS_INTEGRALES_DIR)/$$nombre.err; \
		echo "---- $$nombre ----"; \
		$(BIN) -target assembly -o $$salida_ass $$prueba 2> $$error; \
		res_ass=$$?; \
		$(BIN) -opt all -target assembly -o $$salida_ass_opt $$prueba 2>> $$error; \
		res_ass_opt=$$?; \
		$(BIN) -o $$ejecutable $$prueba 2>> $$error; \
		res_bin=$$?; \
		$(BIN) -opt all -o $$ejecutable_opt $$prueba 2>> $$error; \
		res_bin_opt=$$?; \
		if [ $$res_ass -ne 0 ] || [ $$res_ass_opt -ne 0 ] || [ $$res_bin -ne 0 ] || [ $$res_bin_opt -ne 0 ]; then \
			echo "RESULTADO: FALLIDA (error de compilación/ensamblado)"; \
			fallidas=$$((fallidas + 1)); \
			continue; \
		fi; \
		$$ejecutable > $$salida_actual 2>> $$error; \
		$$ejecutable_opt > $$salida_actual_opt 2>> $$error; \
		if [ ! -s $$error ]; then \
			rm -f $$error; \
		fi; \
		if ! diff -u --strip-trailing-cr $$salida_actual $$salida_actual_opt > /dev/null; then \
			echo "RESULTADO: FALLIDA (salidas de ejecutable normal y con -opt all difieren)"; \
			fallidas=$$((fallidas + 1)); \
			continue; \
		fi; \
		if [ -f $$archivo_esperado ]; then \
			if diff -u --strip-trailing-cr $$salida_actual $$archivo_esperado > /dev/null; then \
				echo "RESULTADO: OK"; \
				ok=$$((ok + 1)); \
			else \
				echo "RESULTADO: FALLIDA (la salida no coincide con el .expected)"; \
				diff -u --strip-trailing-cr $$salida_actual $$archivo_esperado; \
				fallidas=$$((fallidas + 1)); \
			fi; \
		else \
			echo "RESULTADO: OK (compilación y ejecuciones equivalentes)"; \
			ok=$$((ok + 1)); \
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
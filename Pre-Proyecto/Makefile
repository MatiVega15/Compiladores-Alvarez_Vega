# ========== Directorios ==========

BUILD_DIR = build
TEST_DIR = Src/Test
RESULTADOS_DIR = $(TEST_DIR)/Resultados

# ========== Archivos fuente ==========

LEXER_SRC = Src/Lexer/AnalizadorLexico.lex
PARSER_SRC = Src/Parser/AnalizadorSintactico.y
AST_SRC = Src/AST/AST.c
TS_SRC = Src/TS/TS.c
SEMANTICO_SRC = Src/Semantico/AnalizadorSemantico.c
INTERPRETE_SRC = Src/Interprete/Interprete.c
GENERADOR_SRC = Src/Generador/GeneradorPseudoAssembly.c
MAIN_SRC = Src/main.c

TEST_AST_SRC = $(TEST_DIR)/TestAST.c
TEST_TS_SRC = $(TEST_DIR)/TestTS.c

# ========== Archivos de cabecera ==========

TIPOS_H = Src/Common/Tipos.h
SEMANTICO_H = Src/Semantico/AnalizadorSemantico.h
INTERPRETE_H = Src/Interprete/Interprete.h
GENERADOR_H = Src/Generador/GeneradorPseudoAssembly.h

# ========== Archivos generados y ejecutables ==========

PARSER_C = $(BUILD_DIR)/AnalizadorSintactico.tab.c
PARSER_H = $(BUILD_DIR)/AnalizadorSintactico.tab.h
LEXER_C = $(BUILD_DIR)/lex.yy.c

BIN = $(BUILD_DIR)/Interprete
TEST_AST_BIN = $(BUILD_DIR)/TestAST
TEST_TS_BIN = $(BUILD_DIR)/TestTS

# ========== Archivos de prueba ==========

PRUEBAS = 	$(TEST_DIR)/Prueba1_Valida_AsignacionesYExpresiones.txt \
			$(TEST_DIR)/Prueba2_Invalida_Interpretacion_VariableSinInicializar.txt \
			$(TEST_DIR)/Prueba3_Invalida_Lexica_CaracteresNoReconocidos.txt \
			$(TEST_DIR)/Prueba4_Invalida_Lexica_OperadorNoPermitido.txt \
			$(TEST_DIR)/Prueba5_Invalida_Lexica_IdentificadorConGuionBajo.txt \
			$(TEST_DIR)/Prueba6_Invalida_Lexica_ConstanteNoEntera.txt \
			$(TEST_DIR)/Prueba7_Invalida_Sintactica_IdentificadorInvalido.txt \
			$(TEST_DIR)/Prueba8_Invalida_Sintactica_FaltaPuntoYComa.txt \
			$(TEST_DIR)/Prueba9_Invalida_Sintactica_DeclaracionDespuesDeSentencia.txt \
			$(TEST_DIR)/Prueba10_Invalida_Sintactica_AsignacionYReturnInvalidos.txt \
			$(TEST_DIR)/Prueba11_Invalida_Sintactica_ParentesisSinCerrar.txt \
			$(TEST_DIR)/Prueba12_Invalida_Sintactica_SinDeclaraciones.txt \
			$(TEST_DIR)/Prueba13_Invalida_Sintactica_BloqueVacio.txt \
			$(TEST_DIR)/Prueba14_Invalida_Sintactica_SinSentencias.txt \
			$(TEST_DIR)/Prueba15_Invalida_Semantica_VariableNoDeclarada.txt \
			$(TEST_DIR)/Prueba16_Invalida_Semantica_DeclaracionDuplicada.txt \
			$(TEST_DIR)/Prueba17_Invalida_Semantica_AsignacionBoolAInt.txt \
			$(TEST_DIR)/Prueba18_Invalida_Semantica_AsignacionIntABool.txt \
			$(TEST_DIR)/Prueba19_Invalida_Semantica_SumaConBool.txt \
			$(TEST_DIR)/Prueba20_Invalida_Semantica_MultiplicacionConBool.txt \
			$(TEST_DIR)/Prueba21_Invalida_Semantica_ReturnBoolEnMainInt.txt \
			$(TEST_DIR)/Prueba22_Invalida_Semantica_ReturnIntEnMainBool.txt \
			$(TEST_DIR)/Prueba23_Invalida_Semantica_ReturnValorEnMainVoid.txt \
			$(TEST_DIR)/Prueba24_Invalida_Semantica_ReturnVacioEnMainInt.txt \
			$(TEST_DIR)/Prueba25_Invalida_Semantica_MultiplesErrores.txt \
			$(TEST_DIR)/Prueba26_Valida_ExpresionesAritmeticas.txt \
			$(TEST_DIR)/Prueba27_Valida_PrecedenciaYParentesis.txt \
			$(TEST_DIR)/Prueba28_Valida_VariablesBooleanasYReturn.txt \
			$(TEST_DIR)/Prueba29_Valida_ReasignacionesYDependencias.txt \
			$(TEST_DIR)/Prueba30_Valida_ProgramaCompleto.txt \
			$(TEST_DIR)/Prueba31_Valida_ReturnDetieneEjecucion.txt \
			$(TEST_DIR)/Prueba32_Valida_ReturnVacio.txt \
			$(TEST_DIR)/Prueba33_Valida_ReturnFalse.txt \
			$(TEST_DIR)/Prueba34_Valida_ReturnExpresionCompleja.txt \
			$(TEST_DIR)/Prueba35_Valida_ReasignacionAntesDelReturn.txt \
			$(TEST_DIR)/Prueba36_Invalida_Interpretacion_VariableSinInicializarEnExpresion.txt

# ========== Objetivos ==========

.PHONY: all tests prueba test-ast test-ts clean

# ========== Compilación del Intérprete ==========

# Compila el Intérprete.
all: $(BIN)

# Enlazado final.
$(BIN): $(PARSER_C) $(PARSER_H) $(LEXER_C) $(AST_SRC) $(TS_SRC) $(SEMANTICO_SRC) $(SEMANTICO_H) $(INTERPRETE_SRC) $(INTERPRETE_H) $(GENERADOR_SRC) $(GENERADOR_H) $(MAIN_SRC) $(TIPOS_H)
	@mkdir -p $(BUILD_DIR)
	@gcc -I$(BUILD_DIR) -ISrc/AST -ISrc/TS -ISrc/Common -ISrc/Semantico -ISrc/Interprete -ISrc/Generador $(PARSER_C) $(LEXER_C) $(AST_SRC) $(TS_SRC) $(SEMANTICO_SRC) $(INTERPRETE_SRC) $(GENERADOR_SRC) $(MAIN_SRC) -o $(BIN)

# ========== Generación del Parser ==========

# Genera los archivos C y H del Parser con Bison.
$(PARSER_C) $(PARSER_H): $(PARSER_SRC)
	@mkdir -p $(BUILD_DIR)
	@bison -d -o $(PARSER_C) $(PARSER_SRC)

# ========== Generación del Lexer ==========

# Genera el código C del Lexer con Flex.
$(LEXER_C): $(LEXER_SRC) $(PARSER_H)
	@mkdir -p $(BUILD_DIR)
	@flex -o $(LEXER_C) $(LEXER_SRC)

# ========== Ejecución de todas las pruebas ==========

# Ejecuta todas las pruebas.
tests: $(BIN)
	@mkdir -p $(RESULTADOS_DIR)
	@for prueba in $(PRUEBAS); do \
		nombre=$$(basename $$prueba .txt); \
		echo ""; \
		echo "\n========== $$nombre ==========\n\n"; \
		$(BIN) $$prueba $(RESULTADOS_DIR)/$$nombre.dot $(RESULTADOS_DIR)/$$nombre.asm; \
		if [ -f $(RESULTADOS_DIR)/$$nombre.dot ]; then \
			dot -Tpng $(RESULTADOS_DIR)/$$nombre.dot -o $(RESULTADOS_DIR)/$$nombre.png; \
		fi; \
	done

# ========== Ejecución de prueba individual ==========

# Ejecuta una única prueba.
prueba: $(BIN)
	@mkdir -p $(RESULTADOS_DIR)
	@echo ""
	@echo "\n========== $(PRUEBA) ==========\n\n"
	@$(BIN) $(TEST_DIR)/$(PRUEBA).txt $(RESULTADOS_DIR)/$(PRUEBA).dot $(RESULTADOS_DIR)/$(PRUEBA).asm; \
	resultado=$$?; \
	if [ -f $(RESULTADOS_DIR)/$(PRUEBA).dot ]; then \
		dot -Tpng $(RESULTADOS_DIR)/$(PRUEBA).dot -o $(RESULTADOS_DIR)/$(PRUEBA).png; \
	fi; \

# ========== Prueba del AST ==========

# Compila y ejecuta la prueba independiente del AST.
test-ast: $(AST_SRC) $(TEST_AST_SRC) $(TIPOS_H)
	@mkdir -p $(BUILD_DIR) $(RESULTADOS_DIR)
	@gcc -ISrc/AST -ISrc/Common $(AST_SRC) $(TEST_AST_SRC) -o $(TEST_AST_BIN)
	@$(TEST_AST_BIN)
	@dot -Tpng $(RESULTADOS_DIR)/TestAST.dot -o $(RESULTADOS_DIR)/TestAST.png
	@echo "PNG generado: $(RESULTADOS_DIR)/TestAST.png"

# ========== Prueba de la Tabla de Símbolos ==========

# Compila y ejecuta la prueba independiente de la Tabla de Símbolos.
test-ts: $(TS_SRC) $(TEST_TS_SRC) $(TIPOS_H)
	@mkdir -p $(BUILD_DIR)
	@gcc -ISrc/TS -ISrc/Common $(TS_SRC) $(TEST_TS_SRC) -o $(TEST_TS_BIN)
	@$(TEST_TS_BIN)

# ========== Limpieza ==========

# Elimina todos los archivos generados.
clean:
	@rm -rf $(BUILD_DIR) $(RESULTADOS_DIR)
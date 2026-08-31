BUILD_DIR = build
TEST_DIR = Src/Test
RESULTADOS_DIR = $(TEST_DIR)/Resultados

LEXER_SRC = Src/Lexer/AnalizadorLexico.lex
PARSER_SRC = Src/Parser/AnalizadorSintactico.y
AST_SRC = Src/AST/AST.c
MAIN_SRC = Src/main.c
TEST_AST_SRC = $(TEST_DIR)/TestAST.c

PARSER_C = $(BUILD_DIR)/AnalizadorSintactico.tab.c
PARSER_H = $(BUILD_DIR)/AnalizadorSintactico.tab.h
LEXER_C = $(BUILD_DIR)/lex.yy.c
BIN = $(BUILD_DIR)/Analizador
TEST_AST_BIN = $(BUILD_DIR)/TestAST

PRUEBAS = 	$(TEST_DIR)/Prueba1.txt \
			$(TEST_DIR)/Prueba2.txt \
			$(TEST_DIR)/Prueba3.txt \
			$(TEST_DIR)/Prueba4.txt \
			$(TEST_DIR)/Prueba5.txt \
			$(TEST_DIR)/Prueba6.txt \
			$(TEST_DIR)/Prueba7.txt \
			$(TEST_DIR)/Prueba8.txt \
			$(TEST_DIR)/Prueba9.txt \
			$(TEST_DIR)/Prueba10.txt

.PHONY: all ejemplos test-ast clean

# Compila el analizador.
all: $(BIN)

# Enlazado final con la carpeta build en el include path (-I).
$(BIN): $(PARSER_C) $(PARSER_H) $(LEXER_C) $(AST_SRC) $(MAIN_SRC)
	@mkdir -p $(BUILD_DIR)
	@gcc -I$(BUILD_DIR) -ISrc/AST $(PARSER_C) $(LEXER_C) $(AST_SRC) $(MAIN_SRC) -o $(BIN)

# Genera los archivos C y H del Parser con Bison.
$(PARSER_C) $(PARSER_H): $(PARSER_SRC)
	@mkdir -p $(BUILD_DIR)
	@bison -d -o $(PARSER_C) $(PARSER_SRC)

# Genera el código C del Lexer con Flex.
$(LEXER_C): $(LEXER_SRC) $(PARSER_H)
	@mkdir -p $(BUILD_DIR)
	@flex -o $(LEXER_C) $(LEXER_SRC)

# Ejecuta todas las pruebas.
ejemplos: $(BIN)
	@mkdir -p $(RESULTADOS_DIR)
	@for prueba in $(PRUEBAS); do \
		nombre=$$(basename $$prueba .txt); \
		echo ""; \
		echo "========== $$nombre =========="; \
		if $(BIN) $$prueba $(RESULTADOS_DIR)/$$nombre.dot; then \
			dot -Tpng $(RESULTADOS_DIR)/$$nombre.dot -o $(RESULTADOS_DIR)/$$nombre.png; \
			echo "PNG generado: $(RESULTADOS_DIR)/$$nombre.png"; \
		fi; \
	done

# Compila y ejecuta la prueba independiente del AST.
test-ast: $(AST_SRC) $(TEST_AST_SRC)
	@mkdir -p $(BUILD_DIR) $(RESULTADOS_DIR)
	@gcc -ISrc/AST $(AST_SRC) $(TEST_AST_SRC) -o $(TEST_AST_BIN)
	@$(TEST_AST_BIN)
	@dot -Tpng $(RESULTADOS_DIR)/TestAST.dot -o $(RESULTADOS_DIR)/TestAST.png
	@echo "PNG generado: $(RESULTADOS_DIR)/TestAST.png"

# Elimina todos los archivos generados.
clean:
	@rm -rf $(BUILD_DIR) $(RESULTADOS_DIR)
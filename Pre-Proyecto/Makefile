BUILD_DIR = build
LEXER_SRC = Src/Lexer/AnalizadorLexico.lex
PARSER_SRC = Src/Parser/AnalizadorSintactico.y

PARSER_C = $(BUILD_DIR)/AnalizadorSintactico.tab.c
PARSER_H = $(BUILD_DIR)/AnalizadorSintactico.tab.h
LEXER_C = $(BUILD_DIR)/lex.yy.c
BIN = $(BUILD_DIR)/AnalizadorSintactico

.PHONY: all clean

all: $(BIN)

# Enlazado final con la carpeta build en el include path (-I).
$(BIN): $(PARSER_C) $(PARSER_H) $(LEXER_C)
	gcc -I$(BUILD_DIR) $(PARSER_C) $(LEXER_C) -o $(BIN)

# Genera los archivos C y H del Parser con Bison.
$(PARSER_C) $(PARSER_H): $(PARSER_SRC)
	mkdir -p $(BUILD_DIR)
	bison -d -o $(PARSER_C) $(PARSER_SRC)

# Genera el código C del Lexer con Flex.
$(LEXER_C): $(LEXER_SRC) $(PARSER_H)
	mkdir -p $(BUILD_DIR)
	flex -o $(LEXER_C) $(LEXER_SRC)

clean:
	rm -rf $(BUILD_DIR)
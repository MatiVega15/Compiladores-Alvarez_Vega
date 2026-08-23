BUILD_DIR = build
LEXER_SRC = Src/Lexer/AnalizadorLexico.lex
LEXER_C = $(BUILD_DIR)/lex.yy.c
LEXER_BIN = $(BUILD_DIR)/AnalizadorLexico

.PHONY: all clean

all: $(LEXER_BIN)

$(LEXER_BIN): $(LEXER_C)
	gcc $(LEXER_C) -o $(LEXER_BIN)

$(LEXER_C): $(LEXER_SRC)
	mkdir -p $(BUILD_DIR)
	flex -o $(LEXER_C) $(LEXER_SRC)

clean:
	rm -rf $(BUILD_DIR)
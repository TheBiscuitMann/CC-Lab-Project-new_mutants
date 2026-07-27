CC	= gcc
LEXER	= src/lexer/lexer.l
PARSER	= src/parser/parser.y
AST	= src/ast/ast.c
SYMTAB	= src/symbol_table/symtab.c
SEMANTIC	= src/semantic/semantic.c
TARGET	= compiler

all: $(TARGET)

parser.tab.c parser.tab.h: $(PARSER)
	bison -d $(PARSER)

lex.yy.c: $(LEXER) parser.tab.h
	flex $(LEXER)

$(TARGET): parser.tab.c lex.yy.c $(AST) $(SYMTAB) $(SEMANTIC)
	$(CC) parser.tab.c lex.yy.c $(AST) $(SYMTAB) $(SEMANTIC) -o $(TARGET)

clean:
	rm -f parser.tab.c parser.tab.h lex.yy.c $(TARGET)

.PHONY: all clean

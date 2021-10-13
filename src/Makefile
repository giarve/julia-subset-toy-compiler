CODE_DIR = src

FILES = Lexer.c Parser.c main.c

CC = gcc
CFLAGS = -std=c17 -Wall -g
LIB = -lfl

LFLAGS = --debug
YFLAGS = --debug

COMPILER_NAME = minijuliac

$(COMPILER_NAME): $(FILES)
	$(CC) $(CFLAGS) $(LIB) $(FILES) -o $(COMPILER_NAME) 

Lexer.c: Lexer.l
	flex $(LFLAGS) Lexer.l

Parser.c: Parser.y Lexer.c
	bison $(YFLAGS) Parser.y

examples: $(COMPILER_NAME)
	./$(COMPILER_NAME) example.jl example.out.txt

clean:
	rm -f *.o *~ {Lexer,Parser}.{c,h} $(COMPILER_NAME)
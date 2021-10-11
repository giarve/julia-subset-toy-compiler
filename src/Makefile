CODE_DIR = src

FILES = Lexer.c Parser.c main.c

CC = gcc
CFLAGS = -std=c17 -Wall -g
LIB = -lfl

LFLAGS = 
YFLAGS = -d

COMPILER_NAME = compiler

$(COMPILER_NAME): $(FILES)
	$(CC) $(CFLAGS) $(LIB) $(FILES) -o $(COMPILER_NAME) 

Lexer.c: Lexer.l
	flex Lexer.l $(LFLAGS)

Parser.c: Parser.y Lexer.c
	bison Parser.y $(YFLAGS)

examples: $(COMPILER_NAME)
	./$(COMPILER_NAME) example.jl

clean:
	rm -f *.o *~ {Lexer,Parser}.{c,h} $(COMPILER_NAME)
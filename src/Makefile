CODE_DIR = src

FILES = Lexer.c Parser.c main.c

CC = gcc
CFLAGS = -std=c17 -Wall -g
LIB = -lfl

LFLAGS = 
YFLAGS = -d

OUTPUT_BINARY_NAME = test

$(OUTPUT_BINARY_NAME): $(FILES)
	$(CC) $(CFLAGS) $(LIB) $(FILES) -o $(OUTPUT_BINARY_NAME) 

Lexer.c: Lexer.l
	flex Lexer.l $(LFLAGS)

Parser.c: Parser.y Lexer.c
	bison Parser.y $(YFLAGS)

clean:
	rm -f *.o *~ {Lexer,Parser}.{c,h} $(OUTPUT_BINARY_NAME)
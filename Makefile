CC=gcc
CFLAGS=-Wall -Wextra -std=c18
CDFLAGS=-DDEBUG -g3 -O0
CRFLAGS=-O2 -DNDEBUG
SRC=src
BIN=bin
OBJ=$(BIN)/obj
LIB=lib
LIBS=$(LIB)/sv/sv.o
INC=$(LIB)/
EXEC=$(BIN)/aesthetic

all: debug

debug: CFLAGS += $(CDFLAGS)
debug: EXEC = $(BIN)/aesthetic-debug
debug: compiler

release: CFLAGS += $(CRFLAGS)
release: compiler

compiler: $(SRC)/aesthetic.c lexer token sv
	$(CC) $(CFLAGS) -I$(INC) -o $(EXEC) $< $(OBJ)/token.o $(OBJ)/lexer.o $(LIBS)

lexer: $(SRC)/lexer.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $(OBJ)/lexer.o

token: $(SRC)/token.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $(OBJ)/token.o

sv: $(LIB)/sv/sv.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $(LIB)/sv/sv.o

clean:
	rm $(OBJ)/*.o bin/aesthetic*

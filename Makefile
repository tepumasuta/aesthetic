CC=gcc
CFLAGS=-Wall -Wextra -std=c18
CDFLAGS=-g -O0
CRFLAGS=-O2 -DNDEBUG
SRC=src
BIN=bin
OBJ=$(BIN)/obj


compiler: $(SRC)/aesthetic.c token
	$(CC) $(CFLAGS) $(CDFLAGS) $< -o $(BIN)/aesthetic $(OBJ)/token.o

token: $(SRC)/token.c
	$(CC) $(CFLAGS) $(CDFLAGS) -c $< -o $(OBJ)/token.o

clean:
	rm $(OBJ)/*.o bin/aesthetic

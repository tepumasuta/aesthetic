CC=gcc
CFLAGS=-Wall -Wextra -std=c18
CDFLAGS=-g -O0
SRC=src
BIN=bin
OBJ=$(BIN)/obj


compiler: $(SRC)/aesthetic.c
	$(CC) $(CFLAGS) $(CDFLAGS) $< -o $(BIN)/aesthetic

$(OBJ):
	mkdir $(OBJ)

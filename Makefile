CC=gcc
CFLAGS=-Wall -Wextra -std=c18
CDFLAGS=-g -O0
CRFLAGS=-O2 -DNDEBUG
SRC=src
BIN=bin
OBJ=$(BIN)/obj
LIB=lib
LIBS=$(LIB)/sv/sv.o
INC=$(LIB)/

compiler: $(SRC)/aesthetic.c token sv
	$(CC) $(CFLAGS) $(CDFLAGS) -I$(INC) -o $(BIN)/aesthetic $< $(OBJ)/token.o $(LIBS)

token: $(SRC)/token.c
	$(CC) $(CFLAGS) $(CDFLAGS) -I$(INC) -c $< -o $(OBJ)/token.o

sv: $(LIB)/sv/sv.c
	$(CC) $(CFLAGS) $(CRFLAGS) -I$(INC) -c $< -o $(LIB)/sv/sv.o

clean:
	rm $(OBJ)/*.o bin/aesthetic

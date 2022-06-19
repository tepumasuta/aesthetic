CC=gcc
CFLAGS=-Wall -Wextra -std=c18
CDFLAGS=-DDEBUG -DAE_DEBUG -g3 -O0
CRFLAGS=-DNDEBUG -g0 -O2
SRC=src
BIN=bin
OBJ=$(BIN)/obj
OBJS=$(OBJ)/token.o $(OBJ)/lexer.o $(OBJ)/darray.o $(OBJ)/tokenized_program.o
LIB=lib
LIBS=$(LIB)/sv/sv.o
INC=$(LIB)/
EXEC=$(BIN)/aesthetic

all: debug

debug: CFLAGS += $(CDFLAGS)
debug: OBJ=$(BIN)/obj-debug
debug: EXEC=$(BIN)/aesthetic-debug
debug: compiler

release: CFLAGS += $(CRFLAGS)
release: compiler

compiler: $(SRC)/aesthetic.c lexer token sv darray tokenized_program
	$(CC) $(CFLAGS) -I$(INC) -o $(EXEC) $< $(OBJS) $(LIBS)

lexer: $(SRC)/lexer.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $(OBJ)/lexer.o

token: $(SRC)/token.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $(OBJ)/token.o

darray: $(SRC)/darray.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $(OBJ)/darray.o

tokenized_program: $(SRC)/tokenized_program.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $(OBJ)/tokenized_program.o

sv: $(LIB)/sv/sv.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $(LIB)/sv/sv.o

clean:
	rm $(OBJ)/*.o $(OBJ)-debug/*.o bin/aesthetic*

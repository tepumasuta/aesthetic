CC=gcc
CFLAGS=-Wall -Wextra -std=c18
CDFLAGS=-DDEBUG -DAE_DEBUG -g3 -O0
CRFLAGS=-DNDEBUG -g0 -O2
SRC=src
BIN=bin
OBJ=$(BIN)/obj
OBJS=$(OBJ)/token.o $(OBJ)/lexer.o $(OBJ)/darray.o $(OBJ)/tokenized_program.o $(OBJ)/segment.o
LIB=lib
LIBS=$(LIB)/sv/sv.o
INC=$(LIB)/
EXEC=$(BIN)/aesthetic

all: debug

debug: CFLAGS += $(CDFLAGS)
debug: EXEC=$(BIN)/aesthetic-debug
debug: compiler

release: CFLAGS += $(CRFLAGS)
release: compiler

compiler: $(SRC)/aesthetic.c $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) -I$(INC) -o $(EXEC) $< $(OBJS) $(LIBS)

$(OBJ)/lexer.o: $(SRC)/lexer.c $(SRC)/token.c $(LIB)/sv/sv.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

$(OBJ)/token.o: $(SRC)/token.c $(LIB)/sv/sv.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

$(OBJ)/darray.o: $(SRC)/darray.c $(SRC)/token.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

$(OBJ)/tokenized_program.o: $(SRC)/tokenized_program.c $(SRC)/token.c $(SRC)/darray.c $(LIB)/sv/sv.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

$(OBJ)/segment.o: $(SRC)/segment.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

$(LIB)/sv/sv.o: $(LIB)/sv/sv.c
	$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

clean:
	rm $(OBJ)/*.o bin/aesthetic*

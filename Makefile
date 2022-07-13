CC=gcc
CFLAGS=-Wall -Wextra -std=c18
CDFLAGS=-DDEBUG -DAE_DEBUG -ggdb -g3 -O0
CRFLAGS=-DNDEBUG -g0 -O2
SRC=src
BIN=bin
OBJ=$(BIN)/obj
OBJS=$(OBJ)/token.o $(OBJ)/lexer.o $(OBJ)/darray_token.o $(OBJ)/darray_segment.o $(OBJ)/tokenized_program.o $(OBJ)/segment.o $(OBJ)/string_view.o $(OBJ)/character.o
LIB=lib
LIBS=
INC=-I$(LIB)/ -I$(SRC)/
EXEC=$(BIN)/aesthetic

all: debug

debug: CFLAGS += $(CDFLAGS)
debug: EXEC=$(BIN)/aesthetic-debug
debug: compiler

release: CFLAGS += $(CRFLAGS)
release: compiler

compiler: $(SRC)/aesthetic.c $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(INC) -o $(EXEC) $< $(OBJS) $(LIBS)

$(OBJ)/lexer.o: $(SRC)/lexer/lexer.c $(SRC)/lexer/lexer.h $(OBJ)/token.o $(OBJ)/string_view.o $(OBJ)/character.o
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OBJ)/token.o: $(SRC)/lexer/token.c $(SRC)/lexer/token.h $(OBJ)/string_view.o
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OBJ)/tokenized_program.o: $(SRC)/lexer/tokenized_program.c $(SRC)/lexer/tokenized_program.h $(OBJ)/token.o $(OBJ)/darray_token.o $(OBJ)/token.o $(OBJ)/darray_segment.o $(OBJ)/string_view.o
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OBJ)/segment.o: $(SRC)/data_structures/segment.c $(SRC)/data_structures/segment.h
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OBJ)/darray_token.o: $(SRC)/temp_lib_inst/darray/darray_token.c $(SRC)/temp_lib_inst/darray/darray_token.h $(OBJ)/token.o
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OBJ)/darray_segment.o: $(SRC)/temp_lib_inst/darray/darray_segment.c $(SRC)/temp_lib_inst/darray/darray_segment.h $(OBJ)/segment.o
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OBJ)/string_view.o: $(SRC)/data_structures/string_view.c $(SRC)/data_structures/string_view.h $(OBJ)/character.o
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(OBJ)/character.o: $(SRC)/utility/character.c $(SRC)/utility/character.h
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm $(OBJ)/*.o $(BIN)/aesthetic*

clean-obj:
	rm $(OBJ)/*.o

clean-exec:
	rm $(BIN)/aesthetic*

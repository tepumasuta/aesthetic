CC=g++-10
CFLAGS=-Wall -Wextra -std=c++2a
CDFLAGS=-DDEBUG -DAE_DEBUG -ggdb -g3 -O0
CRFLAGS=-DNDEBUG -g0 -O2
SRC=src
BIN=bin
OBJ=$(BIN)/obj
OBJS=$(OBJ)/token.o
LIB=$(SRC)/libs
LIBS=
INC=-I$(SRC)/ -I$(LIB)/
EXEC=$(BIN)/aesthetic

all: debug

debug: CFLAGS += $(CDFLAGS)
debug: EXEC=$(BIN)/aesthetic-debug
debug: compiler

release: CFLAGS += $(CRFLAGS)
release: compiler

compiler: $(SRC)/aesthetic.cpp $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(INC) -o $(EXEC) $< $(OBJS) $(LIBS)

$(OBJ)/%.o: $(SRC)/lexer/%.cpp $(SRC)/lexer/%.hpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm $(OBJ)/*.o $(BIN)/aesthetic*

clean-obj:
	rm $(OBJ)/*.o

clean-exec:
	rm $(BIN)/aesthetic*

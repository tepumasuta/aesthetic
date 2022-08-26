CC=g++-10
CFLAGS=-Wall -Wextra -std=c++2a
CDFLAGS=-DDEBUG -DAE_DEBUG -ggdb -g3 -O0
CRFLAGS=-DNDEBUG -g0 -O2
SRC=src
BIN=bin
OBJ=$(BIN)/obj
OBJS=$(OBJ)/token.o $(OBJ)/lexer.o
LIB=$(SRC)/libs
LIBS=
INC=-I$(SRC)/ -I$(LIB)/
EXEC=$(BIN)/aesthetic
TEST=$(BIN)/aesthetic-test
TESTOBJ=$(OBJ)/test
TESTOBJS=$(TESTOBJ)/test_lexer.o

all: debug

debug: CFLAGS += $(CDFLAGS)
debug: EXEC=$(BIN)/aesthetic-debug
debug: compiler test

release: CFLAGS += $(CRFLAGS)
release: compiler

test: $(SRC)/test.cpp $(OBJS) $(LIBS) $(TESTOBJS)
	$(CC) $(CFLAGS) $(INC) -o $(TEST) $< $(OBJS) $(LIBS) $(TESTOBJS)

compiler: $(SRC)/aesthetic.cpp $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(INC) -o $(EXEC) $< $(OBJS) $(LIBS)

$(OBJ)/lexer.o: $(SRC)/lexer/token.hpp

$(OBJ)/%.o: $(SRC)/lexer/%.cpp $(SRC)/lexer/%.hpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(TESTOBJ)/%.o: $(SRC)/test/%.cpp $(SRC)/test/%.hpp $(SRC)/test/test.hpp
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm $(OBJ)/*.o $(BIN)/aesthetic*

clean-obj:
	rm $(OBJ)/*.o

clean-exec:
	rm $(BIN)/aesthetic*

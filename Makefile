.PHONY: all run depend test gdb doc

EXEC=cscript

CXXFLAGS=-std=gnu++11 -O0 -gmodules -Wall -Werror -Wno-unused-const-variable -Wno-unused-variable -Wno-unneeded-internal-declaration

all: $(EXEC)

SRC_SHARED = src/lex.cc src/parser.cc src/runtime.cc

SRC_EXEC = src/main.cc src/fmemopen.c

SRC_TEST = test/main.cc test/gtest-all.cc \
	test/foobar.cc

SRC = $(SRC_EXEC) $(SRC_SHARED)
OBJ = $(SRC:.cc=.o)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(EXEC)

TEST_SRC = $(SRC_TEST) $(SRC_SHARED)
TEST_OBJ = $(TEST_SRC:.cc=.o)

test/a.out: $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $(TEST_OBJ) -o test/a.out

test: test/a.out
	./test/a.out

depend:
	@makedepend -Iinclude -Y $(SRC) $(TEST_SRC) 2> /dev/null

.SUFFIXES: .cc .M .o

.cc.o:
	$(CXX) -Isrc $(CXXFLAGS) -c -o $*.o $*.cc
# DO NOT DELETE

src/main.o: src/lex.hh
src/lex.o: src/lex.hh
src/parser.o: src/lex.hh
test/main.o: test/gtest.h
test/gtest-all.o: test/gtest.h
test/foobar.o: test/gtest.h src/lex.hh
src/lex.o: src/lex.hh
src/parser.o: src/lex.hh

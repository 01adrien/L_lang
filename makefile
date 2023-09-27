MEMFLAGS := --leak-check=full --show-leak-kinds=all
LOCAL_LIB = /usr/local/lib
TEST_RUNNER = $(LOCAL_LIB)/libcriterion.so
SRC := $(wildcard src/*.c)
TEST := $(wildcard tests/*_test.c)
OBJ := $(SRC:.c = .o)
CFLAGS := -std=c99 -g3
BIN := build
PROG := prog.lang

.DEFAULT_GOAL := $(BIN) 

$(BIN): $(SRC) main.c
	@ gcc $(CFLAGS) $^ -o $@  

memcheck:                                                                       
	@ valgrind $(MEMFLAGS) ./$(BIN) $(PROG)

test: $(TEST) $(OBJ)
	@ gcc -I $(LOCAL_LIB)/criterion/include $(CFLAGS) -o $@ $^ $(TEST_RUNNER) && ./test

run-prog:
	@ ./$(BIN) $(PROG)

clean:
	@ rm -f build test


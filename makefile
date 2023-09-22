.default_goal := build

flags := -std=c99 -g3
src := $(wildcard src/*.c)
obj := $(src:.c = .o)

build: $(obj)
	@ gcc $^ -o $@ $(flags) && ./build prog.lang

memcheck:                                                                       
	@ valgrind --leak-check=full --show-leak-kinds=all ./build prog.lang

run:
	@ ./build prog.lang

clean:
	@ rm build

.default_goal := build

flags := -std=c99 -g3
src := $(wildcard src/*.c)
obj := $(src:.c = .o)

build: $(obj)
	@ gcc $^ -o $@ $(flags) && ./build prog.ll

memcheck:                                                                       
	@ valgrind --leak-check=full --show-leak-kinds=all ./build prog.ll

run:
	@ ./build prog.ll

clean:
	@ rm build

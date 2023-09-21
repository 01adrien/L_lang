.default_goal := build

flags := -std=c99 -g3
src := $(wildcard src/*.c)
obj := $(src:.c = .o)

build: $(obj)
	@ gcc $^ -o $@ $(flags) && ./build prog.rb

memcheck:                                                                       
	@ valgrind --leak-check=full --show-leak-kinds=all ./build prog.rb

run:
	@ ./build prog.rb

clean:
	@ rm build

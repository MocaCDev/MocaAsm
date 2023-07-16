.PHONY: build
.PHONY: run
.PHONY: clean

FLAGS = -std=c++20 -Wall -fsanitize=leak -o

build:
	g++ main.cpp ${FLAGS} bin/main.o

run: build
	./bin/main.o $(ASM)

clean:
	rm -rf bin/*.o

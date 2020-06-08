

run: build
	./main

build: main

main: main.cpp io_context.cpp
	g++ -o $@ $^ -pthread
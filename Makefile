

run: build
	./main

build: main

main: main.cpp
	g++ -o $@ $^ -pthread


run: build
	./main 9000

build: main

main: main.cpp io_context.cpp  timer.cpp acceptor.cpp connection.cpp alternate_test.cpp
	g++ -o $@ $^ -pthread
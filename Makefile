

run: build
	./main

build: main

main: main.cpp io_context.cpp  acceptor.cpp connection.cpp alternate_test.cpp
	g++ -o $@ $^ -pthread
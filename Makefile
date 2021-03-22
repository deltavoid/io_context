

BUILD_DIR:=build



# run: build
# 	./main 9000


build: $(BUILD_DIR)/Makefile
	cd $(BUILD_DIR) && make --trace

$(BUILD_DIR)/Makefile:
	mkdir -p $(BUILD_DIR) && cmake -S . -B $(BUILD_DIR)

clean:
	-rm -rf $(BUILD_DIR)




# build: main

# main: main.cpp io_context.cpp  timer.cpp acceptor.cpp connection.cpp alternate_test.cpp
# 	g++ -o $@ $^ -pthread
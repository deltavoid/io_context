

BUILD_DIR?=build


.PHONY: build
default: build


build: $(BUILD_DIR)/Makefile
	cd $(BUILD_DIR) && make

$(BUILD_DIR)/Makefile: CMakeLists.txt
	mkdir -p $(BUILD_DIR) && cmake -S . -B $(BUILD_DIR)

clean:
	-rm -rf $(BUILD_DIR)



run_alternate_test: build
	$(BUILD_DIR)/examples/alternate_test

run_acceptor_test: build
	$(BUILD_DIR)/examples/acceptor_test 8192
# Compiler settings
CC       = gcc
CXX      = g++
CFLAGS   = -Wall -Wextra -O2 -Werror
CXXFLAGS = -Wall -Wextra -O2 -Werror

# Paths
SRC_ROOT    = .
LIB_DIR     = $(SRC_ROOT)/stringqol
TEST_DIR    = $(SRC_ROOT)/test_suite
BENCH_DIR   = $(SRC_ROOT)/bench
INSTALL_DIR = /usr/local/include/stringqol

# Detect include dir: prefer local lib dir, fall back to install dir
ifeq ($(wildcard $(LIB_DIR)/stringqol.h),)
    INC = -I$(INSTALL_DIR)
else
    INC = -I$(LIB_DIR)
endif

.PHONY: all test bench install uninstall clean

all: test

test: $(TEST_DIR)/c_test $(TEST_DIR)/cpp_test
	@echo "--- Running C tests ---"
	@$(TEST_DIR)/c_test
	@echo "--- Running C++ tests ---"
	@$(TEST_DIR)/cpp_test

bench: $(BENCH_DIR)/bench_c $(BENCH_DIR)/bench_cpp
	@echo "--- C Benchmarks ---"
	@$(BENCH_DIR)/bench_c
	@echo "--- C++ Benchmarks ---"
	@$(BENCH_DIR)/bench_cpp

package:
	@echo "--- Packaging StringQOL ---"
	@echo "Running make clean!"
	@make clean

	@echo "Removing $(TEST_DIR)"
	@rm -rf $(TEST_DIR)

	@echo "Removing $(BENCH_DIR)"
	@rm -rf $(BENCH_DIR)

	@echo "Removing .vscode!"
	@rm -rf .vscode

	@echo "Removing bench.py"
	@rm -f bench.py

	@echo "Renaming $(LIB_DIR) to 'include'"
	@mv $(LIB_DIR) include
	
	@echo "--- Packaging complete! ---"

$(TEST_DIR)/c_test: $(TEST_DIR)/c_test.c
	$(CC) $(CFLAGS) $(INC) $< -o $@

$(TEST_DIR)/cpp_test: $(TEST_DIR)/cpp_test.cpp
	$(CXX) $(CXXFLAGS) $(INC) $< -o $@

$(BENCH_DIR)/bench_c: $(BENCH_DIR)/bench_c.c
	$(CC) $(CFLAGS) -O2 $(INC) $< -o $@

$(BENCH_DIR)/bench_cpp: $(BENCH_DIR)/bench_cpp.cpp
	$(CXX) $(CXXFLAGS) -O2 $(INC) $< -o $@

install:
	@echo "Installing stringqol to $(INSTALL_DIR)..."
	@mkdir -p $(INSTALL_DIR)
	@cp $(LIB_DIR)/* $(INSTALL_DIR)
	@echo "Done."

uninstall:
	@echo "Removing $(INSTALL_DIR)..."
	@rm -rf $(INSTALL_DIR)
	@echo "Done."

clean:
	@rm -f $(TEST_DIR)/c_test $(TEST_DIR)/cpp_test
	@rm -f $(BENCH_DIR)/bench_c $(BENCH_DIR)/bench_cpp
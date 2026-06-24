CC := clang
CFLAGS := -std=c17 -Wall -Wextra -Werror -g
SANITIZE_FLAGS := -fsanitize=address,undefined

BUILD_DIR := build
SRC_DIR := src
TEST_DIR := tests

APP := $(BUILD_DIR)/mini-virt
TEST_VM_APP := $(BUILD_DIR)/test_vm
TEST_ASSEMBLER_APP := $(BUILD_DIR)/test_assembler

APP_SRCS := $(SRC_DIR)/main.c $(SRC_DIR)/vm.c
TEST_VM_SRCS := $(TEST_DIR)/test_vm.c $(SRC_DIR)/vm.c
TEST_ASSEMBLER_SRCS := $(TEST_DIR)/test_assembler.c $(SRC_DIR)/assembler.c

.PHONY: all run test test-vm test-assembler clean

all: $(APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP): $(APP_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SANITIZE_FLAGS) -I$(SRC_DIR) $(APP_SRCS) -o $(APP)

$(TEST_VM_APP): $(TEST_VM_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SANITIZE_FLAGS) -I$(SRC_DIR) $(TEST_VM_SRCS) -o $(TEST_VM_APP)

$(TEST_ASSEMBLER_APP): $(TEST_ASSEMBLER_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SANITIZE_FLAGS) -I$(SRC_DIR) $(TEST_ASSEMBLER_SRCS) -o $(TEST_ASSEMBLER_APP)

run: $(APP)
	$(APP)

test: test-vm test-assembler

test-vm: $(TEST_VM_APP)
	$(TEST_VM_APP)

test-assembler: $(TEST_ASSEMBLER_APP)
	$(TEST_ASSEMBLER_APP)

clean:
	rm -rf $(BUILD_DIR)

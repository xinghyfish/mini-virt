CC := clang
CFLAGS := -std=c17 -Wall -Wextra -Werror -g
SANITIZE_FLAGS := -fsanitize=address,undefined

BUILD_DIR := build
SRC_DIR := src
TEST_DIR := tests

APP := $(BUILD_DIR)/mini-virt
TEST_VM_APP := $(BUILD_DIR)/test_vm
TEST_ASSEMBLER_APP := $(BUILD_DIR)/test_assembler
TEST_INTEGRATION_APP := $(BUILD_DIR)/test_integration
TEST_LOADER_APP := $(BUILD_DIR)/test_loader

APP_SRCS := $(SRC_DIR)/main.c $(SRC_DIR)/vm.c
TEST_VM_SRCS := $(TEST_DIR)/test_vm.c $(SRC_DIR)/vm.c
TEST_ASSEMBLER_SRCS := $(TEST_DIR)/test_assembler.c $(SRC_DIR)/assembler.c
TEST_INTEGRATION_SRCS := $(TEST_DIR)/test_integration.c $(SRC_DIR)/runner.c \
	$(SRC_DIR)/assembler.c $(SRC_DIR)/vm.c
TEST_LOADER_SRCS := $(TEST_DIR)/test_loader.c $(SRC_DIR)/loader.c

.PHONY: all run test test-vm test-assembler test-integration test-loader clean

all: $(APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP): $(APP_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SANITIZE_FLAGS) -I$(SRC_DIR) $(APP_SRCS) -o $(APP)

$(TEST_VM_APP): $(TEST_VM_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SANITIZE_FLAGS) -I$(SRC_DIR) $(TEST_VM_SRCS) -o $(TEST_VM_APP)

$(TEST_ASSEMBLER_APP): $(TEST_ASSEMBLER_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SANITIZE_FLAGS) -I$(SRC_DIR) $(TEST_ASSEMBLER_SRCS) -o $(TEST_ASSEMBLER_APP)

$(TEST_INTEGRATION_APP): $(TEST_INTEGRATION_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SANITIZE_FLAGS) -I$(SRC_DIR) $(TEST_INTEGRATION_SRCS) -o $(TEST_INTEGRATION_APP)

$(TEST_LOADER_APP): $(TEST_LOADER_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SANITIZE_FLAGS) -I$(SRC_DIR) $(TEST_LOADER_SRCS) -o $(TEST_LOADER_APP)

run: $(APP)
	$(APP)

test: test-vm test-assembler test-integration test-loader

test-vm: $(TEST_VM_APP)
	$(TEST_VM_APP)

test-assembler: $(TEST_ASSEMBLER_APP)
	$(TEST_ASSEMBLER_APP)

test-integration: $(TEST_INTEGRATION_APP)
	$(TEST_INTEGRATION_APP)

test-loader: $(TEST_LOADER_APP)
	$(TEST_LOADER_APP)

clean:
	rm -rf $(BUILD_DIR)

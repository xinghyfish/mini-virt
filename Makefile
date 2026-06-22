CC := clang
CFLAGS := -std=c17 -Wall -Wextra -Werror -g
SANITIZE_FLAGS := -fsanitize=address,undefined

BUILD_DIR := build
SRC_DIR := src
TEST_DIR := tests

APP := $(BUILD_DIR)/mini-virt
TEST_APP := $(BUILD_DIR)/test_vm

APP_SRCS := $(SRC_DIR)/main.c $(SRC_DIR)/vm.c
TEST_SRCS := $(TEST_DIR)/test_vm.c $(SRC_DIR)/vm.c

.PHONY: all run test clean

all: $(APP)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(APP): $(APP_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SANITIZE_FLAGS) -I$(SRC_DIR) $(APP_SRCS) -o $(APP)

$(TEST_APP): $(TEST_SRCS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SANITIZE_FLAGS) -I$(SRC_DIR) $(TEST_SRCS) -o $(TEST_APP)

run: $(APP)
	$(APP)

test: $(TEST_APP)
	$(TEST_APP)

clean:
	rm -rf $(BUILD_DIR)

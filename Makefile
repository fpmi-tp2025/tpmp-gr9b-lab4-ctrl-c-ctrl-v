CC = gcc
CFLAGS = -Wall -Wextra -I./includes
# Убираем зависимость от libcrypto
LDFLAGS = -l sqlite3

SRC_DIR = src
INCLUDE_DIR = includes
BUILD_DIR = build
BIN_DIR = bin

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Executable name
TARGET = $(BIN_DIR)/parfum_bazaar

# Test files
TEST_DIR = test
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(TEST_DIR)/%.o)
TEST_MAIN = $(BIN_DIR)/test_main
TEST_DB = $(BIN_DIR)/test_database
TEST_AUTH = $(BIN_DIR)/test_auth
TEST_DEALS = $(BIN_DIR)/test_deals

# Default target
all: $(TARGET)

# Create directories if they don't exist
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Build main application
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Initialize database
init_db:
	sqlite3 parfum_bazaar.db < data/init_db.sql
	sqlite3 parfum_bazaar.db < data/test_data.sql

# Compile test object files
$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build individual test executables
$(TEST_DB): $(TEST_DIR)/test_database.o $(filter-out $(BUILD_DIR)/main.o, $(OBJS)) | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(TEST_AUTH): $(TEST_DIR)/test_auth.o $(filter-out $(BUILD_DIR)/main.o, $(OBJS)) | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(TEST_DEALS): $(TEST_DIR)/test_deals.o $(filter-out $(BUILD_DIR)/main.o, $(OBJS)) | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

$(TEST_MAIN): $(TEST_DIR)/test_main.o | $(BIN_DIR)
	$(CC) $^ -o $@ $(LDFLAGS)

# Build all tests
tests: $(TEST_DB) $(TEST_AUTH) $(TEST_DEALS) $(TEST_MAIN)

# Run individual tests
test_database: $(TEST_DB)
	./$(TEST_DB)

test_auth: $(TEST_AUTH)
	./$(TEST_AUTH)

test_deals: $(TEST_DEALS)
	./$(TEST_DEALS)

# Run all tests
check: tests
	./$(TEST_MAIN)

# Generate coverage report
coverage: CFLAGS += -fprofile-arcs -ftest-coverage
coverage: LDFLAGS += -lgcov
coverage: clean tests
	./$(TEST_MAIN)
	gcov $(SRCS)
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage-report

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) *.gcov *.gcda *.gcno coverage.info coverage-report

# Clean all including database
distclean: clean
	rm -f parfum_bazaar.db
	rm -f test.db test_auth.db test_deals.db

# Debug targets
debug: CFLAGS += -g -DDEBUG
debug: clean all

valgrind: debug
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Phony targets
.PHONY: all clean distclean tests check coverage init_db debug valgrind test_database test_auth test_deals

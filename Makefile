TARGET = matpreter
CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -Iinclude -g

SRC = main.c tokenizer.c preprocessor.c
OBJ_DIR = build/obj
BIN_DIR = build/bin
OBJ = $(SRC:%.c=$(OBJ_DIR)/%.o)
BIN = $(BIN_DIR)/$(TARGET)

# --- Default target ---
all: $(BIN)

# --- Build final executable ---
$(BIN): $(OBJ)
	@echo "Linking -> $@"
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# --- Compile each .c file into build/obj ---
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@echo "Compiling -> $<"
	$(CC) $(CFLAGS) -c $< -o $@

# --- Create required directories ---
$(OBJ_DIR) $(BIN_DIR):
	@mkdir -p $@

# --- Run ---
run: all
	@echo "Running $(TARGET)..."
	@./$(BIN)

# --- Debug ---
debug: all
	gdb ./$(BIN)

# --- Clean ---
clean:
	rm -rf build

.PHONY: all run clean debug
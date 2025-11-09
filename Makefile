TARGET = matpreter
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c17 -Iinclude -g -m64

SRC = demo.c tokenizer.c preprocessor.c parse.c AST.c mpdisplay.c evaluate.c
OBJ_DIR = build/obj
BIN_DIR = build/bin
SRC_DIR = src
OBJ = $(SRC:%.c=$(OBJ_DIR)/%.o)
BIN = $(BIN_DIR)/$(TARGET)

# libraries, m -> math lib
LIB = m
LIB_FLAG = $(addprefix -l, $(LIB))

# --- Default target ---
all: $(BIN)

# --- Build final executable ---
$(BIN): $(OBJ) | $(BIN_DIR)
	@echo "Linking -> $@"
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIB_FLAG)

# --- Compile each .c file into build/obj ---
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
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
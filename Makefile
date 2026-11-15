# Compiler and flags
CC      := gcc
CFLAGS  := -Wall -Wextra -Wimplicit-function-declaration -Iinclude -g
LDFLAGS :=

# Directories
SRC_DIR := src
INC_DIR := include
OBJ_DIR := build
BIN_DIR := bin

# Target executable
TARGET  := $(BIN_DIR)/krill

# Automatically find all .c files in src/ and map them to obj/
SRCS    := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Default rule
all: $(TARGET)

# Link object files into the final binary
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile .c files into .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directories if they don't exist
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Phony targets aren't actual files
.PHONY: all clean

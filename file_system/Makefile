# Compiler settings
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS := 
DEBUG_FLAGS := -g -O0 -DDEBUG -fno-omit-frame-pointer
RELEASE_FLAGS := -O3 -DNDEBUG

# Directory structure
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BIN_DIR := bin
FILES_DIR := files

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
TARGET := $(BIN_DIR)/filesystem

# Ensure output directories exist
$(shell mkdir -p $(BUILD_DIR) $(BIN_DIR) $(FILES_DIR))

# Default target (debug build)
all: debug

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Release build
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

# Build main executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# Pattern rule for object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run the debug program
run: debug
	$(TARGET)

# Run with debugger
debug-run: debug
	gdb --args $(TARGET)

# Run with specific filename
run-with: debug
	$(TARGET) $(FILES_DIR)/custom_fs.dat

# Generate assembly for inspection
asm: CXXFLAGS += -S -fverbose-asm
asm:
	$(CXX) $(CXXFLAGS) $(SRC_DIR)/*.cpp

.PHONY: all debug release clean run debug-run run-with asm
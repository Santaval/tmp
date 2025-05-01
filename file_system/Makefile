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
LIB_DIR := lib
FILES_DIR := files

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
LIB_NAME := libfilesystem.a
LIB_TARGET := $(LIB_DIR)/$(LIB_NAME)

# Ensure output directories exist
$(shell mkdir -p $(BUILD_DIR) $(LIB_DIR) $(FILES_DIR))

# Default target (debug build)
all: debug

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(LIB_TARGET)

# Release build
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(LIB_TARGET)

# Build static library
$(LIB_TARGET): $(OBJS)
	ar rcs $@ $^

# Pattern rule for object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR)

# Install library and headers
install: $(LIB_TARGET)
	@mkdir -p /usr/local/lib
	@mkdir -p /usr/local/include/filesystem
	cp $(LIB_TARGET) /usr/local/lib/
	cp $(INC_DIR)/*.hpp /usr/local/include/filesystem/

# Uninstall library and headers
uninstall:
	rm -f /usr/local/lib/$(LIB_NAME)
	rm -rf /usr/local/include/filesystem

.PHONY: all debug release clean install uninstall
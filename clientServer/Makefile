# Compiler and flags
CXX      := g++
CXXFLAGS := -g -std=c++11 -Wall -Wextra -I. -Isockets
LDFLAGS  := -lpthread
LDFLAGS := -lssl -lcrypto

# Source files (with paths)
SERVER_SRCS := tests/testServer.cc sockets/VSocket.cc sockets/Socket.cc sockets/SSLSocket.cc Server.cc
CLIENT_SRCS := tests/testClient.cc sockets/VSocket.cc sockets/Socket.cc sockets/SSLSocket.cc Client.cc

# Object files (keep directory prefix for .o files)
SERVER_OBJS := $(SERVER_SRCS:.cc=.o)
CLIENT_OBJS := $(CLIENT_SRCS:.cc=.o)

# Targets
TARGETS := testClient.out testServer.out

# Default target
all: $(TARGETS)

# Rule to compile any .cc -> .o (handles subdirectories)
%.o: %.cc
	@echo "Compiling $< -> $@"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link executables
testServer.out: $(SERVER_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

testClient.out: $(CLIENT_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

# Clean
clean:
	rm -f *.o *.out sockets/*.o tests/*.o

.PHONY: all clean
include ../common/Makefile

LIBS += -lssl -lcrypto

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS = -lssl -lcrypto

SRCS = src/main.cpp src/Client.cpp src/Socket.cpp src/VSocket.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = bin/http_client

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean


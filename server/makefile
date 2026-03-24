# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -I inc

# Source files
SRC = $(wildcard src/*.cpp)

# Output
TARGET = server_exe

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
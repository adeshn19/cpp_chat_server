# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread

# Source files
SRC = src/client.cpp

# Output
TARGET = client_exe

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
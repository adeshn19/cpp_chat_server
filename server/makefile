CXX = g++

CXXFLAGS = -std=c++17 -Wall -pthread \
           -I inc \
           -I ../logger/inc \
           -I ../utils/inc

SRC = $(wildcard src/*.cpp) \
      ../logger/src/logger.cpp \
      ../utils/src/utils.cpp

TARGET = server_exe

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
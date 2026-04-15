CXX = g++

CXXFLAGS = -std=c++17 -Wall -pthread \
           -I ../logger/inc \
           -I ../utils/inc

SRC = src/client.cpp \
      ../logger/src/logger.cpp \
      ../utils/src/utils.cpp

TARGET = client_exe

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
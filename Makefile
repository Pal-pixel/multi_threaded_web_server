CXX = g++
CXXFLAGS = -Wall -std=c++17

SRC = src/server.cpp
OUT = server

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)
CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I.
SRC      = main.cpp $(wildcard src/*/*.cpp)
OUT      = exam

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT) $(OUT).exe

CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I.
LIBSRC   = $(wildcard src/models/*.cpp src/storage/*.cpp src/services/*.cpp src/utils/*.cpp)
ifeq ($(OS),Windows_NT)
  NETLIBS = -lws2_32
endif

all: exam server

exam: main.cpp $(LIBSRC)
	$(CXX) $(CXXFLAGS) main.cpp $(LIBSRC) -o exam

server: server_main.cpp src/server/ApiServer.cpp $(LIBSRC)
	$(CXX) $(CXXFLAGS) -pthread server_main.cpp src/server/ApiServer.cpp $(LIBSRC) -o server $(NETLIBS)

clean:
	rm -f exam server exam.exe server.exe

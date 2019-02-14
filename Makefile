#
# Flare Director
# Altitude Sensing Module
# Makefile
#
# This Makefile builds the code necessary to run the ASM software.
#

CXX = g++
CXXFLAGS = -Wall -ansi -pedantic -g -lpthread

# Targets
LIDAR = lib/sensor/lidar/*.cpp
NETWORK = lib/network/*.cpp
SRC = src/*.cpp

# Output of built code
BINARY = bin/asm

ifeq ($(shell uname),SunOS)
  LIBS = -lsocket -lnsl 
endif

all: main

client: demo/TCPEchoClient.cpp $(NETWORK)
	$(CXX) $(CXXFLAGS) -o bin/client demo/TCPEchoClient.cpp $(NETWORK) $(LIBS)

main: $(SRC) $(NETWORK) $(LIDAR)
	$(CXX) -D_GNU_SOURCE -o $(BINARY) $(SRC) $(NETWORK) $(LIDAR) $(LIBS) -lpthread

debug: $(SRC) $(NETWORK) $(LIDAR)
	$(CXX) -D_GNU_SOURCE -o $(BINARY) $(SRC) $(NETWORK) $(LIDAR) $(LIBS) -lpthread -DDEBUG -ggdb -O0

clean:
	$(RM) -rf bin/client.dSYM && $(RM) -f bin/*

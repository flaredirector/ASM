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

TCPEchoClient: demo/TCPEchoClient.cpp $(NETWORK)/NetworkSocket.cpp $(NETWORK)/NetworkSocket.h
	$(CXX) $(CXXFLAGS) -o bin/client demo/TCPEchoClient.cpp lib/NetworkSocket.cpp $(LIBS)

main: $(SRC) $(NETWORK) $(LIDAR)
	$(CXX) -D_GNU_SOURCE -o $(BINARY) $(SRC) $(NETWORK) $(LIDAR) $(LIBS) -lpthread

debug: $(SRC) $(NETWORK) $(LIDAR)
	$(CXX) -D_GNU_SOURCE -o $(BINARY) $(SRC) $(NETWORK) $(LIDAR) $(LIBS) -lpthread -DDEBUG

clean:
	$(RM) -rf bin/client.dSYM && $(RM) -f bin/*

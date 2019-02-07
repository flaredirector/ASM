#
# Flare Director
# Altitude Sensing Module
# Makefile
#
# This Makefile builds the code necessary to run the ASM software.
#

CXX = g++
CXXFLAGS = -I. -std=c++11 -Wall -ansi -pedantic -g

LIDAR = lib/sensor/lidar
NETWORK = lib/network

ifeq ($(shell uname),SunOS)
  LIBS = -lsocket -lnsl 
endif

all: Main

TCPEchoClient: demo/TCPEchoClient.cpp $(NETWORK)/NetworkSocket.cpp $(NETWORK)/NetworkSocket.h
	$(CXX) $(CXXFLAGS) -o bin/client demo/TCPEchoClient.cpp lib/NetworkSocket.cpp $(LIBS)

Main: src/Main.cpp src/ASM.cpp $(NETWORK)/NetworkSocket.cpp $(LIDAR)/LIDARInterface.cpp
	$(CXX) -D_GNU_SOURCE -o bin/server src/Main.cpp src/ASM.cpp $(NETWORK)/NetworkSocket.cpp $(LIDAR)/LIDARInterface.cpp $(LIBS) -lpthread

clean:
	$(RM) -rf bin/TCPEchoClient.dSYM && $(RM) -f bin/*

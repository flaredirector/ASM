#
# Flare Director
# Altitude Sensing Module
# Makefile
#
# This Makefile builds the code necessary to run the ASM software.
#

CXX = g++
CXXFLAGS = -Wall -pedantic -g -lpthread

# Targets
LIDAR = lib/sensor/lidar/*.cpp
SONAR = lib/sensor/sonar/*.cpp
NETWORK = lib/network/*.cpp
ERROR_DISPLAY = lib/gpio/errorCodeDisplay/*.cpp
LED = lib/gpio/led/*.cpp
BATTERY = lib/gpio/battery/*.cpp
SRC = src/*.cpp

# Output of built code
BINARY = bin/asm
DEBUG_BINARY = bin/asmd

ifeq ($(shell uname),SunOS)
  LIBS = -lsocket -lnsl 
endif

all: main

client: demo/TCPEchoClient.cpp $(NETWORK)
	$(CXX) $(CXXFLAGS) -o bin/client demo/TCPEchoClient.cpp $(NETWORK) $(LIBS)

main: $(SRC) $(NETWORK) $(LIDAR)
	$(CXX) -D_GNU_SOURCE -o $(BINARY) $(SRC) $(NETWORK) $(LIDAR) $(SONAR) $(LIBS) -lpthread

debug: $(SRC) $(NETWORK) $(LIDAR)
	$(CXX) -D_GNU_SOURCE -o $(DEBUG_BINARY) $(BATTERY) $(ERROR_DISPLAY) $(LED) $(LIDAR) $(SONAR) $(NETWORK) $(SRC) $(LIBS) -lpthread -DDEBUG -ggdb -O0

clean:
	$(RM) -rf bin/client.dSYM && $(RM) -f bin/*

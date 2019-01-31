#
#   A wrapper for C++ sockets on Unix and Windows
#   Copyright (C) 2002
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

CXX = g++
CXXFLAGS = -Wall -ansi -pedantic -g

ifeq ($(shell uname),SunOS)
  LIBS = -lsocket -lnsl
endif

all: TCPEchoClient TCPEchoServer-Thread

TCPEchoClient: demo/TCPEchoClient.cpp lib/NetworkSocket.cpp lib/NetworkSocket.h
	$(CXX) $(CXXFLAGS) -o bin/client demo/TCPEchoClient.cpp lib/NetworkSocket.cpp $(LIBS)

TCPEchoServer: demo/TCPEchoServer.cpp lib/NetworkSocket.cpp lib/NetworkSocket.h
	$(CXX) $(CXXFLAGS) -o TCPEchoServer TCPEchoServer.cpp lib/NetworkSocket.cpp $(LIBS)

TCPEchoServer-Thread: demo/TCPEchoServer-Thread.cpp lib/NetworkSocket.cpp lib/NetworkSocket.h
	$(CXX) -D_GNU_SOURCE -o bin/server demo/TCPEchoServer-Thread.cpp lib/NetworkSocket.cpp $(LIBS) -lpthread

clean:
	$(RM) -rf bin/TCPEchoClient.dSYM && $(RM) -f bin/*

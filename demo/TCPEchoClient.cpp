/*
 *   C++ sockets on Unix and Windows
 *   Copyright (C) 2002
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "../lib/NetworkSocket.h"  // For Socket and SocketException
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()

using namespace std;

const int RCVBUFSIZE = 32;    // Size of receive buffer

int main(int argc, char *argv[]) {
  if ((argc < 2) || (argc > 3)) {     // Test for correct number of arguments
    cerr << "Usage: " << argv[0] << " <Server> [<Server Port>]" << endl;
    exit(1);
  }

  string serverAddress = argv[1]; // First arg: server address
  char echoString[RCVBUFSIZE] = ""; // Second arg: string to echo
  int echoStringLength; // Input length
  unsigned short echoServerPort = (argc == 3) ? atoi(argv[2]) : 7;

  try {
    // Establish connection with the echo server
    TCPSocket socket(serverAddress, echoServerPort);
  
    while (1) {
      // Get message from user
      cout << "Enter a message: ";
      scanf("%[^\n]%*c", echoString);
      echoStringLength = strlen(echoString);

      // Send the string to the echo server
      socket.send(echoString, echoStringLength);
    
      // Prepare to receive message
      char echoBuffer[RCVBUFSIZE + 1];    // Buffer for echo string + \0
      int bytesReceived = 0;              // Bytes read on each recv()
      int totalBytesReceived = 0;         // Total bytes read

      // Receive the same string back from the server
      cout << "Received: ";               // Setup to print the echoed string
      while (totalBytesReceived < echoStringLength) {
        // Receive up to the buffer size bytes from the sender
        if ((bytesReceived = (socket.recv(echoBuffer, RCVBUFSIZE))) <= 0) {
          cerr << "Unable to read";
          exit(1);
        }
        
        totalBytesReceived += bytesReceived;     // Keep tally of total bytes
        echoBuffer[bytesReceived] = '\0';        // Terminate the string!
        cout << echoBuffer;                      // Print the echo buffer
      }
      cout << endl;

      // Destructor closes the socket
    }

  } catch(SocketException &e) {
    cerr << e.what() << endl;
    exit(1);
  }

  return 0;
}

/*
 * Flare Director
 * ASM
 * TCPEchoClient.cpp
 * 
 */

#include "../lib/network/NetworkSocket.hpp"  // For Socket and SocketException
#include <iostream>           // For cerr and cout
#include <stdlib.h>
#include <string.h>

using namespace std;

const int RCVBUFSIZE = 32;    // Size of receive buffer

int main(int argc, char *argv[]) {
  if ((argc < 1) || (argc > 2)) {     // Test for correct number of arguments
    cerr << "Usage: " << argv[0] << " <Server Port> [<Server>]" << endl;
    exit(1);
  }

  string serverAddress = (argc == 3) ? argv[2] : "127.0.0.1"; // First arg: server address
  unsigned short echoServerPort = atoi(argv[2]);

  try {
    // Establish connection with the echo server
    TCPSocket *socket = new TCPSocket(serverAddress, echoServerPort);

    int counter = 0;
    while (1) {
      // Prepare to receive message
      char echoBuffer[RCVBUFSIZE + 1];    // Buffer for echo string + \0
      int bytesReceived = 0;              // Bytes read on each recv()
      int totalBytesReceived = 0;         // Total bytes read

      // Receive the same string back from the server
      cout << "Received: ";               // Setup to print the echoed string
      // Receive up to the buffer size bytes from the sender
      if ((bytesReceived = (socket->recv(echoBuffer, RCVBUFSIZE))) <= 0) {
        cerr << "Unable to read packet" << endl;
        exit(1);
      }
      
      totalBytesReceived += bytesReceived;     // Keep tally of total bytes
      echoBuffer[bytesReceived] = '\0';        // Terminate the string!
      cout << echoBuffer;                      // Print the echo buffer

      // if (counter % 10 == 0) {
      //   char message[32];
      //   stpcpy(message, "calibrate:1");
      //   socket->send(message, strlen(message));
      // }

      if (counter == 100)
        counter = 0;
      
      if (counter == 30) {
        char message[32];
        stpcpy(message, "reportingToggle:0");
        socket->send(message, strlen(message));
      }

      counter++;
      
      cout << endl;
      // Destructor closes the socket
    }

  } catch(SocketException &e) {
    cerr << e.what() << endl;
    exit(1);
  }

  return 0;
}

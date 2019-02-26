/*-
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

int main(int argc, char *argv[]) {
  if ((argc < 1) || (argc > 3)) {     // Test for correct number of arguments
    cerr << "Usage: " << argv[0] << " <Server Port> [<Server>]" << endl;
    exit(1);
  }

  string serverAddress = (argc == 3) ? argv[2] : "127.0.0.1"; // First arg: server address
  unsigned short echoServerPort = (argc >= 2) ? atoi(argv[1]) : 4000;

  try {
    // Establish connection with the echo server
    TCPSocket *socket = new TCPSocket(serverAddress, echoServerPort);

    int counter = 0;
    while (1) {
      cout << "Received: ";  // Setup to print the echoed string

      char buffer[BUFSIZE];
      int recvMsgSize;
      while ((recvMsgSize = socket->recv(buffer, BUFSIZE)) > 0) { // Zero means end of transmission
          // Convert into string for easier use
          string receivedMessage = buffer; 

          cout << "RECEIVED MESSAGE: " << receivedMessage << endl;
      }

      if (counter % 30 == 0) {
        char message[32];
        stpcpy(message, "calibrate:1");
        socket->send(message, strlen(message));
      }

      if (counter == 100)
        counter = 0;
      
      // if (counter == 30) {
      //   char message[32];
      //   stpcpy(message, "reportingToggle:0");
      //   socket->send(message, strlen(message));
      // }

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

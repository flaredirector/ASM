/*
 * Flare Director
 * ASM
 * TCPEchoServer-Thread.cpp
 * 
 */

#include "../lib/NetworkSocket.h"  // For Socket, ServerSocket, and SocketException
#include <iostream>           // For cout, cerr
#include <cstdlib>            // For atoi()  
#include <pthread.h>          // For POSIX threads  
#include <unistd.h>           // For usleep()
#include "../include/lidar_lite.h"

const int RCVBUFSIZE = 32;    // 32 byte message buffer size

void HandleTCPClient(TCPSocket *socket);     // TCP client handling function
void *ThreadMain(void *arg);               // Main program of a thread  

Lidar_Lite l1(1);

int main(int argc, char *argv[]) {
  if (argc != 2) {  // Test for correct number of arguments  
    cerr << "Usage: " << argv[0] << " <Server Port> " << endl;
    exit(1);
  }

  int err = l1.connect();
  if (err < 0){
    printf("%d", l1.err);
  }

  unsigned short echoServerPort = atoi(argv[1]);    // First arg:  local port  

  try {
    TCPServerSocket *serverSocket = new TCPServerSocket(echoServerPort);   // Socket descriptor for server  
  
    for (;;) {      // Run forever  
      // Create separate memory for client argument 
      cout << "Waiting for Client Connection" << endl;
      TCPSocket *clientSocket = serverSocket->accept(); // Blocks
      cout << "Client Connected" << endl;
      // Create client thread  
      pthread_t threadID;              // Thread ID from pthread_create()  
      if (pthread_create(&threadID, NULL, ThreadMain, (void *) clientSocket) != 0) {
        cerr << "Unable to create thread" << endl;
        exit(1);
      }
    }
  } catch (SocketException &e) {
    cerr << e.what() << endl;
    exit(1);
  }
  // NOT REACHED

  return 0;
}

// TCP client handling function
void HandleTCPClient(TCPSocket *socket) {
  // Send received string and receive again until the end of transmission
  char echoBuffer[RCVBUFSIZE];

  //int altitude = 120;
  while (l1.err >= 0) {
    int dist = l1.getDistance();
    sprintf(echoBuffer, "altitude:%d\4", dist);
    socket->send(echoBuffer, strlen(echoBuffer));
    string sentPacket = echoBuffer;
    cout << "Sent: " << sentPacket << endl;
    // altitude--;
    // if (altitude == 0) {
    //   altitude = 120;
    // }
    usleep(100000); // 100 milliseconds (10 Hz)
  }
}

void *ThreadMain(void *clientSocket) {
  // Guarantees that thread resources are deallocated upon return  
  pthread_detach(pthread_self()); 

  // Extract socket file descriptor from argument  
  HandleTCPClient((TCPSocket *) clientSocket);

  delete (TCPSocket *) clientSocket;
  return NULL;
}

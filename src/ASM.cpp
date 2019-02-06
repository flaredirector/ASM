/*
 * Flare Director
 * ASM
 * Main.cpp
 * 
 */

#include "ASM.h"

const int RCVBUFSIZE = 32;    // 32 byte message buffer size

void handleConnection(TCPSocket *socket);     // TCP client handling function
void *threadMain(void *arg);               // Main program of a thread

ASM::ASM(const unsigned short port) {
    lidar = LIDARInterface(1);

    int err = lidar.connect();
    if (err < 0){
        printf("%d", lidar.err);
    }

    try {
        TCPServerSocket *serverSocket = new TCPServerSocket(echoServerPort);   // Socket descriptor for server  
    
        for (;;) {      // Run forever  
        // Create separate memory for client argument 
        cout << "Waiting for Client Connection" << endl;
        TCPSocket *clientSocket = serverSocket->accept(); // Blocks until new connection received.
        cout << "Client Connected" << endl;
        // Create client thread  
        pthread_t threadID;              // Thread ID from pthread_create()  
        if (pthread_create(&threadID, NULL, threadMain, (void *) clientSocket) != 0) {
            cerr << "Unable to create thread" << endl;
            exit(1);
        }
        }
    } catch (SocketException &e) {
        cerr << e.what() << endl;
        exit(1);
    }
}

// TCP client handling function
ASM::handleConnection(TCPSocket *socket) {
  // Send received string and receive again until the end of transmission
  char echoBuffer[RCVBUFSIZE];

  while (l1.err >= 0) {
    int dist = l1.getDistance();
    sprintf(echoBuffer, "altitude:%d\4", dist);
    socket->send(echoBuffer, strlen(echoBuffer));
    string sentPacket = echoBuffer;
    cout << "Sent: " << sentPacket << endl;
    usleep(100000); // 100 milliseconds (10 Hz)
  }
}

ASM::threadMain(void *clientSocket) {
  // Guarantees that thread resources are deallocated upon return  
  pthread_detach(pthread_self()); 

  // Extract socket file descriptor from argument  
  handleConnection((TCPSocket *) clientSocket);

  delete (TCPSocket *) clientSocket;
  return NULL;
}

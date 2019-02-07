/*
 * Flare Director
 * ASM
 * Main.cpp
 * 
 */

#include "ASM.h"

using namespace std;

#define RCVBUFSIZE 32    // 32 byte message buffer size

ThreadTask::ThreadTask(TCPSocket *cs, LIDARInterface *l) {
    this->lidar = l;
    this->clientSocket = cs;
}

ASM::ASM(unsigned short int port) {
    this->port = port;

    // Initialize new LIDAR interface
    this->lidar = new LIDARInterface(1);

    // Attempt connection to the LIDAR interface
    int err = this->lidar->connect();

    // Check for error in connecting to the LIDAR interface
    if (err < 0){
        printf("%d", this->lidar->err);
    }
}

void ASM::start(void) {
    try {
        TCPServerSocket *serverSocket = new TCPServerSocket(this->port);   // Socket descriptor for server  
    
        for (;;) {      // Run forever  
            // Create separate memory for client argument 
            cout << "Waiting for Client Connection" << endl;
            TCPSocket *clientSocket = serverSocket->accept(); // Blocks until new connection received.
            ThreadTask *ttask = new ThreadTask(clientSocket, this->lidar);
            cout << "Client Connected" << endl;
            // Create client thread  
            pthread_t threadID;              // Thread ID from pthread_create()  
            if (pthread_create(&threadID, NULL, &ASM::threadMainHelper, (void *) ttask) != 0) {
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
void ASM::handleConnection(ThreadTask *task) {
  // Send received string and receive again until the end of transmission
  char echoBuffer[RCVBUFSIZE];
  while (task->lidar->err >= 0) {
    int dist = task->lidar->getDistance();
    sprintf(echoBuffer, "altitude:%d\4", dist);
    task->clientSocket->send(echoBuffer, strlen(echoBuffer));
    string sentPacket = echoBuffer;
    cout << "Sent: " << sentPacket << endl;
    usleep(100000); // 100 milliseconds (10 Hz)
  }
}

// Method to get executed by spawned socket thread.
void *ASM::threadMain(void *args) {
  // Guarantees that thread resources are deallocated upon return  
  pthread_detach(pthread_self()); 

  // Extract socket file descriptor from argument  
  this->handleConnection(((ThreadTask *) args));

  // First cast is to tell delete what type of instance it's deleting
  // Second cast is to tell compilter that there is a clientSocket attribute on args
  delete (TCPSocket *) ((ThreadTask *) args)->clientSocket;
}

// Helper method that enables threadMain to get called in pthread_create
void *ASM::threadMainHelper(void *args) {
    return ((ASM *)args)->threadMain(args);
}

/**
 * Flare Director
 * ASM
 * ASM.cpp
 * 
 * This implementation file contains the implementation
 * details for the ASM class.
 */

#include "ASM.h"

using namespace std;

/**
 * ASM
 ** Instantiates new ASM instance with port argument.
 * @param {port} The port to send/receive messages on
 */
ASM::ASM(unsigned short int port) {
    // Set port to passed in port number
    this->port = port;

    // Initialize new LIDAR interface
    this->lidar = new LIDARInterface(1);
}

/**
 * start
 ** Starts the event loop which listens for new connections
 ** and spwans a new thread for each connection.
 */
void ASM::start(void) {
    try {
        // Socket descriptor for server
        TCPServerSocket *serverSocket = new TCPServerSocket(this->port);  

        // Attempt connection to the LIDAR interface
        int err = this->lidar->connect();
        // Check for error in connecting to the LIDAR interface
        if (err < 0){
            printf("%d", this->lidar->err);
        }
    
        // Run forever  
        for (;;) {      
            // Create separate memory for client argument 
            cout << "Waiting for Client Connection..." << endl;
            // Blocks until new connection received.
            TCPSocket *clientSocket = serverSocket->accept(); 

            // For each new connection, spawn a new thread
            ThreadTask *ttask = new ThreadTask(clientSocket, this->lidar);
            cout << "Client Connected" << endl;
            pthread_t threadID;
            if (pthread_create(&threadID, NULL, &ASM::threadMainHelper, (void *) ttask) != 0) {
                cerr << "Unable to create thread" << endl;
                exit(1);
            }
        }
    } catch (SocketException &e) {
        // Print exception description
        cerr << e.what() << endl;
        exit(1);
    }
}

/**
 * handleConnection
 ** Executes when a new connection is received
 * @param {task} The ThreadTask passed during thread creation
 */
void ASM::handleConnection(ThreadTask *task) {
    // Send received string and receive again until the end of transmission
    char echoBuffer[RCVBUFSIZE];
    while (task->lidar->err >= 0) {
        // Get LIDAR distance
        int distance = task->lidar->getDistance();

        // Encode altitude into string for TCP packet transmission
        sprintf(echoBuffer, "altitude:%d\4", distance);
        task->clientSocket->send(echoBuffer, strlen(echoBuffer));

        // Output debug data
        string sentPacket = echoBuffer;
        cout << "Sent: " << sentPacket << endl;

        // 100 milliseconds (10 Hz)
        usleep(100000); 
    }
}

/**
 * threadMain
 ** Method that executes when pthread_create is called which
 ** controls critical section of handleConnection and deallocates 
 ** socket memory after completion.
 * @param {args} The context passed during thread creation
 */
void *ASM::threadMain(void *args) {
    // Guarantees that thread resources are deallocated upon return  
    pthread_detach(pthread_self()); 

    // Extract socket file descriptor from argument  
    this->handleConnection(((ThreadTask *) args));

    // First cast is to tell delete what type of instance it's deleting
    // Second cast is to tell compilter that there is a clientSocket attribute on args
    delete (TCPSocket *) ((ThreadTask *) args)->clientSocket;

    //return NULL;
}

/**
 * threadMainHelper
 ** Helper method that enables threadMain to get called in 
 ** pthread_create.
 * @param {args} The ThreadTask passed during thread creation.
 */
void *ASM::threadMainHelper(void *args) {
    return ((ASM *)args)->threadMain(args);
}

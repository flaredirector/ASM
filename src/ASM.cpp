/**
 * Flare Director
 * ASM
 * ASM.cpp
 * 
 * This implementation file contains the implementation
 * details for the ASM class.
 */

#include "ASM.h"
#include <iostream>           // For cout, cerr
#include <cstdlib>            // For atoi()  
#include <pthread.h>          // For POSIX threads  
#include <unistd.h>           // For usleep()
#include <signal.h>           // For sigignore()

using namespace std;

/**
 * ASM
 ** Instantiates new ASM instance with port argument.
 * @param {port} The port to send/receive messages on
 */
ASM::ASM(unsigned short int port) {
    // Set port to passed in port number
    this->port = port;

    #ifndef DEBUG
    // Initialize new LIDAR interface
    this->lidar = new LIDARInterface(1);
    #endif
}

/**
 * start
 ** Starts the event loop which listens for new connections
 ** and spwans a new thread for each connection.
 */
void ASM::start(void) {
    // Allows program to continue executing if the program receives
    // a broken pipe signal. 
    sigignore(SIGPIPE); // Do not remove

    TCPServerSocket *serverSocket;
    try {
        // Socket descriptor for server
        serverSocket = new TCPServerSocket(this->port);  
    } catch (SocketException &e) {
        // Print exception description
        cerr << e.what() << endl;
        exit(1);
    }

    #ifndef DEBUG
    // Attempt connection to the LIDAR interface
    int err = this->lidar->connect();
    // Check for error in connecting to the LIDAR interface
    if (err < 0){
        printf("%d", this->lidar->err);
    }
    #endif

    // Run forever  
    for (;;) {      
        cout << "Waiting for Client Connection..." << endl;

        // Create new clientSoket instance and wait for connection
        TCPSocket *clientSocket;
        try {
            // Blocks until new connection received.
            clientSocket = serverSocket->accept();
        } catch (SocketException &e) {
            cerr << e.what() << endl;
            exit(1);
        }

        cout << "Client Connected" << endl;

        // Create new thread task
        ThreadTask *ttask;
        #ifndef DEBUG
        ttask = new ThreadTask(clientSocket, this->lidar);
        #else
        ttask = new ThreadTask(clientSocket);
        #endif

        // Spawn new thread
        pthread_t threadID;
        if (pthread_create(&threadID, NULL, &ASM::threadMainHelper, (void *) ttask) != 0) {
            cerr << "Unable to create client thread" << endl;
            exit(1);
        }
    }
}

/**
 * handleConnection
 ** Executes when a new connection is received
 * @param {task} The ThreadTask passed during thread creation
 */
void ASM::handleConnection(ThreadTask *task) {
    // Send received string and receive again until the end of transmission
    int distance = 0;
    #ifndef DEBUG
    // If not debug, loop unless LIDAR returns error
    while (task->lidar->err >= 0) {
    #else
    // If debug, init distance and loop forever
    distance = 120;
    while (1) {
    #endif
        // If not debug, set distance to LIDAR distance
        #ifndef DEBUG
        distance = task->lidar->getDistance();
        #endif

        // Encode altitude into string for TCP packet transmission
        Message message = Message("altitude", distance);

        // Encode altitude into string for TCP packet transmission
        try {
            task->clientSocket->send(message);
        // On send error, client is most likely disconnected, so exit thread and cleanup
        } catch (SocketException &e) {
            cerr << e.what() << endl;
            pthread_exit(NULL);
        }
        
        #ifdef DEBUG
        // Output debug data
        cout << "Sent: " << message.printableMessage << endl;
        #endif

        // 100 milliseconds (10 Hz)
        usleep(100000); 

        #ifdef DEBUG
        distance--;

        // Reset distance if it drops below 0
        if (distance < 0)
            distance = 120;
        #endif
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

    return NULL;
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

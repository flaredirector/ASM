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
    TCPServerSocket *serverSocket;
    try {
        // Socket descriptor for server
        serverSocket = new TCPServerSocket(this->port);  
    } catch (SocketException &e) {
        // Print exception description
        cerr << e.what() << endl;
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
        // Create separate memory for client argument 
        cout << "Waiting for Client Connection..." << endl;
        TCPSocket *clientSocket;
        try {
            // Blocks until new connection received.
            clientSocket = serverSocket->accept();
        } catch (SocketException &e) {
            cerr << e.what() << endl;
        }

        ThreadTask *ttask;

        #ifndef DEBUG
        // For each new connection, spawn a new thread
        ttask = new ThreadTask(clientSocket, this->lidar);
        #else
        ttask = new ThreadTask(clientSocket);
        #endif

        cout << "Client Connected" << endl;

        pthread_t threadID;
        if (pthread_create(&threadID, NULL, &ASM::threadMainHelper, (void *) ttask) != 0) {
            cerr << "Unable to create thread" << endl;
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
    #ifndef DEBUG
    while (task->lidar->err >= 0) {
        // Encode LIDAR distance into message
        Message message = Message("altitude", task->lidar->getDistance());

        // Encode altitude into string for TCP packet transmission
        task->clientSocket->send(message.encode(), message.length());

        // Output debug data
        // string sentPacket = message->message;
        // cout << "Sent: " << sentPacket << endl;
        printf("Sent: %s", message.message);

        // 100 milliseconds (10 Hz)
        usleep(100000); 
    }
    #else
    int distance = 120;
    while (1) {
        // Encode altitude into string for TCP packet transmission
        Message message = Message("altitude", distance);

        message.encode();

        // Encode altitude into string for TCP packet transmission
        try {
            task->clientSocket->send(message.encode(), message.length());
        } catch (...) {
            cout << "caught exception" << endl;
            //cerr << e.what() << endl;
            pthread_exit(NULL);
        }
        
        // Output debug data
        //cout << "Sent: " << message.printableMessage << endl;

        // 100 milliseconds (10 Hz)
        usleep(100000); 
        distance--;
        if (distance < 0)
            distance = 120;
    }
    #endif
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

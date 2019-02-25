/**
 * Flare Director
 * ASM
 * ASM.cpp
 * 
 * This implementation file contains the implementation
 * details for the ASM class.
 */

#include "ASM.hpp"
#include <iostream>           // For cout, cerr
#include <cstdlib>            // For atoi()  
#include <pthread.h>          // For POSIX threads  
#include <unistd.h>           // For usleep()
#include <signal.h>           // For sigignore()
#include "Events.hpp"

using namespace std;

bool reportingToggle = true;

/**
 * ASM
 ** Instantiates new ASM instance with specified port.
 * @param {port} The port to send/receive messages on
 */
ASM::ASM(unsigned short int port) {
    // Set port to passed in port number
    this->port = port;

    // Setup toggles
    // this->reportingToggle = true;

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
    cout << "Initializing ASM and Sensors..." << endl;
    // Allows program to continue executing if the program receives
    // a broken pipe signal. 
    sigignore(SIGPIPE); // Do not remove

    // TCPServerSocket *serverSocket;
    try {
        // Socket descriptor for server
        this->serverSocket = new TCPServerSocket(this->port);  
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

    // Start listening for connections
    this->listenForConnections(this->serverSocket);    
}

/**
 * listenForConnections
 ** Begins listening for client connections and handling them
 * @param {serverSocket} The TCPServerSocket instance to listen on
 */
void ASM::listenForConnections(TCPServerSocket *serverSocket) {
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

        // Spawn thread for sending client altitude data
        pthread_t firstThreadID;
        if (pthread_create(&firstThreadID, NULL, &ASM::threadMainHelper, (void *) ttask) != 0) {
            cerr << "Unable to create sensor data thread" << endl;
            exit(1);
        }

        // Spawn thread for handling when a client sends a message
        pthread_t secondThreadID;
        if (pthread_create(&secondThreadID, NULL, &ASM::clientMessageHelper, (void *) ttask) != 0) {
            cerr << "Unable to create client message thread" << endl;
            exit(1);
        }
    }
}

/**
 * handleIncomingClientMessage
 ** Determines what to execute based on the received event
 * @param {event} The event received from the client
 * @param {data} The data passed with the event
 */
void ASM::handleEvent(string event, int data) {
    string debugMessage;
    // Decide what to do based on received event
    if (event == CALIBRATION_EVENT) {
        debugMessage = CALIBRATION_EVENT;
    } else if (event == REPORTING_TOGGLE_EVENT) {
        debugMessage = REPORTING_TOGGLE_EVENT;
        //reportingToggle = data ? true : false;
        cout << this->port << endl;
    } else {
        debugMessage = "OTHER";
    }
    cout << "EVENT: " + debugMessage << endl;
}

/**
 * handleIncomingClientMessage
 ** Executes when a new message is received from a client
 * @param {task} The ThreadTask passed during thread creation
 */
void ASM::handleIncomingClientMessage(ThreadTask *task) {
    // Check if there is any received message from client
    try {
        char buffer[RCVBUFSIZE];
        int recvMsgSize;
        while ((recvMsgSize = task->clientSocket->recv(buffer, RCVBUFSIZE)) > 0) { // Zero means end of transmission
            // Convert into string for easier use
            string receivedMessage = buffer; 

            cout << "RECEIVED MESSAGE: " << receivedMessage << endl;

            // Decode message string into message object
            Message message = Message(receivedMessage);

            // Pass message context to event handler
            //this->handleEvent(message.event, message.data);
            string event = message.event;
            int data = message.data;
            string debugMessage;
            // Decide what to do based on received event
            if (event == CALIBRATION_EVENT) {
                debugMessage = CALIBRATION_EVENT;
            } else if (event == REPORTING_TOGGLE_EVENT) {
                debugMessage = REPORTING_TOGGLE_EVENT;
                //reportingToggle = data ? true : false;
                //cout << this->port << endl;
            } else {
                debugMessage = "OTHER";
            }
            cout << "EVENT: " + debugMessage << endl;
        }
    } catch (SocketException &e) {
        cout << e.what() << endl;
        //pthread_exit(NULL); 
    }
}

/**
 * handleConnection
 ** Executes when a new connection is received
 * @param {task} The ThreadTask passed during thread creation
 */
void ASM::sendAltitudeDataTask(ThreadTask *task) {
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
    if (reportingToggle) {
        // If not debug, set distance to LIDAR distance
        #ifndef DEBUG
        distance = task->lidar->getDistance();
        #endif

        // Encode altitude into string for TCP packet transmission
        Message message = Message(ALTITUDE_EVENT, distance);

        // Encode altitude into string for TCP packet transmission
        try {
            task->clientSocket->send(message);
        // On send error, client is most likely disconnected, so exit thread and cleanup
        } catch (SocketException &e) {
            cerr << e.what() << endl;
            pthread_exit(NULL);
        }
        
        // Output debug data
        cout << "Sent: " << message.printableMessage << endl;

        #ifdef DEBUG
        distance--;

        // Reset distance if it drops below 0
        if (distance < 0)
            distance = 120;
        #endif
    }

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
    this->sendAltitudeDataTask(((ThreadTask *) args));

    // First cast is to tell delete what type of instance it's deleting
    // Second cast is to tell compilter that there is a clientSocket attribute on args
    // if ((TCPSocket *) ((ThreadTask *) args)->clientSocket != NULL)
       // delete (TCPSocket *) ((ThreadTask *) args)->clientSocket;

    return NULL;
}

/**
 * clientMessage
 ** Method that executes when pthread_create is called which
 ** controls critical section of handleIncomingClientMessage and deallocates 
 ** socket memory after completion.
 * @param {args} The context passed during thread creation
 */
void *ASM::clientMessage(void *args) {
    // Guarantees that thread resources are deallocated upon return  
    pthread_detach(pthread_self()); 

    // Extract socket file descriptor from argument  
    this->handleIncomingClientMessage(((ThreadTask *) args));

    // First cast is to tell delete what type of instance it's deleting
    // Second cast is to tell compilter that there is a clientSocket attribute on args
    //delete (TCPSocket *) ((ThreadTask *) args)->clientSocket;

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

/**
 * clientMessageHelper
 ** Helper method that enables clientMessage to get called in 
 ** pthread_create.
 * @param {args} The ThreadTask passed during thread creation.
 */
void *ASM::clientMessageHelper(void *args) {
    return ((ASM *)args)->clientMessage(args);
}

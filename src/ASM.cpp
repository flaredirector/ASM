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
    cout << "Initializing ASM and Sensors..." << endl;

    // Set port to passed in port number
    this->port = port;

    // Create new altitude provider instance
    this->altitudeProvider = new AltitudeProvider();
}

/**
 * start
 ** Initializes new TCP server, starts the altitude data
 ** acquisition thread, and begins listening for new connections.
 */
void ASM::start(void) {
    cout << "Starting ASM..." << endl;

    try {
        // Create new socket descriptor for server
        this->serverSocket = new TCPServerSocket(this->port);  
    } catch (SocketException &e) {
        // Print exception description
        cerr << e.what() << endl;
        exit(1);
    }

    // Start the sensor data acquisition thread
    pthread_t altitudeProviderThreadID;
    if (pthread_create(&altitudeProviderThreadID, NULL, 
        &ASM::acquireAltitudeDataThreadHelper, (void *) this->altitudeProvider) != 0) {
        cerr << "Unable to create acquireAltitudeDataThreadHelper thread" << endl;
        exit(1);
    }

    // Start listening for connections
    this->listenForConnections();    
}

/**
 * listenForConnections
 ** Begins listening for client connections and spawns new threads for
 ** each connection.
 */
void ASM::listenForConnections() {
    // Run forever  
    for (;;) {      
        cout << "Waiting for Client Connection..." << endl;

        // Create new clientSoket instance and wait for connection
        TCPSocket *clientSocket;
        try {
            // Blocks until new connection received.
            clientSocket = this->serverSocket->accept();
        } catch (SocketException &e) {
            cerr << e.what() << endl;
            exit(1);
        }

        cout << "Client Connected" << endl;

        // Create new thread task
        ThreadContext *ctx = new ThreadContext(clientSocket, this->altitudeProvider);

        // Spawn thread for sending client altitude data
        pthread_t firstThreadID;
        if (pthread_create(&firstThreadID, NULL, &ASM::reportAltitudeThreadHelper, (void *) ctx) != 0) {
            cerr << "Unable to create reportAltitudeThreadHelper thread" << endl;
            exit(1);
        }

        // Spawn thread for handling when a client sends a message
        pthread_t secondThreadID;
        if (pthread_create(&secondThreadID, NULL, &ASM::handleClientMessageThreadHelper, (void *) ctx) != 0) {
            cerr << "Unable to create handleClientMessageThreadHelper thread" << endl;
            exit(1);
        }
    }
}

/**
 * handleEvent
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
        reportingToggle = data ? true : false;
    } else {
        debugMessage = "OTHER";
    }

    cout << "EVENT: " + debugMessage << endl;
}

/**
 * handleClientMessage
 ** Executes when a new message is received from a client
 * @param {ctx} The ThreadContext passed during thread creation
 */
void ASM::handleClientMessage(ThreadContext *ctx) {
    // Check if there is any received message from client
    try {
        char buffer[RCVBUFSIZE];
        int recvMsgSize;
        while ((recvMsgSize = ctx->clientSocket->recv(buffer, RCVBUFSIZE)) > 0) { // Zero means end of transmission
            // Convert into string for easier use
            string receivedMessage = buffer; 

            cout << "RECEIVED MESSAGE: " << receivedMessage << endl;

            // Decode message string into message object
            Message *message = new Message(receivedMessage);

            // Loop through the parsed events in the decoded message and determine what to do
            // for each event.
            for (int i = 0; i < message->parsedEvents.size(); i++) {
                Event parsedEvent = message->parsedEvents[i];
                this->handleEvent(parsedEvent.event, parsedEvent.data);
            }
        }
    } catch (SocketException &e) {
        cout << e.what() << endl;
    }
}

/**
 * reportAltitude
 ** Executes when a new connection is received
 * @param {ctx} The ThreadTask passed during thread creation
 */
void ASM::reportAltitude(ThreadContext *ctx) {
    for (;;) {
        if (reportingToggle) {
            // Get altitude data from provider
            int altitude = ctx->altitudeProvider->getAltitude();
            
            // Encode altitude into message for transmission
            Message *message = new Message(ALTITUDE_EVENT, altitude);
            message->addEvent(LIDAR_DATA_EVENT, altitude + 12);
            message->addEvent(SONAR_DATA_EVENT, altitude + 4);
            message->encode();

            // Try sending message over connection
            try {
                ctx->clientSocket->send(message);
            // On send error, client is most likely disconnected, so exit thread and cleanup
            } catch (SocketException &e) {
                cerr << e.what() << endl;
                return;
            }

            // Free up message memory to prevent memory leak
            delete message;

            // Output debug data
            // cout << "Sent: " << message->message << endl;
        }

        // 100 milliseconds (10 Hz)
        usleep(100000); 
    }
}

/**
 * reportAltitudeCS
 ** Method that executes when pthread_create is called which
 ** controls critical section of handleConnection and deallocates 
 ** socket memory after completion.
 * @param {ctx} The context passed during thread creation
 */
void *ASM::reportAltitudeCS(void *ctx) {
    // Guarantees that thread resources are deallocated upon return  
    pthread_detach(pthread_self()); 

    // Extract socket file descriptor from argument  
    this->reportAltitude(((ThreadContext *) ctx));

    // First cast is to tell delete what type of instance it's deleting
    // Second cast is to tell compilter that there is a clientSocket attribute on args
    // delete (TCPSocket *) ((ThreadTask *) args)->clientSocket;
    return NULL;
}

/**
 * handleClientMessageCS
 ** Method that executes when pthread_create is called which
 ** controls critical section of handleIncomingClientMessage and deallocates 
 ** socket memory after completion.
 * @param {ctx} The context passed during thread creation
 */
void *ASM::handleClientMessageCS(void *ctx) {
    // Guarantees that thread resources are deallocated upon return  
    pthread_detach(pthread_self()); 

    // Extract socket file descriptor from argument  
    this->handleClientMessage(((ThreadContext *) ctx));

    // First cast is to tell delete what type of instance it's deleting
    // Second cast is to tell compilter that there is a clientSocket attribute on args
    // delete (TCPSocket *) ((ThreadTask *) args)->clientSocket;
    return NULL;
}

/**
 * acquireAltitudeDataCS
 ** Method that executes when pthread_create is called which
 ** controls critical section of the ALtitudeProvider instance.
 * @param {ctx} The context passed during thread creation.
 */
void *ASM::acquireAltitudeDataCS(void *ctx) {
    // Guarantees that thread resources are deallocated upon return 
    pthread_detach(pthread_self());

    // Start the sensor data acquisition loop
    ((AltitudeProvider *) ctx)->acquireDataLoop();

    return NULL;
}

/**
 * reportAltitudeThreadHelper
 ** Helper method that enables threadMain to get called in 
 ** pthread_create.
 * @param {args} The ThreadContext passed during thread creation.
 */
void *ASM::reportAltitudeThreadHelper(void *ctx) {
    return ((ASM *)ctx)->reportAltitudeCS(ctx);
}

/**
 * handleClientMessageThreadHelper
 ** Helper method that enables clientMessage to get called in 
 ** pthread_create.
 * @param {args} The ThreadContext passed during thread creation.
 */
void *ASM::handleClientMessageThreadHelper(void *ctx) {
    return ((ASM *)ctx)->handleClientMessageCS(ctx);
}

/**
 * acquireAltitudeDataThreadHelper
 ** Helper method that enables startAcquiringAltitudeData to 
 ** get called in pthread_create.
 * @param {ctx} The AltitudeProvider instance passed during thread creation.
 */
void *ASM::acquireAltitudeDataThreadHelper(void *ctx) {
    return ((ASM *)ctx)->acquireAltitudeDataCS(ctx);
}

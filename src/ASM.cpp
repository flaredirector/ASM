/**
 * Flare Director
 * ASM
 * ASM.cpp
 *
 * This implementation file contains the implementation
 * details for the ASM class.
 */

#include "ASM.hpp"
#include "Events.hpp"
#include "../lib/gpio/errorCodeDisplay/ErrorCodeDisplay.hpp"
#include "../lib/gpio/led/LEDInterface.hpp"
#include <iostream>           // For cout, cerr
#include <pthread.h>          // For POSIX threads  
#include <unistd.h>           // For usleep()

#define MILLISECONDS *1000

using namespace std;

int currentColor = LED_RED;

/**
 * ASM
 ** Instantiates new ASM instance with specified port.
 * @param {port} The port to send/receive messages on
 */
ASM::ASM(unsigned short int port) {
    cout << "Initializing ASM and Sensors..." << endl;

    // Set port to passed in port number
    this->port = port;

    // Setup toggles
    this->toggles = (ASMToggles *) malloc(sizeof(ASMToggles));
    this->toggles->reportingToggle = false;
    this->toggles->dataLoggingToggle = false;
    this->toggles->hasBeenCalibrated = false;
    this->toggles->ledFlashing = false;

    // Setup the GPIO pins for 7SD and LEDs
    ErrorCodeDisplay::setupPins();
    LEDInterface::setupPins();

    // Display initial values
    ErrorCodeDisplay::display(SSD_NOTHING);
    LEDInterface::setColor(LED_RED);

    // Instantiate and setup new battery interface
    this->battery = new BatteryInterface();
    int batteryError = this->battery->setup();
    if (batteryError != 0) {
        ErrorCodeDisplay::display(5);
        cout << "Battery Interface Error: " << batteryError << endl;
    }

    // Create new altitude provider instance
    this->altitudeProvider = new AltitudeProvider(this->toggles);
}

/**
 * start
 ** Initializes new TCP server, starts the altitude data
 ** acquisition thread, and begins listening for new connections.
 */
void ASM::start() {
    // Start the LED thread for flashing the LED
   pthread_t ledThreadId;
   if (pthread_create(&ledThreadId, NULL, 
       &ASM::ledFlashingThreadHelper, (void *) this->toggles) != 0) {
       cout << "Cant create led thread" << endl;
   }

    try {
        // Create new socket descriptor for server
        this->serverSocket = new TCPServerSocket(this->port);  
    } catch (SocketException &e) {
        // Print exception description
        ErrorCodeDisplay::display(2);
        cerr << e.what() << endl;
        exit(1);
    }

    // Start the sensor data acquisition thread
    pthread_t altitudeProviderThreadID;
    if (pthread_create(&altitudeProviderThreadID, NULL, 
        &ASM::acquireAltitudeDataThreadHelper, (void *) this->altitudeProvider) != 0) {
        ErrorCodeDisplay::display(9);
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
    this->toggles->ledFlashing = true;
    LEDInterface::setColor(LED_GREEN);
    currentColor = LED_GREEN;
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
            continue;
        }

        cout << "Client Connected" << endl;

        // Create new thread context
        ThreadContext *ctx = new ThreadContext(clientSocket, this->altitudeProvider, this->toggles, this->battery);

        // Spawn thread for sending client altitude data
        pthread_t firstThreadID;
        if (pthread_create(&firstThreadID, NULL, &ASM::reportAltitudeThreadHelper, (void *) ctx) != 0) {
            ErrorCodeDisplay::display(9);
            cerr << "Unable to create reportAltitudeThreadHelper thread" << endl;
            exit(1);
        }

        // Spawn thread for handling when a client sends a message
        pthread_t secondThreadID;
        if (pthread_create(&secondThreadID, NULL, &ASM::handleClientMessageThreadHelper, (void *) ctx) != 0) {
            ErrorCodeDisplay::display(9);
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
 * @param {ctx} The current ThreadContext
 */
void ASM::handleEvent(string event, int data, ThreadContext *ctx) {
    // Status reply message to send back to the client
    Message *statusReply;

    // Decide what to do based on received event
    if (event == CALIBRATION_EVENT) {
        cout << "Starting calibration..." << endl;
        LEDInterface::setColor(LED_YELLOW);
        currentColor = LED_YELLOW;
        ctx->toggles->ledFlashing = true;
	    int returnCode = ctx->altitudeProvider->calibrate();
        statusReply = new Message(CALIBRATION_STATUS_EVENT, returnCode);
        if (returnCode < 0) {
            ErrorCodeDisplay::display(3);
            LEDInterface::setColor(LED_RED);
            currentColor = LED_RED;
            ctx->toggles->ledFlashing = true;
        } else {
            ctx->toggles->ledFlashing= false;
            LEDInterface::setColor(LED_GREEN);
            currentColor = LED_GREEN;
        }
    } else if (event == REPORTING_TOGGLE_EVENT) {
        cout << "Toggling reporting..." << endl;
        ctx->toggles->reportingToggle = data ? true : false;
	if (ctx->toggles->reportingToggle) {
	    LEDInterface::setColor(LED_BLUE);
	    currentColor = LED_BLUE;
	    ctx->toggles->ledFlashing = true;
	} else {
	    ctx->toggles->ledFlashing = false;
	    LEDInterface::setColor(LED_GREEN);
	    currentColor = LED_GREEN;
	}
        statusReply = new Message(REPORTING_STATUS_EVENT, ctx->toggles->reportingToggle ? 1 : 0);
    } else if (event == GET_STATUS_EVENT) {
        cout << "Sending system status..." << endl;
        statusReply = new Message(LIDAR_STATUS_EVENT, ctx->altitudeProvider->lidar->err);
        statusReply->addEvent(SONAR_STATUS_EVENT, ctx->altitudeProvider->sonar->err);
        statusReply->addEvent(REPORTING_STATUS_EVENT, ctx->toggles->reportingToggle ? 1 : 0);
        statusReply->addEvent(DATA_LOGGING_STATUS_EVENT, ctx->toggles->dataLoggingToggle ? 1 : 0);
        statusReply->addEvent(CALIBRATION_STATUS_EVENT, ctx->toggles->hasBeenCalibrated ? 0 : 1);
        statusReply->addEvent(BATTERY_STATUS_EVENT, ctx->battery->getPercentage());
    } else if (event == DATA_LOGGING_TOGGLE_EVENT) {
        cout << "Toggling data logging..." << endl;
        ctx->toggles->dataLoggingToggle = data ? true : false;
        ctx->altitudeProvider->dataFile << "LIDAR, SONAR, ALTITUDE" << endl;
        statusReply = new Message(DATA_LOGGING_STATUS_EVENT, ctx->toggles->dataLoggingToggle ? 1 : 0);
    } else {
        cout << "Parsed unrecognized event" << endl;
    }

    // Try sending message over connection
    try {
        ctx->clientSocket->send(statusReply);
        delete statusReply;
    } catch (SocketException &e) {
        cout << e.what() << " in handleEvent()" << endl;
    }

    return;
}

/**
 * handleClientMessage
 ** Executes when a new message is received from a client
 * @param {ctx} The ThreadContext passed during thread creation
 */
void ASM::handleClientMessage(ThreadContext *ctx) {
    // Check if there is any received message from client
    try {
        char buffer[BUFSIZE];
        int recvMsgSize;
        while ((recvMsgSize = ctx->clientSocket->recv(buffer, BUFSIZE)) > 0) { // Zero means end of transmission
            // Convert into string for easier use
            string bufferString = buffer;

            cout << "RECEIVED MESSAGE: " << bufferString << endl;

            // Decode message string into message object
            Message *message = new Message(bufferString);

            // Loop through the parsed events in the decoded message and determine what to do
            // for each event.
            for (int i = 0; i < message->events.size(); i++) {
                Event parsedEvent = message->events[i];
                this->handleEvent(parsedEvent.event, parsedEvent.data, ctx);
            }

            delete message;

            // Flush the input message buffer
            memset(buffer, '\0', BUFSIZE);
        }

        // Set ThreadContext socketIsAlive boolean to false to trigger the exit of the
        // reportAltitude thread.
        ctx->socketIsAlive = false;
    } catch (SocketException &e) {
        cout << e.what() << endl;
        ctx->socketIsAlive = false;
        return;
    }
}

/**
 * reportAltitude
 ** Executes when a new connection is received
 * @param {ctx} The ThreadTask passed during thread creation
 */
void ASM::reportAltitude(ThreadContext *ctx) {
    // While the ThreadContext socketIsAlive toggle is set to true, run the loop.
    // The loop will exit when socketIsAlive is set to false by the handleClientMessage
    // thread when a client disconnects.
    int counter = 0;
    int le = 0, se = 0;
    while (ctx->socketIsAlive) {
        if (ctx->toggles->reportingToggle && ctx->toggles->hasBeenCalibrated) {
            // Encode altitude into message for transmission
            Message *message;
            int altitude = ctx->altitudeProvider->getAltitude();
            if (altitude == -1) {
                message = new Message(LIDAR_DATA_EVENT, ctx->altitudeProvider->lidarDistance);
                message->addEvent(SONAR_DATA_EVENT, ctx->altitudeProvider->sonarDistance);
            } else {
                message = new Message(ALTITUDE_EVENT, ctx->altitudeProvider->getAltitude());
                message->addEvent(LIDAR_DATA_EVENT, ctx->altitudeProvider->lidarDistance);
                message->addEvent(SONAR_DATA_EVENT, ctx->altitudeProvider->sonarDistance);
            }

            // Check sensor error codes and report if the error code has changed
            if (le != ctx->altitudeProvider->lidar->err || se != ctx->altitudeProvider->sonar->err) {
                message->addEvent(LIDAR_STATUS_EVENT, ctx->altitudeProvider->lidar->err);
                message->addEvent(SONAR_STATUS_EVENT, ctx->altitudeProvider->sonar->err);

                if (ctx->altitudeProvider->lidar->err < 0 || ctx->altitudeProvider->sonar->err < 0) {
                    ErrorCodeDisplay::display(1);
                    ctx->toggles->ledFlashing = false;
                    LEDInterface::setColor(LED_RED);
                    currentColor = LED_RED;
                    ctx->toggles->ledFlashing = true;
                } else if (ctx->altitudeProvider->lidar->err == 0 && ctx->altitudeProvider->sonar->err == 0) {
                    LEDInterface::setColor(LED_BLUE);
                    currentColor = LED_BLUE;
                    ctx->toggles->ledFlashing = true;
                }
                le = ctx->altitudeProvider->lidar->err;
                se = ctx->altitudeProvider->sonar->err;
            }

            // Every 20 seconds, send along a battery status
            if (counter == 200) {
                int bp = ctx->battery->getPercentage();
                if (bp < 20)
                    ErrorCodeDisplay::display(4);
                message->addEvent(BATTERY_STATUS_EVENT, bp);
                counter = 0;
            }

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
        // If reporting isn't on, send battery status every 20 seconds.
        } else {
            if (counter == 200) {
                // Encode battery level into message
                int bp = ctx->battery->getPercentage();
                if (bp < 20)
                    ErrorCodeDisplay::display(4);
                Message *m = new Message(BATTERY_STATUS_EVENT, );

                // Try sending message over connection
                try {
                    ctx->clientSocket->send(m);
                } catch (SocketException &e) {
                    cerr << e.what() << endl;
                    return;
                }

                // Free up message memory
                delete m;
                counter = 0;
            }
        }
        counter++;
        // 100 milliseconds (10 Hz)
        usleep(100 MILLISECONDS);
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
    if (pthread_detach(pthread_self()) < 0) {
        cout << "Unable to detach thread" << endl;
        return NULL;
    } 

    // Extract socket file descriptor from argument  
    this->reportAltitude(((ThreadContext *) ctx));

    cout << "Exiting reportAltitude thread..." << endl;
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
    if (pthread_detach(pthread_self()) < 0) {
        cout << "Unable to detach thread" << endl;
        return NULL;
    }

    // Extract socket file descriptor from argument  
    this->handleClientMessage(((ThreadContext *) ctx));

    // First cast is to tell delete what type of instance it's deleting
    // Second cast is to tell compilter that there is a clientSocket attribute on args
    delete (TCPSocket *) ((ThreadContext *) ctx)->clientSocket;

    cout << "Exiting handleClientMessage thread..." << endl;
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
    if (pthread_detach(pthread_self()) < 0) {
        cout << "Unable to detach thread" << endl;
        return NULL;
    }

    // Start the sensor data acquisition loop
    ((AltitudeProvider *) ctx)->acquireDataLoop();

    return NULL;
}

/**
 * ledFlashingThreadCS
 ** Method that executes when pthread_create is called which
 ** controls the flashing of the satus LED.
 * @param {ctx} The context passed during thread creation.
 */
void *ASM::ledFlashingThreadCS(void *ctx) {
    // Guarantees that thread resources are deallocated upon return 
    if (pthread_detach(pthread_self()) < 0) {
	    cout << "Unable to detach thread" << endl;
	    return NULL;
    }

    // Loop forever
    for (;;) {
        // Get current color of the status LED
        usleep(500 MILLISECONDS);
        if (((ASMToggles *) ctx)->ledFlashing) {
           LEDInterface::setColor(LED_OFF);
           usleep(500 MILLISECONDS);
           LEDInterface::setColor(currentColor);
       }
   }

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

/**
 * ledFlashingThreadHelper
 ** Helper method that enables ledFlashingThreadCS to 
 ** get called in pthread_create.
 * @param {ctx} The ASMToggles pointer passed during thread creation.
 */
void *ASM::ledFlashingThreadHelper(void *ctx) {
    return ((ASM *)ctx)->ledFlashingThreadCS(ctx);
}

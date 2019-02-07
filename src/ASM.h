/**
 * Flare Director
 * ASM
 * ASM.h
 * 
 * This header file contains the class, property, and method
 * definitions for the ASM class.
*/

#include <iostream>           // For cout, cerr
#include <cstdlib>            // For atoi()  
#include <pthread.h>          // For POSIX threads  
#include <unistd.h>           // For usleep()
#include "../lib/network/NetworkSocket.h"  // For Sockets
#include "../lib/sensor/lidar/LIDARInterface.h" // For Lidar interface

/**
 * @class ThreadTask
 * 
 * The ThreadTask class contains the necessary properties
 * to pass to pthread_create so that the thread can access
 * important class instances such as the sensors and newly
 * created TCP socket.
 */
class ThreadTask {
    public:
    LIDARInterface *lidar;
    TCPSocket *clientSocket;
    ThreadTask(TCPSocket *cs, LIDARInterface *l);
};

/**
 * @class ASM
 * 
 * The ASM class is the super class
 * responsible for initializing and managing the Sensor Module 
 * logic. 
 */
class ASM {
    public:
        ASM(unsigned short int port);
        ~ASM();
        void start(void);
    
    private:
        LIDARInterface *lidar;
        int port;
        void handleConnection(ThreadTask*);
        void *threadMain(void *);
        static void *threadMainHelper(void *clientSocket);
};
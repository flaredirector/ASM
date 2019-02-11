/**
 * Flare Director
 * ASM
 * ThreadTask.h
 * 
 * This header file contains the class, property, and method
 * definitions for the ThreadTask class.
*/

#include "../lib/network/NetworkSocket.h"  // For Sockets
#include "../lib/sensor/lidar/LIDARInterface.h" // For Lidar interface

/**
 * @class ThreadTask
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
    ThreadTask(TCPSocket *cs);
};
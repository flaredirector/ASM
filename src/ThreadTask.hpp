/**
 * Flare Director
 * ASM
 * ThreadTask.hpp
 * 
 * This header file contains the class, property, and method
 * definitions for the ThreadTask class.
*/

#include "../lib/network/NetworkSocket.hpp"  // For Sockets
#include "AltitudeProvider.hpp"

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
    AltitudeProvider *altitudeProvider;
    ThreadTask(TCPSocket *cs, LIDARInterface *l);
    ThreadTask(TCPSocket *cs);
    ThreadTask(TCPSocket *cs, AltitudeProvider *ap);
};
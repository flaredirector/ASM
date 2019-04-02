/**
 * Flare Director
 * ASM
 * ThreadContext.hpp
 * 
 * This header file contains the class, property, and method
 * definitions for the ThreadContext class.
*/

#include "../lib/network/NetworkSocket.hpp"  // For Sockets
#include "../lib/gpio/battery/BatteryInterface.hpp"
#include "AltitudeProvider.hpp"

/**
 * @class ThreadContext
 * The ThreadContext class contains the necessary properties
 * to pass to pthread_create so that the thread can access
 * important class instances such as the sensors and newly
 * created TCP socket.
 */
class ThreadContext {
    public:
    TCPSocket *clientSocket;
    AltitudeProvider *altitudeProvider;
    ASMToggles *toggles;
    BatteryInterface *battery;
    ThreadContext(TCPSocket *cs, AltitudeProvider *ap, ASMToggles *t, BatteryInterface *b);
    bool socketIsAlive;
};
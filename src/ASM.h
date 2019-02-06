/*
 * Flare Director
 * ASM
 * ASM.h
 * 
*/

#include <iostream>           // For cout, cerr
#include <cstdlib>            // For atoi()  
#include <pthread.h>          // For POSIX threads  
#include <unistd.h>           // For usleep()
#include "../lib/network/NetworkSocket.h"  // For Socket, ServerSocket, and SocketException
#include "../lib/sensor/lidar/LIDARInterface.h"

using namespace std;

class ASM {
    public:
    ASM(const unsigned short port);
    ~ASM();

    private:
    void handleConnection(TCPSocket *socket);
    void *threadMain(void *clientSocket);
    LIDARInterface lidar;
}
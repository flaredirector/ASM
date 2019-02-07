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

class ThreadTask {
    public:
    LIDARInterface *lidar;
    TCPSocket *clientSocket;
    ThreadTask(TCPSocket *cs, LIDARInterface *l);
};

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
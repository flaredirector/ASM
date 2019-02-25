/**
 * Flare Director
 * ASM
 * ASM.hpp
 * 
 * This header file contains the class, property, and method
 * definitions for the ASM class.
*/

#include "ThreadTask.hpp"

// 32 byte message buffer size
#define RCVBUFSIZE 32    

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
        TCPServerSocket *serverSocket;
    
    private:
        LIDARInterface *lidar;
        int port;

        void handleEvent(string event, int data);
        void listenForConnections(TCPServerSocket *serverSocket);

        void sendAltitudeDataTask(ThreadTask*);
        void handleIncomingClientMessage(ThreadTask*);

        void *threadMain(void *);
        void *clientMessage(void *);

        static void *threadMainHelper(void *args);
        static void *clientMessageHelper(void *args);
};
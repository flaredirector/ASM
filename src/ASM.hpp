/**
 * Flare Director
 * ASM
 * ASM.hpp
 * 
 * This header file contains the class, property, and method
 * definitions for the ASM class.
*/

#include "ThreadContext.hpp"

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
        AltitudeProvider *altitudeProvider;
        int port;

        void handleEvent(string event, int data);
        void listenForConnections(TCPServerSocket *serverSocket);

        void reportAltitude(ThreadContext*);
        void handleClientMessage(ThreadContext*);

        void *reportAltitudeCS(void *ctx);
        void *handleClientMessageCS(void *ctx);
        void *acquireAltitudeDataCS(void *ctx);

        static void *reportAltitudeThreadHelper(void *ctx);
        static void *handleClientMessageThreadHelper(void *ctx);
        static void *acquireAltitudeDataThreadHelper(void *ctx);
};
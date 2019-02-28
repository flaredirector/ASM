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

        void sendAltitudeDataTask(ThreadContext*);
        void handleIncomingClientMessage(ThreadContext*);

        void *threadMain(void *);
        void *clientMessage(void *);
        void *startAcquiringAltitudeData(void *);

        static void *threadMainHelper(void *args);
        static void *clientMessageHelper(void *args);
        static void *altitudeProviderHelper(void *args);
};
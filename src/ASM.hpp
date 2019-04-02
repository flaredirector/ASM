/**
 * Flare Director
 * ASM
 * ASM.hpp
 * 
 * This header file contains the class, property, and method
 * definitions for the ASM class.
*/

#include "ThreadContext.hpp" 

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
        void start();
        TCPServerSocket *serverSocket;
 	    AltitudeProvider *altitudeProvider;   
        BatteryInterface *battery;
    private:
        int port;
        ASMToggles *toggles;

        void handleEvent(string event, int data, ThreadContext *ctx);
        void listenForConnections();

        void reportAltitude(ThreadContext*);
        void handleClientMessage(ThreadContext*);

        void *reportAltitudeCS(void *ctx);
        void *handleClientMessageCS(void *ctx);
        void *acquireAltitudeDataCS(void *ctx);

        static void *reportAltitudeThreadHelper(void *ctx);
        static void *handleClientMessageThreadHelper(void *ctx);
        static void *acquireAltitudeDataThreadHelper(void *ctx);
};

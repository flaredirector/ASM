/**
 * Flare Director
 * ASM
 * Message.hpp
 * 
 * This header file contains the class, property, and method
 * definitions for the Message class.
*/
#include <string>

// 32 byte message buffer size
#define BUFSIZE 32    

/**
 * @class ASM
 * 
 * The ASM class is the super class
 * responsible for initializing and managing the Sensor Module 
 * logic. 
 */
class Message {
    public:
        Message(std::string event, int data);
        Message(std::string receivedMessage);
        void encode();
        std::string message;
        std::string event;
        int data;
        int messageLength;
};

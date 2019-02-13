/**
 * Flare Director
 * ASM
 * Message.hpp
 * 
 * This header file contains the class, property, and method
 * definitions for the Message class.
*/

#include <iostream>
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
        char *encode();
        int length();
        char *message;
        std::string printableMessage;
    
    private:
        std::string event;
        int data;
};
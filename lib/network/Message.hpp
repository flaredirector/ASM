/**
 * Flare Director
 * ASM
 * Message.hpp
 * 
 * This header file contains the class, property, and method
 * definitions for the Message class.
*/
#include <string>
#include <vector>

// 32 byte message buffer size
#define BUFSIZE 64

typedef struct event {
    std::string event;
    int data;
} Event;

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
        void addEvent(std::string event, int data);
        std::string message;
        int messageLength;
        std::vector<std::string> events;
        std::vector<Event> parsedEvents;
};

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

// Message buffer size
#define BUFSIZE 64

/**
 * Event structure to hold event name and data
 */
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
        void addEvent(std::string event, int data);
        void encode();
        std::string message;
        std::vector<Event> events;
};

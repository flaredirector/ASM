/**
 * Flare Director
 * ASM
 * ASM.cpp
 * 
 * This implementation file contains the implementation
 * details for the ASM class.
 */

#include "Message.hpp"
#include <string.h>
#include <iostream>

using namespace std;

/**
 * Message
 ** Instantiates new Message instance with arguments.
 * @param {event} The event string to encode in the message
 * @param {data} The data to encode in the message
 */
Message::Message(string event, int data) {
    this->event = event;
    this->data = data;
    this->encode();
}

/**
 * encode
 ** Encodes the event string and data into a transmittable message
 */
char * Message::encode() {
    char message[BUFSIZE];
    sprintf(message, "%s:%d\4", this->event.c_str(), this->data);
    this->message = message;
    string pm = this->message;
    this->printableMessage = pm;
    return this->message;
}

/**
 * length
 ** Returns the length of the encoded message
 */
int Message::length() {
    return strlen(this->message);
}
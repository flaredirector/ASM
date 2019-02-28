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
 * Message
 ** Instantiates new Message instance with received client message buffer
 ** and parses the buffer into an event and data.
 * @param {receivedMessage} The string received from the client
 */
Message::Message(string receivedMessage) {
    string event, data, delimeter = ":";
    size_t pos = 0;
    while ((pos = receivedMessage.find(delimeter)) != string::npos) {
        // Assign event
        event = receivedMessage.substr(0, pos);
        receivedMessage.erase(0, pos + delimeter.length());
    }
    // Assgin data
    data = receivedMessage;
    
    this->event = event;
    this->data = stoi(data);
    this->encode();
}

/**
 * encode
 ** Encodes the event string and data into a transmittable message
 * @return {char*} The encoded message
 */
void Message::encode() {
    char packet[BUFSIZE];
    sprintf(packet, "%s:%d\4", this->event.c_str(), this->data);
    this->messageLength = strlen(packet);
    string pm = packet;
    this->message = pm;
}

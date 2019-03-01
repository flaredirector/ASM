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
    this->addEvent(event, data);
    this->encode();
}

/**
 * Message
 ** Instantiates new Message instance with received client message buffer
 ** and parses the buffer into an event and data.
 * @param {receivedMessage} The string received from the client
 */
Message::Message(string receivedMessage) {
    string messageDelimeter = "|";
    size_t pos = 0;
    while ((pos = receivedMessage.find(messageDelimeter)) != string::npos) {
        this->events.push_back(receivedMessage.substr(0, pos));
        receivedMessage.erase(0, pos + messageDelimeter.length());
    }

    this->events.push_back(receivedMessage);

    for (int i = 0; i < this->events.size(); i++) {
        string event, data, eventDelimeter = ":";
        size_t pos = 0;
        while ((pos = this->events[i].find(eventDelimeter)) != string::npos) {
            // Assign event
            event = this->events[i].substr(0, pos);
            this->events[i].erase(0, pos + eventDelimeter.length());
        }
        // Assgin data
        data = this->events[i];

        Event newEvent;
        newEvent.event = event;

        newEvent.data = stoi(data);

        this->parsedEvents.push_back(newEvent);
    }
}

void Message::addEvent(string event, int data) {
    Event newEvent;
    newEvent.event = event;
    newEvent.data = data;

    this->parsedEvents.push_back(newEvent);
    this->encode();
}

/**
 * encode
 ** Encodes the event string and data into a transmittable message
 * @return {char*} The encoded message
 */
void Message::encode() {
    string packet;
    for (int i = 0; i < this->parsedEvents.size(); i++) {
        packet += this->parsedEvents[i].event;
        packet += ":";
        packet += to_string(this->parsedEvents[i].data);
        if (i != this->parsedEvents.size()-1)
            packet += "|";
    }
    packet[packet.size()] = '\4';
    this->message = packet;
    this->messageLength = packet.size();
}

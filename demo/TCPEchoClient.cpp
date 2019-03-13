/*-
 * Flare Director
 * ASM
 * TCPEchoClient.cpp
 * 
 */

#include "../lib/network/NetworkSocket.hpp"  // For Socket and SocketException
#include <iostream>           // For cerr and cout
#include <stdlib.h>
#include <unistd.h>

using namespace std;

void handleEvent(string event, int data) {
	if (event == "calibrationSuccessful") {
		cout << "Calibration Successful!" << endl;
	} else if (event == "altitude") {
		cout << "Altitude: " << data << endl;
	}
}

int main(int argc, char *argv[]) {
	if ((argc < 1) || (argc > 3)) {     // Test for correct number of arguments
		cerr << "Usage: " << argv[0] << " <Server Port> [<Server>]" << endl;
		exit(1);
	}

	string serverAddress = (argc == 3) ? argv[2] : "127.0.0.1"; // First arg: server address
	unsigned short echoServerPort = (argc >= 2) ? atoi(argv[1]) : 4000;

	try {
		// Establish connection with the echo server
		TCPSocket *socket = new TCPSocket(serverAddress, echoServerPort);

		Message *msg = new Message("calibrate", 1);
		msg->addEvent("reportingToggle", 1);
		msg->encode();
		socket->send(msg);
		delete msg;

		char buffer[BUFSIZE];
		int recvMsgSize;
		while ((recvMsgSize = socket->recv(buffer, BUFSIZE)) > 0) { // Zero means end of transmission
			// Terminate string
			buffer[recvMsgSize] = '\0';

			// Convert into string for easier use
			string receivedMessage = buffer; 
			
			Message *parsedMessage = new Message(receivedMessage);
			for (int i = 0; i < parsedMessage->events.size(); i++) {
                Event parsedEvent = parsedMessage->events[i];
                handleEvent(parsedEvent.event, parsedEvent.data);
            }
			delete parsedMessage;
		}
	} catch(SocketException &e) {
		cerr << e.what() << endl;
		exit(1);
	}

	return 0;
}

/**
 * Flare Director
 * ASM
 * Main.cpp
 * 
 * This file contains the bootstrap code and 
 * starts the ASM code.
 */

#include "ASM.hpp"
#include <iostream>
#include <signal.h>

using namespace std;

ASM *sensorModule;

void quitHandler(int sig_num) {
  cout << "Quitting program..." << endl;
  delete sensorModule->serverSocket;
  exit(0);
}

/**
 * main
 * The entry point for the program.
 * @param argc The argument count
 * @param argv Array of arguments
 */
int main(int argc, char *argv[]) {
    // Test for correct number of arguments 
    if (argc != 2) { 
      	cerr << "Usage: " << argv[0] << " <Server Port> " << endl;
      	exit(1);
    }

    // Get port to listen to messages on
    unsigned short int serverPort = atoi(argv[1]);

    try {
      	// Initialize the ASM Software
      	sensorModule = new ASM(serverPort);

        signal(SIGINT, quitHandler);

      	// Start the TCP Server
      	sensorModule->start();
    // Catch generic exception
    } catch (...) {
      	cout << "Something went wrong. Exiting program..." << endl;
        exit(1);
    }

    return 0;
}

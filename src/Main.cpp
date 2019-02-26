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

/**
 * quitHandler
 ** This handler deallocates the serverSocket on
 ** SIGINT. This makes sure the network port is
 ** freed up so that we don't have to keep changing
 ** port numbers.
 */
void quitHandler(int sig_num) {
  cout << endl << "Exiting Sensor Module program..." << endl;
  delete sensorModule->serverSocket;
  exit(0);
}

/**
 * main
 ** The entry point for the program.
 * @param argc The argument count
 * @param argv Array of arguments
 */
int main(int argc, char *argv[]) {
    // Get port to listen to messages on. Defaults to 4000
    unsigned short int serverPort = (argc == 2) ? atoi(argv[1]) : 4000;

    try {
      	// Initialize the ASM Software
      	sensorModule = new ASM(serverPort);

        // Allows program to continue executing if the program receives
        // a broken pipe signal. 
        sigignore(SIGPIPE);

        // Handle Ctrl+C event
        signal(SIGINT, quitHandler);

      	// Start the Sensor Module code
      	sensorModule->start();
    // Catch generic exception
    } catch (...) {
      	cout << "Something went wrong. Exiting program..." << endl;
        exit(1);
    }

    return 0;
}

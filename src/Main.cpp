/**
 * Flare Director
 * ASM
 * Main.cpp
 * 
 * This file contains the bootstrap code and 
 * starts the ASM code.
 */

#include "ASM.h"

using namespace std;

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
		ASM *altitudeSensorModule = new ASM(serverPort);
		// Start the TCP Server
		altitudeSensorModule->start();
    } catch (...) {
      	cout << "caught exception in main" << endl;
    }
    return 0;
}

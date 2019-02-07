/*
 * Flare Director
 * ASM
 * Main.cpp
 * 
 */

#include "ASM.h"

using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 2) {  // Test for correct number of arguments  
    cerr << "Usage: " << argv[0] << " <Server Port> " << endl;
    exit(1);
  }

  unsigned short int serverPort = atoi(argv[1]); // Get port to listen to messages on

  ASM *altitudeSensorModule = new ASM(serverPort); // Initialize the ASM Software
  altitudeSensorModule->start(); // Start the TCP Server

  return 0;
}

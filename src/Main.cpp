/*
 * Flare Director
 * ASM
 * Main.cpp
 * 
 */

#include "ASM.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {  // Test for correct number of arguments  
    cerr << "Usage: " << argv[0] << " <Server Port> " << endl;
    exit(1);
  }

  const unsigned short port = atoi(argv[1]);    // First arg:  local port  

  ASM(port); // Start the ASM Software

  return 0;
}

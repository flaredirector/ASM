/**
 * Flare Director
 * ASM
 * ErrorCodeDisplay.cpp
 * 
 * This file contains the implementation details for the ErrorCodeDisplay
 * class.
*/

#include "ErrorCodeDisplay.hpp"
#ifndef DEBUG
#include <wiringPi.h>
#endif
#include <iostream>

// Pin values in correlation to wiring pi values
#define pinA 23
#define pinB 21
#define pinC 22
#define pinD 26

using namespace std;

/**
 * setupPins
 ** Sets up the GPIO pins for writing.
 */
void ErrorCodeDisplay::setupPins() {
    #ifndef DEBUG
    #ifndef WIRINGPI_SETUP
    wiringPiSetup();
    #define WIRINGPI_SETUP
    #endif
    pinMode(pinA, OUTPUT);
    pinMode(pinB, OUTPUT);
    pinMode(pinC, OUTPUT);
    pinMode(pinD, OUTPUT);
    #endif
    cout << "Done setting up 7 Segment Display" << endl;
}

/**
 * display
 ** Displays the passed error code.
 * @param {code} The code to display
 */
void ErrorCodeDisplay::display(short int code) {
    if (code < -1 || code > 9)
        return;

    // Perform binary conversion
    int binMap[4];
    for (int i = 3; i >= 0; i--) {
        // If code is -1 (Display Nothing), fill binary map with all 1s
        if (code == -1) {
            binMap[i] = 1;
        } else {
            int k = code >> i; 
            if (k & 1) 
                binMap[i] = 1; 
            else
                binMap[i] = 0;
        }
    }

    // Set pins to correct values based on bit map index
    #ifndef DEBUG
    digitalWrite(pinA, binMap[0] ? HIGH : LOW); 
    digitalWrite(pinB, binMap[1] ? HIGH : LOW);  
    digitalWrite(pinC, binMap[2] ? HIGH : LOW);
    digitalWrite(pinD, binMap[3] ? HIGH : LOW);
    #endif
    
    cout << "Set Error Code Display to ";
    if (code < 0)
       cout << "OFF";
    else
       cout << code;
    cout << endl;
}

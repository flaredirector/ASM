/**
 * Flare Director
 * ASM
 * LEDInterface.cpp
 * 
 * This file contains the implementation details for the LEDInterface
 * class. 
*/

#include "LEDInterface.hpp"
#ifndef DEBUG
#include <wiringPi.h>
#endif
#include <iostream>

// Pin values in correlation to wiring pi values
#define pinRED   3
#define pinGREEN 4
#define pinBLUE  6

using namespace std;

int LEDInterface::currentColor = 3;

/**
 * ASM
 ** Sets up the GPIO pins for writing.
 */
void LEDInterface::setupPins() {
    #ifndef DEBUG
    #ifndef WIRINGPI_SETUP
    wiringPiSetup();
    #define WIRINGPI_SETUP
    #endif
    pinMode(pinRED, OUTPUT);
    pinMode(pinBLUE, OUTPUT);
    pinMode(pinGREEN, OUTPUT);
    #endif
    cout << "Done setting up LED pins." << endl;
}

/**
 * ASM
 ** Sets up the GPIO pins for writing.
 * @param {color} The color to set the LED to.
 */
void LEDInterface::setColor(int color) {
    if (color < 2 || color > 7)
        return;

    // Perform binary conversion
    int binMap[3];
    for (int i = 2; i >= 0; i--) { 
        int k = color >> i; 
        if (k & 1) 
            binMap[i] = 1; 
        else
            binMap[i] = 0;
    }

    // Set pins to correct values based on bit map index
    #ifndef DEBUG
    digitalWrite(pinGREEN, binMap[0] ? HIGH : LOW);
    digitalWrite(pinBLUE, binMap[1] ? HIGH : LOW);  
    digitalWrite(pinRED, binMap[2] ? HIGH : LOW); 
    #endif
    LEDInterface::currentColor = color;
}

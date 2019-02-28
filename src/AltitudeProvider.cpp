/**
 * Flare Director
 * ASM
 * ASM.cpp
 * 
 * This class provides an interface for the software to
 * easily request what the current detected altitude of
 * the aircraft is. This class handles the sensor data
 * acquisition and signal processing.
 */

#include "AltitudeProvider.hpp"
#include <unistd.h>  // for usleep
#include <iostream>

using namespace std;

/**
 * AltitudeProvider
 ** Instantiates new AltitudeProvider instance and
 ** initializes sensor interfaces.
 */
AltitudeProvider::AltitudeProvider() {
    #ifndef DEBUG
    this->lidar = new LIDARInterface();
    this->sonar = new SONARInterface();
    
    // Attempt connection to the LIDAR interface
    int lidarError = this->lidar->connect();
    int sonarError = this->sonar->setup();
    // Check for error in connecting to the LIDAR interface
    if (lidarError < 0) {
        printf("LIDAR ERROR: %d\n", this->lidar->err);
    }
    if (sonarError < 0) {
        printf("SONAR ERROR: %d\n", this->sonar->err);
    }
    #else
    this->altitude = 120;
    #endif
}

/**
 * acquireDataLoop
 ** Runs the loop that acquires sensor data and processes
 ** the data.
 */
void AltitudeProvider::acquireDataLoop() {
    #ifndef DEBUG
    while (lidar->err >= 0 && sonar->err >= 0) {
        cout << this->sonar->getDistance() << endl;
        this->altitude = this->lidar->getDistance();
    #else
    for (;;) {
        this->altitude--;
        if (this->altitude < 0)
            this->altitude = 120;
    #endif

        usleep(100000); // 10hz
    }
}

/**
 * getAltitude
 ** Returns the acquired and processed altitude.
 */
int AltitudeProvider::getAltitude() {
    return this->altitude;
}
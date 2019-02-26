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
#include <unistd.h> // for usleep

/**
 * Altitude Provider
 ** Instantiates new AltitudeProvider instance and
 ** initializes sensor interfaces.
 */
AltitudeProvider::AltitudeProvider() {
    #ifndef DEBUG
    lidar = new LIDARInterface(1);
    
    // Attempt connection to the LIDAR interface
    int err = lidar->connect();
    // Check for error in connecting to the LIDAR interface
    if (err < 0){
        printf("%d", lidar->err);
    }
    #else
    altitude = 120;
    #endif
}

/**
 * acquireDataLoop
 ** Runs the loop that acquires sensor data and processes
 ** the data.
 */
void AltitudeProvider::acquireDataLoop() {
    #ifndef DEBUG
    while (lidar->err >= 0) {
        altitude = lidar->getDistance();
        usleep(100000);
    }
    #else
    while (1) {
        altitude--;
        if (altitude < 0)
            altitude = 120;
        usleep(100000);
    }
    #endif
}

/**
 * getAltitude
 ** Returns the acquired and processed altitude.
 */
int AltitudeProvider::getAltitude() {
    return altitude;
}
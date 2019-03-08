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

#define LIDAR_FACTOR 0.8
#define SONAR_FACTOR 0.2
#define DATA_LOGGING_FILENAME "data.csv"

using namespace std;

/**
 * AltitudeProvider
 ** Instantiates new AltitudeProvider instance and
 ** initializes sensor interfaces.
 */
AltitudeProvider::AltitudeProvider(ASMToggles *asmToggles) {
    this->toggles = asmToggles;
    #ifndef DEBUG
    this->lidar = new LIDARInterface();
    this->sonar = new SONARInterface();

    this->dataFile.open(DATA_LOGGING_FILENAME);
    this->dataFile << "LIDAR,SONAR" << endl;
    
    // Attempt connection to the LIDAR interface
    int lidarError = this->lidar->connect();
    int sonarError = this->sonar->setup();
    // Check for error in connecting to the LIDAR interface
    if (lidarError < 0) {
        cout << "LIDAR ERROR: " << this->lidar->err << endl;
    }
    if (sonarError < 0) {
        cout << "SONAR ERROR: " << this->sonar->err << endl;
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
    // Acquire data while both sensors are operational
    while (this->lidar->err >= 0 && this->sonar->err >= 0) {
        this->sonarDistance = this->sonar->getDistance();
        this->lidarDistance = this->lidar->getDistance();

	if (this->toggles->dataLoggingToggle)
		this->dataFile << this->lidarDistance << "," << this->sonarDistance << endl;

        // TODO: Perform sensor weighting and signal processing
        int processedAltitude = (int) (LIDAR_FACTOR * this->lidarDistance) + (SONAR_FACTOR * this->sonarDistance);

        this->altitude = processedAltitude;
    #else
    for (;;) {
        this->altitude--;
        if (this->altitude < 0)
            this->altitude = 120;
    #endif
        if (this->toggles->reportingToggle)
            usleep(100000); // 10hz
        else
            usleep(1000000); // 1hz
    }
}

/**
 * getAltitude
 ** Returns the acquired and processed altitude.
 */
int AltitudeProvider::getAltitude() {
    return this->altitude;
}

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
#include "FixedQueue.hpp"
#include <iostream>
#include <unistd.h>  // for usleep
#include <ctime>

#define LIDAR_FACTOR 0.9
#define SONAR_FACTOR 0.1
#define MAX_OFFSET_FEET 6
#define LIDAR_OUT_OF_RANGE_VALUE 0
#define SONAR_OUT_OF_RANGE_VALUE 1068
#define SONAR_MIN_VALUE 25

#define MILLISECONDS *1000

using namespace std;

/**
 * AltitudeProvider
 ** Instantiates new AltitudeProvider instance and
 ** initializes sensor interfaces.
 */
AltitudeProvider::AltitudeProvider(ASMToggles *asmToggles) {
    this->toggles = asmToggles;
    
    // Initialize sensor interfaces
    this->lidar = new LIDARInterface();
    this->sonar = new SONARInterface();

    // Open file up for data logging
    #ifndef DEBUG
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char dataFilename[50];
    sprintf(dataFilename, "data_%d_%d_%d_%d_%d.csv", ltm->tm_mon, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    this->dataFile.open(dataFilename);
    this->dataFile << "LIDAR,SONAR, ALTITUDE" << endl;
    #endif

    // Set inital values
    this->lidarDistance = 0;
    this->sonarDistance = 0;
    this->lidarOffset = 0;
    this->sonarOffset = 0;
    
    // Connect and setup the sensors
    this->lidar->connect();
    this->sonar->setup();

    // Check for error in connecting to the LIDAR interface
    if (this->lidar->err < 0) {
        cout << "LIDAR ERROR: " << this->lidar->err << endl;
    }
    if (this->sonar->err < 0) {
        cout << "SONAR ERROR: " << this->sonar->err << endl;
    }
}

/**
 * acquireDataLoop
 ** Runs the loop that acquires sensor data and processes
 ** the data.
 */
void AltitudeProvider::acquireDataLoop() {
    // Initialize data buffers for sensor data
    FixedQueue *lidarBuffer = new FixedQueue(5);
    FixedQueue *sonarBuffer = new FixedQueue(5);

    // Acquire data while both sensors are operational
    for (;;) {
        // Log the raw sensor data
        #ifndef DEBUG
        if (this->toggles->dataLoggingToggle)
            this->dataFile << this->lidarDistance << "," << this->sonarDistance;
        #endif

        // Get sensor data
        // int rawLidarDistance = 0, rawSonarDistance = 1068; 
        int rawLidarDistance = this->lidar->getDistance();
        int rawSonarDistance = this->sonar->getDistance();

        // Push data into FIFO buffer
        if (rawLidarDistance != -1 && this->lidar->err == 0)
            lidarBuffer->push(rawLidarDistance);
            
        if (rawSonarDistance != -1 && this->sonar->err == 0)
            sonarBuffer->push(rawSonarDistance);

        // Adjust sensor distances for calibration offsets
        int adjustedLidarDistance = lidarBuffer->average() - this->lidarOffset;
        int adjustedSonarDistance = sonarBuffer->average() - this->sonarOffset;
        
        // If adjusted sensor distance is less than 0, just set to 0
        this->sonarDistance = (adjustedSonarDistance < 0) ? 0 : adjustedSonarDistance;
        this->lidarDistance = (adjustedLidarDistance < 0) ? 0 : adjustedLidarDistance;

        // Check if lidar is detecting ground and if sonar is at max range.
        // If this is the case, aircraft is at upper boundary of lidar and out
        // of range of sonar, which needs to be handled.
        if (rawLidarDistance != -1 && rawSonarDistance != -1) {
            if (rawLidarDistance != LIDAR_OUT_OF_RANGE_VALUE && 
            (this->sonarDistance == 0 || rawSonarDistance == SONAR_OUT_OF_RANGE_VALUE)) {
                this->altitude = this->lidarDistance;
            } else if (rawSonarDistance != SONAR_OUT_OF_RANGE_VALUE && this->sonarDistance != 0 && 
                        abs(this->lidarDistance - this->sonarDistance) <= 50) {        
                this->altitude = (int) (LIDAR_FACTOR * this->lidarDistance) + (SONAR_FACTOR * this->sonarDistance);
            } else if (rawLidarDistance == LIDAR_OUT_OF_RANGE_VALUE && rawSonarDistance == SONAR_OUT_OF_RANGE_VALUE) {
                this->altitude = -1;
            // Fallback to LIDAR if sensor discrepancy happens
            } else {
                this->altitude = this->lidarDistance;
            }
        } else if (rawLidarDistance != -1 && rawSonarDistance == -1) {
            this->altitude = this->lidarDistance;
        } else if (rawLidarDistance == -1 && rawSonarDistance != -1) {
            this->altitude = this->sonarDistance;
        } else {
            this->altitude = -1;
        }
        

        // Log when a sensor discrepancy happens
        if (abs(this->lidarDistance - this->sonarDistance) >= 50 && rawSonarDistance != SONAR_OUT_OF_RANGE_VALUE) {
            cout << "Sensor Threshold Discrepancy: " << abs(this->lidarDistance - this->sonarDistance) << endl; 
        }

        #ifndef DEBUG
        if (this->toggles->dataLoggingToggle)
            this->dataFile << "," << this->altitude << endl;
        #endif

        // If reporting is on, run at 10hz, if not, 1hz
        if (this->toggles->reportingToggle)
            usleep(100 MILLISECONDS); // 10hz
        else
            usleep(1000 MILLISECONDS); // 1hz
    }
}

/**
 * acquireDataLoop
 ** Runs the loop that acquires sensor data and processes
 ** the data.
 * @return (int) The return code of the calibration operation
 */
int AltitudeProvider::calibrate() {
    cout << "Calibrating..." << endl;

    #ifndef DEBUG
    if (this->lidar->err != 0 || this->sonar->err != 0)
        return -2;

    this->toggles->hasBeenCalibrated = false;
    // Reset offsets prior to new calibration
    this->lidarOffset = 0; this->sonarOffset = 0;
    // Get current state of reportingToggle
    bool reportingWasOn = this->toggles->reportingToggle;
    // Toggle reporting on to trigger high polling rate
    this->toggles->reportingToggle = true;
    int lidarSampleTotal = 0;
    int sonarSampleTotal = 0;

    // Get 20 samples of data
    for (int sample = 0; sample < 20; sample++) {
        lidarSampleTotal += this->lidarDistance;
        sonarSampleTotal += this->sonarDistance;
        usleep(100 MILLISECONDS);
    }

    // Average samples and set offset values
    this->lidarOffset = (lidarSampleTotal /= 20);
    this->sonarOffset = (sonarSampleTotal /= 20);

    // Check offset values and return error if something looks wrong
    int maximumAllowableOffset = (int)(MAX_OFFSET_FEET*12*2.54); // 6 feet, 183 centimeters
    if (this->lidarOffset >= maximumAllowableOffset || 
        this->sonarOffset >= maximumAllowableOffset) {
            this->toggles->reportingToggle = false;
            cout << "Calibration Error: Maxium allowable offset exceded." << endl;
            cout << "LIDAR Offset: " << this->lidarOffset << ", SONAR Offset: " << this->sonarOffset << endl;
            return -1;
    }
    // If the reportingToggle was not on prior to calibration, turn it back off
    if (!reportingWasOn)
        this->toggles->reportingToggle = false;
    #else
    // Simulate calibration processing
    usleep(2000 MILLISECONDS); // 2 seconds
    #endif

    cout << "Done Calibrating" << endl;
    cout << "LIDAR Offset: " << this->lidarOffset << ", SONAR Offset: " << this->sonarOffset << endl;
    this->toggles->hasBeenCalibrated = true;
    return 0;
}

/**
 * getAltitude
 ** Returns the acquired and processed altitude.
 * @return (int) The processed altitude
 */
int AltitudeProvider::getAltitude() {
    return this->altitude;
}

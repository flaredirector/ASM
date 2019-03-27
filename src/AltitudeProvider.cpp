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
#include <unistd.h>  // for usleep

#define LIDAR_FACTOR 0.8
#define SONAR_FACTOR 0.2
#define DATA_LOGGING_FILENAME "data.csv"
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
    
    this->lidar = new LIDARInterface();
    this->sonar = new SONARInterface();
    #ifndef DEBUG
    this->dataFile.open(DATA_LOGGING_FILENAME);
    this->dataFile << "LIDAR,SONAR" << endl;

    this->lidarDistance = 0;
    this->sonarDistance = 0;
    this->lidarOffset = 0;
    this->sonarOffset = 0;
    
    // Attempt connection to the LIDAR interface
    this->lidar->connect();
    this->sonar->setup();

    // Check for error in connecting to the LIDAR interface
    if (this->lidar->err < 0) {
        cout << "LIDAR ERROR: " << this->lidar->err << endl;
    }
    if (this->sonar->err < 0) {
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
    FixedQueue *lidarBuffer = new FixedQueue(5);
    FixedQueue *sonarBuffer = new FixedQueue(5);
    #ifndef DEBUG
    // Acquire data while both sensors are operational
    // this->lidar->err == 0 || this->sonar->err == 0
    for (;;) {
        // Log the raw sensor data
        if (this->toggles->dataLoggingToggle)
            this->dataFile << this->lidarDistance << "," << this->sonarDistance << endl;

        // Get sensor data
        int rawLidarDistance = 0, rawSonarDistance = 1068; 
        if (this->lidar->err == 0)
            rawLidarDistance = this->lidar->getDistance();
        if (this->sonar->err == 0)
            rawSonarDistance = this->sonar->getDistance();

        // Push data into FIFO buffer
        if (rawLidarDistance != LIDAR_OUT_OF_RANGE_VALUE) {
            lidarBuffer->push(rawLidarDistance);
        }
            
        if (rawSonarDistance != SONAR_OUT_OF_RANGE_VALUE) {
            sonarBuffer->push(rawSonarDistance);
        }

        // Adjust sensor distances for calibration offsets
        int adjustedSonarDistance = lidarBuffer->average() - this->sonarOffset;
        int adjustedLidarDistance = sonarBuffer->average() - this->lidarOffset;

        // If adjusted sensor distance is less than 0, just set to 0
        this->sonarDistance = (adjustedSonarDistance < 0) ? 0 : adjustedSonarDistance;
        this->lidarDistance = (adjustedLidarDistance < 0) ? 0 : adjustedLidarDistance;
            
        // TODO: Perform sensor weighting and signal processing

        // Check if lidar is detecting ground and if sonar is at max range.
        // If this is the case, aircraft is at upper boundary of lidar and out
        // of range of sonar, which needs to be handled.
        if (this->lidarDistance != LIDAR_OUT_OF_RANGE_VALUE && this->sonarDistance == SONAR_OUT_OF_RANGE_VALUE) 
        {
            this->altitude = this->lidarDistance;
        } else {
            this->altitude = (int) (LIDAR_FACTOR * this->lidarDistance) + (SONAR_FACTOR * this->sonarDistance);
        }
        
    #else
    for (;;) {
        this->altitude--;
        if (this->altitude < 0)
            this->altitude = 120;
    #endif
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

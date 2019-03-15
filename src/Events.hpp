/**
 * Flare Director
 * ASM
 * Events.hpp
 * 
 * This header file contains the string values for the 
 * events that are sent through the network packets to
 * control the hardware and send altitude data.
*/

#define CALIBRATION_EVENT "calibrate"
#define ALTITUDE_EVENT "altitude"
#define REPORTING_TOGGLE_EVENT "reportingToggle"
#define DATA_LOGGING_TOGGLE_EVENT "loggingToggle"
#define LIDAR_STATUS_EVENT "lidarStatus"
#define SONAR_STATUS_EVENT "sonarStatus"
#define HARDWARE_FAILURE_EVENT "hardwareFailure"
#define CALIBRATION_STATUS_EVENT "calibrationStatus"
#define REPORTING_STATUS_EVENT "reportingStatus"
#define DATA_LOGGING_STATUS_EVENT "loggingStatus"
#define LIDAR_DATA_EVENT "lidarData"
#define SONAR_DATA_EVENT "sonarData"
#define GET_STATUS_EVENT "getStatus"

/**
 * Flare Director
 * ASM
 * ASM.cpp
 * 
 * This header file contains the class definition
 * for the AltitudeProvider class.
 */

#include "../lib/sensor/lidar/LIDARInterface.hpp"
#include "../lib/sensor/sonar/SONARInterface.hpp"

class AltitudeProvider {
    public:
        AltitudeProvider();
        void acquireDataLoop();
        int getAltitude();
    private:
        LIDARInterface *lidar;
        SONARInterface *sonar;
        int altitude;
};

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
#include "ASMToggles.hpp"

class AltitudeProvider {
    public:
        AltitudeProvider(ASMToggles *asmToggles);
        void acquireDataLoop();
        int getAltitude();
    private:
        ASMToggles *toggles;
        LIDARInterface *lidar;
        SONARInterface *sonar;
        int altitude;
        int lidarDistance;
        int sonarDistance;
};

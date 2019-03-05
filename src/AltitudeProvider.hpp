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
#include <fstream>

class AltitudeProvider {
    public:
        AltitudeProvider(ASMToggles *asmToggles);
        void acquireDataLoop();
        int getAltitude();
        int lidarDistance;
        int sonarDistance;
	std::ofstream dataFile;
    private:
        ASMToggles *toggles;
        LIDARInterface *lidar;
        SONARInterface *sonar;
        int altitude;
};

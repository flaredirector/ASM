#include "../lib/sensor/lidar/LIDARInterface.hpp"

class AltitudeProvider {
    public:
        AltitudeProvider();
        void acquireDataLoop();
        int getAltitude();
    private:
        LIDARInterface *lidar;
        int altitude;
};

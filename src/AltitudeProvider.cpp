#include "AltitudeProvider.hpp"
#include <unistd.h>
#include <iostream>

using namespace std;

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

void AltitudeProvider::acquireDataLoop() {
    std::cout << "starting sensor data acquisition" << std::endl;
    
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

int AltitudeProvider::getAltitude() {
    return altitude;
}
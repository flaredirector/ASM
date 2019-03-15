/**
 * Flare Director
 * ASM
 * SensorInterface.hpp
 * 
 * This header file contains the class definition
 * for the SensorInterface abstract class.
 */

class SensorInterface {
    public:
        int err;
        virtual int getDistance(void) = 0;
};
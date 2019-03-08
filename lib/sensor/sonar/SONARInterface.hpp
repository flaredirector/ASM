/**
 * Flare Director
 * ASM
 * SONARInterface.hpp
 * 
 * This header file contains the class definition
 * for the SONARInterface class.
 */

class SONARInterface: public SensorInterface {
    int uartFilestream;
    char rx_buffer[256];

    public:
        SONARInterface();
        int setup();
};
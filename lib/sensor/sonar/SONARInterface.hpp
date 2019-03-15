/**
 * Flare Director
 * ASM
 * SONARInterface.hpp
 * 
 * This header file contains the class definition
 * for the SONARInterface class.
 */

class SONARInterface {
        int uartFilestream;
        char rx_buffer[256];
        int err;
    public:
        SONARInterface();
        int setup();
        int getDistance();
};
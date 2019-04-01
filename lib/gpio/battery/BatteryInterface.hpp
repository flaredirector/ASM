/**
 * Flare Director
 * ASM
 * BatteryInterface.hpp
 * 
 * The BatteryInterface class provides an interface for the software to
 * get the status of the battery such as the percentage.
*/

class BatteryInterface {
    int busFileDescriptor;
    public:
        int getPercentage();
        int setup();
};
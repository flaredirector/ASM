/**
 * Flare Director
 * ASM
 * LEDInterface.hpp
 * 
 * The LEDInterface class contains static methods to set the color of the
 * main status LED on the PCB. 
*/

#define LED_YELLOW 2
#define LED_RED 3
#define LED_PURPLE 4
#define LED_BLUE 5
#define LED_GREEN 6
#define LED_OFF 7

class LEDInterface {
    public:
	    static int currentColor;
        static void setupPins();
        static void setColor(int color);
};

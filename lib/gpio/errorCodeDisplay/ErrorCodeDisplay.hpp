/**
 * Flare Director
 * ASM
 * ErrorCodeDisplay.hpp
 * 
 * The ErrorCodeDisplay class contains static methods to control the
 * 7 segment display on the PCB. It can display a code to the display
 * between 0 and 9.
*/

class ErrorCodeDisplay {
    public:
        static void setupPins();
        static void display(int code);
};
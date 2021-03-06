/**
 * Flare Director
 * ASM
 * SONARInterface.cpp
 * 
 * This file contains the implementation details for the
 * SONARInteface class which handles the setup, configutation, and
 * serial (TTL) details for acquiring the SONAR sensor data.
 */

#include "SONARInterface.hpp"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#ifndef DEBUG
#include <fcntl.h>	 //Used for UART
#include <termios.h> //Used for UART
#else
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#endif

/**
 * SONARInterface
 ** Instantiates new SONARInterface instance and
 ** and sets up some properties.
 */
SONARInterface::SONARInterface() {
    // Set initial error code to 0 (OK)
    this->err = 0;
    #ifdef DEBUG
        srand(time(NULL));
    #endif
}

/**
 * setup
 ** Sets up the UART filestream with the necessary
 ** configuration settings.
 */
int SONARInterface::setup() {
    #ifndef DEBUG
    // At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
    // int uart0_filestream = -1;
    this->uartFilestream = -1;
    // OPEN THE UART
    // The flags (defined in fcntl.h):
    // Access modes (use 1 of these):
    //		O_RDONLY - Open for reading only.
    //		O_RDWR - Open for reading and writing.
    //		O_WRONLY - Open for writing only.
    //
    // O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return 
    //      immediately with a failure status if there is no input immediately available (instead of blocking). 
    //      Likewise, write requests can also return immediately with a failure status if the output can't be 
    //      written immediately.
    //
    // O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device
    //      to become the controlling terminal for the process.
    this->uartFilestream = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
    if (this->uartFilestream == -1)
    {
        //ERROR - CAN'T OPEN SERIAL PORT
        printf("Sonar Error - Unable to open UART. Ensure it is not in use by another application\n");
        return this->err = -2;
    }

    // CONFIGURE THE UART
    // The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
    // Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, 
    //      B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
    // CSIZE:- CS5, CS6, CS7, CS8
    // CLOCAL - Ignore modem status lines
    // CREAD - Enable receiver
    // IGNPAR = Ignore characters with parity errors
    // ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - 
    //      don't use for bianry comms!)
    // PARENB - Parity enable
    // PARODD - Odd parity (else even)
    struct termios options;
    tcgetattr(this->uartFilestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;	// Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(this->uartFilestream, TCIFLUSH);
    tcsetattr(this->uartFilestream, TCSANOW, &options);

    // ----- CHECK FOR ANY RX BYTES -----
    if (this->uartFilestream == -1) {
        return this->err = -1;
    }
    #endif
    return 0;
}

/**
 * getDistance
 ** Returns the sensed distance from the SONAR sensor.
 */
int SONARInterface::getDistance() {
    #ifndef DEBUG
    //Filestream, buffer to store in, number of bytes to read (max)
    int rx_length = read(this->uartFilestream, (void*)this->rx_buffer, 255);
    // printf("rx_length: %d\n", rx_length);
    if (rx_length < 0) {
        // An error occured (will occur if there are no bytes)
        this->err = -1;
        return -1;
    } else if (rx_length == 0) {
        // No data waiting
        this->err = -2;
        return -1;
    } else {
        // Bytes received
        this->rx_buffer[rx_length] = '\0';
        // Check for valid buffer data
        if (this->rx_buffer[0] == 'R') {
            this->err = 0;

            char data_buffer[4];
            // Fill data buffer with rx_buffer, but without 'R'
            for (int index = 0; index < 4; index++) {
                data_buffer[index] = this->rx_buffer[index+1];
            }

            // Convert data_buffer character array to integer
            int sonarData = strtol(data_buffer, NULL, 10); 

            return sonarData;
        }

        // If the second character is empty, most likely not able to get data from sensor so set error
        if (this->rx_buffer[1] == '\0') {
            this->err = -3;
        }

        return -1;
    }
    return 0;
    #else
    static int testSonarAlt = 4000;
    if (testSonarAlt > 1068) {
        testSonarAlt -= 25;
        return 1068;
    } else {
        int noise = rand() % 200 - 50;
        return (testSonarAlt < 0) ? testSonarAlt = 4000 : (testSonarAlt -= 25) + noise;
    }   
    #endif
}

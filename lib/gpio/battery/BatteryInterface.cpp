/**
 * Flare Director
 * ASM
 * BatteryInterface.cpp
 * 
 * This file contains the implementation details for the BatteryInterface
 * class.
*/

#include "BatteryInterface.hpp"
#ifndef DEBUG
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#endif

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

#define VREG 2
#define CREG 4
#define BUFSIZE	16
#define DEV "/dev/i2c-1"
#define ADRS 0x36

#define BILLION 1000000000L;

using namespace std;

/**
 * readReg
 ** Utility function to read a register.
 */
static int readReg(int busfd, __uint16_t reg, unsigned char *buf, int bufsize)
{
    #ifndef DEBUG
    unsigned char reg_buf[2];

    reg_buf[0] = (reg >> 0) & 0xFF;
    reg_buf[1] = (reg >> 8) & 0xFF;

    int ret = write(busfd, reg_buf, 2);

    if (ret < 0) {
        printf("Write failed trying to read reg: %04x (0x%02x 0x%02x)\n", reg, reg_buf[0], reg_buf[1], reg);
        return ret;
    }

    return read(busfd, buf, bufsize);
    #else
    return 0;
    #endif
}

/**
 * setup
 ** sets up the I2C interface for communicating with the UPS.
 */
int BatteryInterface::setup() {
    cout << "Setting up Battery Interface..." << endl;
    #ifndef DEBUG
    if ((this->busFileDescriptor = open(DEV, O_RDWR)) < 0) {
        cout << "Can't open " << DEV << " (running as root?)" << endl;
        return(-1);
    }

    int ret = ioctl(this->busFileDescriptor, I2C_SLAVE, ADRS);
    if (ret < 0)
        cout << "I2C device initialisation failed" << endl;

    if (ret < 0)
        return(-1);
    #endif
    cout << "Done setting up Battery Interface" << endl;
    return 0;
}

/**
 * getPercentage
 ** Gets the current battery percentage.
 */
int BatteryInterface::getPercentage() {
    #ifndef DEBUG
    unsigned char buf[BUFSIZE] = {0};
    // int voltage = 10000000;
    readReg(this->busFileDescriptor, VREG, buf, 2);
	
    int hi,lo;
    hi = buf[0];
    lo = buf[1];
    // voltage = (hi << 8)+lo;

    readReg(this->busFileDescriptor, CREG, buf, 2);
    hi = buf[0];
    lo = buf[1];
    int v = (hi << 8)+lo;

    // Voltage: (((float)voltage)* 78.125 / 1000000.0)
    return ((int)((float)v) / 256.0);
    #else
    return 27;
    #endif
}
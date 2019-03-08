/**
 * Flare Director
 * ASM
 * LIDARInterface.cpp
 * 
 * This file contains the implementation details for the
 * LIDARInterface class which handles the setup, configuration,
 * and I2C details for acquiring the LIDAR sensor data.
 */

#ifndef DEBUG
#include "LIDARInterface.hpp"
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <cstdlib>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define LIDAR_LITE_ADDRESS = 0x62
#define LIDAR_LITE_DIST_WRITE_REG = 0x00
#define LIDAR_LITE_DIST_WRITE_VAL = 0x04
#define LIDAR_LITE_DIST_READ_REG = 0x8f
#define LIDAR_LITE_DIST_READ_REG_2 = 0x98

using namespace std;

/**
 * LIDARInterface
 ** Instantiates new LIDARInterface instance and
 ** and sets up some properties.
 */
LIDARInterface::LIDARInterface (void){
  err = 0;
  adapter_num = 1;
  snprintf(filename, 19, "/dev/i2c-%d", adapter_num);
}

/**
 * LIDARInterface
 ** Closes the I2C data bus on instance deletion.
 */
LIDARInterface::~LIDARInterface( void ) {
  printf("Ending Lidar-Lite Session\n");
  if (i2c_bus > 0){
   int e = close(i2c_bus);
  }
}

/**
 * connect
 ** Attempts connection to the sensor over the
 ** I2C bus.
 */
int LIDARInterface::connect( void ){
  printf("Connecting to Lidar: %s\n", filename);
  i2c_bus = open(filename, O_RDWR);
  if (i2c_bus < 0){
    err = errno;
    printf("Connect Error: %d\n", err);
    return -1;
  }
  if (ioctl(i2c_bus, I2C_SLAVE, 0x62) < 0) {
    err = errno;
    printf("Bus Error: %d\n", err);
    return -1;
  }
  return 0;
}

/**
 * writeAndWait
 ** Writes data to the LIDAR.
 */
int LIDARInterface::writeAndWait(int writeRegister, int value){
  res = i2c_smbus_write_byte_data(i2c_bus, writeRegister, value);
  usleep(10000);
  if (res < 0){
    err = errno;
    printf("Write Error %d\n", err);
    return -1;
  } else {
    return 0;
  }
}

/**
 * readAndWait
 ** Reads data from the LIDAR.
 */
int LIDARInterface::readAndWait(int readRegister){
  res = i2c_smbus_read_byte_data(i2c_bus, readRegister);
  usleep(10000);
  if (res < 0){
    err = errno;
    printf("Read Error: %d\n", err);
    return -1;
  } else {
    return 0;
  }
}

/**
 * getDistance
 ** Gets the reported distance from the LIDAR.
 */
int LIDARInterface::getDistance( void ){
  int buf[2];
  int e = 0;
  e = writeAndWait(0x00,0x04);
  if (e < 0){
    return e;
  }
  e = readAndWait(0x8f);
  if (e < 0){
    return e;
  } else {
    buf[0] = res;
  }
  e = readAndWait(0x10);
  if (e < 0){
    return e;
  } else {
    buf[1] = res;
  }
  return (buf[0] << 8) + buf[1];
}
#endif
/**
 * Flare Director
 * ASM
 * LIDARInterface.hpp
 * 
 * This header file contains the class definition
 * for the LIDARInterface class.
 */

#ifndef LIDAR_LITE_H
#define LIDAR_LITE_H
#endif

class LIDARInterface {
    int i2c_bus;
    int adapter_num;
    char filename[20];
    int res;
    int err;
  public:
    LIDARInterface(void);
    ~LIDARInterface(void);
    int connect(void);
    int writeAndWait(int, int);
    int readAndWait(int);
    int getDistance();
};
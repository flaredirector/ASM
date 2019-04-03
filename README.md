# ASM
This repository contains C++ code for the Altitude Sensing Module (ASM). The ASM software lives on the microcontroller (in this case, a Raspberry Pi 3) and controls the various sensors and external interfaces present in the ASM hardware.

## Directory Structure
The directory structure of the ASM code was designed to facilitate rapid feature development and code maintenance.
```
asm/
    demo/
    lib/
        network/
        gpio/
            battery/
            errorCodeDisplay/
            led/
        sensor/
            lidar/
            sonar/
    src/
    Makefile
    README.md
``` 
At a top level the ```src``` directory contains a majority of the logic for the ASM. The logic includes sensor data fusion and signal processing and network message sending.


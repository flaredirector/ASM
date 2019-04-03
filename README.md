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
At a top level the ```src``` directory contains a majority of the logic for the ASM. The logic includes sensor data fusion and signal processing and network message sending. The ```lib``` folder contains folders for lower level networking code, I2C, and Serical interface code for interfacing with sensors, batteries, and LEDs. 
## Requirements
The following are required before installing the ASM software:
* Raspberry Pi 3B, 3A or greater
* Wiring Pi installed
* WiFi Hotspot software configured
* I2C and other interface software
## Installation
To install the ASM code on the RPI, clone this repository like so:
```
git clone https://github.com/flaredirector/asm
``` 
This will download the project files into an `asm` folder.

Next, create a build folder for the binaries by running:
```
cd asm && mkdir bin
```
## Building
To build the ASM software make sure you are `cd`'ed into the `asm` folder and run 
```
make
```
This will build the binary ```asm``` in the ```bin``` folder. 
## Running
To run the ASM software, run:
```
./bin/asm
```
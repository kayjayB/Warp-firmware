# Design and Implementation of a Digital Pedometer
### Kayla-Jade Butkow (kjb85)
### Magdalene College

## Project Overview
This repository presents a digital pedometer implemented on the FRDMKL03 board using the Warp firmware. 


## Layout of the repository
All of the edited firmware lies in  `src/boot/ksdk1.1.0/`. The files that were edited are as follows:
- devSSD1331.h
- devSSD1331.c
- devMMA8451Q.h
- devMMA8451Q.c
- warp-kl03-ksdk1.1-boot.c
- CMakeLists.txt

Additionally, the following files were added to the firmware in `src/boot/ksdk1.1.0/`:
-  pedometer.h
-  pedometer.c

All files relating to the other sensors in the firmware were removed to increase the available memory on the device

Other files that were edited include: 

- `tools/scripts/jlink.commands`: Changed path to firmware
- `build/build.sh`: Added pedometer files to build script and removed files for other sensors
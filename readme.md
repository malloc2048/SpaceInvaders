# Space Invaders Emulator in C++
This is my attempt at an emulator for the classic arcade game Space Invaders.

##  Dependencies
* Google test for unit test build
  * ``` apt-get install -y libgtest-dev ```
* SDL 2.0
  * ``` apt-get install -y apt-get install -y libsdl2-dev ```
* CMake

## Build Instructions
### Available Targets
All built executables are put into the bin directory in the root of the project.
* all - this is the default of the generated make file (builds all following targets)
* invaders - the game
* disassembler - utility to disassemble the invaders ROM file
* unittests - build the GTEST unit test executable

### Windows
Tested Windows 10 using the following:
* MinGW 64 compiler suite
* GIT Bash

1. Clone this repository ```git clone git@github.com:malloc2048/goinvaders.git```
2. Open a terminal in the root directory of the project 
3. Create a build directory ```mkdir build```
4. Change to build directory ```cd build```
5. Run CMake ```cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - MinGW Makefiles" ..```
6. Run Make ```mingw32-make```

### Linux
Tested on Ubuntu 20.04 using the following:
* GCC
* CMake

1. Clone this repository ```git clone git@github.com:malloc2048/goinvaders.git```
2. Open a terminal in the root directory of the project 
3. Create a build directory ```mkdir build```
4. Change to build directory ```cd build```
5. Run CMake ```cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" ..```
6. Run Make ```make```

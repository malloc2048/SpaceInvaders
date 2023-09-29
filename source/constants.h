#ifndef EMULATION_CONSTANTS_H
#define EMULATION_CONSTANTS_H

#include <string>

typedef unsigned int dword;
typedef unsigned char byte;
typedef unsigned long qword;
typedef unsigned short word;

enum DATA_SRC_DST {
    B = 0,
    C = 1,
    D = 2,
    E = 3,
    H = 4,
    L = 5,
    M = 6,
    A = 7,
    BC = 8,
    DE = 9,
    HL = 10,
    SP = 11
};

int FPS();
int SCREEN_WIDTH();
int SCREEN_HEIGHT();
word VRAM_ADDRESS();
dword CYCLES_PER_FRAME();
std::string ROM_FILENAME();
std::string ConfigFileName();
dword HALF_CYCLES_PER_FRAME();
std::string InstructionSetConfig();

// newer to keep
std::string DISASSEMBLY_FILENAME();
std::string DATA_FILEPATH();
std::string ROM_FILENAME_();
std::string INSTRUCTION_CFG();
#endif

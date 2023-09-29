#include "constants.h"

int FPS() { return 60; }
int SCREEN_WIDTH() { return 224; }
int SCREEN_HEIGHT() { return 256; }
word VRAM_ADDRESS() { return 0x2400; }
dword CYCLES_PER_FRAME() { return 2000000 / FPS(); }
std::string ROM_FILENAME() { return "../source/invaders/docs/invaders"; }
std::string ConfigFileName() { return "../docs/invaders.cfg"; }
std::string InstructionSetConfig() { return "../source/invaders/docs/instruction_set.cfg"; }
dword HALF_CYCLES_PER_FRAME() { return CYCLES_PER_FRAME() / 2; }

std::string DISASSEMBLY_FILENAME() { return "invaders.dis"; }
std::string DATA_FILEPATH() { return "../source/invaders/docs/"; }
std::string ROM_FILENAME_() { return "invaders"; }
std::string INSTRUCTION_CFG() { return "instruction_set.cfg"; }

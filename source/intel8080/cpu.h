#ifndef EMULATION_CPU_H
#define EMULATION_CPU_H

#include <fstream>
#include "constants.h"

class Operation;

struct Flags {
    bool sign = false;
    bool zero = false;
    bool carry = false;
    bool parity = false;
    bool half_carry = false;

    bool halted = false;
    bool interruptEnabled = false;

    void calculate_parity(byte value);
};

struct Registers {
    [[nodiscard]] word read(byte reg) const;
    void write(byte reg, word data);

    byte b = 0;
    byte c = 0;
    byte d = 0;
    byte e = 0;
    byte h = 0;
    byte l = 0;

    byte accumulator = 0;
    word stack_pointer = 0;
    word program_counter = 0;
};

class Memory {
public:
    Memory() = default;
    ~Memory() = default;

    void load_rom(std::ifstream &rom_file);

    [[nodiscard]] byte read_byte(word address) const;
    [[nodiscard]] word read_word(word address) const;
    bool write(word address, byte value);
    bool write_word(word address, word value);

    static const uint16_t ROM_SIZE = 0x2000;

private:
    byte memory[0x10000] = {0};
};

struct Instruction {
    byte size;
    byte cycles;
    byte opcode;
    std::string mnemonic;
    Operation* operation;
};
#endif

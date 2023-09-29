#include "cpu.h"

void Memory::load_rom(std::ifstream &rom_file) {
    if(rom_file.is_open()) {
        rom_file.read((char*) memory, ROM_SIZE);
    }
}

byte Memory::read_byte(word address) const {
    return memory[address];
}

word Memory::read_word(word address) const {
    return memory[address] | (memory[address + 1] << 0x08u);
}

bool Memory::write(word address, byte value) {
    if(address < ROM_SIZE) {
        fprintf(stderr, "attempt to write to protected ROM\n");
        return false;
    }
    memory[address] = value;
    return true;
}

bool Memory::write_word(word address, word value) {
    if(write(address, value & 0x00ffu))
        return write(address + 1, (value & 0xff00u) >> 0x08u);
    return false;
}

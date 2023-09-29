#include <iomanip>
#include "constants.h"
#include "disassembler.h"
#include "intel8080/cpu.h"
#include "intel8080/intel_8080.h"

void Disassembler::disassemble(const std::string& filepath) {
    std::ifstream rom_file(filepath + ROM_FILENAME_());
    if(rom_file.is_open()) {
        Memory memory;
        memory.load_rom(rom_file);
        Intel8080 cpu(memory);

        cpu.load_instruction_set(filepath + INSTRUCTION_CFG());
        auto instructions = cpu.get_instructions();
        std::ofstream disassembly_file(filepath + DISASSEMBLY_FILENAME());
        for(auto i = 0x000; i < Memory::ROM_SIZE; i++) {
            auto opcode = memory.read_byte(i);
            try {
                auto instruction = instructions.at(opcode);
                disassembly_file << std::right << std::setfill('0') << std::setw(4) << std::hex << i << "\t";
                disassembly_file << std::setfill('0') << std::setw(2) << int(opcode) << "\t";
                disassembly_file << std::left << std::setfill(' ') << std::setw(9) << instruction.mnemonic << "\t";

                for(size_t n = instruction.size - 1; n > 0; n--) {
                    disassembly_file << std::setfill('0') << std::setw(2) << int(memory.read_byte(i + n));
                }
                disassembly_file << std::endl;
                i += instruction.size - 1;

            } catch (std::out_of_range& e) {

            }
        }
    }
}

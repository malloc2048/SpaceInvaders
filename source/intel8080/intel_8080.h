#ifndef EMULATION_INTEL_8080_H
#define EMULATION_INTEL_8080_H

#include <map>
#include "cpu.h"
#include <vector>

class Intel8080 {
public:
    explicit Intel8080(Memory& memory);
    ~Intel8080() = default;

    void step();
    void readCfg();
    void load_instruction_set(const std::string& instruction_conf = "");
    void make_instruction(const std::vector<std::string>& tokens);

    byte next_byte();
    std::map<byte, Instruction>& get_instructions();

public:
    Flags flags;
    Memory& memory;
    qword cycles = 0;
    Registers registers;
    std::map<std::string, bool> cfg;
    qword instructions_processed = 0;
    std::map<byte, Instruction> instructions;
    std::map<std::string, Operation*> operations_map;

    std::ofstream log;
};
#endif

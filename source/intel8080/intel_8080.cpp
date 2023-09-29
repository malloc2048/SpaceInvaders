#include <regex>
#include <fstream>
#include "intel_8080.h"
#include "intel8080/operations/logical.h"
#include "intel8080/operations/machine.h"
#include "intel8080/operations/branching.h"
#include "intel8080/operations/arithmetic.h"
#include "intel8080/operations/data_transfer.h"

Intel8080::Intel8080(Memory &memory) : memory(memory) {
    operations_map = {
        {"machine", new Machine(flags, memory, registers)},
        {"logical", new Logical(flags, memory, registers)},
        {"branch", new Branching(flags, memory, registers)},
        {"transfer", new DataTransfer(flags, memory, registers)},
        {"arithmetic", new Arithmetic(flags, memory, registers)},
    };

    readCfg();
    if(cfg["LogEnable"])
        log.open("../roms/invaders.log");
}

void Intel8080::step() {
    auto address = registers.program_counter;
    auto opcode = next_byte();

    if(cfg["LogEnable"])
        log << "Opcode: " << std::hex << (int)opcode << "\tprogram counter: " << std::hex << address
            << "\tstack pointer: " << std::hex << registers.stack_pointer
            << "\tstack: " << std::hex << memory.read_word(registers.stack_pointer)
            << "\tB: " << (int)registers.b
            << "\tC: " << (int)registers.c
            << "\tA: " << (int)registers.accumulator
            << "\tD: " << (int)registers.d
            << "\tE: " << (int)registers.e
            << "\tH: " << (int)registers.h
            << "\tL: " << (int)registers.l
            << "\tzero: " << flags.zero
            << "\tsign: " << flags.sign
            << "\tparity: " << flags.parity
            << "\tcarry: " << flags.carry
            << "\thalf carry: " << flags.half_carry
            <<std::endl;

    if(instructions.find(opcode) != instructions.end()) {
        cycles += instructions[opcode].cycles;
        instructions[opcode].operation->execute(opcode);
    }
}

void Intel8080::readCfg() {
    std::string line;
    std::ifstream cfgFile(ConfigFileName());

    if(cfgFile.is_open()) {
        while (std::getline(cfgFile, line)) {
            auto const re = std::regex{R"(:+)"};
            auto const tokens = std::vector<std::string>(
                std::sregex_token_iterator{begin(line), end(line), re, -1},
                std::sregex_token_iterator{}
            );

            if (tokens.size() == 2) {
                std::istringstream(tokens[1]) >> std::boolalpha >> cfg[tokens[0]];
            }
        }
    } else {
        cfg["logEnabled"] = false;
    }
}

void Intel8080::load_instruction_set(const std::string& instruction_conf) {
    std::string line;
    auto instructions_file = instruction_conf.empty() ? instruction_conf : InstructionSetConfig();
    std::ifstream instruction_set(instruction_conf);

    if(instruction_set.is_open()) {
        while (std::getline(instruction_set, line)) {
            auto const re = std::regex{R"(:+)"};
            auto const tokens = std::vector<std::string>(
                std::sregex_token_iterator{begin(line), end(line), re, -1},
                std::sregex_token_iterator{}
            );

            if (tokens.size() == 5) {
                make_instruction(tokens);
            }
        }
        instruction_set.close();
    }
}

void Intel8080::make_instruction(const std::vector<std::string> &tokens) {
    try {
        auto opcode = std::stoi(tokens[0], nullptr, 16);
        auto mnemonic = tokens[1];
        auto size = std::stoi(tokens[2], nullptr, 10);
        auto instrCycles = std::stoi(tokens[3], nullptr, 10);
        Operation *operation = operations_map.at(tokens[4]);

        instructions.insert(
            std::pair<byte, Instruction>(
                opcode, Instruction{
                    .size = byte(size),
                    .cycles = byte(instrCycles),
                    .opcode =  byte(opcode),
                    .mnemonic =  mnemonic,
                    .operation = operation,
                }));
    } catch (std::exception& ex) {
        if(cfg["LogEnable"])
            log << ex.what() << std::endl;
    }
}

byte Intel8080::next_byte() {
    return memory.read_byte(registers.program_counter++);
}

std::map<byte, Instruction>& Intel8080::get_instructions() {
    return instructions;
}

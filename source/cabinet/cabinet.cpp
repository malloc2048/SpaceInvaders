#include "cabinet/cabinet.h"

Cabinet::Cabinet(): memory(), cpu(memory) {
    cpu.load_instruction_set();
}

word Cabinet::get_program_counter() const {
    return cpu.registers.program_counter;
}

qword Cabinet::get_cycle_count() const {
    return cpu.cycles;
}

void Cabinet::interrupt(word address) {
    if(cpu.flags.interruptEnabled) {
        cpu.flags.interruptEnabled = false;

        // PUSH current PC to stack
        cpu.registers.stack_pointer -= 2;
        memory.write_word(cpu.registers.stack_pointer, cpu.registers.program_counter);

        cpu.registers.program_counter = address;
        cpu.cycles += 11;
    }
}

void Cabinet::set_cycle_count(word count) {
    cpu.cycles = count;
}

byte Cabinet::get_accumulator() const {
    return cpu.registers.accumulator;
}

void Cabinet::set_accumulator(byte value) {
    cpu.registers.accumulator = value;
}

void Cabinet::increment_program_counter(byte increment) {
    cpu.registers.program_counter += increment;
}

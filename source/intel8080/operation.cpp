#include "operation.h"

Operation::Operation(Flags &flags, Memory &memory, Registers &registers) :
    flags(flags), memory(memory), registers(registers){}

word Operation::get_data(byte src) const {
    switch(src) {
        case B:
            return registers.b;
        case C:
            return registers.c;
        case D:
            return registers.d;
        case E:
            return registers.e;
        case H:
            return registers.h;
        case L:
            return registers.l;
        case M:
            return memory.read_byte(registers.read(HL));
        case A:
            return registers.accumulator;
        case BC:
            return (registers.b << 8u) | registers.c;
        case DE:
            return (registers.d << 8u) | registers.e;
        case HL:
            return (registers.h << 8u) | registers.l;
        case SP:
            return registers.stack_pointer;
        default:
            return 0;
    }
}

byte Operation::next_byte() const {
    return memory.read_byte(registers.program_counter++);
}

word Operation::next_word() const {
    auto data = memory.read_word(registers.program_counter);
    registers.program_counter += 2;
    return data;
}

void Operation::set_data(byte dst, word data) {
    switch(dst) {
        case B:
            registers.b = data;
            break;
        case C:
            registers.c = data;
            break;
        case D:
            registers.d = data;
            break;
        case E:
            registers.e = data;
            break;
        case H:
            registers.h = data;
            break;
        case L:
            registers.l = data;
            break;
        case M:
            memory.write(registers.read(HL), data);
            break;
        case A:
            registers.accumulator = data;
            break;
        case BC:
            registers.b = (data & 0xff00u) >> 0x08u;
            registers.c = data & 0xffu;
            break;
        case DE:
            registers.d = (data & 0xff00u) >> 0x08u;
            registers.e = data & 0xffu;
            break;
        case HL:
            registers.h = (data & 0xff00u) >> 0x08u;
            registers.l = data & 0xffu;
            break;
        case SP:
            registers.stack_pointer = data;
            break;
        default:
            return;
    }
}

#include "intel8080//operations/logical.h"

Logical::Logical(Flags &flags, Memory &memory, Registers &registers)
    : Operation(flags, memory, registers) {

}

void Logical::execute(byte opcode) {
    if ((opcode & 0xf8u) == 0xa0) {          // ANA
        ana(get_data(opcode & 0x07u));
    } else if (opcode == 0xe6) {               // ANI
        ana(next_byte());
        flags.half_carry = false;
    } else if ((opcode & 0xf8u) == 0xa8) {     // XRA
        xra(get_data(opcode & 0x07u));
    } else if (opcode == 0xeeu)                 // XRI
        xra(next_byte());
    else if ((opcode & 0xf8u) == 0xb0)        // ORA
        ora(get_data(opcode & 0x07u));
    else if (opcode == 0xf6)                  // ORI
        ora(next_byte());
    else if ((opcode & 0xf8u) == 0xb8)        // CMP
        compare(get_data(opcode & 0x07u));
    else if (opcode == 0xfeu)                // CPI
        compare(next_byte());
    else if (opcode == 0x07u) {               // RLC
        flags.carry = registers.accumulator >> 7u;
        registers.accumulator = (registers.accumulator << 0x01u) | (flags.carry ? 0x01 : 0x00);
    } else if (opcode == 0x0fu) {                // RRC
        flags.carry = registers.accumulator & 0x01u;
        registers.accumulator = (registers.accumulator >> 0x01u) | (flags.carry ? 0x80u : 0x00);
    } else if(opcode == 0x17u) {                 // RAL
        byte carry = flags.carry ? 0x01u : 0x00;
        flags.carry = registers.accumulator >> 7u;
        registers.accumulator = (registers.accumulator << 1u) | carry;
    } else if(opcode == 0x1fu) {               // RAR
        byte carry = flags.carry ? 0x80u : 0x00;
        flags.carry = registers.accumulator & 1u;
        registers.accumulator = (registers.accumulator >> 1u) | carry;
    }
    else if(opcode == 0x2fu)                 // CMA
        registers.accumulator ^= 0xffu;
    else if(opcode == 0x3fu)                 // CMC
        flags.carry = !flags.carry;
    else if(opcode == 0x37u)                 // STC
        flags.carry = true;
}

void Logical::ana(byte data) {
    const byte accumulator = registers.accumulator;
    registers.accumulator &= data;

    flags.zero = registers.accumulator == 0;
    flags.sign = registers.accumulator > 0x7fu;
    flags.calculate_parity(registers.accumulator);
    flags.carry = false;
    flags.half_carry = ((registers.accumulator | accumulator) & 0x08u) != 0;
}

void Logical::xra(byte data) {
    word result = registers.accumulator ^ data;

    flags.zero = result == 0;
    flags.sign = result > 0x7fu;
    flags.calculate_parity(result);
    flags.carry = false;
    flags.half_carry = false;
    registers.accumulator = result & 0x00ffu;
}

void Logical::ora(byte data) {
    word result = registers.accumulator | data;

    flags.zero = result == 0;
    flags.sign = result > 0x7fu;
    flags.calculate_parity(result);
    flags.carry = false;
    flags.half_carry = false;
    registers.accumulator = result & 0x00ffu;
}

void Logical::compare(byte data) {
    word result = registers.accumulator - data;

    flags.zero = result == 0;
    flags.sign = (result & 0x00ff) > 0x7fu;
    flags.calculate_parity(result);
    flags.carry = (result & 0x0100u) != 0;
    flags.half_carry = (result & 0x000fu) > (registers.accumulator & 0x0fu);
}

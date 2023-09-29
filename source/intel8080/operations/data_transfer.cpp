#include "intel8080//operations/data_transfer.h"

DataTransfer::DataTransfer(Flags &flags, Memory &memory, Registers &registers)
    : Operation(flags, memory, registers) {}

void DataTransfer::execute(byte opcode) {
    byte src = opcode & 0x07u;
    byte dst = (opcode & 0x38u) >> 0x03u;
    byte regPair = ((opcode & 0x30u) >> 0x04u) + BC;

    if(opcode >= 0x40u && opcode <= 0x7f)   // MOV
        set_data(dst, get_data(src));
    else if((opcode & 0xc7u) == 0x06u)      // MVI
        set_data(dst, next_byte());
    else if((opcode & 0xcfu) == 0x01) {     // LXI
        word data = next_word();
        set_data(regPair, data);
    }
    else if(opcode == 0x3a)     // LDA
        registers.accumulator = memory.read_byte(next_word());
    else if(opcode == 0x32) {    // STA
        memory.write(next_word(), registers.accumulator);
    } else if(opcode == 0x2a) {   // LHLD
        auto address = next_word();
        registers.l = memory.read_byte(address);
        registers.h = memory.read_byte(address + 1);
    } else if(opcode == 0x22) { // SHLD
        auto address = next_word();
        memory.write(address, get_data(L));
        memory.write(address + 1, get_data(H));
    } else if((opcode & 0xcfu) == 0x0a) { // LDAX
        word address = get_data(regPair);
        registers.accumulator = memory.read_byte(address);
    } else if(opcode == 0x02 || opcode == 0x12) // STAX
        memory.write(get_data(regPair), registers.accumulator);
    else if(opcode == 0xeb) {
        auto temp = registers.h;
        registers.h = registers.d;
        registers.d = temp;

        temp = registers.l;
        registers.l = registers.e;
        registers.e = temp;
    }
}

#include "cpu.h"

void Flags::calculate_parity(byte value) {
    byte bit_set = 0;
    for(auto i = 0; i < 8; i++) {
        if(value & (0x01u << i))
            bit_set++;
    }
    parity = (bit_set & 0x01u) == 0;
}

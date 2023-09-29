#ifndef INVADERS_CABINET_H
#define INVADERS_CABINET_H

#include "memory.h"
#include "constants.h"
#include "intel8080/intel_8080.h"

class Cabinet {
public:
    Cabinet();
    ~Cabinet() = default;

    word get_program_counter() const ;
    qword get_cycle_count() const;
    void interrupt(word address);
    void set_cycle_count(word count);

    byte get_accumulator() const;
    void set_accumulator(byte value);
    void increment_program_counter(byte increment);

    Memory memory;
    Intel8080 cpu;

    byte next_interrupt = 0x08;
    byte screen_buffer[256][224][4]{};

    // space invaders ports and shift registers
    byte port1 = 0x08, port2 = 0x03;
    byte shift0 = 0, shift1 = 0, shiftOffset = 0;
};
#endif

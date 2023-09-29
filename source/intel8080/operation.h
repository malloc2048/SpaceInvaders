#ifndef INVADERS_OPERATION_H
#define INVADERS_OPERATION_H

#include "constants.h"
#include "intel8080/cpu.h"

class Operation {
public:
    Operation() = delete;
    ~Operation() = default;
    Operation(Flags& flags, Memory& memory, Registers& registers);

    [[nodiscard]] byte next_byte() const;
    [[nodiscard]] word next_word() const;
    [[nodiscard]] word get_data(byte src) const;
    void set_data(byte dst, word data);

    virtual void execute(byte opcode) = 0;

protected:
    Flags& flags;
    Memory& memory;
    Registers& registers;
};
#endif

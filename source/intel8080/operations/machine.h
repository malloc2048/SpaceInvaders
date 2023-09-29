#ifndef INVADERS_MACHINE_H
#define INVADERS_MACHINE_H

#include "intel8080/operation.h"

class Machine: public Operation {
public:
    Machine(Flags& flags, Memory& memory, Registers& registers);
    ~Machine() = default;

    void execute(byte opcode) override;

    void xthl();
    void setPSW();
    void pop(byte dst);
    word getPSW() const;
    void push(word data);
};
#endif

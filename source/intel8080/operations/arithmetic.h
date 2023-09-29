#ifndef EMULATION_ARITHMETIC_H
#define EMULATION_ARITHMETIC_H

#include "intel8080/operation.h"

class Arithmetic: public Operation {
public:
    Arithmetic(Flags& flags, Memory& memory, Registers& registers);
    ~Arithmetic() = default;

    void execute(byte opcode) override;

protected:
    void daa();
    void dad(byte src);
    void add(byte data);
    void subtract(byte data);
    void increment(byte dst);
    void decrement(byte dst);

private:
};

#endif

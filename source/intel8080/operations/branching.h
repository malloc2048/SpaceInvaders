#ifndef EMULATION_BRANCHING_H
#define EMULATION_BRANCHING_H

#include "intel8080/operation.h"

class Branching: public Operation {
public:
    Branching(Flags& flags, Memory& memory, Registers& registers);
    ~Branching() = default;

    void execute(byte opcode) override;

protected:
    void ret();
    void call();
    void restart(byte restartNum);
    bool check_condition(byte condition);

private:
};
#endif

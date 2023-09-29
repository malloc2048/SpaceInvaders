#ifndef INVADERS_DATA_TRANSFER_H
#define INVADERS_DATA_TRANSFER_H

#include "intel8080//operation.h"

class DataTransfer: public Operation {
public:
    DataTransfer(Flags& flags, Memory& memory, Registers& registers);
    ~DataTransfer() = default;

    void execute(byte opcode) override;
};
#endif

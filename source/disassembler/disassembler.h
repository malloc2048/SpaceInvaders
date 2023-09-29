#ifndef EMULATION_DISASSEMBLER_H
#define EMULATION_DISASSEMBLER_H

#include <string>

class Disassembler {
public:
    Disassembler() = default;
    ~Disassembler() = default;

    void disassemble(const std::string& filepath);

protected:
private:
};
#endif

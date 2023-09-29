#include <gtest/gtest.h>
#include "intel8080/operations/branching.h"

class BranchingTestFixture: public testing::Test {
public:
    BranchingTestFixture() = default;
    ~BranchingTestFixture() override = default;

protected:
    Flags flags;
    Memory memory;
    Registers registers;
    Branching branching = Branching(flags, memory, registers);
};

TEST_F(BranchingTestFixture, jump) {
    registers.program_counter = 0x5000;
    memory.write_word(0x5000, 0x0a0a);

    branching.execute(0xc3);
    ASSERT_EQ(0x0a0a, registers.program_counter);

    std::vector<byte> opcodes = {0xca, 0xda, 0xea, 0xfa};
    std::vector<byte> notOpcodes = {0xc2, 0xd2, 0xe2, 0xf2};

    // conditional jumps with conditions set to take jumps
    flags.sign = false;
    flags.zero = false;
    flags.carry = false;
    flags.parity = false;
    for (auto opcode: notOpcodes) {
        registers.program_counter = 0x5000;

        branching.execute(opcode);
        ASSERT_EQ(0x0a0a, registers.program_counter);
    }

    flags.sign = true;
    flags.zero = true;
    flags.carry = true;
    flags.parity = true;
    for (auto opcode: opcodes) {
        registers.program_counter = 0x5000;
        memory.write_word(0x5000, 0x0a0a);

        branching.execute(opcode);
        ASSERT_EQ(0x0a0a, registers.program_counter);
    }

    for (auto opcode: notOpcodes) {
        registers.program_counter = 0x5000;

        branching.execute(opcode);
        ASSERT_EQ(0x5002, registers.program_counter);
    }

    // conditional jumps with conditions not set to take jumps
    flags.sign = false;
    flags.zero = false;
    flags.carry = false;
    flags.parity = false;
    for (auto opcode: opcodes) {
        registers.program_counter = 0x5000;
        memory.write_word(0x5000, 0x0a0a);

        branching.execute(opcode);
        ASSERT_EQ(0x5002, registers.program_counter);
    }
}

TEST_F(BranchingTestFixture, call) {
    registers.stack_pointer = 0x2400;
    registers.program_counter = 0x5000;
    memory.write_word(0x5000, 0x0a0a);

    branching.execute(0xcd);
    ASSERT_EQ(0x23fe, registers.stack_pointer);
    ASSERT_EQ(0x0a0a, registers.program_counter);
    ASSERT_EQ(0x5002, memory.read_word(registers.stack_pointer));

    std::vector<byte> opcodes = {0xcc, 0xdc, 0xec, 0xfc};
    std::vector<byte> notOpcodes = {0xc4, 0xd4, 0xe4, 0xf4};

    // conditional call with conditions set to take jumps
    flags.sign = false;
    flags.zero = false;
    flags.carry = false;
    flags.parity = false;
    for (auto opcode: notOpcodes) {
        registers.stack_pointer = 0x2400;
        registers.program_counter = 0x5000;

        branching.execute(opcode);
        ASSERT_EQ(0x23fe, registers.stack_pointer);
        ASSERT_EQ(0x0a0a, registers.program_counter);
        ASSERT_EQ(0x5002, memory.read_word(registers.stack_pointer));
    }

    for (auto opcode: opcodes) {
        registers.stack_pointer = 0x2400;
        registers.program_counter = 0x5000;

        branching.execute(opcode);
        ASSERT_EQ(0x2400, registers.stack_pointer);
        ASSERT_EQ(0x5002, registers.program_counter);
    }

    flags.sign = true;
    flags.zero = true;
    flags.carry = true;
    flags.parity = true;
    for (auto opcode: opcodes) {
        registers.stack_pointer = 0x2400;
        registers.program_counter = 0x5000;

        branching.execute(opcode);
        ASSERT_EQ(0x23fe, registers.stack_pointer);
        ASSERT_EQ(0x0a0a, registers.program_counter);
        ASSERT_EQ(0x5002, memory.read_word(registers.stack_pointer));
    }

    for (auto opcode: notOpcodes) {
        registers.stack_pointer = 0x2400;
        registers.program_counter = 0x5000;

        branching.execute(opcode);
        ASSERT_EQ(0x2400, registers.stack_pointer);
        ASSERT_EQ(0x5002, registers.program_counter);
    }
}

TEST_F(BranchingTestFixture, ret) {
    registers.stack_pointer = 0x23fe;
    registers.program_counter = 0x5000;
    memory.write_word(0x23fe, 0x0a0a);

    branching.execute(0xc9);
    ASSERT_EQ(0x2400, registers.stack_pointer);
    ASSERT_EQ(0x0a0a, registers.program_counter);

    std::vector<byte> opcodes = {0xc8, 0xd8, 0xe8, 0xf8};
    std::vector<byte> notOpcodes = {0xc0, 0xd0, 0xe0, 0xf0};

    // conditional call with conditions set to take jumps
    flags.sign = false;
    flags.zero = false;
    flags.carry = false;
    flags.parity = false;
    for (auto opcode: notOpcodes) {
        registers.stack_pointer = 0x23fe;
        registers.program_counter = 0x5000;
        memory.write_word(0x23fe, 0x0a0a);

        branching.execute(opcode);
        ASSERT_EQ(0x2400, registers.stack_pointer);
        ASSERT_EQ(0x0a0a, registers.program_counter);
    }

    for (auto opcode: opcodes) {
        registers.stack_pointer = 0x23fe;
        registers.program_counter = 0x5000;
        memory.write_word(0x23fe, 0x0a0a);

        branching.execute(opcode);
        ASSERT_EQ(0x23fe, registers.stack_pointer);
        ASSERT_EQ(0x5000, registers.program_counter);
    }

    flags.sign = true;
    flags.zero = true;
    flags.carry = true;
    flags.parity = true;
    for (auto opcode: opcodes) {
        registers.stack_pointer = 0x23fe;
        registers.program_counter = 0x5000;
        memory.write_word(0x23fe, 0x0a0a);

        branching.execute(opcode);
        ASSERT_EQ(0x2400, registers.stack_pointer);
        ASSERT_EQ(0x0a0a, registers.program_counter);
    }

    for (auto opcode: notOpcodes) {
        registers.stack_pointer = 0x23fe;
        registers.program_counter = 0x5000;
        memory.write_word(0x23fe, 0x0a0a);

        branching.execute(opcode);
        ASSERT_EQ(0x23fe, registers.stack_pointer);
        ASSERT_EQ(0x5000, registers.program_counter);
    }
}

TEST_F(BranchingTestFixture, restart) {
    std::vector<byte> opcodes = {0xc7, 0xdf, 0xe7, 0xff, 0xc7, 0xdf, 0xe7, 0xff};
    for (auto opcode: opcodes) {
        registers.stack_pointer = 0x2400;
        registers.program_counter = 0x5000;
        auto rstNumber = (opcode & 0x38u) >> 0x03u;

        branching.execute(opcode);
        ASSERT_EQ(0x23fe, registers.stack_pointer);
        ASSERT_EQ(0x5000, memory.read_word(registers.stack_pointer));
        ASSERT_EQ(byte(0x0008 * rstNumber), registers.program_counter);
    }
}

TEST_F(BranchingTestFixture, JumpToHL) {
    registers.program_counter = 0x5000;
    registers.write(HL, 0x18d4);

    branching.execute(0xe9);
    ASSERT_EQ(0x18d4, registers.program_counter);
}

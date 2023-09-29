#include <random>
#include <gtest/gtest.h>
#include "intel8080/operations/machine.h"

class MachineTestFixture: public testing::Test {
public:
    MachineTestFixture() = default;
    ~MachineTestFixture() override = default;

protected:
    Flags flags;
    Memory memory;
    Registers registers;
    Machine operation = Machine(flags, memory, registers);
};

TEST_F(MachineTestFixture, push) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    std::vector<byte> opcodes = { 0xc5u, 0xd5u, 0xe5u };
    for(auto& opcode : opcodes) {
        registers.stack_pointer = 0x2400;

        auto value = dist(mt);
        auto rp = ((opcode & 0x30u) >> 0x04u) + BC;
        registers.write(rp, value);

        operation.execute(opcode);
        ASSERT_EQ(0x23fe, registers.stack_pointer);
        ASSERT_EQ(value, memory.read_word(registers.stack_pointer));
    }
}

TEST_F(MachineTestFixture, pushPSW) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<word> dist(0, 0xff);

    auto expectedPSW = dist(mt);
    expectedPSW &= 0xff00u;
    expectedPSW |= 0x001fu;
    registers.accumulator = expectedPSW >> 0x08u;
    registers.stack_pointer = 0x2400;

    flags.zero = true;
    flags.sign = true;
    flags.carry = true;
    flags.parity = true;
    flags.half_carry = true;

    operation.execute(0xf5);
    ASSERT_EQ(0x23fe, registers.stack_pointer);
    ASSERT_EQ(expectedPSW, memory.read_word(registers.stack_pointer));
}

TEST_F(MachineTestFixture, pop) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<word> dist(0, 0xff);

    std::vector<byte> opcodes = { 0xc1u, 0xd1u, 0xe1u };
    for(auto& opcode : opcodes) {
        auto value = dist(mt);
        registers.stack_pointer = 0x23fe;
        memory.write_word(registers.stack_pointer, value);
        auto rp = ((opcode & 0x30u) >> 0x04u) + BC;

        operation.execute(opcode);
        ASSERT_EQ(0x2400, registers.stack_pointer);
        ASSERT_EQ(value, registers.read(rp));
    }
}

TEST_F(MachineTestFixture, popPSW) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<word> dist(0, 0xff);

    registers.stack_pointer = 0x23fe;
    auto value = dist(mt) & 0xff00u;
    memory.write_word(registers.stack_pointer, value);

    operation.execute(0xf1);
    ASSERT_EQ(0x2400, registers.stack_pointer);
    ASSERT_EQ(byte(value >> 0x08u), registers.accumulator);
    ASSERT_FALSE(flags.sign);
    ASSERT_FALSE(flags.zero);
    ASSERT_FALSE(flags.carry);
    ASSERT_FALSE(flags.parity);
    ASSERT_FALSE(flags.half_carry);
}

TEST_F(MachineTestFixture, xthl) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<word> dist(0, 0xff);

    auto value = dist(mt);
    registers.stack_pointer = 0x23fe;
    memory.write_word(registers.stack_pointer, value);

    auto value2 = dist(mt);
    registers.write(HL, value2);

    operation.execute(0xe3);
    ASSERT_EQ(value, registers.read(HL));
    ASSERT_EQ(value2, memory.read_word(registers.stack_pointer));
}

TEST_F(MachineTestFixture, SPHL) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<word> dist(0, 0xff);

    auto value = dist(mt);
    registers.stack_pointer = 0x2400;
    registers.write(HL, value);

    operation.execute(0xf9u);
    ASSERT_EQ(value, registers.stack_pointer);
}

TEST_F(MachineTestFixture, InterruptAndHalt) {
    flags.halted = false;
    flags.interruptEnabled = false;

    operation.execute(0xfb);
    ASSERT_TRUE(flags.interruptEnabled);

    operation.execute(0xf3u);
    ASSERT_FALSE(flags.interruptEnabled);

    operation.execute(0x76u);
    ASSERT_TRUE(flags.halted);
}

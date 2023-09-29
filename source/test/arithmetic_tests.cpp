#include <random>
#include <gtest/gtest.h>
#include "intel8080/operations/arithmetic.h"

class ArithmeticTestFixture : public testing::Test {
public:
    ArithmeticTestFixture() = default;
    ~ArithmeticTestFixture() override = default;

protected:
    Flags flags;
    Memory memory;
    Registers registers;
    Arithmetic arithmetic = Arithmetic(flags, memory, registers);
};

TEST_F(ArithmeticTestFixture, add) {
    registers.b = 0x56;
    registers.c = 0x02;
    registers.d = 0x03;
    registers.e = 0x04;
    registers.h = 0x50;
    registers.l = 0x06;
    registers.accumulator = 0xaa;
    memory.write(0x5006, 0xff);

    arithmetic.execute(0x80);
    ASSERT_FALSE(flags.sign);
    ASSERT_TRUE(flags.zero);
    ASSERT_TRUE(flags.carry);
    ASSERT_TRUE(flags.parity);
    ASSERT_TRUE(flags.half_carry);
    ASSERT_EQ(0x00, registers.accumulator);

    arithmetic.execute(0x81);
    ASSERT_FALSE(flags.sign);
    ASSERT_FALSE(flags.zero);
    ASSERT_FALSE(flags.carry);
    ASSERT_FALSE(flags.parity);
    ASSERT_FALSE(flags.half_carry);
    ASSERT_EQ(0x02, registers.accumulator);

    arithmetic.execute(0x82);
    ASSERT_FALSE(flags.sign);
    ASSERT_FALSE(flags.zero);
    ASSERT_FALSE(flags.carry);
    ASSERT_TRUE(flags.parity);
    ASSERT_FALSE(flags.half_carry);
    ASSERT_EQ(0x05, registers.accumulator);

    arithmetic.execute(0x83);
    ASSERT_FALSE(flags.sign);
    ASSERT_FALSE(flags.zero);
    ASSERT_FALSE(flags.carry);
    ASSERT_TRUE(flags.parity);
    ASSERT_FALSE(flags.half_carry);
    ASSERT_EQ(0x09, registers.accumulator);

    arithmetic.execute(0x84);
    ASSERT_FALSE(flags.sign);
    ASSERT_FALSE(flags.zero);
    ASSERT_FALSE(flags.carry);
    ASSERT_TRUE(flags.parity);
    ASSERT_FALSE(flags.half_carry);
    ASSERT_EQ(0x59, registers.accumulator);

    arithmetic.execute(0x85);
    ASSERT_FALSE(flags.sign);
    ASSERT_FALSE(flags.zero);
    ASSERT_FALSE(flags.carry);
    ASSERT_TRUE(flags.parity);
    ASSERT_FALSE(flags.half_carry);
    ASSERT_EQ(0x5f, registers.accumulator);

    arithmetic.execute(0x86);
    ASSERT_FALSE(flags.sign);
    ASSERT_FALSE(flags.zero);
    ASSERT_TRUE(flags.carry);
    ASSERT_FALSE(flags.parity);
    ASSERT_TRUE(flags.half_carry);
    ASSERT_EQ(0x5e, registers.accumulator);

    arithmetic.execute(0x87);
    ASSERT_TRUE(flags.sign);
    ASSERT_FALSE(flags.zero);
    ASSERT_FALSE(flags.carry);
    ASSERT_FALSE(flags.parity);
    ASSERT_TRUE(flags.half_carry);
    ASSERT_EQ(0xbc, registers.accumulator);
}

TEST_F(ArithmeticTestFixture, addWithCarry) {
    registers.b = 0x56;
    registers.c = 0x02;
    registers.d = 0x03;
    registers.e = 0x04;
    registers.h = 0x50;
    registers.l = 0x06;
    flags.carry = true;
    registers.accumulator = 0xaa;
    memory.write(0x5006, 0xff);

    arithmetic.execute(0x88);
    ASSERT_EQ(0x01, registers.accumulator);

    arithmetic.execute(0x89);
    ASSERT_EQ(0x04, registers.accumulator);

    arithmetic.execute(0x8a);
    ASSERT_EQ(0x07, registers.accumulator);

    arithmetic.execute(0x8b);
    ASSERT_EQ(0x0b, registers.accumulator);

    arithmetic.execute(0x8c);
    ASSERT_EQ(0x5b, registers.accumulator);

    arithmetic.execute(0x8d);
    ASSERT_EQ(0x61, registers.accumulator);

    arithmetic.execute(0x8e);
    ASSERT_EQ(0x60, registers.accumulator);

    arithmetic.execute(0x8f);
    ASSERT_EQ(0xc1, registers.accumulator);
}

TEST_F(ArithmeticTestFixture, addImmediate) {
    registers.accumulator = 0xaa;
    registers.program_counter = 0x5000;
    memory.write(0x5000, 0x56);
    memory.write(0x5001, 0xaa);

    arithmetic.execute(0xc6);
    ASSERT_EQ(0x00, registers.accumulator);
    ASSERT_EQ(0x5001, registers.program_counter);

    arithmetic.execute(0xce);
    ASSERT_EQ(0xab, registers.accumulator);
    ASSERT_EQ(0x5002, registers.program_counter);
}

TEST_F(ArithmeticTestFixture, addWrongOpcode) {
    registers.b = 0x01;
    registers.c = 0x02;
    registers.d = 0x03;
    registers.e = 0x04;
    registers.h = 0x50;
    registers.l = 0x06;
    registers.accumulator = 0x00;
    memory.write(0x5006, 0x07);

    flags.sign = true;
    flags.zero = true;
    flags.carry = true;
    flags.parity = true;
    flags.half_carry = true;

    arithmetic.execute(0x79);
    ASSERT_TRUE(flags.sign);
    ASSERT_TRUE(flags.zero);
    ASSERT_TRUE(flags.carry);
    ASSERT_TRUE(flags.parity);
    ASSERT_TRUE(flags.half_carry);
    ASSERT_EQ(0x00, registers.accumulator);
}

TEST_F(ArithmeticTestFixture, subtraction) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 255);

    for (byte opcode = 0x90; opcode < 0x98; opcode++) {
        byte value = dist(mt);
        registers.accumulator = 0x01;
        memory.write(0x5000, value);
        registers.write(HL, 0x5000);
        registers.write(opcode & 0x0fu, value);

        arithmetic.execute(opcode);
        if (opcode == 0x97) {
            ASSERT_EQ(byte(0), registers.accumulator);
        } else {
            ASSERT_EQ(byte(1 - value), registers.accumulator);
        }
    }
}

TEST_F(ArithmeticTestFixture, subtractionWithBorrow) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 255);

    for (byte opcode = 0x98; opcode < 0xa0; opcode++) {
        flags.carry = true;
        byte value = dist(mt);
        registers.accumulator = 0x01;
        memory.write(0x5000, value);

        registers.write(HL, 0x5000);
        registers.write((opcode & 0x0fu) - 8, value);

        arithmetic.execute(opcode);
        if (opcode == 0x9f) {
            ASSERT_EQ(byte(0xff), registers.accumulator);
        } else {
            ASSERT_EQ(byte(1 - (value + 1)), registers.accumulator);
        }
    }
}

TEST_F(ArithmeticTestFixture, subtractionImmediate) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 255);
    byte value = dist(mt);

    registers.accumulator = 0x01;
    registers.program_counter = 0x5000;
    memory.write(0x5000, value);

    arithmetic.execute(0xd6);
    ASSERT_EQ(byte(1 - value), registers.accumulator);

    flags.carry = true;
    registers.accumulator = 0x01;
    registers.program_counter = 0x5000;

    arithmetic.execute(0xde);
    ASSERT_EQ(byte(1 - (value + 1)), registers.accumulator);
}

TEST_F(ArithmeticTestFixture, increment) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 255);
    std::vector<byte> opcodes = {0x04, 0x0c, 0x14, 0x1c, 0x24, 0x2c, 0x34, 0x3c};

    for (auto opcode: opcodes) {
        byte value = dist(mt);
        auto reg = (opcode & 0x38u) >> 0x03u;

        registers.write(HL,
                        0x5000);
        memory.write(0x5000, value);
        registers.write(reg, value);

        arithmetic.execute(opcode);
        if (opcode == 0x34) {
            ASSERT_EQ(byte(value + 1), memory.read_byte(0x5000));
        } else {
            ASSERT_EQ(byte(value + 1), registers.read(reg)
            );
        }
    }
}

TEST_F(ArithmeticTestFixture, incrementPair) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 255);
    std::vector<byte> opcodes = {0x03, 0x13, 0x23, 0x33};

    for (
        auto opcode
        : opcodes) {
        byte value = dist(mt);
        auto reg = ((opcode & 0x30u) >> 0x04u) + BC;
        registers.
            write(reg, value
        );

        arithmetic.
            execute(opcode);
        ASSERT_EQ(byte(value + 1), byte(registers.read(reg))
        );
    }
}

TEST_F(ArithmeticTestFixture, decrement) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 255);
    std::vector<byte> opcodes = {0x05, 0x0d, 0x15, 0x1d, 0x25, 0x2d, 0x35, 0x3d};

    for (auto opcode: opcodes) {
        byte value = dist(mt);
        auto reg = (opcode & 0x38u) >> 0x03u;

        registers.write(HL, 0x5000);
        memory.write(0x5000, value);
        registers.write(reg, value);

        arithmetic.execute(opcode);
        if (opcode == 0x35) {
            ASSERT_EQ(byte(value - 1), memory.read_byte(0x5000));
        } else {
            ASSERT_EQ(byte(value - 1), registers.read(reg));
        }
    }
}

TEST_F(ArithmeticTestFixture, decrementPair) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 255);
    std::vector<byte> opcodes = {0x0b, 0x1b, 0x2b, 0x3b};

    for (auto opcode: opcodes) {
        byte value = dist(mt);
        auto reg = ((opcode & 0x30u) >> 0x04u) + BC;
        registers.write(reg, value);

        arithmetic.execute(opcode);
        ASSERT_EQ(byte(value - 1), byte(registers.read(reg)));
    }
}

TEST_F(ArithmeticTestFixture, dad) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 255);
    std::vector<byte> opcodes = {0x09, 0x19, 0x29, 0x39};

    for (auto opcode: opcodes) {
        byte value = dist(mt);
        auto reg = ((opcode & 0x30u) >> 0x04u) + BC;

        if (opcode != 0x29)
            registers.write(HL, 0x01);

        registers.write(reg, value);
        arithmetic.execute(opcode);

        if (opcode == 0x29)
            ASSERT_EQ(word(value + value), registers.read(HL));
        else
            ASSERT_EQ(word(value + 1), registers.read(HL));
    }
}

TEST_F(ArithmeticTestFixture, daa) {
    registers.accumulator = 0x00;
    arithmetic.execute(0x27);
    ASSERT_EQ(byte(0x00), registers.accumulator);

    registers.accumulator = 0x0a;
    arithmetic.execute(0x27);
    ASSERT_EQ(byte(0x06 + 0x0a), registers.accumulator);

    registers.accumulator = 0xa0;
    arithmetic.execute(0x27);
    ASSERT_EQ(byte(0x60 + 0xa0), registers.accumulator);

    flags.carry = false;
    flags.half_carry = true;
    registers.accumulator = 0x00;
    arithmetic.execute(0x27);
    ASSERT_EQ(byte(0x06), registers.accumulator);

    flags.carry = true;
    flags.half_carry = false;
    registers.accumulator = 0x00;
    arithmetic.execute(0x27);
    ASSERT_EQ(byte(0x60), registers.accumulator);
}

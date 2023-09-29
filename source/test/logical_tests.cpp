#include <random>
#include <gtest/gtest.h>
#include "intel8080//operations/logical.h"

class LogicalTestFixture: public testing::Test {
public:
    LogicalTestFixture() = default;
    ~LogicalTestFixture() override = default;

protected:
    Flags flags;
    Memory memory;
    Registers registers;
    Logical operation = Logical(flags, memory, registers);
};

TEST_F(LogicalTestFixture, ana) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    std::vector<byte> opcodes = { 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7 };
    for(auto& opcode : opcodes) {
        auto value = dist(mt);
        registers.accumulator = 0xff;

        if((opcode & 0x07u) == 0x06) {
            memory.write(0x2000, value);
            registers.write(HL, 0x2000);
        } else {
            registers.write(opcode & 0x07u, value);
        }

        operation.execute(opcode);
        ASSERT_EQ(value, registers.accumulator);
    }
}

TEST_F(LogicalTestFixture, ani) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    auto value = dist(mt);
    registers.accumulator = 0xff;
    registers.program_counter = 0x2000;
    memory.write(0x2000, value);

    operation.execute(0xe6);
    ASSERT_EQ(value, registers.accumulator);
}

TEST_F(LogicalTestFixture, xra) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    std::vector<byte> opcodes = { 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf };
    for(auto& opcode : opcodes) {
        auto value = dist(mt);
        registers.accumulator = 0x00;

        if((opcode & 0x07u) == 0x06) {
            memory.write(0x2000, value);
            registers.write(HL, 0x2000);
        } else {
            registers.write(opcode & 0x07u, value);
        }

        operation.execute(opcode);

        if((opcode & 0x07u) == 0x07)
            ASSERT_EQ(0x00, registers.accumulator);
        else
            ASSERT_EQ(value, registers.accumulator);

        ASSERT_FALSE(flags.carry);
        ASSERT_FALSE(flags.half_carry);
    }
}

TEST_F(LogicalTestFixture, xri) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    auto value = dist(mt);
    registers.accumulator = 0x00;
    registers.program_counter = 0x2000;
    memory.write(0x2000, value);

    operation.execute(0xeeu);
    ASSERT_EQ(value, registers.accumulator);
}

TEST_F(LogicalTestFixture, ora) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    std::vector<byte> opcodes = { 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7 };
    for(auto& opcode : opcodes) {
        auto value = dist(mt);
        registers.accumulator = 0x00;

        if((opcode & 0x07u) == 0x06) {
            memory.write(0x2000, value);
            registers.write(HL, 0x2000);
        } else {
            registers.write(opcode & 0x07u, value);
        }

        operation.execute(opcode);

        ASSERT_EQ(value, registers.accumulator);
    }
}

TEST_F(LogicalTestFixture, ori) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    auto value = dist(mt);
    registers.accumulator = 0x00;
    registers.program_counter = 0x2000;
    memory.write(0x2000, value);

    operation.execute(0xf6);
    ASSERT_EQ(value, registers.accumulator);
}

TEST_F(LogicalTestFixture, cmpEqual) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    std::vector<byte> opcodes = {0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf};
    for (auto &opcode : opcodes) {
        auto value = dist(mt);
        registers.accumulator = value;

        if ((opcode & 0x07u) == 0x06) {
            memory.write(0x2000, value);
            registers.write(HL, 0x2000);
        } else {
            registers.write(opcode & 0x07u, value);
        }

        operation.execute(opcode);
        ASSERT_EQ(value, registers.accumulator);
        ASSERT_TRUE(flags.zero);
        ASSERT_FALSE(flags.sign);
        ASSERT_TRUE(flags.parity);
        ASSERT_FALSE(flags.carry);
        ASSERT_FALSE(flags.half_carry);
    }
}

TEST_F(LogicalTestFixture, cmpA_Larger) {
    std::vector<byte> opcodes = {0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf};

    for(auto& opcode : opcodes) {
        registers.accumulator = 0xaa;

        if((opcode & 0x07u) == 0x06) {
            memory.write(0x2000, 0x55);
            registers.write(HL, 0x2000);
        } else {
            registers.write(opcode & 0x07u, 0x55);
        }

        operation.execute(opcode);
        if((opcode & 0x07u) == 0x07){
            ASSERT_EQ(0x55, registers.accumulator);
            ASSERT_TRUE(flags.zero);
            ASSERT_FALSE(flags.sign);
            ASSERT_TRUE(flags.parity);
            ASSERT_FALSE(flags.carry);
            ASSERT_FALSE(flags.half_carry);
        } else {
            ASSERT_EQ(0xaa, registers.accumulator);
            ASSERT_FALSE(flags.zero);
            ASSERT_FALSE(flags.sign);
            ASSERT_FALSE(flags.carry);
            ASSERT_TRUE(flags.parity);
            ASSERT_FALSE(flags.half_carry);
        }
    }
}

TEST_F(LogicalTestFixture, cmpA_Smaller) {
    std::vector<byte> opcodes = {0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf};

    for(auto& opcode : opcodes) {
        registers.accumulator = 0xaa;

        if((opcode & 0x07u) == 0x06) {
            memory.write(0x2000, 0x55);
            registers.write(HL, 0x2000);
        } else {
            registers.write(opcode & 0x07u, 0x55);
        }

        operation.execute(opcode);
        if((opcode & 0x07u) == 0x07){
            ASSERT_EQ(0x55, registers.accumulator);
            ASSERT_TRUE(flags.zero);
            ASSERT_FALSE(flags.sign);
            ASSERT_TRUE(flags.parity);
            ASSERT_FALSE(flags.carry);
            ASSERT_FALSE(flags.half_carry);
        } else {
            ASSERT_EQ(0xaa, registers.accumulator);
            ASSERT_FALSE(flags.zero);
            ASSERT_FALSE(flags.sign);
            ASSERT_FALSE(flags.carry);
            ASSERT_TRUE(flags.parity);
            ASSERT_FALSE(flags.half_carry);
        }
    }
}

TEST_F(LogicalTestFixture, cpi) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    auto value = dist(mt);
    registers.accumulator = value;
    registers.program_counter = 0x2000;
    memory.write(0x2000, value);

    operation.execute(0xfe);
    ASSERT_EQ(value, registers.accumulator);
    ASSERT_TRUE(flags.zero);
    ASSERT_FALSE(flags.sign);
    ASSERT_TRUE(flags.parity);
    ASSERT_FALSE(flags.carry);
    ASSERT_FALSE(flags.half_carry);
}

TEST_F(LogicalTestFixture, cpi_ALarger) {
    registers.accumulator = 0xaa;
    registers.program_counter = 0x2000;
    memory.write(0x2000, 0x55);

    operation.execute(0xfe);
    ASSERT_EQ(0xaa, registers.accumulator);
    ASSERT_FALSE(flags.zero);
    ASSERT_FALSE(flags.sign);
    ASSERT_TRUE(flags.parity);
    ASSERT_FALSE(flags.carry);
    ASSERT_FALSE(flags.half_carry);
}

TEST_F(LogicalTestFixture, cpi_ASmaller) {
    registers.accumulator = 0x55;
    registers.program_counter = 0x2000;
    memory.write(0x2000, 0xaa);

    operation.execute(0xfe);
    ASSERT_EQ(0x55, registers.accumulator);
    ASSERT_FALSE(flags.zero);
    ASSERT_TRUE(flags.sign);
    ASSERT_TRUE(flags.carry);
    ASSERT_FALSE(flags.parity);
    ASSERT_TRUE(flags.half_carry);
}

TEST_F(LogicalTestFixture, rlc) {
    registers.accumulator = 0x55;

    operation.execute(0x07);
    ASSERT_EQ(0xaa, registers.accumulator);
    ASSERT_FALSE(flags.carry);

    operation.execute(0x07);
    ASSERT_EQ(0x55, registers.accumulator);
    ASSERT_TRUE(flags.carry);
}

TEST_F(LogicalTestFixture, rrc) {
    registers.accumulator = 0xaa;

    operation.execute(0x0f);
    ASSERT_EQ(0x55, registers.accumulator);
    ASSERT_FALSE(flags.carry);

    operation.execute(0x0f);
    ASSERT_EQ(0xaa, registers.accumulator);
    ASSERT_TRUE(flags.carry);
}

TEST_F(LogicalTestFixture, ral) {
    flags.carry = false;
    registers.accumulator = 0x55;

    operation.execute(0x17);
    ASSERT_EQ(0xaa, registers.accumulator);
    ASSERT_FALSE(flags.carry);

    operation.execute(0x17);
    ASSERT_EQ(0x54, registers.accumulator);
    ASSERT_TRUE(flags.carry);

    operation.execute(0x17);
    ASSERT_EQ(0xa9, registers.accumulator);
    ASSERT_FALSE(flags.carry);
}

TEST_F(LogicalTestFixture, rar) {
    flags.carry = false;
    registers.accumulator = 0xaa;

    operation.execute(0x1f);
    ASSERT_EQ(0x55, registers.accumulator);
    ASSERT_FALSE(flags.carry);

    operation.execute(0x1f);
    ASSERT_EQ(0x2a, registers.accumulator);
    ASSERT_TRUE(flags.carry);

    operation.execute(0x1f);
    ASSERT_EQ(0x95, registers.accumulator);
    ASSERT_FALSE(flags.carry);
}

TEST_F(LogicalTestFixture, cma) {
    registers.accumulator = 0xaa;
    operation.execute(0x2f);
    ASSERT_EQ(0x55, registers.accumulator);
}

TEST_F(LogicalTestFixture, cmc) {
    flags.carry = false;
    operation.execute(0x3f);
    ASSERT_TRUE(flags.carry);

    operation.execute(0x3f);
    ASSERT_FALSE(flags.carry);
}

TEST_F(LogicalTestFixture, stc) {
    flags.carry = false;
    operation.execute(0x37);
    ASSERT_TRUE(flags.carry);

    operation.execute(0x37);
    ASSERT_TRUE(flags.carry);
}

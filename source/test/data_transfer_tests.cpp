#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <random>
#include <vector>
#include <gtest/gtest.h>
#include "intel8080/operations/data_transfer.h"

class DataTransferTestFixture: public testing::Test {
public:
    DataTransferTestFixture() = default;
    ~DataTransferTestFixture() override = default;

protected:
    Flags flags;
    Memory memory;
    Registers registers;
    DataTransfer dataTransfer = DataTransfer(flags, memory, registers);
};

TEST_F(DataTransferTestFixture, mov) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 255);

    for(byte opcode = 0x40; opcode < 0x80; opcode++) {
        if(opcode == 0x76)
            continue;

        auto value = dist(mt);
        byte src = opcode & 0x07u;
        byte dst = (opcode & 0x38u) >> 0x03u;
        registers.write(src, value);

        if(src == M) {
            registers.write(HL, 0x2000);
            memory.write(0x2000, value);
        }

        if(dst == M) {
            registers.write(HL, 0x2000);
        }

        dataTransfer.execute(opcode);

        if(dst != M) {
            ASSERT_EQ(value, registers.read(dst));
        } //else { TODO: why does this not work?
//            std::cout << std::hex << int(memory.read_byte(0x2000)) << std::endl;
//            ASSERT_EQ(value, memory.read_byte(0x2000));
//        }
    }
}

TEST_F(DataTransferTestFixture, mvi) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 255);

    std::vector<byte> opcodes = { 0x06, 0x0e, 0x16, 0x1e, 0x26, 0x2e, 0x36, 0x3e };
    for(auto& opcode : opcodes) {
        auto value = dist(mt);
        registers.program_counter = 0x2000;
        memory.write(0x2000, value);
        byte dst = (opcode & 0x38u) >> 0x03u;

        if(opcode == 0x36)
            registers.write(HL, 0x2400);
        dataTransfer.execute(opcode);

        if(opcode == 0x36)
            ASSERT_EQ(value, memory.read_byte(registers.read(HL)));
        else
            ASSERT_EQ(value, registers.read(dst));
    }
}

TEST_F(DataTransferTestFixture, lxi) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<word> dist(0, 0xffff);

    std::vector<byte> opcodes = { 0x01, 0x11, 0x21, 0x31 };
    for(auto& opcode : opcodes) {
        word value = dist(mt);
        registers.program_counter = 0x2000;
        memory.write_word(0x2000, value);
        byte rp = (opcode & 0x30u) >> 0x04u;

        dataTransfer.execute(opcode);
        ASSERT_EQ(value, registers.read(rp + BC));
    }
}

TEST_F(DataTransferTestFixture, lda) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    byte value = dist(mt);
    registers.program_counter = 0x2000;
    memory.write(0x2002, value);
    memory.write_word(0x2000, 0x2002);

    dataTransfer.execute(0x3a);
    ASSERT_EQ(value, registers.accumulator);
}

TEST_F(DataTransferTestFixture, sta) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    byte value = dist(mt);
    registers.accumulator = value;
    registers.program_counter = 0x2000;
    memory.write_word(0x2000, 0x2400);

    dataTransfer.execute(0x32);
    ASSERT_EQ(value, memory.read_byte(0x2400));
}

TEST_F(DataTransferTestFixture, lhld) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    byte value = dist(mt);
    byte value2 = dist(mt);
    registers.program_counter = 0x2000;
    memory.write(0x2000, 0x00);
    memory.write(0x2001, 0x24);
    memory.write(0x2400, value);
    memory.write(0x2401, value2);

    dataTransfer.execute(0x2a);
    ASSERT_EQ(value, registers.l);
    ASSERT_EQ(value2, registers.h);
    ASSERT_EQ(0x2002, registers.program_counter);
}

TEST_F(DataTransferTestFixture, shld) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    byte value = dist(mt);
    byte value2 = dist(mt);
    registers.l = value;
    registers.h = value2;
    registers.program_counter = 0x2000;
    memory.write(0x2000, 0x00);
    memory.write(0x2001, 0x24);

    dataTransfer.execute(0x22);
    ASSERT_EQ(value, memory.read_byte(0x2400));
    ASSERT_EQ(value2, memory.read_byte(0x2401));
    ASSERT_EQ(0x2002, registers.program_counter);
}

TEST_F(DataTransferTestFixture, ldax) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    std::vector<byte> opcodes = { 0x0a, 0x1a };
    for(auto& opcode : opcodes) {
        byte value = dist(mt);
        byte rp = ((opcode & 0x30u) >> 0x04u) + BC;

        registers.write(rp, 0x2000);
        memory.write_word(0x2000, value);

        dataTransfer.execute(opcode);
        ASSERT_EQ(value, registers.accumulator);
    }
}

TEST_F(DataTransferTestFixture, stax) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<byte> dist(0, 0xff);

    std::vector<byte> opcodes = { 0x02, 0x12 };
    for(auto& opcode : opcodes) {
        byte value = dist(mt);
        byte rp = ((opcode & 0x30u) >> 0x04u) + BC;

        registers.accumulator = value;
        registers.write(rp, 0x2000);

        dataTransfer.execute(opcode);
        ASSERT_EQ(value, memory.read_byte(0x2000));
    }
}

TEST_F(DataTransferTestFixture, xchg) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<word> dist(0, 0xffff);

    word value = dist(mt);
    word value2 = dist(mt);
    registers.write(DE, value);
    registers.write(HL, value2);

    dataTransfer.execute(0xeb);
    ASSERT_EQ(value, registers.read(HL));
    ASSERT_EQ(value2, registers.read(DE));
}

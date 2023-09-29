#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"

#include <gtest/gtest.h>
#include "intel8080/intel_8080.h"

class CpuTestFixture: public testing::Test {
public:
    CpuTestFixture() : cpu(memory){}
    ~CpuTestFixture() override = default;

protected:
    Intel8080 cpu;
    Memory memory;
};

TEST_F(CpuTestFixture, step) {
    cpu.load_instruction_set();
    cpu.step();
    ASSERT_EQ(0x0001, cpu.registers.program_counter);
}

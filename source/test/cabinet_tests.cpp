#include <gtest/gtest.h>
#include "cabinet/cabinet.h"

TEST(CabinetTest, cabinet) {
    Cabinet cabinet;

    cabinet.cpu.step();
    word address = 0x2000;
    cabinet.cpu.flags.interruptEnabled = true;
    cabinet.interrupt(address);

    ASSERT_EQ(address, cabinet.get_program_counter());
    ASSERT_EQ(11, cabinet.get_cycle_count());
    ASSERT_FALSE(cabinet.cpu.flags.interruptEnabled);

    cabinet.set_cycle_count(0xaaaa);
    ASSERT_EQ(0xaaaa, cabinet.get_cycle_count());

    cabinet.set_accumulator(0xaa);
    ASSERT_EQ(0xaa, cabinet.get_accumulator());

    cabinet.increment_program_counter(0x55);
    ASSERT_EQ(address + 0x55, cabinet.get_program_counter());
}

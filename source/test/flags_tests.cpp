#include <gtest/gtest.h>
#include "intel8080/cpu.h"

TEST(FlagsTest, CalculateParity) {
    Flags flags;
    ASSERT_FALSE(flags.parity);
    flags.calculate_parity(0x02);
    ASSERT_FALSE(flags.parity);

    // TODO: find a way to test all values for parity calculation.
}


#include <gtest/gtest.h>
#include "intel8080/cpu.h"

TEST(MemoryTest, WriteToRomSegment) {
    Memory memory;

    ASSERT_EQ(memory.read_byte(0x00), 0x00);
    ASSERT_FALSE(memory.write(0x00, 0xff));
    ASSERT_EQ(memory.read_byte(0x00), 0x00);
}

TEST(MemoryTest, WriteWordToRomSegment) {
    Memory memory;

    ASSERT_EQ(memory.read_word(0x0000), 0x0000);
    ASSERT_FALSE(memory.write_word(0x0000, 0xaa55));
    ASSERT_EQ(memory.read_word(0x0000), 0x0000);
}

TEST(MemoryTest, WriteWordAcrossRomBorder) {
    Memory memory;

    ASSERT_EQ(memory.read_word(Memory::ROM_SIZE - 1), 0x0000);
    ASSERT_FALSE(memory.write_word(Memory::ROM_SIZE - 1, 0xaa55));
    ASSERT_EQ(memory.read_word(Memory::ROM_SIZE - 1), 0x0000);
}

TEST(MemoryTest, WriteToRam) {
    Memory memory;

    ASSERT_EQ(memory.read_byte(Memory::ROM_SIZE), 0x00);
    ASSERT_TRUE(memory.write(Memory::ROM_SIZE, 0xaa));
    ASSERT_EQ(memory.read_byte(Memory::ROM_SIZE), 0xaa);
}

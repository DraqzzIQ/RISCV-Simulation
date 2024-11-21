#include <gtest/gtest.h>

#include "../simulator/Memory.h"

Memory memory;

TEST(MemoryTestSuite, Word)
{
    memory.Write(0, 0x12345678);
    EXPECT_EQ(memory.Read(0), 0x12345678);
}

TEST(MemoryTestSuite, HalfWord) { EXPECT_EQ(memory.ReadHalfWord(0), 0x5678); }

TEST(MemoryTestSuite, Byte) { EXPECT_EQ(memory.ReadByte(0), 0x78); }

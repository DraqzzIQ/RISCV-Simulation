#include <bitset>
#include <gtest/gtest.h>

#include "../emulator/CPU.h"

TEST(CPUTestSuite, GetFunct7)
{
    CPU cpu(nullptr);
    uint32_t instruction = 0b11101110000000000000000000000000;
    EXPECT_EQ(0b01110111, cpu.GetFunct7(instruction));
}

TEST(CPUTestSuite, GetFunct3)
{
    CPU cpu(nullptr);
    uint32_t instruction = 0b00000000000000000101000000000000;
    EXPECT_EQ(0b101, cpu.GetFunct3(instruction));
}

TEST(CPUTestSuite, GetRD)
{
    CPU cpu(nullptr);
    uint32_t instruction = 0b00000000000000000001110111000000;
    EXPECT_EQ(0b11011, cpu.GetRD(instruction));
}

TEST(CPUTestSuite, GetRS1)
{
    CPU cpu(nullptr);
    uint32_t instruction = 0b00000000000111011100000000000000;
    EXPECT_EQ(0b11011, cpu.GetRS1(instruction));
}

TEST(CPUTestSuite, GetRS2)
{
    CPU cpu(nullptr);
    uint32_t instruction = 0b00000011101110000000000000000000;
    EXPECT_EQ(0b11011, cpu.GetRS2(instruction));
}

TEST(CPUTestSuite, GetImm12)
{
    CPU cpu(nullptr);
    uint32_t instruction = 0b11111111100010000000000000000000;
    EXPECT_EQ(0b111111111000, cpu.GetImm12(instruction));
}

TEST(CPUTestSuite, GetImm5)
{
    CPU cpu(nullptr);
    uint32_t instruction = 0b11111111100010000000000000000000;
    EXPECT_EQ(0b11000, cpu.GetImm5(instruction));
}
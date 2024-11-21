#include <bitset>
#include <gtest/gtest.h>

#include "../simulator/CPUUtil.h"

TEST(CPUTestSuite, GetFunct7)
{
    uint32_t instruction = 0b11101110000000000000000000000000;
    EXPECT_EQ(0b01110111, CPUUtil::GetFunct7(instruction));
}

TEST(CPUTestSuite, GetFunct3)
{
    uint32_t instruction = 0b00000000000000000101000000000000;
    EXPECT_EQ(0b101, CPUUtil::GetFunct3(instruction));
}

TEST(CPUTestSuite, GetRD)
{
    uint32_t instruction = 0b00000000000000000001110111000000;
    EXPECT_EQ(0b11011, CPUUtil::GetRD(instruction));
}

TEST(CPUTestSuite, GetRS1)
{
    uint32_t instruction = 0b00000000000111011100000000000000;
    EXPECT_EQ(0b11011, CPUUtil::GetRS1(instruction));
}

TEST(CPUTestSuite, GetRS2)
{
    uint32_t instruction = 0b00000011101110000000000000000000;
    EXPECT_EQ(0b11011, CPUUtil::GetRS2(instruction));
}

TEST(CPUTestSuite, GetImm12)
{
    uint32_t instruction = 0b11111111100010000000000000000000;
    EXPECT_EQ(0b111111111000, CPUUtil::GetImm12(instruction));
}

TEST(CPUTestSuite, GetImm5)
{
    uint32_t instruction = 0b11111111100010000000000000000000;
    EXPECT_EQ(0b11000, CPUUtil::GetImm5(instruction));
}

TEST(CPUTestSuite, GetImm20)
{
    uint32_t instruction = 0b11111111100010000000000000000000;
    EXPECT_EQ(0b11111111100010000000, CPUUtil::GetImm20(instruction));
}

#include <bitset>
#include <gtest/gtest.h>

#include "../parser/Parser.h"
#include "../simulator/CPU.h"
#include "../simulator/CPUUtil.h"

CPU cpu(new Memory());

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
    EXPECT_EQ(static_cast<int16_t>(0b1111111111111000), CPUUtil::GetImm12(instruction));
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


TEST(CPUTestSuite, Registers)
{
    cpu.Reset();
    vector<string> strings = {"addi x0, x0, 10"};
    for (int i = 1; i < 32; i++) {
        strings.push_back("addi x" + std::to_string(i) + ", x" + std::to_string(i - 1) + ", 1");
    }
    vector<uint32_t> instructions = Parser::Parse(strings).instructions;
    cpu.LoadInstructions(instructions);

    auto result = cpu.Step();
    EXPECT_EQ(result.error, ExecutionError::NONE);
    EXPECT_EQ(result.pc, 4);
    EXPECT_EQ(result.registerChanged, true);
    RegisterChange expectedChange{0, 0};
    EXPECT_EQ(result.registerChange, expectedChange);

    for (uint8_t i = 1; i < 32; i++) {
        result = cpu.Step();
        EXPECT_EQ(result.error, ExecutionError::NONE);
        EXPECT_EQ(result.pc, 4 + i * 4);
        EXPECT_EQ(result.registerChanged, true);
        RegisterChange expectedChange{i, i};
        EXPECT_EQ(result.registerChange, expectedChange);
    }
}

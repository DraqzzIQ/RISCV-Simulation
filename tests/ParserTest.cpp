#include <bitset>

#include "../parser/Parser.h"
#include "gtest/gtest.h"

// Register tests
TEST(ParserTestSuite, PositiveRegisterBounds)
{
    ParsingResult result = Parser::Parse({"add x5, x8, x32"});
    EXPECT_EQ(result.success, false);
    EXPECT_EQ(result.errorType, ParsingError::REGISTER_OUT_OF_BOUNDS);
}

TEST(ParserTestSuite, NegativeRegisterBounds)
{
    ParsingResult result = Parser::Parse({"add x5, x8, x-1"});
    EXPECT_EQ(result.success, false);
    EXPECT_EQ(result.errorType, ParsingError::REGISTER_OUT_OF_BOUNDS);
}

TEST(ParserTestSuite, InvalidRegisterFormat)
{
    ParsingResult result = Parser::Parse({"add x5, x8, xb"});
    EXPECT_EQ(result.success, false);
    EXPECT_EQ(result.errorType, ParsingError::INVALID_REGISTER_FORMAT);
}

// R-Type tests
TEST(ParserTestSuite, ADD)
{
    ParsingResult result = Parser::Parse({"add x5, x8, x31"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000001111101000000001010110011);
}

TEST(ParserTestSuite, SUB)
{
    ParsingResult result = Parser::Parse({"sub x5, x8, x31"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b01000001111101000000001010110011);
}

TEST(ParserTestSuite, XOR)
{
    ParsingResult result = Parser::Parse({"xor x3, x12, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000001001001100100000110110011);
}

TEST(ParserTestSuite, OR)
{
    ParsingResult result = Parser::Parse({"or x3, x12, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000001001001100110000110110011);
}

TEST(ParserTestSuite, AND)
{
    ParsingResult result = Parser::Parse({"and x3, x12, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000001001001100111000110110011);
}

TEST(ParserTestSuite, SLL)
{
    ParsingResult result = Parser::Parse({"sll x3, x12, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000001001001100001000110110011);
}

TEST(ParserTestSuite, SRL)
{
    ParsingResult result = Parser::Parse({"srl x3, x12, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000001001001100101000110110011);
}

TEST(ParserTestSuite, SRA)
{
    ParsingResult result = Parser::Parse({"sra x3, x12, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b01000001001001100101000110110011);
}

TEST(ParserTestSuite, SLT)
{
    ParsingResult result = Parser::Parse({"slt x3, x12, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000001001001100010000110110011);
}

TEST(ParserTestSuite, SLTU)
{
    ParsingResult result = Parser::Parse({"sltu x3, x12, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000001001001100011000110110011);
}


// Immediate tests
TEST(ParserTestSuite, NegativeImmediateOutOfBounds)
{
    ParsingResult result = Parser::Parse({"addi x2, x0, -3124242424112312"});
    EXPECT_EQ(result.success, false);
    EXPECT_EQ(result.errorType, ParsingError::IMMEDIATE_OUT_OF_BOUNDS);
}

TEST(ParserTestSuite, PositeveImmediateOutOfBounds)
{
    ParsingResult result = Parser::Parse({"addi x2, x0, 3124242424112312"});
    EXPECT_EQ(result.success, false);
    EXPECT_EQ(result.errorType, ParsingError::IMMEDIATE_OUT_OF_BOUNDS);
}

TEST(ParserTestSuite, InvalidImmediateFormat)
{
    ParsingResult result = Parser::Parse({"addi x2, x0, 0x"});
    EXPECT_EQ(result.success, false);
    EXPECT_EQ(result.errorType, ParsingError::INVALID_IMMEDIATE_FORMAT);
}

TEST(ParserTestSuite, LargeImmediate)
{
    ParsingResult result = Parser::Parse({"addi x2, x0, 2047"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b01111111111100000000000100010011);
}

TEST(ParserTestSuite, NegativeLargeImmediate)
{
    ParsingResult result = Parser::Parse({"addi x2, x0, -2048"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b10000000000000000000000100010011);
}

// I-Type tests
TEST(ParserTestSuite, ADDI)
{
    ParsingResult result = Parser::Parse({"addi x2, x0, 31"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000001111100000000000100010011);
}

TEST(ParserTestSuite, XORI)
{
    ParsingResult result = Parser::Parse({"xori x2, x0, 76"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000100110000000100000100010011);
}

TEST(ParserTestSuite, ORI)
{
    ParsingResult result = Parser::Parse({"ori x15, x17, 77"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000100110110001110011110010011);
}

TEST(ParserTestSuite, ANDI)
{
    ParsingResult result = Parser::Parse({"andi x15, x17, 77"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000100110110001111011110010011);
}

TEST(ParserTestSuite, SLLI)
{
    ParsingResult result = Parser::Parse({"slli x15, x17, 77"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000100110110001001011110010011);
}

TEST(ParserTestSuite, SRLI)
{
    ParsingResult result = Parser::Parse({"srli x15, x17, 77"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000100110110001101011110010011);
}

TEST(ParserTestSuite, SRAI)
{
    ParsingResult result = Parser::Parse({"srai x15, x17, 127"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b01000111111110001101011110010011);
}

TEST(ParserTestSuite, SLTI)
{
    ParsingResult result = Parser::Parse({"slti x15, x17, 77"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000100110110001010011110010011);
}

TEST(ParserTestSuite, SLTIU)
{
    ParsingResult result = Parser::Parse({"sltiu x15, x17, 77"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000100110110001011011110010011);
}

TEST(ParserTestSuite, JALR)
{
    ParsingResult result = Parser::Parse({"jalr x15, 77(x17)"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000100110110001000011111100111);
}

// I-Type load tests
TEST(ParserTestSuite, LB)
{
    ParsingResult result = Parser::Parse({"lb x15, 77(x17)"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000100110110001000011110000011);
}

TEST(ParserTestSuite, LH)
{
    ParsingResult result = Parser::Parse({"lh x15, -77(x17)"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b11111011001110001001011110000011);
}

TEST(ParserTestSuite, LW)
{
    ParsingResult result = Parser::Parse({"lw x15, 77(x17)"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000100110110001010011110000011);
}

TEST(ParserTestSuite, LBU)
{
    ParsingResult result = Parser::Parse({"lbu x15, 77(x17)"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000100110110001100011110000011);
}

TEST(ParserTestSuite, LHU)
{
    ParsingResult result = Parser::Parse({"lhu x15, 77(x17)"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000100110110001101011110000011);
}


// S-Type tests
TEST(ParserTestSuite, SB)
{
    ParsingResult result = Parser::Parse({"sb x15, 113(x17)"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000110111110001000100010100011);
}

TEST(ParserTestSuite, SH)
{
    ParsingResult result = Parser::Parse({"sh x15, -77(x17)"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b11111010111110001001100110100011);
}

TEST(ParserTestSuite, SW)
{
    ParsingResult result = Parser::Parse({"sw x15, -99(x17)"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b11111000111110001010111010100011);
}


// B-Type tests
TEST(ParserTestSuite, BEQ)
{
    ParsingResult result = Parser::Parse({"beq x13, x12, 123"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000110110001101000110101100011);
}

TEST(ParserTestSuite, BNE)
{
    ParsingResult result = Parser::Parse({"bne x13, x19, 3255"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b01001011001101101001101111100011);
}

TEST(ParserTestSuite, BLT)
{
    ParsingResult result = Parser::Parse({"blt x13, x19, 3255"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b01001011001101101100101111100011);
}

TEST(ParserTestSuite, BGE)
{
    ParsingResult result = Parser::Parse({"bge x13, x19, 3255"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b01001011001101101101101111100011);
}

TEST(ParserTestSuite, BLTU)
{
    ParsingResult result = Parser::Parse({"bltu x13, x19, -3255"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b10110101001101101110010001100011);
}

TEST(ParserTestSuite, BGEU)
{
    ParsingResult result = Parser::Parse({"bgeu x0, x1, -1"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b11111110000100000111111111100011);
}


// J-Type tests
TEST(ParserTestSuite, JAL)
{
    ParsingResult result = Parser::Parse({"jal x15, 123"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000111101000000000011111101111);
}


// U-Type tests
TEST(ParserTestSuite, LUI1)
{
    ParsingResult result = Parser::Parse({"lui x15, 524287"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b01111111111111111111011110110111);
}

// U-Type tests
TEST(ParserTestSuite, LUI2)
{
    ParsingResult result = Parser::Parse({"lui x15, -524288"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b10000000000000000000011110110111);
}

TEST(ParserTestSuite, AUIPC)
{
    ParsingResult result = Parser::Parse({"auipc x15, 123"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000000000001111011011110010111);
}

TEST(ParserTestSuite, OutOfBoundsAUIPC)
{
    ParsingResult result = Parser::Parse({"auipc x15, 524288"});
    EXPECT_EQ(result.success, false);
    EXPECT_EQ(result.errorType, ParsingError::IMMEDIATE_OUT_OF_BOUNDS);
}


// MExtension tests
TEST(ParserTestSuite, MUL)
{
    ParsingResult result = Parser::Parse({"mul x15, x17, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000011001010001000011110110011);
}

TEST(ParserTestSuite, MULH)
{
    ParsingResult result = Parser::Parse({"mulh x15, x17, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000011001010001001011110110011);
}

TEST(ParserTestSuite, MULHSU)
{
    ParsingResult result = Parser::Parse({"mulhsu x15, x17, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000011001010001010011110110011);
}

TEST(ParserTestSuite, MULHU)
{
    ParsingResult result = Parser::Parse({"mulhu x15, x17, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000011001010001011011110110011);
}

TEST(ParserTestSuite, DIV)
{
    ParsingResult result = Parser::Parse({"div x15, x17, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000011001010001100011110110011);
}

TEST(ParserTestSuite, DIVU)
{
    ParsingResult result = Parser::Parse({"divu x15, x17, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000011001010001101011110110011);
}

TEST(ParserTestSuite, REM)
{
    ParsingResult result = Parser::Parse({"rem x15, x17, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000011001010001110011110110011);
}

TEST(ParserTestSuite, REMU)
{
    ParsingResult result = Parser::Parse({"remu x15, x17, x18"});
    EXPECT_EQ(result.success, true);
    EXPECT_EQ(result.instructions.size(), 1);
    EXPECT_EQ(result.instructions[0], 0b00000011001010001111011110110011);
}

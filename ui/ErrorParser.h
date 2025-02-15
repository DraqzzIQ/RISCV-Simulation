#ifndef ERRORPARSER_H
#define ERRORPARSER_H
#include <map>
#include <string>

#include "../parser/OpCodes.h"
#include "../simulator/CPUUtil.h"


enum class ParsingError : uint8_t;
using std::string;

class ErrorParser
{
public:
    static string ParseError(ExecutionError error, uint32_t instruction);
    static string ParseError(ParsingError error, uint32_t lineNumber);
};

static const map<string, string> InstructionFormats = {
    {OpCodes::ADD, "add rd, rs1, rs2 # rd = rs1 + rs2"},
    {OpCodes::SUB, "sub rd, rs1, rs2 # rd = rs1 - rs2"},
    {OpCodes::SLL, "sll rd, rs1, rs2 # rd = rs1 << rs2"},
    {OpCodes::SLT, "slt rd, rs1, rs2 # rd = (rs1 < rs2) ? 1 : 0"},
    {OpCodes::SLTU, "sltu rd, rs1, rs2 # rd = (rs1 < rs2) ? 1 : 0"},
    {OpCodes::XOR, "xor rd, rs1, rs2 # rd = rs1 ^ rs2"},
    {OpCodes::SRL, "srl rd, rs1, rs2 # rd = rs1 >> rs2"},
    {OpCodes::SRA, "sra rd, rs1, rs2 # rd = rs1 >> rs2 (arithmetic)"},
    {OpCodes::OR, "or rd, rs1, rs2 # rd = rs1 | rs2"},
    {OpCodes::AND, "and rd, rs1, rs2 # rd = rs1 & rs2"},
    {OpCodes::ADDI, "addi rd, rs1, imm # rd = rs1 + imm"},
    {OpCodes::SLLI, "slli rd, rs1, imm # rd = rs1 << imm"},
    {OpCodes::SLTI, "slti rd, rs1, imm # rd = (rs1 < imm) ? 1 : 0"},
    {OpCodes::SLTIU, "sltiu rd, rs1, imm # rd = (rs1 < imm) ? 1 : 0"},
    {OpCodes::XORI, "xori rd, rs1, imm # rd = rs1 ^ imm"},
    {OpCodes::SRLI, "srli rd, rs1, imm # rd = rs1 >> imm"},
    {OpCodes::SRAI, "srai rd, rs1, imm # rd = rs1 >> imm (arithmetic)"},
    {OpCodes::ORI, "ori rd, rs1, imm # rd = rs1 | imm"},
    {OpCodes::ANDI, "andi rd, rs1, imm # rd = rs1 & imm"},
    {OpCodes::LB, "lb rd, offset(rs1) # rd = mem[rs1 + offset]"},
    {OpCodes::LH, "lh rd, offset(rs1) # rd = mem[rs1 + offset]"},
    {OpCodes::LW, "lw rd, offset(rs1) # rd = mem[rs1 + offset]"},
    {OpCodes::LBU, "lbu rd, offset(rs1) # rd = mem[rs1 + offset]"},
    {OpCodes::LHU, "lhu rd, offset(rs1) # rd = mem[rs1 + offset]"},
    {OpCodes::JALR, "jalr rd, offset(rs1) # rd = pc + 4; pc = rs1 + offset"},
    {OpCodes::SB, "sb rs2, offset(rs1) # mem[rs1 + offset] = rs2"},
    {OpCodes::SH, "sh rs2, offset(rs1) # mem[rs1 + offset] = rs2"},
    {OpCodes::SW, "sw rs2, offset(rs1) # mem[rs1 + offset] = rs2"},
    {OpCodes::BEQ, "beq rs1, rs2, offset # if (rs1 == rs2) pc += offset"},
    {OpCodes::BNE, "bne rs1, rs2, offset # if (rs1 != rs2) pc += offset"},
    {OpCodes::BLT, "blt rs1, rs2, offset # if (rs1 < rs2) pc += offset"},
    {OpCodes::BGE, "bge rs1, rs2, offset # if (rs1 >= rs2) pc += offset"},
    {OpCodes::BLTU, "bltu rs1, rs2, offset # if (rs1 < rs2) pc += offset"},
    {OpCodes::BGEU, "bgeu rs1, rs2, offset # if (rs1 >= rs2) pc += offset"},
    {OpCodes::JAL, "jal rd, offset # rd = pc + 4; pc += offset"},
    {OpCodes::LUI, "lui rd, imm # rd = imm << 12"},
    {OpCodes::AUIPC, "auipc rd, imm # rd = pc + (imm << 12)"},
    {OpCodes::MUL, "mul rd, rs1, rs2 # rd = rs1 * rs2"},
    {OpCodes::MULH, "mulh rd, rs1, rs2 # rd = (rs1 * rs2) >> 32"},
    {OpCodes::MULHSU, "mulhsu rd, rs1, rs2 # rd = (rs1 * (usigned)rs2) >> 32"},
    {OpCodes::MULHU, "mulhu rd, rs1, rs2 # rd = ((usigned)rs1 * (usigned)rs2) >> 32"},
    {OpCodes::DIV, "div rd, rs1, rs2 # rd = rs1 / rs2"},
    {OpCodes::DIVU, "divu rd, rs1, rs2 # rd = (usigned)rs1 / (usigned)rs2"},
    {OpCodes::REM, "rem rd, rs1, rs2 # rd = rs1 % rs2"},
    {OpCodes::REMU, "remu rd, rs1, rs2 # rd = (usigned)rs1 % (usigned)rs2"}};

#endif // ERRORPARSER_H

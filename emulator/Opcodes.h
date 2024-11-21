#ifndef OPCODES_H
#define OPCODES_H
#include <cstdint>

static constexpr uint8_t R_Type = 0b00110011;
static constexpr uint8_t I_Type = 0b00010011;
static constexpr uint8_t Load_Type = 0b00000011;
static constexpr uint8_t S_Type = 0b00100011;
static constexpr uint8_t B_Type = 0b01100011;
static constexpr uint8_t LUI_Type = 0b00110111;
static constexpr uint8_t AUIPC_Type = 0b00100111;
static constexpr uint8_t J_Type = 0b01101111;
static constexpr uint8_t JALR_Type = 0b01100111;

static constexpr uint8_t ADD = 0x0;
static constexpr uint8_t SUB = 0x0;
static constexpr uint8_t SLL = 0x1;
static constexpr uint8_t SLT = 0x2;
static constexpr uint8_t SLTU = 0x3;
static constexpr uint8_t XOR = 0x4;
static constexpr uint8_t SRL = 0x5;
static constexpr uint8_t SRA = 0x5;
static constexpr uint8_t OR = 0x6;
static constexpr uint8_t AND = 0x7;

static constexpr uint8_t MUL = 0x0;
static constexpr uint8_t MULH = 0x1;
static constexpr uint8_t MULHSU = 0x2;
static constexpr uint8_t MULHU = 0x3;
static constexpr uint8_t DIV = 0x4;
static constexpr uint8_t DIVU = 0x5;
static constexpr uint8_t REM = 0x6;
static constexpr uint8_t REMU = 0x7;

static constexpr uint8_t ADDI = 0x0;
static constexpr uint8_t SLLI = 0x1;
static constexpr uint8_t SLTI = 0x2;
static constexpr uint8_t SLTIU = 0x3;
static constexpr uint8_t XORI = 0x4;
static constexpr uint8_t SRLI_SRAI = 0x5;
static constexpr uint8_t ORI = 0x6;
static constexpr uint8_t ANDI = 0x7;

static constexpr uint8_t LB = 0x0;
static constexpr uint8_t LH = 0x1;
static constexpr uint8_t LW = 0x2;
static constexpr uint8_t LBU = 0x4;
static constexpr uint8_t LHU = 0x5;

static constexpr uint8_t SB = 0x0;
static constexpr uint8_t SH = 0x1;
static constexpr uint8_t SW = 0x2;

static constexpr uint8_t BEQ = 0x0;
static constexpr uint8_t BNE = 0x1;
static constexpr uint8_t BLT = 0x4;
static constexpr uint8_t BGE = 0x5;
static constexpr uint8_t BLTU = 0x6;
static constexpr uint8_t BGEU = 0x7;

static constexpr uint8_t JALR = 0x0;
#endif //OPCODES_H

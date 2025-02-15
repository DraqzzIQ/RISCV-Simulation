#ifndef OPCODES_H
#define OPCODES_H
#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

enum ParameterType
{
    REGISTER,
    IMMEDIATE,
};

struct ParameterData
{
    ParameterType type;
    int length;
};

class OpCodes
{
public:
    static constexpr string ADD = "add";
    static constexpr string SUB = "sub";
    static constexpr string SLL = "sll";
    static constexpr string SLT = "slt";
    static constexpr string SLTU = "sltu";
    static constexpr string XOR = "xor";
    static constexpr string SRL = "srl";
    static constexpr string SRA = "sra";
    static constexpr string OR = "or";
    static constexpr string AND = "and";
    static constexpr string ADDI = "addi";
    static constexpr string SLLI = "slli";
    static constexpr string SLTI = "slti";
    static constexpr string SLTIU = "sltiu";
    static constexpr string XORI = "xori";
    static constexpr string SRLI = "srli";
    static constexpr string SRAI = "srai";
    static constexpr string ORI = "ori";
    static constexpr string ANDI = "andi";
    static constexpr string LB = "lb";
    static constexpr string LH = "lh";
    static constexpr string LW = "lw";
    static constexpr string LBU = "lbu";
    static constexpr string LHU = "lhu";
    static constexpr string JALR = "jalr";
    static constexpr string SB = "sb";
    static constexpr string SH = "sh";
    static constexpr string SW = "sw";
    static constexpr string BEQ = "beq";
    static constexpr string BNE = "bne";
    static constexpr string BLT = "blt";
    static constexpr string BGE = "bge";
    static constexpr string BLTU = "bltu";
    static constexpr string BGEU = "bgeu";
    static constexpr string JAL = "jal";
    static constexpr string LUI = "lui";
    static constexpr string AUIPC = "auipc";
    static constexpr string MUL = "mul";
    static constexpr string MULH = "mulh";
    static constexpr string MULHSU = "mulhsu";
    static constexpr string MULHU = "mulhu";
    static constexpr string DIV = "div";
    static constexpr string DIVU = "divu";
    static constexpr string REM = "rem";
    static constexpr string REMU = "remu";
};

static const map<string, string> RTypeOpcodes = {
    {OpCodes::ADD, "000"}, {OpCodes::SUB, "000"}, {OpCodes::SLL, "001"}, {OpCodes::SLT, "010"}, {OpCodes::SLTU, "011"},
    {OpCodes::XOR, "100"}, {OpCodes::SRL, "101"}, {OpCodes::SRA, "101"}, {OpCodes::OR, "110"},  {OpCodes::AND, "111"}};
static const map<string, string> MTypeOpcodes = {
    {OpCodes::ADDI, "000"}, {OpCodes::SLLI, "001"}, {OpCodes::SLTI, "010"}, {OpCodes::SLTIU, "011"},
    {OpCodes::XORI, "100"}, {OpCodes::SRLI, "101"}, {OpCodes::SRAI, "101"}, {OpCodes::ORI, "110"},
    {OpCodes::ANDI, "111"}, {OpCodes::LB, "000"},   {OpCodes::LH, "001"},   {OpCodes::LW, "010"},
    {OpCodes::LBU, "100"},  {OpCodes::LHU, "101"},  {OpCodes::JALR, "000"}};
static const map<string, string> STypeOpcodes = {{OpCodes::SB, "000"}, {OpCodes::SH, "001"}, {OpCodes::SW, "010"}};
static const map<string, string> BTypeOpcodes = {{OpCodes::BEQ, "000"}, {OpCodes::BNE, "001"},  {OpCodes::BLT, "100"},
                                                 {OpCodes::BGE, "101"}, {OpCodes::BLTU, "110"}, {OpCodes::BGEU, "111"}};
static const map<string, string> JTypeOpcodes = {{OpCodes::JAL, "000"}};
static const map<string, string> UTypeOpcodes = {{OpCodes::LUI, "000"}, {OpCodes::AUIPC, "000"}};
static const map<string, string> Rv32mExtensionOpcodes = {
    {OpCodes::MUL, "000"}, {OpCodes::MULH, "001"}, {OpCodes::MULHSU, "010"}, {OpCodes::MULHU, "011"},
    {OpCodes::DIV, "100"}, {OpCodes::DIVU, "101"}, {OpCodes::REM, "110"},    {OpCodes::REMU, "111"}};

static const map<string, vector<ParameterData>> InstructionParameters = {
    {OpCodes::ADD, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::SUB, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::SLL, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::SLT, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::SLTU, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::XOR, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::SRL, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::SRA, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::OR, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::AND, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::ADDI, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}}},
    {OpCodes::SLLI, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 7}}},
    {OpCodes::SLTI, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}}},
    {OpCodes::SLTIU, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}}},
    {OpCodes::XORI, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}}},
    {OpCodes::SRLI, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 7}}},
    {OpCodes::SRAI, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 7}}},
    {OpCodes::ORI, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}}},
    {OpCodes::ANDI, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}}},
    {OpCodes::LB, {{ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}, {ParameterType::REGISTER, 5}}},
    {OpCodes::LH, {{ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}, {ParameterType::REGISTER, 5}}},
    {OpCodes::LW, {{ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}, {ParameterType::REGISTER, 5}}},
    {OpCodes::LBU, {{ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}, {ParameterType::REGISTER, 5}}},
    {OpCodes::LHU, {{ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}, {ParameterType::REGISTER, 5}}},
    {OpCodes::JALR, {{ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}, {ParameterType::REGISTER, 5}}},
    {OpCodes::SB, {{ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}, {ParameterType::REGISTER, 5}}},
    {OpCodes::SH, {{ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}, {ParameterType::REGISTER, 5}}},
    {OpCodes::SW, {{ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 12}, {ParameterType::REGISTER, 5}}},
    {OpCodes::BEQ, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 13}}},
    {OpCodes::BNE, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 13}}},
    {OpCodes::BLT, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 13}}},
    {OpCodes::BGE, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 13}}},
    {OpCodes::BLTU, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 13}}},
    {OpCodes::BGEU, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 13}}},
    {OpCodes::JAL, {{ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 21}}},
    {OpCodes::LUI, {{ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 20}}},
    {OpCodes::AUIPC, {{ParameterType::REGISTER, 5}, {ParameterType::IMMEDIATE, 20}}},
    {OpCodes::MUL, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::MULH, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::MULHSU, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::MULHU, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::DIV, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::DIVU, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::REM, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}},
    {OpCodes::REMU, {{ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}, {ParameterType::REGISTER, 5}}}};

#endif // OPCODES_H

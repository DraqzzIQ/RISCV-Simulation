#include "Parser.h"

#include <bitset>
#include <sstream>
#include <ranges>
#include <regex>
#include <bits/ranges_algo.h>

using std::stoi;
using std::bitset;

static const map<string, string> m_rTypeOpcodes = {
    {"add", "000"}, {"sub", "000"}, {"sll", "001"}, {"slt", "010"},
    {"sltu", "011"}, {"xor", "100"}, {"srl", "101"}, {"sra", "101"},
    {"or", "110"}, {"and", "111"}
};
static const map<string, string> m_iTypeOpcodes = {
    {"addi", "000"}, {"slli", "001"}, {"slti", "010"}, {"sltiu", "011"},
    {"xori", "100"}, {"srli", "101"}, {"srai", "101"}, {"ori", "110"},
    {"andi", "111"}, {"lb", "000"}, {"lh", "001"}, {"lw", "010"},
    {"lbu", "100"}, {"lhu", "101"}, {"jalr", "000"}
};
static const map<string, string> m_sTypeOpcodes = {{"sb", "000"}, {"sh", "001"}, {"sw", "010"}};
static const map<string, string> m_bTypeOpcodes = {
    {"beq", "000"}, {"bne", "001"}, {"blt", "100"}, {"bge", "101"},
    {"bltu", "110"}, {"bgeu", "111"}
};
static const map<string, string> m_jTypeOpcodes = {{"jal", "000"}};


ParsingResult Parser::Parse(const vector<string>& instructions)
{
    vector<uint32_t> parsedInstructions;
    for (string instruction : instructions)
    {
        while (instruction[0] == ' ')
        {
            instruction.erase(0, 1);
        }
        for (int i = 0; i < instruction.size(); i++)
        {
            if (instruction[i] == '(')
            {
                instruction.insert(i, ",");
                instruction.erase(i + 1, 1);
            }
            if (instruction[i] == ')')
            {
                instruction.erase(i, 1);
            }
        }
        const size_t index = instruction.find(' ');
        string opcode = ToLowerCase(instruction.substr(0, index));
        string operands = ToLowerCase(RemoveSpaces(instruction.substr(index + 1)));
        if (opcode.size() < 2 || opcode.size() > 5 || operands.empty())
        {
            return ParsingResult{false, parsedInstructions, instruction, ParsingError::INVALID_OPCODE};
        }

        auto [parsedInstruction, parsingError] = ParseInstruction(opcode, operands);
        if (ParsingError::NONE != parsingError)
        {
            return ParsingResult{false, parsedInstructions, instruction, parsingError};
        }

        parsedInstructions.push_back(parsedInstruction);
    }
    return ParsingResult{true, parsedInstructions, "", ParsingError::NONE};
}

std::pair<uint32_t, ParsingError> Parser::ParseInstruction(const string& opcode, const string& operands)
{
    if (m_rTypeOpcodes.contains(opcode))
    {
        return ParseRType(opcode, operands);
    }
    if (m_iTypeOpcodes.contains(opcode))
    {
        return ParseIType(opcode, operands);
    }
    if (m_sTypeOpcodes.contains(opcode))
    {
        return ParseSType(opcode, operands);
    }
    if (m_bTypeOpcodes.contains(opcode))
    {
        return ParseBType(opcode, operands);
    }
    if (m_jTypeOpcodes.contains(opcode))
    {
        return ParseJType(opcode, operands);
    }

    return {0, ParsingError::OPCODE_NOT_FOUND};
}

std::pair<uint32_t, ParsingError> Parser::ParseRType(const string& opcode, const string& operands)
{
    auto [args, error] = SplitArguments(operands);
    if (error != ParsingError::NONE)
    {
        return {0, error};
    }

    const string funct7 = (opcode == "sub" || opcode == "sra") ? "0100000" : "0000000";
    const string parsedInstruction = funct7 + args[2] + args[1] + m_rTypeOpcodes.at(opcode) + args[0] + "0110011";
    return {stoul(parsedInstruction, nullptr, 2), ParsingError::NONE};
}

std::pair<uint32_t, ParsingError> Parser::ParseIType(const string& opcode, const string& operands)
{
    const vector<string> twelveBitArg2Opcodes = {
        "lb", "lh", "lw", "lbu", "lhu", "jalr"
    };
    
    const int arg3Length =  std::ranges::find(twelveBitArg2Opcodes, opcode) != twelveBitArg2Opcodes.end() ? 5 : opcode == "srai" ? 7 : 12;
    const int arg2Length = std::ranges::find(twelveBitArg2Opcodes, opcode) != twelveBitArg2Opcodes.end() ? 12 : 5;
    const bool isSigned = !(opcode == "srai" || opcode == "slli" || opcode == "srli");
    auto [args, error] = SplitArguments(operands, 3, arg2Length, arg3Length, isSigned);
    if (error != ParsingError::NONE)
    {
        return {0, error};
    }

    string parsedInstruction;
    if (!isSigned) // shift instructions
    {
        const string funct7 = (opcode == "srai") ? "01000" : "00000";
        parsedInstruction = funct7 + args[2] + args[1] + m_iTypeOpcodes.at(opcode) + args[0] + "0010011";
    }
    else if (opcode == "jalr")
    {
        parsedInstruction = args[1] + args[2] + m_iTypeOpcodes.at(opcode) + args[0] + "1100111";
    }
    else if(opcode[0] == 'l') // load instructions
    {
        parsedInstruction = args[1] + args[2] + m_iTypeOpcodes.at(opcode) + args[0] + "0000011";
    }
    else
    {
        parsedInstruction = args[2] + args[1] + m_iTypeOpcodes.at(opcode) + args[0] + "0010011";
    }
    return {std::stoul(parsedInstruction, nullptr, 2), ParsingError::NONE};
}

std::pair<uint32_t, ParsingError> Parser::ParseSType(const string& opcode, const string& operands)
{
    auto [args, error] = SplitArguments(operands, 3, 12);
    if (error != ParsingError::NONE)
    {
        return {0, error};
    }

    const string upperImm = args[1].substr(0, 7);
    const string lowerImm = args[1].substr(7);

    const string parsedInstruction = upperImm + args[0] + args[2] + m_sTypeOpcodes.at(opcode) + lowerImm + "0100011";
    return {stoul(parsedInstruction, nullptr, 2), ParsingError::NONE};
}

std::pair<uint32_t, ParsingError> Parser::ParseBType(const string& opcode, const string& operands)
{
    auto [args, error] = SplitArguments(operands, 3, 5, 13);
    if (error != ParsingError::NONE)
    {
        return {0, error};
    }

    const string parsedInstruction = args[2][0] + args[2].substr(2, 6) + args[1] + args[0] + m_bTypeOpcodes.at(opcode) +
        args[2].substr(8, 4) + args[2][1] + "1100011";

    return {stoul(parsedInstruction, nullptr, 2), ParsingError::NONE};
}

std::pair<uint32_t, ParsingError> Parser::ParseJType(const string& opcode, const string& operands)
{
    auto [args, error] = SplitArguments(operands, 2, 21);
    if (error != ParsingError::NONE)
    {
        return {0, error};
    }

    const string parsedInstruction = args[1][0] + args[1].substr(10, 10) + args[1][9] +
        args[1].substr(1, 8)+ args[0] + "1101111";
    return {stoul(parsedInstruction, nullptr, 2), ParsingError::NONE};
}

std::pair<vector<string>, ParsingError> Parser::SplitArguments(const string& operands, const int count,
                                                               const int arg2Length, const int arg3Length, const bool isSigned)
{
    const size_t firstComma = operands.find(',');
    const size_t secondComma = (count == 3) ? operands.find(',', firstComma + 1) : 0;

    if (firstComma == string::npos || secondComma == string::npos)
    {
        return {{}, ParsingError::INVALID_OPERANDS};
    }

    const string arg1Str = operands.substr(0, firstComma);
    const string arg2Str = operands.substr(firstComma + 1, secondComma - firstComma - 1);
    const string arg3Str = (count == 3) ? operands.substr(secondComma + 1) : "";

    auto [arg1, rdError] = ArgumentToBinary(arg1Str, 5, isSigned);
    if (rdError != ParsingError::NONE) return {{}, rdError};
    auto [arg2, rs1Error] = ArgumentToBinary(arg2Str, arg2Length, isSigned);
    if (rs1Error != ParsingError::NONE) return {{}, rs1Error};
    auto [arg3, rs2Error] = (count == 3)
                                ? ArgumentToBinary(arg3Str, arg3Length, isSigned)
                                : std::pair<string, ParsingError>{"", ParsingError::NONE};
    if (rs2Error != ParsingError::NONE) return {{}, rs2Error};

    return {{arg1, arg2, arg3}, ParsingError::NONE};
}

vector<string> SplitOperands(const string& operands)
{
    std::stringstream ss(operands);
    string operand;
    vector<string> result;

    while (getline(ss, operand, ','))
    {
        // Trim whitespace around the operand.
        operand.erase(0, operand.find_first_not_of(' '));
        operand.erase(operand.find_last_not_of(' ') + 1);
        result.push_back(operand);
    }

    return result;
}


std::pair<string, ParsingError> Parser::ArgumentToBinary(const string& argument, const uint8_t length, const bool isSigned)
{
    // Check if the argument is a register (e.g., x5).
    if (argument[0] == 'x')
    {
        auto [regNumber, error] = RegisterToNumber(argument);
        if (error != ParsingError::NONE)
        {
            return {"", error};
        }
        if (regNumber < 0 || regNumber >= (1 << length))
        {
            return {"", ParsingError::REGISTER_OUT_OF_BOUNDS};
        }
        return {std::bitset<32>(regNumber).to_string().substr(32 - length), ParsingError::NONE};
    }

    // Parse immediate value in various formats.
    auto [immediate, error] = ParseImmediate(argument);
    if (error != ParsingError::NONE)
    {
        return {"", error};
    }
    if (immediate < 0 && !isSigned)
    {
        return {"", ParsingError::IMMEDIATE_OUT_OF_BOUNDS};
    }
    if (isSigned && (immediate < -(1 << (length - 1)) || immediate >= (1 << (length - 1))))
    {
        return {"", ParsingError::IMMEDIATE_OUT_OF_BOUNDS};
    }
    
    return {std::bitset<32>(immediate).to_string().substr(32 - length), ParsingError::NONE};
}

std::pair<int, ParsingError> Parser::RegisterToNumber(const string& reg)
{
    // Extract the register number (e.g., x5 -> 5).
    if (reg[0] != 'x')
    {
        return {0, ParsingError::INVALID_REGISTER_FORMAT};
    }

    try
    {
        int regNumber = stoi(reg.substr(1));
        if (regNumber < 0 || regNumber > 31)
        {
            return {0, ParsingError::REGISTER_OUT_OF_BOUNDS};
        }
        return {regNumber, ParsingError::NONE};
    }
    catch (std::invalid_argument&)
    {
        return {0, ParsingError::INVALID_REGISTER_FORMAT};
    }
    catch (std::out_of_range&)
    {
        return {0, ParsingError::REGISTER_OUT_OF_BOUNDS};
    }
}

std::pair<int, ParsingError> Parser::ParseOffset(const string& offset)
{
    const std::regex offsetRegex("^(-?[0-9]+)\\("); // e.g. 12(x5)
    std::smatch match;

    if (!regex_match(offset, match, offsetRegex))
    {
        return {0, ParsingError::INVALID_OFFSET_FORMAT};
    }

    try
    {
        return {stoi(match[1].str()), ParsingError::NONE};
    }
    catch (std::invalid_argument&)
    {
        return {0, ParsingError::INVALID_OFFSET_FORMAT};
    }
    catch (std::out_of_range&)
    {
        return {0, ParsingError::OFFSET_OUT_OF_BOUNDS};
    }
}

std::pair<int, ParsingError> Parser::ParseImmediate(const string& immediate)
{
    const std::regex hexRegex("^0x[0-9a-fA-F]+$"); // 0x[]
    const std::regex binRegex("^0b[01]+$"); // 0b[]
    const std::regex decRegex("^-?[0-9]+$"); // []
    const bool isHex = std::regex_match(immediate, hexRegex);
    const bool isBin = std::regex_match(immediate, binRegex);
    const bool isDec = std::regex_match(immediate, decRegex);

    const int base = isHex ? 16 : isBin ? 2 : isDec ? 10 : 0;
    if (base == 0)
    {
        return {0, ParsingError::INVALID_IMMEDIATE_FORMAT};
    }

    const string parsedImmediate = isDec ? immediate : immediate.substr(2); // Remove prefix (0x or 0b)
    try
    {
        return {stoi(parsedImmediate, nullptr, base), ParsingError::NONE};
    }
    catch (std::invalid_argument&)
    {
        return {0, ParsingError::INVALID_IMMEDIATE_FORMAT};
    }
    catch (std::out_of_range&)
    {
        return {0, ParsingError::IMMEDIATE_OUT_OF_BOUNDS};
    }
}

string Parser::ToLowerCase(const string& input)
{
    string result = input;
    std::ranges::transform(result, result.begin(), [](const unsigned char c) { return std::tolower(c); });
    return result;
}

string Parser::RemoveSpaces(const string& input)
{
    string result = input;
    std::erase_if(result, ::isspace);
    return result;
}

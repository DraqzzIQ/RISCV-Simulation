#include "Parser.h"
#include "OpCodes.h"

#include <algorithm>
#include <bitset>
#include <sstream>
#include <string>

using std::stoi;

const std::regex Parser::m_labelRegex = std::regex(R"(^\s*[a-z_][a-z0-9_]*:\s*$)");


ParsingResult Parser::Parse(const vector<string>& instructions)
{
    vector<uint32_t> parsedInstructions;
    vector<uint32_t> instructionMap;
    const auto [preprocessedInstructions, error] = Preprocess(instructions);
    if (error.errorType != ParsingError::NONE) {
        return error;
    }
    for (int i = 0; i < preprocessedInstructions.size(); i++) {
        string instruction = preprocessedInstructions[i];
        if (std::regex_match(instruction, m_labelRegex)) {
            continue;
        }
        if (RemoveSpaces(instruction).empty()) {
            continue;
        }

        while (instruction[0] == ' ') {
            instruction.erase(0, 1);
        }
        for (int j = 0; j < instruction.size(); j++) {
            if (instruction[j] == '(') {
                instruction.insert(j, ",");
                instruction.erase(j + 1, 1);
            }
            if (instruction[j] == ')') {
                instruction.erase(j, 1);
            }
        }
        const size_t index = instruction.find(' ');
        string opcode = instruction.substr(0, index);
        string operands = RemoveSpaces(instruction.substr(index + 1));
        if (!InstructionParameters.contains(opcode)) {
            return ParsingResult{false, parsedInstructions,          opcode, instructionMap,
                                 i + 1, ParsingError::INVALID_OPCODE};
        }
        if (operands.empty()) {
            return ParsingResult{
                false, parsedInstructions, "", instructionMap, i + 1, ParsingError::INVALID_OPERAND_COUNT};
        }

        auto [parsedInstruction, parsingError] = ParseInstruction(opcode, operands);
        if (ParsingError::NONE != parsingError) {
            return ParsingResult{false, parsedInstructions, opcode, instructionMap, i + 1, parsingError};
        }

        parsedInstructions.push_back(parsedInstruction);
        instructionMap.push_back(i);
    }
    if (parsedInstructions.empty()) {
        return ParsingResult{false, parsedInstructions, "", instructionMap, 0, ParsingError::EMPTY_INPUT};
    }

    return ParsingResult{true, parsedInstructions, "", instructionMap, -1, ParsingError::NONE};
}

std::pair<vector<string>, ParsingResult> Parser::Preprocess(const vector<string>& instructions)
{
    map<string, int32_t> labelMap;
    vector<string> preprocessedInstructions;
    int nonCodeLines = 0;
    for (int i = 0; i < instructions.size(); i++) {
        string instruction = ToLowerCase(instructions[i]);
        if (RemoveSpaces(instruction).empty() || RemoveSpaces(instruction)[0] == ';') {
            nonCodeLines++;
        }
        const int commentIndex = instruction.find(';');
        if (commentIndex != string::npos) {
            instruction = instruction.substr(0, commentIndex);
        }

        if (std::regex_match(instruction, m_labelRegex)) {
            const int labelStart = instruction.find_first_not_of(' ');
            const int labelEnd = instruction.find(':');
            const string label = instruction.substr(labelStart, labelEnd - labelStart);
            if (labelMap.contains(label)) {
                return {{}, {false, {}, label, {}, i + 1, ParsingError::DUPLICATE_LABEL_DEFINITION}};
            }
            labelMap[label] = (preprocessedInstructions.size() - nonCodeLines) * 4;
            nonCodeLines++;
        }
        preprocessedInstructions.push_back(instruction);
    }
    nonCodeLines = 0;
    for (int i = 0; i < preprocessedInstructions.size(); i++) {
        string& instruction = preprocessedInstructions[i];
        if (RemoveSpaces(instruction).empty() || RemoveSpaces(instruction)[0] == ';') {
            nonCodeLines++;
            continue;
        }
        if (std::regex_match(instruction, m_labelRegex)) {
            nonCodeLines++;
            continue;
        }
        for (const auto& [label, address] : labelMap) {
            const string labelRegex = R"(\b)" + label + R"(\b(?!:))";
            instruction = std::regex_replace(instruction, std::regex(labelRegex),
                                             std::to_string(address - (i - nonCodeLines) * 4));
        }
    }
    return {preprocessedInstructions, {true, {}, "", {}, -1, ParsingError::NONE}};
}

std::pair<uint32_t, ParsingError> Parser::ParseInstruction(const string& opcode, const string& operands)
{
    if (!InstructionParameters.contains(opcode)) {
        return {0, ParsingError::OPCODE_NOT_FOUND};
    }

    if (RTypeOpcodes.contains(opcode)) {
        return ParseRType(opcode, operands);
    }
    if (MTypeOpcodes.contains(opcode)) {
        return ParseIType(opcode, operands);
    }
    if (STypeOpcodes.contains(opcode)) {
        return ParseSType(opcode, operands);
    }
    if (BTypeOpcodes.contains(opcode)) {
        return ParseBType(opcode, operands);
    }
    if (JTypeOpcodes.contains(opcode)) {
        return ParseJType(opcode, operands);
    }
    if (UTypeOpcodes.contains(opcode)) {
        return ParseUType(opcode, operands);
    }
    if (Rv32mExtensionOpcodes.contains(opcode)) {
        return ParseMExtension(opcode, operands);
    }

    return {0, ParsingError::OPCODE_NOT_FOUND};
}

std::pair<uint32_t, ParsingError> Parser::ParseRType(const string& opcode, const string& operands)
{
    auto [args, error] = ParseArguments(opcode, operands);
    if (error != ParsingError::NONE) {
        return {0, error};
    }

    const string funct7 = (opcode == OpCodes::SUB || opcode == OpCodes::SRA) ? "0100000" : "0000000";
    const string parsedInstruction = funct7 + args[2] + args[1] + RTypeOpcodes.at(opcode) + args[0] + "0110011";
    return {stoul(parsedInstruction, nullptr, 2), ParsingError::NONE};
}

std::pair<uint32_t, ParsingError> Parser::ParseIType(const string& opcode, const string& operands)
{
    const bool isSigned = !(opcode == OpCodes::SRAI || opcode == OpCodes::SLLI || opcode == OpCodes::SRLI);
    auto [args, error] = ParseArguments(opcode, operands);
    if (error != ParsingError::NONE) {
        return {0, error};
    }

    string parsedInstruction;
    if (!isSigned) // shift instructions
    {
        const string funct7 = (opcode == OpCodes::SRAI) ? "01000" : "00000";
        parsedInstruction = funct7 + args[2] + args[1] + MTypeOpcodes.at(opcode) + args[0] + "0010011";
    }
    else if (opcode == OpCodes::JALR) {
        parsedInstruction = args[1] + args[2] + MTypeOpcodes.at(opcode) + args[0] + "1100111";
    }
    else if (opcode[0] == 'l') // load instructions
    {
        parsedInstruction = args[1] + args[2] + MTypeOpcodes.at(opcode) + args[0] + "0000011";
    }
    else {
        parsedInstruction = args[2] + args[1] + MTypeOpcodes.at(opcode) + args[0] + "0010011";
    }
    return {std::stoul(parsedInstruction, nullptr, 2), ParsingError::NONE};
}

std::pair<uint32_t, ParsingError> Parser::ParseSType(const string& opcode, const string& operands)
{
    auto [args, error] = ParseArguments(opcode, operands);
    if (error != ParsingError::NONE) {
        return {0, error};
    }

    const string upperImm = args[1].substr(0, 7);
    const string lowerImm = args[1].substr(7);

    const string parsedInstruction = upperImm + args[0] + args[2] + STypeOpcodes.at(opcode) + lowerImm + "0100011";
    return {stoul(parsedInstruction, nullptr, 2), ParsingError::NONE};
}

std::pair<uint32_t, ParsingError> Parser::ParseBType(const string& opcode, const string& operands)
{
    auto [args, error] = ParseArguments(opcode, operands);
    if (error != ParsingError::NONE) {
        return {0, error};
    }
    args[2] = args[2].substr(0, 12);

    const string parsedInstruction = args[2][0] + args[2].substr(2, 6) + args[1] + args[0] + BTypeOpcodes.at(opcode) +
        args[2].substr(8, 4) + args[2][1] + "1100011";

    return {stoul(parsedInstruction, nullptr, 2), ParsingError::NONE};
}

std::pair<uint32_t, ParsingError> Parser::ParseJType(const string& opcode, const string& operands)
{
    auto [args, error] = ParseArguments(opcode, operands);
    if (error != ParsingError::NONE) {
        return {0, error};
    }
    args[1] = args[1].substr(0, 20);

    const string parsedInstruction =
        args[1][0] + args[1].substr(10, 10) + args[1][9] + args[1].substr(1, 8) + args[0] + "1101111";
    return {stoul(parsedInstruction, nullptr, 2), ParsingError::NONE};
}

std::pair<uint32_t, ParsingError> Parser::ParseUType(const string& opcode, const string& operands)
{
    auto [args, error] = ParseArguments(opcode, operands);
    if (error != ParsingError::NONE) {
        return {0, error};
    }

    const string parsedInstruction = args[1] + args[0] + (opcode == OpCodes::LUI ? "0110111" : "0010111");
    return {stoul(parsedInstruction, nullptr, 2), ParsingError::NONE};
}

std::pair<uint32_t, ParsingError> Parser::ParseMExtension(const string& opcode, const string& operands)
{
    auto [args, error] = ParseArguments(opcode, operands);
    if (error != ParsingError::NONE) {
        return {0, error};
    }

    const string funct7 = "0000001";
    const string parsedInstruction =
        funct7 + args[2] + args[1] + Rv32mExtensionOpcodes.at(opcode) + args[0] + "0110011";
    return {stoul(parsedInstruction, nullptr, 2), ParsingError::NONE};
}

vector<string> SplitOperands(const string& operands)
{
    vector<string> args;
    std::istringstream ss(operands);
    string arg;
    while (std::getline(ss, arg, ',')) {
        args.push_back(arg);
    }
    return args;
}

std::pair<vector<string>, ParsingError> Parser::ParseArguments(const string& opcode, const string& operands)
{
    const auto parameterTypes = InstructionParameters.at(opcode);
    const auto parameters = SplitOperands(operands);
    if (parameters.size() != parameterTypes.size()) {
        return {{}, ParsingError::INVALID_OPERAND_COUNT};
    }

    vector<string> args(parameters.size());
    for (int i = 0; i < parameters.size(); i++) {
        const auto parameterType = parameterTypes[i];
        const auto parameter = parameters[i];
        auto [result, error] = parameterType.type == REGISTER ? RegisterToNumber(parameter) : ParseImmediate(parameter);

        if (error != ParsingError::NONE) {
            return {{}, error};
        }
        args[i] = std::bitset<32>(result).to_string().substr(32 - parameterType.length);
    }
    return {args, ParsingError::NONE};
}

std::pair<int, ParsingError> Parser::RegisterToNumber(const string& reg)
{
    // Extract the register number (e.g., x5 -> 5).
    if (reg[0] != 'x') {
        return {0, ParsingError::INVALID_REGISTER_FORMAT};
    }

    try {
        int regNumber = stoi(reg.substr(1));
        if (regNumber < 0 || regNumber > 31) {
            return {0, ParsingError::REGISTER_OUT_OF_BOUNDS};
        }
        return {regNumber, ParsingError::NONE};
    }
    catch (std::invalid_argument&) {
        return {0, ParsingError::INVALID_REGISTER_FORMAT};
    }
    catch (std::out_of_range&) {
        return {0, ParsingError::REGISTER_OUT_OF_BOUNDS};
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
    if (base == 0) {
        return {0, ParsingError::INVALID_IMMEDIATE_FORMAT};
    }

    const string parsedImmediate = isDec ? immediate : immediate.substr(2); // Remove prefix (0x or 0b)
    try {
        return {stoi(parsedImmediate, nullptr, base), ParsingError::NONE};
    }
    catch (std::invalid_argument&) {
        return {0, ParsingError::INVALID_IMMEDIATE_FORMAT};
    }
    catch (std::out_of_range&) {
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
    std::erase_if(result, isspace);
    return result;
}

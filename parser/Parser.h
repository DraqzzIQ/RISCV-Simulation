#ifndef PARSER_LIBRARY_H
#define PARSER_LIBRARY_H
#include <map>
#include <string>

#include "ParsingResult.h"

using std::string;
using std::vector;
using std::map;

class Parser
{
public:
    // Parse instructions from a vector of strings to a vector of 32-bit unsigned integers
    // Each string is a single instruction
    static ParsingResult Parse(const vector<string>& instructions);
private:
    static std::pair<uint32_t, ParsingError> ParseInstruction(const string& opcode, const string& operands);
    static std::pair<uint32_t, ParsingError> ParseRType(const string& opcode, const string& operands);
    static std::pair<uint32_t, ParsingError> ParseIType(const string& opcode, const string& operands);
    static std::pair<uint32_t, ParsingError> ParseSType(const string& opcode, const string& operands);
    static std::pair<uint32_t, ParsingError> ParseBType(const string& opcode, const string& operands);
    static std::pair<uint32_t, ParsingError> ParseJType(const string& opcode, const string& operands);
    static std::pair<uint32_t, ParsingError> ParseMExtension(const string& opcode, const string& operands);
    static std::pair<vector<string>, ParsingError> SplitArguments(const string& operands, int count = 3,
        int arg2Length = 5, int arg3Length = 5, bool isSigned = true);
    static string ToLowerCase(const string& input);
    static string RemoveSpaces(const string& input);
    static std::pair<string, ParsingError> ArgumentToBinary(const string& argument, uint8_t length, bool isSigned);
    static std::pair<int, ParsingError> RegisterToNumber(const string& reg);
    static std::pair<int, ParsingError> ParseOffset(const string& offset);
    static std::pair<int, ParsingError> ParseImmediate(const string& immediate);
};

#endif //PARSER_LIBRARY_H

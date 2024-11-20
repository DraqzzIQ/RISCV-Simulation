#ifndef PARSINGERROR_H
#define PARSINGERROR_H
#include <cstdint>
#include <string>
#include <vector>

using std::string;
using std::vector;

enum class ParsingError : uint8_t
{
    NONE = 0,
    INVALID_OPCODE = 1,
    INVALID_OPERANDS = 2,
    INVALID_OPERAND_COUNT = 3,
    INVALID_REGISTER_FORMAT = 4,
    REGISTER_OUT_OF_BOUNDS = 5,
    OFFSET_OUT_OF_BOUNDS = 6,
    IMMEDIATE_OUT_OF_BOUNDS = 7,
    INVALID_OFFSET_FORMAT = 8,
    INVALID_IMMEDIATE_FORMAT = 9,
    OPCODE_NOT_FOUND = 10
};

struct ParsingResult
{
    bool success;
    vector<uint32_t> instructions;
    string errorInstruction;
    ParsingError errorType;
};

#endif //PARSINGERROR_H

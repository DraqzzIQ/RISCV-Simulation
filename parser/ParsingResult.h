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

constexpr std::string_view toString(ParsingError error)
{
    switch (error) {
    case ParsingError::NONE:
        return "No error";
    case ParsingError::INVALID_OPCODE:
        return "Invalid opcode";
    case ParsingError::INVALID_OPERANDS:
        return "Invalid operands";
    case ParsingError::INVALID_OPERAND_COUNT:
        return "Invalid operand count";
    case ParsingError::INVALID_REGISTER_FORMAT:
        return "Invalid register format";
    case ParsingError::REGISTER_OUT_OF_BOUNDS:
        return "Register out of bounds";
    case ParsingError::OFFSET_OUT_OF_BOUNDS:
        return "Offset out of bounds";
    case ParsingError::IMMEDIATE_OUT_OF_BOUNDS:
        return "Immediate out of bounds";
    case ParsingError::INVALID_OFFSET_FORMAT:
        return "Invalid offset format";
    case ParsingError::INVALID_IMMEDIATE_FORMAT:
        return "Invalid immediate format";
    case ParsingError::OPCODE_NOT_FOUND:
        return "Opcode not found";
    default:
        return "Unknown error";
    }
}

inline std::ostream& operator<<(std::ostream& os, ParsingError error) { return os << toString(error); }

struct ParsingResult
{
    bool success;
    vector<uint32_t> instructions;
    string errorInstruction;
    int errorLine;
    ParsingError errorType;
};

#endif // PARSINGERROR_H

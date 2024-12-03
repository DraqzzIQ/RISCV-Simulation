#include "ErrorParser.h"

#include "../parser/ParsingResult.h"

string ErrorParser::ParseError(const ExecutionError error, const uint32_t instruction)
{
    const string line = "Error at line " + std::to_string(instruction) + ": ";

    switch (error) {
    case ExecutionError::NONE:
        return line + "No error.";
    case ExecutionError::UNSUPPORTED_OPCODE:
        return line + "Unsupported opcode.";
    case ExecutionError::INVALID_REGISTER:
        return line + "Invalid register. The register number must be between 0 and 31";
    case ExecutionError::INVALID_MEMORY_ACCESS:
        return line + "Invalid memory access. The memory address must be a multiple of 4 and within the memory bounds";
    case ExecutionError::DIVISION_BY_ZERO:
        return line + "Division by zero.";
    case ExecutionError::PC_OUT_OF_BOUNDS:
        return line + "Program counter out of bounds.";
    case ExecutionError::OFFSET_NOT_32_BIT_ALIGNED:
        return line + "Offset not 32-bit aligned. The offset must be a multiple of 4";
    default:
        return line + "Unknown error";
    }
}
string ErrorParser::ParseError(const ParsingError error, const uint32_t lineNumber)
{
    const string line = "Error at line " + std::to_string(lineNumber) + ": ";

    switch (error) {
    case ParsingError::NONE:
        return line + "No error.";
    case ParsingError::INVALID_OPCODE:
        return line + "Invalid opcode.";
    case ParsingError::INVALID_OPERANDS:
        return line + "Invalid operands.";
    case ParsingError::INVALID_OPERAND_COUNT:
        return line + "Invalid operand count.";
    case ParsingError::INVALID_REGISTER_FORMAT:
        return line + "Invalid register format.";
    case ParsingError::REGISTER_OUT_OF_BOUNDS:
        return line + "Register out of bounds.";
    case ParsingError::OFFSET_OUT_OF_BOUNDS:
        return line + "Offset out of bounds.";
    case ParsingError::IMMEDIATE_OUT_OF_BOUNDS:
        return line + "Immediate out of bounds.";
    case ParsingError::INVALID_OFFSET_FORMAT:
        return line + "Invalid offset format.";
    case ParsingError::INVALID_IMMEDIATE_FORMAT:
        return line + "Invalid immediate format.";
    case ParsingError::OPCODE_NOT_FOUND:
        return line + "Opcode not found.";
    case ParsingError::EMPTY_INPUT:
        return line + "Empty input.";
    default:
        return "Unknown error";
    }
}

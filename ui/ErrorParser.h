#ifndef ERRORPARSER_H
#define ERRORPARSER_H
#include <string>

#include "../simulator/CPUUtil.h"


enum class ParsingError : uint8_t;
using std::string;

class ErrorParser
{
public:
    static string ParseError(ExecutionError error, uint32_t instruction);
    static string ParseError(ParsingError error, uint32_t lineNumber);
};


#endif // ERRORPARSER_H

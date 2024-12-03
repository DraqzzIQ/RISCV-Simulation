#ifndef CPUUTIL_H
#define CPUUTIL_H
#include <cstdint>
#include <vector>

using std::vector;

struct CpuStatus
{
    vector<uint32_t> registers;
    uint32_t pc;
};

struct MemoryChange
{
    uint32_t address;
    uint32_t value;
};

struct RegisterChange
{
    uint8_t reg;
    uint32_t value;
};

enum class ExecutionError
{
    NONE = 0,
    UNSUPPORTED_OPCODE = 1,
    INVALID_REGISTER = 2,
    INVALID_MEMORY_ACCESS = 3,
    DIVISION_BY_ZERO = 4,
    PC_OUT_OF_BOUNDS = 5,
    OFFSET_NOT_32_BIT_ALIGNED = 6
};

struct ExecutionResult
{
    bool success;
    ExecutionError error;
    bool memoryChanged;
    MemoryChange memoryChange;
    bool registerChanged;
    RegisterChange registerChange;
    uint32_t pc;
    uint32_t errorInstruction;
};


class CPUUtil
{
public:
    static uint8_t GetOpcode(uint32_t instruction);
    static uint8_t GetFunct7(uint32_t instruction);
    static uint8_t GetFunct3(uint32_t instruction);
    static uint8_t GetRD(uint32_t instruction);
    static uint8_t GetRS1(uint32_t instruction);
    static uint8_t GetRS2(uint32_t instruction);
    static uint32_t GetImm20(uint32_t instruction);
    static uint16_t GetImm12(uint32_t instruction);
    static uint8_t GetImm5(uint32_t instruction);
    static bool IsValidRegister(uint8_t reg);
    static ExecutionResult ExecutionErrorResult(ExecutionError error);
};

#endif // CPUUTIL_H

#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <vector>

#include "Registers.h"
#include "Memory.h"
#include "../tests/lib/googletest/googletest/include/gtest/gtest_prod.h"

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
    INVALID_OPCODE,
    INVALID_REGISTER,
    INVALID_MEMORY_ACCESS,
    DIVISION_BY_ZERO
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
};

class CPU
{
public:
    explicit CPU(Memory* memory);
    ~CPU();
    void LoadInstructions(const std::vector<uint32_t>& instructions);
    ExecutionResult Step() const;
    void Reset() const;
    CpuStatus GetStatus() const;

private:
    FRIEND_TEST(CPUTestSuite, GetFunct7);
    FRIEND_TEST(CPUTestSuite, GetFunct3);
    FRIEND_TEST(CPUTestSuite, GetRD);
    FRIEND_TEST(CPUTestSuite, GetRS1);
    FRIEND_TEST(CPUTestSuite, GetRS2);
    FRIEND_TEST(CPUTestSuite, GetImm12);
    FRIEND_TEST(CPUTestSuite, GetImm5);
    ExecutionResult ExecuteInstruction(uint32_t instruction) const;
    ExecutionResult ExecuteRType(uint32_t instruction) const;
    ExecutionResult ExecuteIType(uint32_t instruction) const;
    ExecutionResult ExecuteLoadType(uint32_t instruction) const;
    ExecutionResult ExecuteSType(uint32_t instruction) const;
    ExecutionResult ExecuteBType(uint32_t instruction) const;
    ExecutionResult ExecuteLUIType(uint32_t instruction) const;
    ExecutionResult ExecuteAUIPCType(uint32_t instruction) const;
    ExecutionResult ExecuteJType(uint32_t instruction) const;
    ExecutionResult ExecuteJALRType(uint32_t instruction) const;
    ExecutionResult ExecuteMExtension(uint8_t funct3, uint8_t rd, uint8_t rs1, uint8_t rs2) const;
    uint8_t GetFunct7(uint32_t instruction) const;
    uint8_t GetFunct3(uint32_t instruction) const;
    uint8_t GetRD(uint32_t instruction) const;
    uint8_t GetRS1(uint32_t instruction) const;
    uint8_t GetRS2(uint32_t instruction) const;
    uint16_t GetImm12(uint32_t instruction) const;
    uint8_t GetImm5(uint32_t instruction) const;
    bool IsValidRegister(uint8_t reg) const;
    ExecutionResult ExecutionErrorResult(ExecutionError error) const;
    uint32_t GetPC() const;
    uint8_t GetOpcode(uint32_t instruction) const;
    vector<uint32_t> m_instructions;
    Registers* m_registers;
    Memory* m_memory;
};


#endif //CPU_H
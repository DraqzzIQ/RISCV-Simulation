#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <vector>

#include "../tests/lib/googletest/googletest/include/gtest/gtest_prod.h"
#include "CPUUtil.h"
#include "Memory.h"
#include "Registers.h"

using std::vector;

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
    ExecutionResult ExecuteInstruction(uint32_t instruction) const;
    ExecutionResult ExecuteRType(uint32_t instruction) const;
    ExecutionResult ExecuteIType(uint32_t instruction) const;
    ExecutionResult ExecuteLoadType(uint32_t instruction) const;
    ExecutionResult ExecuteSType(uint32_t instruction) const;
    ExecutionResult ExecuteBType(uint32_t instruction) const;
    ExecutionResult ExecuteLUIType(uint32_t instruction) const;
    ExecutionResult ExecuteAUIPCType(uint32_t instruction) const;
    ExecutionResult ExecuteJALType(uint32_t instruction) const;
    ExecutionResult ExecuteJALRType(uint32_t instruction) const;
    ExecutionResult ExecuteMExtension(uint8_t funct3, uint8_t rd, uint8_t rs1, uint8_t rs2) const;

    uint32_t GetPC() const;
    vector<uint32_t> m_instructions;
    Registers* m_registers;
    Memory* m_memory;
};


#endif // CPU_H

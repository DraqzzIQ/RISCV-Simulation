#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <vector>

#include "Registers.h"

using std::vector;


class CPU {
public:
    CPU();
    ~CPU();
    void LoadInstructions(const std::vector<uint32_t>& instructions);
    void Step() const;
    void Reset() const;
    vector<uint32_t> GetRegisters() const { return m_registers->GetRegisters(); }
private:
    void ExecuteInstruction(uint32_t instruction) const;
    vector<uint32_t> m_instructions;
    Registers* m_registers;
};



#endif //CPU_H

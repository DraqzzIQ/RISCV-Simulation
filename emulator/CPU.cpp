#include "CPU.h"

CPU::CPU()
{
    m_registers = new Registers();
}

CPU::~CPU()
{
    delete m_registers;
}

void CPU::LoadInstructions(const std::vector<uint32_t>& instructions)
{
    this->m_instructions = instructions;
    Reset();
}

void CPU::Reset() const
{
    m_registers->Reset();
}

void CPU::Step() const
{
    const uint32_t instruction = m_instructions[m_registers->GetPC()];
    ExecuteInstruction(instruction);
    m_registers->IncrementPC();
}

void CPU::ExecuteInstruction(uint32_t instruction) const
{
    
}
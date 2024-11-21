#include "Emulator.h"

Emulator::Emulator(const uint32_t memorySize)
{
    m_memory = new Memory(memorySize);
    m_cpu = new CPU(m_memory);
}

Emulator::Emulator()
{
    m_memory = new Memory();
    m_cpu = new CPU(m_memory);
}

Emulator::~Emulator()
{
    delete m_cpu;
    delete m_memory;
}

void Emulator::SetInstructions(const vector<uint32_t>& instructions) const
{
    m_cpu->LoadInstructions(instructions);
}

ExecutionResult Emulator::Step() const
{
    return m_cpu->Step();
}

CpuStatus Emulator::GetCpuStatus() const
{
    return m_cpu->GetStatus();
}

void Emulator::ResizeMemory(const uint32_t size) const
{
    m_memory->Resize(size);
}

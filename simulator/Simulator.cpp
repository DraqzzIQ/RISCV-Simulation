#include "Simulator.h"

Simulator::Simulator(const uint32_t memorySize)
{
    m_memory = new Memory(memorySize);
    m_cpu = new CPU(m_memory);
}

Simulator::Simulator()
{
    m_memory = new Memory();
    m_cpu = new CPU(m_memory);
}

Simulator::~Simulator()
{
    delete m_cpu;
    delete m_memory;
}

void Simulator::SetInstructions(const vector<uint32_t>& instructions) const { m_cpu->LoadInstructions(instructions); }

ExecutionResult Simulator::Step() const { return m_cpu->Step(); }

CpuStatus Simulator::GetCpuStatus() const { return m_cpu->GetStatus(); }

void Simulator::ResizeMemory(const uint32_t size) const { m_memory->Resize(size); }

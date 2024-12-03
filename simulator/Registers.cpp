#include "Registers.h"

Registers::Registers()
{
    m_registers = std::vector<uint32_t>(33, 0);
    Reset();
}

Registers::~Registers() = default;

void Registers::Reset()
{
    for (int i = 0; i < 33; i++) {
        m_registers[i] = 0;
    }
}

void Registers::SetPC(const uint32_t value) { m_registers[PC] = value; }

void Registers::IncrementPC() { m_registers[PC] += 4; }

uint32_t Registers::GetPC() const { return m_registers[PC]; }

void Registers::SetRegister(const uint8_t reg, const uint32_t value)
{
    if (reg == 0) // Register x0 is hardwired to 0
        return;
    m_registers[reg] = value;
}

uint32_t Registers::GetRegister(const uint8_t reg) const { return m_registers[reg]; }

vector<uint32_t> Registers::GetRegisters() const { return vector(m_registers.begin(), m_registers.begin() + 32); }

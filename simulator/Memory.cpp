#include "Memory.h"

Memory::Memory() { Resize(2048); }

Memory::Memory(const uint32_t size) { Resize(size); }

vector<uint32_t>* Memory::GetMemory() { return &m_memory; }

uint32_t Memory::Read(const uint32_t address) const
{
    if (address >= m_size)
    {
        return 0;
    }
    return m_memory[address];
}

uint16_t Memory::ReadHalfWord(const uint32_t address) const
{
    if (address >= m_size)
    {
        return 0;
    }
    return static_cast<uint16_t>(m_memory[address]);
}

uint8_t Memory::ReadByte(const uint32_t address) const
{
    if (address >= m_size)
    {
        return 0;
    }
    return static_cast<uint8_t>(m_memory[address]);
}

void Memory::Write(const uint32_t address, const uint32_t value)
{
    if (address >= m_size)
    {
        return;
    }
    m_memory[address] = value;
}

void Memory::Reset()
{
    m_memory.clear();
    m_memory.resize(m_size, 0);
}

void Memory::Resize(const uint32_t size)
{
    m_size = size;
    m_memory.resize(size, 0);
}
uint32_t Memory::GetSize() const { return m_memory.size(); }

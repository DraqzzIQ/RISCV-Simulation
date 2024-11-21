#ifndef MEMORY_H
#define MEMORY_H
#include <cstdint>
#include <vector>

using std::vector;

class Memory
{
public:
    Memory();
    explicit Memory(uint32_t size);
    ~Memory() = default;
    vector<uint32_t>* GetMemory();
    uint32_t Read(uint32_t address) const;
    uint16_t ReadHalfWord(uint32_t address) const;
    uint8_t ReadByte(uint32_t address) const;
    void Write(uint32_t address, uint32_t value);
    void Reset();
    void Resize(uint32_t size);
    uint32_t GetSize() const;

private:
    uint32_t m_size;
    vector<uint32_t> m_memory;
};

#endif // MEMORY_H

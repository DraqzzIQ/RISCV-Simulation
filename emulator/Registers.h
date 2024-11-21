#ifndef REGISTERS_H
#define REGISTERS_H
#include <cstdint>
#include <vector>

using std::vector;

constexpr uint8_t PC = 32;

class Registers {
public:
    Registers();
    ~Registers();
    void Reset();
    void SetPC(uint32_t value);
    void IncrementPC();
    uint32_t GetPC() const;
    void SetRegister(uint8_t reg, uint32_t value);
    void SetHighRegister(uint8_t reg, uint16_t value);
    void SetLowRegister(uint8_t reg, uint16_t value);
    uint32_t GetRegister(uint8_t reg) const;
    uint16_t GetHighRegister(uint8_t reg) const;
    uint16_t GetLowRegister(uint8_t reg) const;
    vector<uint32_t> GetRegisters() const;
private:
    // 0: constant 0
    // 1-31: general purpose registers
    // 32: stack pointer
    vector<uint32_t> m_registers;
};

#endif //REGISTERS_H
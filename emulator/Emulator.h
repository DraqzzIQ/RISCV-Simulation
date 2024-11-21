#ifndef EMULATOR_LIBRARY_H
#define EMULATOR_LIBRARY_H

#include <vector>
#include <cstdint>

#include "CPU.h"

using std::vector;

class Emulator
{
public:
    explicit Emulator(uint32_t memorySize);
    Emulator();
    ~Emulator();
    void SetInstructions(const vector<uint32_t>& instructions) const;
    ExecutionResult Step() const;
    CpuStatus GetCpuStatus() const;
    void ResizeMemory(uint32_t size) const;
private:
    Memory* m_memory;
    CPU* m_cpu;
};

#endif //EMULATOR_LIBRARY_H
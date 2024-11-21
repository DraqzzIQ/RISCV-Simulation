#ifndef SIMULATOR_LIBRARY_H
#define SIMULATOR_LIBRARY_H

#include <cstdint>
#include <vector>

#include "CPU.h"

using std::vector;

class Simulator
{
public:
    explicit Simulator(uint32_t memorySize);
    Simulator();
    ~Simulator();
    void SetInstructions(const vector<uint32_t>& instructions) const;
    ExecutionResult Step() const;
    CpuStatus GetCpuStatus() const;
    void ResizeMemory(uint32_t size) const;

private:
    Memory* m_memory;
    CPU* m_cpu;
};

#endif // SIMULATOR_LIBRARY_H

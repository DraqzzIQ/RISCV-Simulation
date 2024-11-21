#include "CPUUtil.h"

uint8_t CPUUtil::GetFunct7(const uint32_t instruction) { return static_cast<uint8_t>((instruction >> 25) & 0x7F); }

uint8_t CPUUtil::GetFunct3(const uint32_t instruction) { return static_cast<uint8_t>((instruction >> 12) & 0x7); }

uint8_t CPUUtil::GetRD(const uint32_t instruction) { return static_cast<uint8_t>((instruction >> 7) & 0x1F); }

uint8_t CPUUtil::GetRS1(const uint32_t instruction) { return static_cast<uint8_t>((instruction >> 15) & 0x1F); }

uint8_t CPUUtil::GetRS2(const uint32_t instruction) { return static_cast<uint8_t>((instruction >> 20) & 0x1F); }

uint32_t CPUUtil::GetImm20(const uint32_t instruction) { return instruction >> 12; }

uint16_t CPUUtil::GetImm12(const uint32_t instruction) { return static_cast<uint16_t>(instruction >> 20); }

uint8_t CPUUtil::GetImm5(const uint32_t instruction) { return static_cast<uint8_t>((instruction >> 20) & 0x1F); }

bool CPUUtil::IsValidRegister(const uint8_t reg) { return reg <= 31; }

ExecutionResult CPUUtil::ExecutionErrorResult(const ExecutionError error)
{
    return {false, error, false, {0, 0}, false, {0, 0}, 0};
}

uint8_t CPUUtil::GetOpcode(const uint32_t instruction) { return static_cast<uint8_t>(instruction & 0x0000007F); }

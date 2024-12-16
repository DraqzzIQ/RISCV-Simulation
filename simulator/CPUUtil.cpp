#include "CPUUtil.h"

uint8_t CPUUtil::GetFunct7(const uint32_t instruction) { return static_cast<uint8_t>((instruction >> 25) & 0x7F); }

uint8_t CPUUtil::GetFunct3(const uint32_t instruction) { return static_cast<uint8_t>((instruction >> 12) & 0x7); }

uint8_t CPUUtil::GetRD(const uint32_t instruction) { return static_cast<uint8_t>((instruction >> 7) & 0x1F); }

uint8_t CPUUtil::GetRS1(const uint32_t instruction) { return static_cast<uint8_t>((instruction >> 15) & 0x1F); }

uint8_t CPUUtil::GetRS2(const uint32_t instruction) { return static_cast<uint8_t>((instruction >> 20) & 0x1F); }

uint32_t CPUUtil::GetImm20(const uint32_t instruction) { return instruction >> 12; }

int16_t CPUUtil::GetImm12(const uint32_t instruction)
{
    int32_t imm12 = static_cast<int32_t>(instruction) >> 20;
    imm12 = imm12 & 0xFFF;
    if (imm12 & 0x800) { // Check if sign bit (bit 11) is set
        imm12 |= 0xFFFFF000; // Sign-extend by setting upper 20 bits to 1
    }
    return imm12;
}

uint8_t CPUUtil::GetImm5(const uint32_t instruction) { return static_cast<uint8_t>((instruction >> 20) & 0x1F); }

bool CPUUtil::IsValidRegister(const uint8_t reg) { return reg <= 31; }

ExecutionResult CPUUtil::ExecutionErrorResult(const ExecutionError error)
{
    return {false, error, false, {0, 0}, false, {0, 0}, 0};
}

uint8_t CPUUtil::GetOpcode(const uint32_t instruction) { return static_cast<uint8_t>(instruction & 0x0000007F); }

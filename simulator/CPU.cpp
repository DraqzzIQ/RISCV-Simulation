#include <map>

#include "CPU.h"
#include "Opcodes.h"

using std::map;


CPU::CPU(Memory* memory) : m_memory(memory) { m_registers = new Registers(); }

CPU::~CPU() { delete m_registers; }

void CPU::LoadInstructions(const std::vector<uint32_t>& instructions) { this->m_instructions = instructions; }

void CPU::Reset() const { m_registers->Reset(); }

CpuStatus CPU::GetStatus() const
{
    CpuStatus status;
    status.registers = m_registers->GetRegisters();
    status.pc = m_registers->GetPC();
    return status;
}

ExecutionResult CPU::Step() const
{
    const uint32_t pc = m_registers->GetPC() / 4;
    if (pc >= m_instructions.size()) {
        return CPUUtil::ExecutionErrorResult(ExecutionError::PC_OUT_OF_BOUNDS);
    }

    const uint32_t instruction = m_instructions[pc];
    ExecutionResult result = ExecuteInstruction(instruction);
    if (result.error != ExecutionError::NONE) {
        result.errorInstruction = pc + 1;
        Reset();
    }
    result.pc = m_registers->GetPC();
    return result;
}

ExecutionResult CPU::ExecuteInstruction(const uint32_t instruction) const
{
    switch (CPUUtil::GetOpcode(instruction)) {
    case R_Type:
        {
            const ExecutionResult result = ExecuteRType(instruction);
            m_registers->IncrementPC();
            return result;
        }
    case I_Type:
        {
            const ExecutionResult result = ExecuteIType(instruction);
            m_registers->IncrementPC();
            return result;
        }
    case Load_Type:
        {
            const ExecutionResult result = ExecuteLoadType(instruction);
            m_registers->IncrementPC();
            return result;
        }
    case S_Type:
        {
            const ExecutionResult result = ExecuteSType(instruction);
            m_registers->IncrementPC();
            return result;
        }
    case B_Type:
        {
            return ExecuteBType(instruction);
        }
    case LUI_Type:
        {
            const ExecutionResult result = ExecuteLUIType(instruction);
            m_registers->IncrementPC();
            return result;
        }
    case AUIPC_Type:
        {
            const ExecutionResult result = ExecuteAUIPCType(instruction);
            m_registers->IncrementPC();
            return result;
        }
    case JAL_Type:
        {
            return ExecuteJALType(instruction);
        }
    case JALR_Type:
        {
            return ExecuteJALRType(instruction);
        }
    default:
        {
            return {false, ExecutionError::UNSUPPORTED_OPCODE, false, {0, 0}, false, {0, 0}, 0};
        }
    }
}

ExecutionResult CPU::ExecuteRType(const uint32_t instruction) const
{
    const uint8_t funct7 = CPUUtil::GetFunct7(instruction);
    const uint8_t funct3 = CPUUtil::GetFunct3(instruction);
    const uint8_t rd = CPUUtil::GetRD(instruction);
    const uint8_t rs1 = CPUUtil::GetRS1(instruction);
    const uint8_t rs2 = CPUUtil::GetRS2(instruction);
    if (!CPUUtil::IsValidRegister(rd) || !CPUUtil::IsValidRegister(rs1) || !CPUUtil::IsValidRegister(rs2)) {
        return CPUUtil::ExecutionErrorResult(ExecutionError::INVALID_REGISTER);
    }

    if (funct7 == 0x1) {
        return ExecuteMExtension(funct3, rd, rs1, rs2);
    }

    if (funct7 == 0x2) {
        if (funct3 == SUB) {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) - m_registers->GetRegister(rs2));
        }
        else if (funct3 == SRA) {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) >> m_registers->GetRegister(rs2));
        }
        else {
            return CPUUtil::ExecutionErrorResult(ExecutionError::UNSUPPORTED_OPCODE);
        }
        return {true, ExecutionError::NONE, false, {0, 0}, true, {rd, m_registers->GetRegister(rd)}, 0};
    }

    switch (funct3) {
    case ADD:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) + m_registers->GetRegister(rs2));
            break;
        }
    case SLL:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) << m_registers->GetRegister(rs2));
            break;
        }
    case SLT:
        {
            m_registers->SetRegister(rd,
                                     static_cast<int32_t>(m_registers->GetRegister(rs1)) <
                                         static_cast<int32_t>(m_registers->GetRegister(rs2)));
            break;
        }
    case SLTU:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) < m_registers->GetRegister(rs2));
            break;
        }
    case XOR:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) ^ m_registers->GetRegister(rs2));
            break;
        }
    case SRL:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) >> m_registers->GetRegister(rs2));
            break;
        }
    case OR:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) | m_registers->GetRegister(rs2));
            break;
        }
    case AND:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) & m_registers->GetRegister(rs2));
            break;
        }
    default:
        {
            return CPUUtil::ExecutionErrorResult(ExecutionError::UNSUPPORTED_OPCODE);
        }
    }
    return {true, ExecutionError::NONE, false, {0, 0}, true, {rd, m_registers->GetRegister(rd)}, 0};
}

ExecutionResult CPU::ExecuteIType(const uint32_t instruction) const
{
    const uint8_t rd = CPUUtil::GetRD(instruction);
    const uint8_t rs1 = CPUUtil::GetRS1(instruction);
    if (!CPUUtil::IsValidRegister(rd) || !CPUUtil::IsValidRegister(rs1)) {
        return CPUUtil::ExecutionErrorResult(ExecutionError::INVALID_REGISTER);
    }
    const int16_t immediate12 = CPUUtil::GetImm12(instruction);
    const uint8_t immediate5 = CPUUtil::GetImm5(instruction);

    switch (CPUUtil::GetFunct3(instruction)) {
    case ADDI:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) + immediate12);
            break;
        }
    case SLTI:
        {
            m_registers->SetRegister(rd, static_cast<int32_t>(m_registers->GetRegister(rs1)) < immediate12);
            break;
        }
    case SLTIU:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) < static_cast<uint32_t>(immediate12));
            break;
        }
    case XORI:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) ^ immediate12);
            break;
        }
    case ORI:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) | immediate12);
            break;
        }
    case ANDI:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) & immediate12);
            break;
        }
    case SLLI:
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) << immediate5);
            break;
        }
    case SRLI_SRAI:
        {
            // bit 30 determines if it's SRLI or SRAI
            if (instruction & (1 << 30)) {
                // SRAI
                m_registers->SetRegister(rd, static_cast<int32_t>(m_registers->GetRegister(rs1)) >> immediate5);
            }
            else {
                // SRLI
                m_registers->SetRegister(rd, m_registers->GetRegister(rs1) >> immediate5);
            }
            break;
        }
    default:
        {
            return CPUUtil::ExecutionErrorResult(ExecutionError::UNSUPPORTED_OPCODE);
        }
    }
    return {true, ExecutionError::NONE, false, {0, 0}, true, {rd, m_registers->GetRegister(rd)}, 0};
}

ExecutionResult CPU::ExecuteLoadType(const uint32_t instruction) const
{
    const uint8_t rd = CPUUtil::GetRD(instruction);
    const uint8_t rs1 = CPUUtil::GetRS1(instruction);
    if (!CPUUtil::IsValidRegister(rd) || !CPUUtil::IsValidRegister(rs1)) {
        return CPUUtil::ExecutionErrorResult(ExecutionError::INVALID_REGISTER);
    }
    const int32_t immediate12 = CPUUtil::GetImm12(instruction);
    const uint32_t address = m_registers->GetRegister(rs1) + immediate12;
    if (m_memory->GetSize() <= address) {
        return CPUUtil::ExecutionErrorResult(ExecutionError::INVALID_MEMORY_ACCESS);
    }

    switch (CPUUtil::GetFunct3(instruction)) {
    case LB:
        {
            const int32_t value = static_cast<int8_t>(m_memory->ReadByte(address));
            m_registers->SetRegister(rd, value);
            break;
            break;
        }
    case LH:
        {
            const int32_t value = static_cast<int16_t>(m_memory->ReadHalfWord(address));
            m_registers->SetRegister(rd, value);
            break;
        }
    case LW:
        {
            const uint32_t value = m_memory->ReadHalfWord(address);
            m_registers->SetRegister(rd, value);
            break;
        }
    case LBU:
        {
            const uint32_t value = m_memory->ReadByte(address);
            m_registers->SetRegister(rd, value);
            break;
        }
    case LHU:
        {
            const uint32_t value = m_memory->ReadHalfWord(address);
            m_registers->SetRegister(rd, value);
            break;
        }
    default:
        {
            return CPUUtil::ExecutionErrorResult(ExecutionError::UNSUPPORTED_OPCODE);
        }
    }
    return {true, ExecutionError::NONE, false, {0, 0}, true, {rd, m_registers->GetRegister(rd)}};
}

ExecutionResult CPU::ExecuteSType(const uint32_t instruction) const
{
    const uint8_t rs1 = CPUUtil::GetRS1(instruction);
    const uint8_t rs2 = CPUUtil::GetRS2(instruction);
    if (!CPUUtil::IsValidRegister(rs1) || !CPUUtil::IsValidRegister(rs2)) {
        return CPUUtil::ExecutionErrorResult(ExecutionError::INVALID_REGISTER);
    }
    const uint16_t upperImm = CPUUtil::GetFunct7(instruction);
    const uint16_t lowerImm = CPUUtil::GetRD(instruction);
    const uint16_t imm = upperImm << 5 | lowerImm;
    const uint32_t address = m_registers->GetRegister(rs1) + imm;
    if (m_memory->GetSize() <= address) {
        return CPUUtil::ExecutionErrorResult(ExecutionError::INVALID_MEMORY_ACCESS);
    }

    switch (CPUUtil::GetFunct3(instruction)) {
    case SB:
        {
            const uint8_t value = m_registers->GetRegister(rs2);
            m_memory->Write(address, value);
            break;
        }
    case SH:
        {
            const uint16_t value = m_registers->GetRegister(rs2);
            m_memory->Write(address, value);
            break;
        }
    case SW:
        {
            const uint32_t value = m_registers->GetRegister(rs2);
            m_memory->Write(address, value);
            break;
        }
    default:
        {
            return CPUUtil::ExecutionErrorResult(ExecutionError::UNSUPPORTED_OPCODE);
        }
    }
    return {true, ExecutionError::NONE, true, {address, m_memory->Read(address)}};
}

ExecutionResult CPU::ExecuteBType(const uint32_t instruction) const
{
    const uint8_t rs1 = CPUUtil::GetRS1(instruction);
    const uint8_t rs2 = CPUUtil::GetRS2(instruction);
    if (!CPUUtil::IsValidRegister(rs1) || !CPUUtil::IsValidRegister(rs2)) {
        return CPUUtil::ExecutionErrorResult(ExecutionError::INVALID_REGISTER);
    }
    const uint32_t imm12 = instruction >> 31;
    const uint32_t imm10To5 = instruction >> 25 & 0b111111;
    const uint32_t imm4To1 = instruction >> 8 & 0b1111;
    const uint32_t imm11 = instruction >> 7 & 0b1;

    int32_t imm = imm12 << 12 | imm11 << 11 | imm10To5 << 5 | imm4To1 << 1 | 0;
    if (imm >> 12) {
        imm |= 0xFFFFE000;
    }
    if (imm % 4 != 0) {
        // RISC-V instructions are 4-byte aligned
        return CPUUtil::ExecutionErrorResult(ExecutionError::OFFSET_NOT_32_BIT_ALIGNED);
    }

    const uint32_t val1 = m_registers->GetRegister(rs1);
    const uint32_t val2 = m_registers->GetRegister(rs2);
    const uint32_t pc = m_registers->GetPC();

    switch (CPUUtil::GetFunct3(instruction)) {
    case BEQ:
        {
            if (val1 == val2) {
                m_registers->SetPC(pc + imm);
            }
            else {
                m_registers->IncrementPC();
            }
            break;
        }
    case BNE:
        {
            if (val1 != val2) {
                m_registers->SetPC(pc + imm);
            }
            else {
                m_registers->IncrementPC();
            }
            break;
        }
    case BLT:
        {
            if (static_cast<int32_t>(val1) < static_cast<int32_t>(val2)) {
                m_registers->SetPC(pc + imm);
            }
            else {
                m_registers->IncrementPC();
            }
            break;
        }
    case BGE:
        {
            if (static_cast<int32_t>(val1) >= static_cast<int32_t>(val2)) {
                m_registers->SetPC(pc + imm);
            }
            else {
                m_registers->IncrementPC();
            }
            break;
        }
    case BLTU:
        {
            if (val1 < val2) {
                m_registers->SetPC(pc + imm);
            }
            else {
                m_registers->IncrementPC();
            }
            break;
        }
    case BGEU:
        {
            if (val1 >= val2) {
                m_registers->SetPC(pc + imm);
            }
            else {
                m_registers->IncrementPC();
            }
            break;
        }
    default:
        {
            return CPUUtil::ExecutionErrorResult(ExecutionError::UNSUPPORTED_OPCODE);
        }
    }
    return {true, ExecutionError::NONE, false, {0, 0}, false, {0, 0}};
}

ExecutionResult CPU::ExecuteJALType(const uint32_t instruction) const
{
    const uint8_t rd = CPUUtil::GetRD(instruction);
    if (!CPUUtil::IsValidRegister(rd)) {
        return CPUUtil::ExecutionErrorResult(ExecutionError::INVALID_REGISTER);
    }
    const uint32_t imm20 = instruction >> 31;
    const uint32_t imm10To1 = instruction >> 21 & 0b1111111111;
    const uint32_t imm11 = instruction >> 20 & 0b1;
    const uint32_t imm19To12 = instruction >> 12 & 0b11111111;
    int32_t imm = static_cast<int32_t>(0 | imm10To1 << 1 | imm11 << 11 | imm19To12 << 12 | imm20 << 20);
    // Sign-extend the immediate to 32 bits
    if (imm20) { // If the sign bit is set, extend it
        imm |= 0xFFF00000;
    }

    if (imm % 4 != 0) {
        // RISC-V instructions are 4-byte aligned
        return CPUUtil::ExecutionErrorResult(ExecutionError::OFFSET_NOT_32_BIT_ALIGNED);
    }

    const uint32_t pc = m_registers->GetPC();
    m_registers->SetRegister(rd, pc + 4);
    m_registers->SetPC(pc + imm);

    return {true, ExecutionError::NONE, false, {0, 0}, true, {rd, m_registers->GetRegister(rd)}};
}

ExecutionResult CPU::ExecuteJALRType(const uint32_t instruction) const
{
    const uint8_t rd = CPUUtil::GetRD(instruction);
    const uint8_t rs1 = CPUUtil::GetRS1(instruction);
    if (!CPUUtil::IsValidRegister(rd) || !CPUUtil::IsValidRegister(rs1)) {
        return CPUUtil::ExecutionErrorResult(ExecutionError::INVALID_REGISTER);
    }
    const uint32_t pc = m_registers->GetPC();
    const int16_t imm = CPUUtil::GetImm12(instruction);
    if (imm % 4 != 0) {
        // RISC-V instructions are 4-byte aligned
        return CPUUtil::ExecutionErrorResult(ExecutionError::OFFSET_NOT_32_BIT_ALIGNED);
    }

    m_registers->SetRegister(rd, pc + 4);
    m_registers->SetPC(m_registers->GetRegister(rs1) + imm);

    return {true, ExecutionError::NONE, false, {0, 0}, true, {rd, m_registers->GetRegister(rd)}};
}

ExecutionResult CPU::ExecuteLUIType(const uint32_t instruction) const
{
    const uint8_t rd = CPUUtil::GetRD(instruction);
    if (!CPUUtil::IsValidRegister(rd)) {
        return CPUUtil::ExecutionErrorResult(ExecutionError::INVALID_REGISTER);
    }
    const int32_t imm = static_cast<int32_t>(CPUUtil::GetImm20(instruction));
    m_registers->SetRegister(rd, imm << 12);

    return {true, ExecutionError::NONE, false, {0, 0}, true, {rd, m_registers->GetRegister(rd)}};
}

ExecutionResult CPU::ExecuteAUIPCType(const uint32_t instruction) const
{
    const uint8_t rd = CPUUtil::GetRD(instruction);
    if (!CPUUtil::IsValidRegister(rd)) {
        return CPUUtil::ExecutionErrorResult(ExecutionError::INVALID_REGISTER);
    }
    const int32_t imm = static_cast<int32_t>(CPUUtil::GetImm20(instruction));
    m_registers->SetRegister(rd, m_registers->GetPC() + (imm << 12));

    return {true, ExecutionError::NONE, false, {0, 0}, true, {rd, m_registers->GetRegister(rd)}};
}


ExecutionResult CPU::ExecuteMExtension(const uint8_t funct3, const uint8_t rd, const uint8_t rs1,
                                       const uint8_t rs2) const
{
    switch (funct3) {
    case MUL: // signed x signed
        {
            const int64_t result = static_cast<int64_t>(m_registers->GetRegister(rs1)) *
                static_cast<int64_t>(m_registers->GetRegister(rs2));
            m_registers->SetRegister(rd, static_cast<uint32_t>(result & 0xFFFFFFFF)); // lower 32 bits
            break;
        }
    case MULH: // signed x signed
        {
            const int64_t result = static_cast<int64_t>(m_registers->GetRegister(rs1)) *
                static_cast<int64_t>(m_registers->GetRegister(rs2));
            m_registers->SetRegister(rd, static_cast<uint32_t>(result >> 32) & 0xFFFFFFFF); // upper 32 bits
            break;
        }
    case MULHSU: // signed x unsigned
        {
            const int64_t result = static_cast<int64_t>(static_cast<int64_t>(m_registers->GetRegister(rs1)) *
                                                        static_cast<uint64_t>(m_registers->GetRegister(rs2)));
            m_registers->SetRegister(rd, static_cast<uint32_t>(result >> 32) & 0xFFFFFFFF); // upper 32 bits
            break;
        }
    case MULHU: // unsigned x unsigned
        {
            const uint64_t result = static_cast<uint64_t>(m_registers->GetRegister(rs1)) *
                static_cast<uint64_t>(m_registers->GetRegister(rs2));
            m_registers->SetRegister(rd, static_cast<uint32_t>(result >> 32) & 0xFFFFFFFF); // upper 32 bits
            break;
        }
    case DIV: // signed / signed
        {
            if (m_registers->GetRegister(rs2) == 0) {
                return CPUUtil::ExecutionErrorResult(ExecutionError::DIVISION_BY_ZERO);
            }
            m_registers->SetRegister(rd,
                                     static_cast<uint32_t>(static_cast<int32_t>(m_registers->GetRegister(rs1)) /
                                                           static_cast<int32_t>(m_registers->GetRegister(rs2))));
            break;
        }
    case DIVU: // unsigned / unsigned
        {
            if (m_registers->GetRegister(rs2) == 0) {
                return CPUUtil::ExecutionErrorResult(ExecutionError::DIVISION_BY_ZERO);
            }
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) / m_registers->GetRegister(rs2));
            break;
        }
    case REM: // signed % signed
        {
            if (m_registers->GetRegister(rs2) == 0) {
                return CPUUtil::ExecutionErrorResult(ExecutionError::DIVISION_BY_ZERO);
            }
            m_registers->SetRegister(rd,
                                     static_cast<uint32_t>(static_cast<int32_t>(m_registers->GetRegister(rs1)) %
                                                           static_cast<int32_t>(m_registers->GetRegister(rs2))));
            break;
        }
    case REMU: // unsigned % unsigned
        {
            if (m_registers->GetRegister(rs2) == 0) {
                return CPUUtil::ExecutionErrorResult(ExecutionError::DIVISION_BY_ZERO);
            }
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) % m_registers->GetRegister(rs2));
            break;
        }
    default:
        {
            return CPUUtil::ExecutionErrorResult(ExecutionError::UNSUPPORTED_OPCODE);
        }
    }
    return {true, ExecutionError::NONE, false, {0, 0}, true, {rd, m_registers->GetRegister(rd)}, 0};
}


uint32_t CPU::GetPC() const { return m_registers->GetPC(); }

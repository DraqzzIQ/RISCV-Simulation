#include <map>

#include "CPU.h"
#include "Opcodes.h"

using std::map;


CPU::CPU(Memory* memory)
    : m_memory(memory)
{
    m_registers = new Registers();
}

CPU::~CPU()
{
    delete m_registers;
}

void CPU::LoadInstructions(const std::vector<uint32_t>& instructions)
{
    this->m_instructions = instructions;
    Reset();
}

void CPU::Reset() const
{
    m_registers->Reset();
}

CpuStatus CPU::GetStatus() const
{
    CpuStatus status;
    status.registers = m_registers->GetRegisters();
    status.pc = m_registers->GetPC();
    return status;
}

ExecutionResult CPU::Step() const
{
    const uint32_t instruction = m_instructions[m_registers->GetPC()];
    ExecutionResult result = ExecuteInstruction(instruction);
    m_registers->IncrementPC();
    result.pc = m_registers->GetPC();
    return result;
}

ExecutionResult CPU::ExecuteInstruction(const uint32_t instruction) const
{
    switch (GetOpcode(instruction))
    {
    case R_Type:
        {
            return ExecuteRType(instruction);
        }
    case I_Type:
        {
            return ExecuteIType(instruction);
        }
    case Load_Type:
        {
            return ExecuteLoadType(instruction);
        }
    case S_Type:
        {
            return ExecuteSType(instruction);
        }
    case B_Type:
        {
            return ExecuteBType(instruction);
        }
    case LUI_Type:
        {
            return ExecuteLUIType(instruction);
        }
    case AUIPC_Type:
        {
            return ExecuteAUIPCType(instruction);
        }
    case JALR_Type:
        {
            return ExecuteJALRType(instruction);
        }
    default:
        {
            return {false, ExecutionError::INVALID_OPCODE, false, {0, 0}, false, {0, 0}, 0};
        }
    }
}

ExecutionResult CPU::ExecuteRType(const uint32_t instruction) const
{
    const uint8_t funct7 = GetFunct7(instruction);
    const uint8_t funct3 = GetFunct3(instruction);
    const uint8_t rd = GetRD(instruction);
    const uint8_t rs1 = GetRS1(instruction);
    const uint8_t rs2 = GetRS2(instruction);
    if (!IsValidRegister(rd) || !IsValidRegister(rs1) || !IsValidRegister(rs2))
    {
        return ExecutionErrorResult(ExecutionError::INVALID_REGISTER);
    }

    if (funct7 == 0x1)
    {
        return ExecuteMExtension(funct3, rd, rs1, rs2);
    }

    if (funct7 == 0x2)
    {
        if (funct3 == SUB)
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) - m_registers->GetRegister(rs2));
        }
        else if (funct3 == SRA)
        {
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) >> m_registers->GetRegister(rs2));
        }
        else
        {
            return ExecutionErrorResult(ExecutionError::INVALID_OPCODE);
        }
        return {true, ExecutionError::NONE, false, {0, 0}, true, {rd, m_registers->GetRegister(rd)}, 0};
    }

    switch (funct3)
    {
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
            m_registers->SetRegister(
                rd, static_cast<int32_t>(m_registers->GetRegister(rs1)) < static_cast<int32_t>(m_registers->
                    GetRegister(rs2)));
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
            return ExecutionErrorResult(ExecutionError::INVALID_OPCODE);
        }
    }
    return {true, ExecutionError::NONE, false, {0, 0}, true, {rd, m_registers->GetRegister(rd)}, 0};
}

ExecutionResult CPU::ExecuteIType(const uint32_t instruction) const
{
    const uint8_t rd = GetRD(instruction);
    const uint8_t rs1 = GetRS1(instruction);
    const int32_t immediate12 = GetImm12(instruction);
    const uint8_t immediate5 = GetImm5(instruction);
    switch (GetFunct3(instruction))
    {
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
            if (instruction & (1 << 30)) // SRAI
            {
                m_registers->SetRegister(rd, static_cast<int32_t>(m_registers->GetRegister(rs1)) >> immediate5);
            }
            else // SRLI
            {
                m_registers->SetRegister(rd, m_registers->GetRegister(rs1) >> immediate5);
            }
            break;
        }
    }
    
}

ExecutionResult CPU::ExecuteLoadType(uint32_t instruction) const
{
}

ExecutionResult CPU::ExecuteSType(uint32_t instruction) const
{
}

ExecutionResult CPU::ExecuteBType(uint32_t instruction) const
{
}

ExecutionResult CPU::ExecuteLUIType(uint32_t instruction) const
{
}

ExecutionResult CPU::ExecuteAUIPCType(uint32_t instruction) const
{
}

ExecutionResult CPU::ExecuteJType(uint32_t instruction) const
{
}

ExecutionResult CPU::ExecuteJALRType(uint32_t instruction) const
{
}

ExecutionResult CPU::ExecuteMExtension(const uint8_t funct3, const uint8_t rd, const uint8_t rs1,
                                       const uint8_t rs2) const
{
    switch (funct3)
    {
    case MUL: // signed x signed
        {
            const int64_t result = static_cast<int64_t>(m_registers->GetRegister(rs1)) * static_cast<int64_t>(
                m_registers->GetRegister(rs2));
            m_registers->SetRegister(rd, static_cast<uint32_t>(result & 0xFFFFFFFF)); // lower 32 bits
            break;
        }
    case MULH: // signed x signed
        {
            const int64_t result = static_cast<int64_t>(m_registers->GetRegister(rs1)) * static_cast<int64_t>(
                m_registers->GetRegister(rs2));
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
            const uint64_t result = static_cast<uint64_t>(m_registers->GetRegister(rs1)) * static_cast<uint64_t>(
                m_registers->GetRegister(rs2));
            m_registers->SetRegister(rd, static_cast<uint32_t>(result >> 32) & 0xFFFFFFFF); // upper 32 bits
            break;
        }
    case DIV: // signed / signed
        {
            if (m_registers->GetRegister(rs2) == 0)
            {
                return ExecutionErrorResult(ExecutionError::DIVISION_BY_ZERO);
            }
            m_registers->SetRegister(
                rd, static_cast<uint32_t>(static_cast<int32_t>(m_registers->GetRegister(rs1)) / static_cast<int32_t>(
                    m_registers->GetRegister(rs2))));
            break;
        }
    case DIVU: // unsigned / unsigned
        {
            if (m_registers->GetRegister(rs2) == 0)
            {
                return ExecutionErrorResult(ExecutionError::DIVISION_BY_ZERO);
            }
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) / m_registers->GetRegister(rs2));
            break;
        }
    case REM: // signed % signed
        {
            if (m_registers->GetRegister(rs2) == 0)
            {
                return ExecutionErrorResult(ExecutionError::DIVISION_BY_ZERO);
            }
            m_registers->SetRegister(
                rd, static_cast<uint32_t>(static_cast<int32_t>(m_registers->GetRegister(rs1)) % static_cast<int32_t>(
                    m_registers->GetRegister(rs2))));
            break;
        }
    case REMU: // unsigned % unsigned
        {
            if (m_registers->GetRegister(rs2) == 0)
            {
                return ExecutionErrorResult(ExecutionError::DIVISION_BY_ZERO);
            }
            m_registers->SetRegister(rd, m_registers->GetRegister(rs1) % m_registers->GetRegister(rs2));
            break;
        }
    default:
        {
            return ExecutionErrorResult(ExecutionError::INVALID_OPCODE);
        }
    }
    return {true, ExecutionError::NONE, false, {0, 0}, true, {rd, m_registers->GetRegister(rd)}, 0};
}

uint8_t CPU::GetFunct7(const uint32_t instruction) const
{
    return static_cast<uint8_t>((instruction >> 25) & 0x7F);
}

uint8_t CPU::GetFunct3(const uint32_t instruction) const
{
    return static_cast<uint8_t>((instruction >> 12) & 0x7);
}

uint8_t CPU::GetRD(const uint32_t instruction) const
{
    return static_cast<uint8_t>((instruction >> 7) & 0x1F);
}

uint8_t CPU::GetRS1(const uint32_t instruction) const
{
    return static_cast<uint8_t>((instruction >> 15) & 0x1F);
}

uint8_t CPU::GetRS2(const uint32_t instruction) const
{
    return static_cast<uint8_t>((instruction >> 20) & 0x1F);
}

uint16_t CPU::GetImm12(const uint32_t instruction) const
{
    return static_cast<uint16_t>(instruction >> 20);
}

uint8_t CPU::GetImm5(const uint32_t instruction) const
{
    return static_cast<uint8_t>((instruction >> 20) & 0x1F);
}

bool CPU::IsValidRegister(const uint8_t reg) const
{
    return reg <= 31;
}

ExecutionResult CPU::ExecutionErrorResult(const ExecutionError error) const
{
    return {false, error, false, {0, 0}, false, {0, 0}, 0};
}


uint32_t CPU::GetPC() const
{
    return m_registers->GetPC();
}

uint8_t CPU::GetOpcode(const uint32_t instruction) const
{
    return static_cast<uint8_t>(instruction & 0x0000007F);
}

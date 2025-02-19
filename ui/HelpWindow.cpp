#include "HelpWindow.h"
#include <QVBoxLayout>

HelpWindow::HelpWindow(QWidget* parent) : QDialog(parent)
{
    this->setProperty("cssClass", "themedBackground");
    setWindowTitle("RISC-V Instructions");
    resize(900, 600);

    tableWidget = new QTableWidget(this);
    tableWidget->setProperty("cssClass", "themedBackground");
    tableWidget->setColumnCount(7);
    tableWidget->setHorizontalHeaderLabels(
        {"Opcode", "Operand 1", "Operand 2", "Operand 3", "Max Imm/Offset", "Description", "Example"});

    populateTable();

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(tableWidget);
    setLayout(layout);
}

void HelpWindow::populateTable() const
{
    struct Instruction
    {
        QString opcode;
        QString arg1, arg2, arg3;
        QString immRange;
        QString description;
        QString example;
    };

    QList<Instruction> instructions = {
        {"ADD", "rd", "rs1", "rs2", "-", "rd = rs1 + rs2", "add x3, x1, x2"},
        {"SUB", "rd", "rs1", "rs2", "-", "rd = rs1 - rs2", "sub x3, x1, x2"},
        {"SLL", "rd", "rs1", "rs2", "-", "rd = rs1 << rs2", "sll x3, x1, x2"},
        {"SLT", "rd", "rs1", "rs2", "-", "rd = (rs1 < rs2) ? 1 : 0", "slt x3, x1, x2"},
        {"SLTU", "rd", "rs1", "rs2", "-", "rd = (rs1 < rs2) ? 1 : 0 (unsigned)", "sltu x3, x1, x2"},
        {"XOR", "rd", "rs1", "rs2", "-", "rd = rs1 ^ rs2", "xor x3, x1, x2"},
        {"SRL", "rd", "rs1", "rs2", "-", "rd = rs1 >> rs2 (logical)", "srl x3, x1, x2"},
        {"SRA", "rd", "rs1", "rs2", "-", "rd = rs1 >> rs2 (arithmetic)", "sra x3, x1, x2"},
        {"OR", "rd", "rs1", "rs2", "-", "rd = rs1 | rs2", "or x3, x1, x2"},
        {"AND", "rd", "rs1", "rs2", "-", "rd = rs1 & rs2", "and x3, x1, x2"},
        {"ADDI", "rd", "rs1", "imm", "-2048 to 2047", "rd = rs1 + imm", "addi x3, x1, 45"},
        {"SLLI", "rd", "rs1", "imm", "0 to 31", "rd = rs1 << imm", "slli x3, x1, 2"},
        {"SLTI", "rd", "rs1", "imm", "-2048 to 2047", "rd = (rs1 < imm) ? 1 : 0", "slti x3, x1, 10"},
        {"SLTIU", "rd", "rs1", "imm", "-2048 to 2047", "rd = (rs1 < imm) ? 1 : 0 (unsigned)", "sltiu x3, x1, 10"},
        {"XORI", "rd", "rs1", "imm", "-2048 to 2047", "rd = rs1 ^ imm", "xori x3, x1, 5"},
        {"SRLI", "rd", "rs1", "imm", "0 to 31", "rd = rs1 >> imm (logical)", "srli x3, x1, 3"},
        {"SRAI", "rd", "rs1", "imm", "0 to 31", "rd = rs1 >> imm (arithmetic)", "srai x3, x1, 3"},
        {"ORI", "rd", "rs1", "imm", "-2048 to 2047", "rd = rs1 | imm", "ori x3, x1, 15"},
        {"ANDI", "rd", "rs1", "imm", "-2048 to 2047", "rd = rs1 & imm", "andi x3, x1, 7"},
        {"LB", "rd", "offset(rs1)", "-", "-2048 to 2047", "rd = mem[rs1 + offset]", "lb x3, 100(x1)"},
        {"LH", "rd", "offset(rs1)", "-", "-2048 to 2047", "rd = mem[rs1 + offset]", "lh x3, 100(x1)"},
        {"LW", "rd", "offset(rs1)", "-", "-2048 to 2047", "rd = mem[rs1 + offset]", "lw x3, 100(x1)"},
        {"LBU", "rd", "offset(rs1)", "-", "-2048 to 2047", "rd = mem[rs1 + offset] (unsigned)", "lbu x3, 100(x1)"},
        {"LHU", "rd", "offset(rs1)", "-", "-2048 to 2047", "rd = mem[rs1 + offset] (unsigned)", "lhu x3, 100(x1)"},
        {"SB", "rs2", "offset(rs1)", "-", "-2048 to 2047", "mem[rs1 + offset] = rs2", "sb x3, 100(x1)"},
        {"SH", "rs2", "offset(rs1)", "-", "-2048 to 2047", "mem[rs1 + offset] = rs2", "sh x3, 100(x1)"},
        {"SW", "rs2", "offset(rs1)", "-", "-2048 to 2047", "mem[rs1 + offset] = rs2", "sw x3, 100(x1)"},
        {"JAL", "rd", "offset", "-", "-1048576 to 1048575", "rd = pc + 4; pc += offset", "jal x3, 200"},
        {"JALR", "rd", "offset(rs1)", "-", "-2048 to 2047", "rd = pc + 4; pc = rs1 + offset", "jalr x3, 100(x1)"},
        {"BEQ", "rs1", "rs2", "offset", "-4096 to 4095", "if (rs1 == rs2) pc += offset", "beq x1, x2, 100"},
        {"BNE", "rs1", "rs2", "offset", "-4096 to 4095", "if (rs1 != rs2) pc += offset", "bne x1, x2, 100"},
        {"BLT", "rs1", "rs2", "offset", "-4096 to 4095", "if (rs1 < rs2) pc += offset", "blt x1, x2, 100"},
        {"BGE", "rs1", "rs2", "offset", "-4096 to 4095", "if (rs1 >= rs2) pc += offset", "bge x1, x2, 100"},
        {"LUI", "rd", "imm", "-", "-524288 to 524287", "rd = imm << 12", "lui x3, 4096"},
        {"AUIPC", "rd", "imm", "-", "-524288 to 524287", "rd = pc + (imm << 12)", "auipc x3, 4096"},
        {"MUL", "rd", "rs1", "rs2", "-", "rd = rs1 * rs2", "mul x3, x1, x2"},
        {"DIV", "rd", "rs1", "rs2", "-", "rd = rs1 / rs2", "div x3, x1, x2"},
        {"REM", "rd", "rs1", "rs2", "-", "rd = rs1 % rs2", "rem x3, x1, x2"},
    };

    tableWidget->setRowCount(instructions.size());

    for (int i = 0; i < instructions.size(); ++i) {
        tableWidget->setItem(i, 0, new QTableWidgetItem(instructions[i].opcode));
        tableWidget->setItem(i, 1, new QTableWidgetItem(instructions[i].arg1));
        tableWidget->setItem(i, 2, new QTableWidgetItem(instructions[i].arg2));
        tableWidget->setItem(i, 3, new QTableWidgetItem(instructions[i].arg3));
        tableWidget->setItem(i, 4, new QTableWidgetItem(instructions[i].immRange));
        tableWidget->setItem(i, 5, new QTableWidgetItem(instructions[i].description));
        tableWidget->setItem(i, 6, new QTableWidgetItem(instructions[i].example));
    }

    tableWidget->resizeColumnsToContents();
}

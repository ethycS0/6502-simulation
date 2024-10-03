#pragma once
#include <cstdint>
#include <sys/types.h>
#include <vector>

// Required For Disassembly
#include <string>
#include <map>

// Required For Emulation Behviour Logging
#ifndef LOGMODE
#include <stdio.h>
#endif

class Bus;

class cpu6502 {
public:
    // Constructor Deconstructor
    cpu6502();
    ~cpu6502();

    // Core Registers
    uint8_t a = 0x00;
    uint8_t x = 0x00;
    uint8_t y = 0x00;
    uint8_t stkp = 0x00;
    uint16_t pc = 0x0000;
    uint8_t status = 0x00;

    // External Event Funtions
    void reset();
    void irq();
    void nmi();
    void clock();

    bool complete();

    // Bus Connection
    void connectBus(Bus *n) {bus = n;}
    
    // Generates a map of instruction addresses to disassembled instructions within a memory range    
    std::map<uint16_t, std::string> disassemble(uint16_t nStart, uint16_t nsStop);

    // Status Register Flags
    enum flags6502 {
        C = (1 << 0),       // Carry
        Z = (1 << 1),       // Zero
        I = (1 << 2),       // Disable Interrupts
        D = (1 << 3),       // Decimal Mode
        B = (1 << 4),       // Break  
        U = (1 << 5),       // Unused
        V = (1 << 6),       // Overflow
        N = (1 << 7),       // Negative
    };

private:

    // Status Flag Access Functions
    uint8_t getFlag(flags6502 f);
    void setFlag(flags6502 f, bool v);

    // Assistive Variables
    uint16_t absAdd = 0x0000;
    uint16_t relAdd = 0x0000;
    uint16_t temp = 0x0000;
    uint8_t fetched = 0x00;     // Working Input Value of the ALU
    uint8_t opcode = 0x00;
    uint8_t cycles = 0;
    uint32_t clock_count = 0;

    // Communication Bus Linkage
    Bus *bus = nullptr;
    uint8_t read(uint16_t a);
    void write(uint16_t a, uint8_t d);
    uint8_t fetch();    // Reading of data based on address mode of the instruction byte
    

    // Opcode Translation Table
    struct Instruction {
        std::string name;
        uint8_t(cpu6502::*operate)(void) = nullptr;
        uint8_t(cpu6502::*addrmode)(void) = nullptr;
        uint8_t cycles = 0;
    };
    std::vector<Instruction> lookup;

    // Addressing Modes
    uint8_t IMP();      // Implied Addressing
    uint8_t IMM();      // Immediate Addressing
    uint8_t ZP0();      // Zero Page Addressing
    uint8_t ZPX();      // Zero Page Addressing X Offset
    uint8_t ZPY();      // Zero Page Addressing Y Offset
    uint8_t REL();      // Relative Addressing
    uint8_t ABS();      // Absolute Addressing
    uint8_t ABX();      // Absolute Addressing X Offset
    uint8_t ABY();      // Absolute Addressing Y Offset
    uint8_t IND();      // Indirect Addressing
    uint8_t IZX();      // Indirect Addressing X Offset 
    uint8_t IZY();      // Indirect Addressing Y Offset
    
    // Opcodes
    // Legal Opcodes
    uint8_t ADC();      // Add Memory to Accumulator with Carry
    uint8_t AND();      // AND Memory with Accumulator
    uint8_t ASL();      // Shift Left One bit (Memory or Accumulator)
    uint8_t BCC();      // Branch On Carry Clear
    uint8_t BCS();      // Branch on Carry Set
    uint8_t BEQ();      // Branch on Result Zero
    uint8_t BIT();      // Test Bits in Memory with Accumulator
    uint8_t BMI();      // Branch on Result Minux
    uint8_t BNE();      // Branch on Result not Zero
    uint8_t BPL();      // Branch on Result Plus
    uint8_t BRK();      // Force Break
    uint8_t BVC();      // Branch on Overflow Clear
    uint8_t BVS();      // Branch on Overflow Set
    uint8_t CLC();      // Clear Carry Flag
    uint8_t CLD();      // Clear Decimal Mode
    uint8_t CLI();      // Clear Interrupt Disable Bit
    uint8_t CLV();      // Clear Overflow Flag
    uint8_t CMP();      // Compare Memory and Accumulator
    uint8_t CPX();      // Compare memory and Index X
    uint8_t CPY();      // Compare Memory and Index Y
    uint8_t DEC();      // Decrement Memory By One
    uint8_t DEX();      // Decrement Index X by One
    uint8_t DEY();      // Decrement Index Y by One
    uint8_t EOR();      // Exclusive-OR Memory with Accumulator
    uint8_t INC();      // Increment Memory by One
    uint8_t INX();      // Increment INdex X by One
    uint8_t INY();      // Increment Index Y by One
    uint8_t JMP();      // Jump New Location
    uint8_t JSR();      // Jump to New Location Saving Return Address
    uint8_t LDA();      // Load Accumulator with Memory
    uint8_t LDX();      // Load Index X with Memory
    uint8_t LDY();      // Load Index Y with Memory
    uint8_t LSR();      // Shift One Bit Right (Memory or Accumulator) 
    uint8_t NOP();      // No Operation
    uint8_t ORA();      // OR Memory with Accumulator
    uint8_t PHA();      // Push Accumulator on Stack
    uint8_t PHP();      // Push Processor Status on Stack
    uint8_t PLA();      // Pull Accumulator from Stack
    uint8_t PLP();      // Pull Processor Status from Stack
    uint8_t ROL();      // Rotate One Bit left (Memory or Accumulator)
    uint8_t ROR();      // Rotate One Bit right (Memory or Accumulator)
    uint8_t RTI();      // Return from Interrupt
    uint8_t RTS();      // Return from Subroutine
    uint8_t SBC();      // Subtract memory from Accumulator with Borrow
    uint8_t SEC();      // Set Carry Flag
    uint8_t SED();      // Set Decimal Mode
    uint8_t SEI();      // Set Interrupt Disable Status
    uint8_t STA();      // Store Accumulator in Memory
    uint8_t STX();      // Store Index X in Memory
    uint8_t STY();      // Store Index Y in Memory
    uint8_t TAX();      // Transfer Accumulator to Index X
    uint8_t TAY();      // Transfer Accumulator to Index Y
    uint8_t TSX();      // Transfer Stack Pointer to Index X
    uint8_t TXA();      // Transfer Index X to Accumulator
    uint8_t TXS();      // Transfer Index X to Stack Register
    uint8_t TYA();      // Trnsfer Index Y to Accumulator
    // Illegal Opcodes
    uint8_t XXX();      // Undefined Opcode

#ifndef LOGMODE
private:
    FILE* logfile = nullptr;
    
#endif // !LOGMODE    
};

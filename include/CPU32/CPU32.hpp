//
// Created by John on 6/3/2024.
//
#include <CPUComponent.hpp>
#include <CPU32/Register32.hpp>
#include <CPU32/Memory32.hpp>
#include <CPU32/Clock32.hpp>
#include <CPU32/ALU32.hpp>
#include <CPU32/Flags32.hpp>

#include <memory>
#include <vector>
#include <map>

#ifndef CPUSIMULATOR_CPU32_HPP
#define CPUSIMULATOR_CPU32_HPP


class CPU32 : public CPUComponent {
public:
    CPU32(size_t memorySize);
    void loadInstruction(uint32_t instruction, uint32_t immediate = 0);
    void tickClock();
    void run();
    void loadProgram(const std::vector<uint32_t>& program, uint32_t startAddress);

    std::vector<std::shared_ptr<Register32>> GetRegisters() const;
    std::shared_ptr<Register32> GetProgramCounter() const;
    std::shared_ptr<Memory32> GetMemory() const;
    uint32_t GetState() const override;
    void Update(uint32_t state) override;

    bool GetZeroFlag() const;
    void SetZeroFlag(bool zFlag);
    bool halted = false;

private:
    void fetch();
    void decodeExecute();

    void nop();
    void movRegisterToRegister();
    void movImmediateToRegister();
    void movImmediate32ToRegister();
    void load();
    void store();
    void add();
    void sub();
    void andOp();
    void orOp();
    void xorOp();
    void notOp();
    void cmpImmediateToRegister();
    void cmpRegisterToRegister();
    void jmp();
    void jz();
    void jnz();
    void jl();
    void jg();
    void jle();
    void jge();
    void call();
    void ret();
    void inOp();
    void outOp();
    void hlt();

    uint32_t instruction;
    uint32_t immediateOperand;
    uint32_t returnAddress;

    std::shared_ptr<Clock32> clock;
    std::shared_ptr<Register32> programCounter;
    std::shared_ptr<ALU32> alu;
    std::shared_ptr<Flags32> flagsRegister;
    std::vector<std::shared_ptr<Register32>> registers;
    std::map<uint32_t, void (CPU32::*)()> opcodeMap;
    std::shared_ptr<Memory32> memory;
};

#endif //CPUSIMULATOR_CPU32_HPP
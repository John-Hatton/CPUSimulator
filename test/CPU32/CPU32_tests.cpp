#include <gtest/gtest.h>
#include <CPU32/CPU32.hpp>

class CPU32Test : public ::testing::Test {
protected:
    CPU32* cpu;

    void SetUp() override {
        cpu = new CPU32(1024);
    }

    void TearDown() override {
        delete cpu;
    }
};

TEST_F(CPU32Test, LoadAndFetchInstruction) {
    std::vector<uint32_t> program = {0x01020100, 0x01030200}; // MOV R2, R1; MOV R3, R2
    cpu->loadProgram(program, 0);
    cpu->tickClock();
    EXPECT_EQ(cpu->GetProgramCounter()->GetState(), 1);
    cpu->tickClock();
    EXPECT_EQ(cpu->GetProgramCounter()->GetState(), 2);
}

TEST_F(CPU32Test, ExecuteAddInstruction) {
    std::vector<uint32_t> program = {0x05010200, 0xFF000000}; // ADD R1, R2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(10);
    cpu->GetRegisters()[2]->loadValue(20);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 30);
    EXPECT_EQ(cpu->GetZeroFlag(), false);
}

TEST_F(CPU32Test, ExecuteSubInstruction) {
    std::vector<uint32_t> program = {0x06010200, 0xFF000000}; // SUB R1, R2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(30);
    cpu->GetRegisters()[2]->loadValue(10);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 20);
    EXPECT_EQ(cpu->GetZeroFlag(), false);
}

TEST_F(CPU32Test, ZeroFlag) {
    std::vector<uint32_t> program = {0x06010200, 0xFF000000}; // SUB R1, R2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(10);
    cpu->GetRegisters()[2]->loadValue(10);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 0);
    EXPECT_EQ(cpu->GetZeroFlag(), true);
}

TEST_F(CPU32Test, HaltInstruction) {
    std::vector<uint32_t> program = {0xFF000000}; // HLT
    cpu->loadProgram(program, 0);
    cpu->run();
    EXPECT_EQ(cpu->halted, true);
}

TEST_F(CPU32Test, LoadImmediate) {
    std::vector<uint32_t> program = {0x02011111, 0xFF000000}; // MOV R1, 0x1111
    cpu->loadProgram(program, 0);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 0x1111);
}

TEST_F(CPU32Test, LoadImmediate32) {
    std::vector<uint32_t> program = {0xE201FFFF, 0x12345678, 0xFF000000}; // MOV R1, 0x12345678
    cpu->loadProgram(program, 0);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 0x12345678);
}

TEST_F(CPU32Test, ExecuteAndInstruction) {
    std::vector<uint32_t> program = {0x07010200, 0xFF000000}; // AND R1, R2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(0xFF00FF00);
    cpu->GetRegisters()[2]->loadValue(0x00FF00FF);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 0x00000000);
    EXPECT_EQ(cpu->GetZeroFlag(), true);
}

TEST_F(CPU32Test, ExecuteOrInstruction) {
    std::vector<uint32_t> program = {0x08010200, 0xFF000000}; // OR R1, R2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(0xFF00FF00);
    cpu->GetRegisters()[2]->loadValue(0x00FF00FF);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 0xFFFFFFFF);
    EXPECT_EQ(cpu->GetZeroFlag(), false);
}

TEST_F(CPU32Test, ExecuteXorInstruction) {
    std::vector<uint32_t> program = {0x09010200, 0xFF000000}; // XOR R1, R2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(0xFF00FF00);
    cpu->GetRegisters()[2]->loadValue(0x00FF00FF);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 0xFFFFFFFF);
    EXPECT_EQ(cpu->GetZeroFlag(), false);
}

TEST_F(CPU32Test, ExecuteNotInstruction) {
    std::vector<uint32_t> program = {0x0A010000, 0xFF000000}; // NOT R1
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(0x0000FFFF);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 0xFFFF0000);
    EXPECT_EQ(cpu->GetZeroFlag(), false);
}

TEST_F(CPU32Test, ExecuteCmpImmediateToRegisterInstruction) {
    std::vector<uint32_t> program = {0x10010001, 0xFF000000}; // CMP R1, 1
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(1);
    cpu->run();
    EXPECT_EQ(cpu->GetZeroFlag(), true);
}

TEST_F(CPU32Test, ExecuteCmpRegisterToRegisterInstruction) {
    std::vector<uint32_t> program = {0x11010200, 0xFF000000}; // CMP R1, R2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(1);
    cpu->GetRegisters()[2]->loadValue(1);
    cpu->run();
    EXPECT_EQ(cpu->GetZeroFlag(), true);
}

TEST_F(CPU32Test, ExecuteJmpInstruction) {
    std::vector<uint32_t> program = {0x12000005}; // JMP 5
    cpu->loadProgram(program, 0);
    cpu->GetMemory()->store(0x5, 0xFF000000);
    cpu->run();
    EXPECT_EQ(cpu->GetProgramCounter()->GetState(), 6);
}

TEST_F(CPU32Test, ExecuteJzInstruction) {
    std::vector<uint32_t> program = {0x06010200, 0x13000002}; // SUB R1, R2; JZ 2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(10);
    cpu->GetRegisters()[2]->loadValue(10);
    cpu->GetMemory()->store(2, 0xFF000000); // Store HLT at address 2
    cpu->run();
    EXPECT_EQ(cpu->GetProgramCounter()->GetState(), 3);
    EXPECT_EQ(cpu->halted, true);
}

TEST_F(CPU32Test, ExecuteJnzInstruction) {
    std::vector<uint32_t> program = {0x06010200, 0x14000002}; // SUB R1, R2; JNZ 2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(10);
    cpu->GetRegisters()[2]->loadValue(5);
    cpu->GetMemory()->store(2, 0xFF000000); // Store HLT at address 2
    cpu->run();
    EXPECT_EQ(cpu->GetProgramCounter()->GetState(), 3);
    EXPECT_EQ(cpu->halted, true);
}

TEST_F(CPU32Test, ExecuteJlInstruction) {
    std::vector<uint32_t> program = {0x11010200, 0x15000002}; // CMP R1, R2; JL 2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(1);
    cpu->GetRegisters()[2]->loadValue(10);
    cpu->GetMemory()->store(2, 0xFF000000); // Store HLT at address 2
    cpu->run();
    EXPECT_EQ(cpu->GetProgramCounter()->GetState(), 3);
    EXPECT_EQ(cpu->halted, true);
}

TEST_F(CPU32Test, ExecuteJgInstruction) {
    std::vector<uint32_t> program = {0x11010200, 0x16000002}; // CMP R1, R2; JG 2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(10);
    cpu->GetRegisters()[2]->loadValue(1);
    cpu->GetMemory()->store(2, 0xFF000000); // Store HLT at address 2
    cpu->run();
    EXPECT_EQ(cpu->GetProgramCounter()->GetState(), 3);
    EXPECT_EQ(cpu->halted, true);
}

TEST_F(CPU32Test, ExecuteJleInstruction) {
    std::vector<uint32_t> program = {0x11010200, 0x17000002}; // CMP R1, R2; JLE 2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(1);
    cpu->GetRegisters()[2]->loadValue(10);
    cpu->GetMemory()->store(2, 0xFF000000); // Store HLT at address 2
    cpu->run();
    EXPECT_EQ(cpu->GetProgramCounter()->GetState(), 3);
    EXPECT_EQ(cpu->halted, true);
}

TEST_F(CPU32Test, ExecuteJgeInstruction) {
    std::vector<uint32_t> program = {0x11010200, 0x18000002}; // CMP R1, R2; JGE 2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(10);
    cpu->GetRegisters()[2]->loadValue(1);
    cpu->GetMemory()->store(2, 0xFF000000); // Store HLT at address 2
    cpu->run();
    EXPECT_EQ(cpu->GetProgramCounter()->GetState(), 3);
    EXPECT_EQ(cpu->halted, true);
}

TEST_F(CPU32Test, ExecuteCallAndRetInstruction) {
    std::vector<uint32_t> program = {0x30000002, 0xFF000000, 0x31000000}; // CALL 2; HLT; RET
    cpu->loadProgram(program, 0);
    cpu->run();
    EXPECT_EQ(cpu->GetProgramCounter()->GetState(), 2);
//    cpu->tickClock(); // Simulate RET
//    EXPECT_EQ(cpu->GetProgramCounter()->GetState(), 1);
}

TEST_F(CPU32Test, LoadInstructionWithImmediateValue) {
    std::vector<uint32_t> program = {0xE201FFFF, 0x00001111, 0xFF000000}; // MOV R1, 0x1111
    cpu->loadProgram(program, 0);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 0x1111);
}

TEST_F(CPU32Test, ExecuteLoadInstruction) {
    std::vector<uint32_t> program = {0x03010200, 0xFF000000}; // LOAD R1, R2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[2]->loadValue(100);
    cpu->GetMemory()->store(100, 0x12345678);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 0x12345678);
}

TEST_F(CPU32Test, ExecuteStoreInstruction) {
    std::vector<uint32_t> program = {0x04010200, 0xFF000000}; // STORE R1, R2
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(0x12345678);
    cpu->GetRegisters()[2]->loadValue(100);
    cpu->run();
    EXPECT_EQ(cpu->GetMemory()->load(100), 0x12345678);
}

TEST_F(CPU32Test, ExecuteInInstruction) {
    // This test will need to be modified to handle actual input values if implemented
    std::vector<uint32_t> program = {0x40010000, 0xFF000000}; // IN R1
    cpu->loadProgram(program, 0);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 0); // Expecting 0 as placeholder
}

TEST_F(CPU32Test, ExecuteOutInstruction) {
    // This test will need to be modified to handle actual output values if implemented
    std::vector<uint32_t> program = {0x41010000, 0xFF000000}; // OUT R1
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(0x12345678);
    cpu->run();
    // No state to check as OUT is placeholder
}

TEST_F(CPU32Test, InvalidInstruction) {
    std::vector<uint32_t> program = {0xDEADBEEF}; // Invalid instruction
    cpu->loadProgram(program, 0);
    cpu->run();
    EXPECT_EQ(cpu->halted, true);
}

TEST_F(CPU32Test, ImmediateValueEdgeCase) {
    std::vector<uint32_t> program = {0xE201FFFF, 0xFFFFFFFF, 0xFF000000}; // MOV R1, 0xFFFFFFFF
    cpu->loadProgram(program, 0);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 0xFFFFFFFF);
}

TEST_F(CPU32Test, StoreImmediate32) {
    auto expected = 0x05010400;
    std::vector<uint32_t> program = {0xE4FFFFFF, 0x000000A0, 0x05010400, 0xFF000000}; // STORE 0x000000A0, 0x05010400 (Store the immediate value at address 0xA0)
    cpu->loadProgram(program, 0);
    cpu->run();
    auto actual = cpu->GetMemory()->load(0xA0);
    EXPECT_EQ(expected, actual);
}

TEST_F(CPU32Test, AddImmediate16) {
    std::vector<uint32_t> program = {0xE5010001, 0xFF000000}; // ADD R1, 1
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(10);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[1]->GetState(), 11);
    EXPECT_EQ(cpu->GetZeroFlag(), false);
}

// Tests for Stack Operations
TEST_F(CPU32Test, PushAndPop) {
    std::vector<uint32_t> program = {
            0x32010000, // PUSH R1
            0x32020000, // PUSH R2
            0x33030000, // POP R3
            0x33040000, // POP R4
            0xFF000000  // HALT
    };
    cpu->loadProgram(program, 0);
    cpu->GetRegisters()[1]->loadValue(0x12345678);
    cpu->GetRegisters()[2]->loadValue(0x9ABCDEF0);
    cpu->run();
    EXPECT_EQ(cpu->GetRegisters()[3]->GetState(), 0x9ABCDEF0);
    EXPECT_EQ(cpu->GetRegisters()[4]->GetState(), 0x12345678);
    EXPECT_EQ(cpu->GetStackPointer()->GetState(), cpu->GetMemory()->getSize()); // Stack should be back at the initial position
}

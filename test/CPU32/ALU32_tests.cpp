//
// Created by John on 6/3/2024.
//
#include <gtest/gtest.h>
#include <CPU32/ALU32.hpp>
class ALU32Test : public ::testing::Test {
protected:
    ALU32 alu;

    void SetUp() override {
        // Initialize ALU
    }
};

TEST_F(ALU32Test, AddOperation) {
    alu.setInputs(10, 20);
    alu.setOperation(ALU32::ADD);
    EXPECT_EQ(alu.GetState(), 30);
}

TEST_F(ALU32Test, SubOperation) {
    alu.setInputs(30, 10);
    alu.setOperation(ALU32::SUB);
    EXPECT_EQ(alu.GetState(), 20);
}

TEST_F(ALU32Test, AndOperation) {
    alu.setInputs(0b1100, 0b1010);
    alu.setOperation(ALU32::AND);
    EXPECT_EQ(alu.GetState(), 0b1000);
}

TEST_F(ALU32Test, OrOperation) {
    alu.setInputs(0b1100, 0b1010);
    alu.setOperation(ALU32::OR);
    EXPECT_EQ(alu.GetState(), 0b1110);
}

TEST_F(ALU32Test, XorOperation) {
    alu.setInputs(0b1100, 0b1010);
    alu.setOperation(ALU32::XOR);
    EXPECT_EQ(alu.GetState(), 0b0110);
}

TEST_F(ALU32Test, NotOperation) {
    alu.setInputs(0b1100, 0);
    alu.setOperation(ALU32::NOT);
    EXPECT_EQ(alu.GetState(), ~0b1100);
}

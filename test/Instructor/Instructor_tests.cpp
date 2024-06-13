//
// Created by John on 6/6/2024.
//
#include <gtest/gtest.h>
#include <Instructor/Instructor.hpp>
#include <gtest/gtest.h>

class InstructorTest : public ::testing::Test {
protected:
    Instructor instructor;

    void SetUp() override {}

    void TearDown() override {}
};

// Test for simple MOV, ADD, CMP, and JLE instructions with a label
TEST_F(InstructorTest, AssembleSimpleProgram) {
    std::string code = R"(
        MOV r1, 1
        ADD r2, 1
        CMP r2, 9
        JLE addOne
        HLT
        addOne:
        ADD r1, 1
        ADD r2, 1
    )";

    std::vector<uint32_t> expectedInstructions = {
            0x02010001, 0xE5020001, 0x10020009, 0x17000005, 0xFF000000, 0xE5010001, 0xE5020001
    };

    std::vector<uint32_t> actualInstructions = instructor.assemble(code);

    EXPECT_EQ(expectedInstructions.size(), actualInstructions.size());
    for (size_t i = 0; i < expectedInstructions.size(); ++i) {
        std::cout << "Expected: " << std::hex << std::setw(8) << std::setfill('0') << expectedInstructions[i]
                  << ", Actual: " << std::hex << std::setw(8) << std::setfill('0') << actualInstructions[i] << std::endl;
        EXPECT_EQ(expectedInstructions[i], actualInstructions[i]);
    }
}

// Test for MOV with register and immediate (16-bit) values
TEST_F(InstructorTest, AssembleMovImmediate16) {
    std::string code = R"(
        MOV r1, 0x1234
        MOV r2, 42
    )";

    std::vector<uint32_t> expectedInstructions = {
            0x02011234, 0x0202002A
    };

    std::vector<uint32_t> actualInstructions = instructor.assemble(code);

    EXPECT_EQ(expectedInstructions.size(), actualInstructions.size());
    for (size_t i = 0; i < expectedInstructions.size(); ++i) {
        std::cout << "Expected: " << std::hex << std::setw(8) << std::setfill('0') << expectedInstructions[i]
                  << ", Actual: " << std::hex << std::setw(8) << std::setfill('0') << actualInstructions[i] << std::endl;
        EXPECT_EQ(expectedInstructions[i], actualInstructions[i]);
    }
}

// Test for MOV with immediate (32-bit) values
TEST_F(InstructorTest, AssembleMovImmediate32) {
    std::string code = R"(
        MOV r1, 0x12345678
        MOV r2, 0x9ABCDEF0
    )";

    std::vector<uint32_t> expectedInstructions = {
            0xE201FFFF, 0x12345678, 0xE202FFFF, 0x9ABCDEF0
    };

    std::vector<uint32_t> actualInstructions = instructor.assemble(code);

    EXPECT_EQ(expectedInstructions.size(), actualInstructions.size());
    for (size_t i = 0; i < expectedInstructions.size(); ++i) {
        std::cout << "Expected: " << std::hex << std::setw(8) << std::setfill('0') << expectedInstructions[i]
                  << ", Actual: " << std::hex << std::setw(8) << std::setfill('0') << actualInstructions[i] << std::endl;
        EXPECT_EQ(expectedInstructions[i], actualInstructions[i]);
    }
}

// Test for simple arithmetic operations (ADD, SUB)
TEST_F(InstructorTest, AssembleArithmeticOperations) {
    std::string code = R"(
        ADD r1, r2
        SUB r3, r4
    )";

    std::vector<uint32_t> expectedInstructions = {
            0x05010200, 0x06030400
    };

    std::vector<uint32_t> actualInstructions = instructor.assemble(code);

    EXPECT_EQ(expectedInstructions.size(), actualInstructions.size());
    for (size_t i = 0; i < expectedInstructions.size(); ++i) {
        std::cout << "Expected: " << std::hex << std::setw(8) << std::setfill('0') << expectedInstructions[i]
                  << ", Actual: " << std::hex << std::setw(8) << std::setfill('0') << actualInstructions[i] << std::endl;
        EXPECT_EQ(expectedInstructions[i], actualInstructions[i]);
    }
}

// Test for logical operations (AND, OR, XOR, NOT)
TEST_F(InstructorTest, AssembleLogicalOperations) {
    std::string code = R"(
        AND r1, r2
        OR r3, r4
        XOR r5, r6
        NOT r7
    )";

    std::vector<uint32_t> expectedInstructions = {
            0x07010200, 0x08030400, 0x09050600, 0x0A070000
    };

    std::vector<uint32_t> actualInstructions = instructor.assemble(code);

    EXPECT_EQ(expectedInstructions.size(), actualInstructions.size());
    for (size_t i = 0; i < expectedInstructions.size(); ++i) {
        std::cout << "Expected: " << std::hex << std::setw(8) << std::setfill('0') << expectedInstructions[i]
                  << ", Actual: " << std::hex << std::setw(8) << std::setfill('0') << actualInstructions[i] << std::endl;
        EXPECT_EQ(expectedInstructions[i], actualInstructions[i]);
    }
}

// Test for jump instructions with labels
TEST_F(InstructorTest, AssembleJumpInstructions) {
    std::string code = R"(
        JMP label
        label:
        HLT
    )";

    std::vector<uint32_t> expectedInstructions = {
            0x12000001, 0xFF000000
    };

    std::vector<uint32_t> actualInstructions = instructor.assemble(code);

    EXPECT_EQ(expectedInstructions.size(), actualInstructions.size());
    for (size_t i = 0; i < expectedInstructions.size(); ++i) {
        std::cout << "Expected: " << std::hex << std::setw(8) << std::setfill('0') << expectedInstructions[i]
                  << ", Actual: " << std::hex << std::setw(8) << std::setfill('0') << actualInstructions[i] << std::endl;
        EXPECT_EQ(expectedInstructions[i], actualInstructions[i]);
    }
}

// Test for store immediate 32-bit values
TEST_F(InstructorTest, AssembleStoreImmediate32) {
    std::string code = R"(
        STORE 0x12345678, 0x9ABCDEF0
    )";

    std::vector<uint32_t> expectedInstructions = {
            0xE4FFFFFF, 0x12345678, 0x9ABCDEF0
    };

    std::vector<uint32_t> actualInstructions = instructor.assemble(code);

    EXPECT_EQ(expectedInstructions.size(), actualInstructions.size());
    for (size_t i = 0; i < expectedInstructions.size(); ++i) {
        std::cout << "Expected: " << std::hex << std::setw(8) << std::setfill('0') << expectedInstructions[i]
                  << ", Actual: " << std::hex << std::setw(8) << std::setfill('0') << actualInstructions[i] << std::endl;
        EXPECT_EQ(expectedInstructions[i], actualInstructions[i]);
    }
}

// Test for handling invalid instructions
TEST_F(InstructorTest, AssembleInvalidInstruction) {
    std::string code = R"(
        INVALID r1, r2
    )";

    EXPECT_THROW({
                     instructor.assemble(code);
                 }, std::runtime_error);
}
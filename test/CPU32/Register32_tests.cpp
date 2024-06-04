//
// Created by John on 6/3/2024.
//
#include <gtest/gtest.h>
#include <CPU32/Register32.hpp>

class Register32Test : public ::testing::Test {
protected:
    Register32* reg = new Register32();

    void SetUp() override {
        // Initialize Register
        reg->loadValue(0);
    }
};

TEST_F(Register32Test, InitialState) {
    EXPECT_EQ(reg->GetState(), 0);
}

TEST_F(Register32Test, LoadValue) {
    reg->loadValue(42);
    EXPECT_EQ(reg->GetState(), 42);
}

TEST_F(Register32Test, LoadMaxValue) {
    reg->loadValue(0xFFFFFFFF);
    EXPECT_EQ(reg->GetState(), 4294967295);
}
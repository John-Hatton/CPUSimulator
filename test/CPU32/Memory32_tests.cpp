//
// Created by John on 6/3/2024.
//
#include <gtest/gtest.h>
#include <CPU32/Memory32.hpp>

class Memory32Test : public ::testing::Test {
protected:
    Memory32* memory;

    void SetUp() override {
        memory = new Memory32(1024);
    }
};

TEST_F(Memory32Test, StoreAndLoad) {
    memory->store(100, 42);
    EXPECT_EQ(memory->load(100), 42);
}

TEST_F(Memory32Test, OutOfBoundsAccess) {
    EXPECT_THROW(memory->load(2048), std::out_of_range);
    EXPECT_THROW(memory->store(2048, 42), std::out_of_range);
}

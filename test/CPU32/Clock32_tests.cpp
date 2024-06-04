//
// Created by John on 6/3/2024.
//
#include <gtest/gtest.h>
#include <CPU32/Clock32.hpp>

class Clock32Test : public ::testing::Test {
protected:
    Clock32* clock;

    void SetUp() override {
        clock = new Clock32(1);
    }
};

TEST_F(Clock32Test, InitialState) {
    EXPECT_EQ(clock->GetState(), false);
}

TEST_F(Clock32Test, Tick) {
    clock->tick();
    EXPECT_EQ(clock->GetState(), true);
    clock->tick();
    EXPECT_EQ(clock->GetState(), false);
}

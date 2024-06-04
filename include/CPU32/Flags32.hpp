//
// Created by John on 6/4/2024.
//
#include <CPU32/Register32.hpp>

#ifndef CPUSIMULATOR_FLAGS32_HPP
#define CPUSIMULATOR_FLAGS32_HPP


class Flags32 : public Register32 {
public:
    enum Flags {
        CARRY = 1 << 0,
        ZERO = 1 << 1,
        INTERRUPT = 1 << 2,
        SIGN = 1 << 3,
        OVERFLOW = 1 << 4,
        PARITY = 1 << 5
    };

    Flags32() : Register32() {}

    void setFlag(Flags flag) { state |= flag; }
    void clearFlag(Flags flag) { state &= ~flag; }
    bool isFlagSet(Flags flag) const { return state & flag; }
    uint32_t getFlags() const { return state; }

    // Override loadValue to ensure flags are set correctly
    // No need to override, because we already have this function in Register32.
//    void loadValue(uint32_t value) override {
//        state = value;
//    }
};


#endif //CPUSIMULATOR_FLAGS32_HPP

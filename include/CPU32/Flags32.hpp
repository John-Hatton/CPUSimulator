//
// Created by John on 6/4/2024.
//
#include <CPU32/Register32.hpp>
#include <sstream>
#include <utility>

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

    void setFlag(Flags flag) {
        state |= flag;
        updateFlagNames();
    }

    void clearFlag(Flags flag) {
        state &= ~flag;
        updateFlagNames();
    }

    bool isFlagSet(Flags flag) const {
        return state & flag;
    }

    uint32_t getFlags() const {
        return state;
    }

    std::string getFlagNames() const {
        return collectionOfFlags.empty() ? "None" : collectionOfFlags;
    }

private:
    std::string collectionOfFlags;

    void updateFlagNames() {
        std::ostringstream ss;
        if (isFlagSet(CARRY)) ss << "CARRY ";
        if (isFlagSet(ZERO)) ss << "ZERO ";
        if (isFlagSet(INTERRUPT)) ss << "INTERRUPT ";
        if (isFlagSet(SIGN)) ss << "SIGN ";
        if (isFlagSet(OVERFLOW)) ss << "OVERFLOW ";
        if (isFlagSet(PARITY)) ss << "PARITY ";
        collectionOfFlags = ss.str();
        if (!collectionOfFlags.empty()) {
            collectionOfFlags.pop_back(); // Remove trailing space
        }
    }
};


#endif //CPUSIMULATOR_FLAGS32_HPP

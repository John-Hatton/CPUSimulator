//
// Created by John on 6/3/2024.
//
#include <CPUComponent.hpp>

#ifndef CPUSIMULATOR_CLOCK32_HPP
#define CPUSIMULATOR_CLOCK32_HPP

class Clock32 : public CPUComponent {
public:
    Clock32(int frequency) : frequency(frequency), state(false) {}

    void tick() {
        state = !state;
        CPUComponent::Notify();
    }

    void Update(uint32_t state) override {
        Notify();
    }

    uint32_t GetState() const override {
        return state;
    }

private:
    int frequency;
    bool state;
};



#endif //CPUSIMULATOR_CLOCK32_HPP

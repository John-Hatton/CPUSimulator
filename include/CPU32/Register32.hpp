//
// Created by John on 6/3/2024.
//
#include <CPUComponent.hpp>

#ifndef CPUSIMULATOR_REGISTER32_HPP
#define CPUSIMULATOR_REGISTER32_HPP


class Register32 : public CPUComponent {
public:
    void loadValue(uint32_t value) {
        state = value;
        CPUComponent::Notify();
    }

    uint32_t GetState() const override {

        return state;
    }

    void Update(uint32_t state) override {
        Notify();
    }

protected:
    uint32_t state = 0;
};


#endif //CPUSIMULATOR_REGISTER32_HPP

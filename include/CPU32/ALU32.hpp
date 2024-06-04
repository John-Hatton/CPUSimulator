//
// Created by John on 6/3/2024.
//
#include <CPUComponent.hpp>
#include <cstdint>

#ifndef CPUSIMULATOR_ALU32_HPP
#define CPUSIMULATOR_ALU32_HPP

// setup observer interface?

class ALU32 : public CPUComponent {
public:
    enum Operation { ADD, SUB, AND, OR, XOR, NOT };

    void setInputs(uint32_t a, uint32_t b) {
        this->a = a;
        this->b = b;
        Update(0); // Note we don't need any value here.
    }

    void setOperation(Operation op) {
        this->op = op;
        Update(0); // Note we don't need any value here.
    }

    // rename
    void Update(uint32_t) override {
        switch (op) {
            case ADD:
                result = a + b;
                break;
            case SUB:
                result = a - b;
                break;
            case AND:
                result = a & b;
                break;
            case OR:
                result = a | b;
                break;
            case XOR:
                result = a ^ b;
                break;
            case NOT:
                result = ~a;
                break;
        }
        Notify();
    }

    uint32_t GetState() const override {
        return result;
    }

private:
    uint32_t a, b;
    uint32_t result;
    Operation op;
};


#endif //CPUSIMULATOR_ALU32_HPP

//
// Created by John on 6/3/2024.
//
#include <cstdint>
#include <vector>
#include <stdexcept>

#ifndef CPUSIMULATOR_MEMORY32_HPP
#define CPUSIMULATOR_MEMORY32_HPP


class Memory32 {
public:
    Memory32(size_t size) : memory(size, 0) {}

    uint32_t load(uint32_t address) const {
        if (address < memory.size()) {
            return memory[address];
        } else {
            throw std::out_of_range("Memory access out of bounds");
        }
    }

    void store(uint32_t address, uint32_t value) {
        if (address < memory.size()) {
            memory[address] = value;
        } else {
            throw std::out_of_range("Memory access out of bounds");
        }
    }

private:
    std::vector<uint32_t> memory;
};


#endif //CPUSIMULATOR_MEMORY32_HPP

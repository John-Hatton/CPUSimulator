//
// Created by John on 6/6/2024.
//
#include <Instructor/Instructor.hpp>

// Instructor.cpp
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>

Instructor::Instructor() {
    initializeOpcodeMap();
}

std::vector<std::string> Instructor::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

uint32_t Instructor::parseRegister(const std::string& reg) {
    if (reg[0] != 'r' && reg[0] != 'R') {
        throw std::runtime_error("Invalid register format");
    }
    return std::stoi(reg.substr(1));
}

uint32_t Instructor::parseImmediate(const std::string& imm, int bitSize) {
    uint32_t value;
    std::stringstream ss;

    if (imm.find("0x") == 0 || imm.find("0X") == 0) {
        ss << std::hex << imm.substr(2);
    } else {
        ss << std::dec << imm;
    }

    ss >> value;
    if (bitSize == 16 && value > 0xFFFF) {
        throw std::runtime_error("Immediate value out of range for 16-bit");
    }
    if (bitSize == 32 && value > 0xFFFFFFFF) {
        throw std::runtime_error("Immediate value out of range for 32-bit");
    }
    return value;
}

uint32_t Instructor::getOpcode(const std::vector<std::string>& tokens) {
    std::string mnemonic = tokens[0];
    if (mnemonic == "MOV") {
        if (tokens.size() == 3) {
            uint32_t immValue = parseImmediate(tokens[2], 32);
            return (immValue > 0xFFFF) ? opcodeMap["MOV_IMM32"] : opcodeMap["MOV_IMM16"];
        } else {
            return opcodeMap["MOV_REG"];
        }
    } else if (mnemonic == "CMP") {
        if (tokens.size() == 3 && std::all_of(tokens[2].begin(), tokens[2].end(), ::isdigit)) {
            return opcodeMap["CMP_IMM16"];
        } else {
            return opcodeMap["CMP_REG"];
        }
    } else if (mnemonic == "ADD" && tokens.size() == 3 && std::all_of(tokens[2].begin(), tokens[2].end(), ::isdigit)) {
        return opcodeMap["ADD_IMM16"];
    } else if (mnemonic == "STORE" && tokens.size() == 3) {
        uint32_t immValue = parseImmediate(tokens[1], 32);
        return (immValue > 0xFFFF) ? opcodeMap["STORE_IMM32"] : opcodeMap["STORE"];
    } else {
        return opcodeMap[mnemonic];
    }
}

bool Instructor::isLabel(const std::string& token) {
    return std::all_of(token.begin(), token.end(), ::isalnum) && !std::isdigit(token[0]);
}

void Instructor::resolveLabels(std::vector<uint32_t>& instructions) {
    for (const auto& [index, label] : labelUsages) {
        uint32_t opcode = (instructions[index] >> 24) & 0xFF;
        if (labelMap.find(label) == labelMap.end()) {
            throw std::runtime_error("Label not found: " + label);
        }
        uint32_t address = labelMap[label];
        instructions[index] = (opcode << 24) | (address & 0xFFFFFF);
    }
}

std::vector<uint32_t> Instructor::assemble(const std::string& code) {
    std::istringstream iss(code);
    std::string line;
    std::vector<uint32_t> instructions;
    uint32_t address = 0;

    while (std::getline(iss, line)) {
        auto tokens = tokenize(line);

        if (tokens.empty() || tokens[0].back() == ':') {
            if (!tokens.empty()) {
                std::string label = tokens[0].substr(0, tokens[0].length() - 1);
                labelMap[label] = address;
            }
            continue;
        }

        uint32_t opcode = getOpcode(tokens);
        uint32_t instruction = 0;

        if (opcode == opcodeMap["MOV_IMM32"]) {
            instruction = (opcode << 24) | (parseRegister(tokens[1]) << 16) | 0xFFFF;
            instructions.push_back(instruction);
            instructions.push_back(parseImmediate(tokens[2], 32));
            address += 2;
        } else if (opcode == opcodeMap["STORE_IMM32"]) {
            instruction = (opcode << 24) | 0xFFFFFF;
            instructions.push_back(instruction);
            instructions.push_back(parseImmediate(tokens[1], 32));
            instructions.push_back(parseImmediate(tokens[2], 32));
            address += 3;
        } else if (opcode == opcodeMap["MOV_IMM16"] || opcode == opcodeMap["CMP_IMM16"] || opcode == opcodeMap["ADD_IMM16"]) {
            instruction = (opcode << 24) | (parseRegister(tokens[1]) << 16) | parseImmediate(tokens[2], 16);
            instructions.push_back(instruction);
            address++;
        } else if (opcode == opcodeMap["MOV_REG"] || opcode == opcodeMap["CMP_REG"] || opcode == opcodeMap["ADD"] ||
                   opcode == opcodeMap["SUB"] || opcode == opcodeMap["AND"] || opcode == opcodeMap["OR"] || opcode == opcodeMap["XOR"]) {
            instruction = (opcode << 24) | (parseRegister(tokens[1]) << 16) | (parseRegister(tokens[2]) << 8);
            instructions.push_back(instruction);
            address++;
        } else if (opcode == opcodeMap["NOT"] || opcode == opcodeMap["IN"] || opcode == opcodeMap["OUT"]) {
            instruction = (opcode << 24) | (parseRegister(tokens[1]) << 16);
            instructions.push_back(instruction);
            address++;
        } else if (opcode >= 0x12 && opcode <= 0x18) {
            // Jump operations with labels
            std::string label = tokens[1];
            if (!isLabel(label)) {
                throw std::runtime_error("Invalid label format: " + label);
            }
            instruction = (opcode << 24);
            instructions.push_back(instruction);
            labelUsages.emplace_back(instructions.size() - 1, label);
            address++;
        } else if (opcode == opcodeMap["HLT"]) {
            instruction = 0xFF000000;
            instructions.push_back(instruction);
            address++;
        } else if (tokens.size() == 1) {
            // Single operand instructions
            instruction = (opcode << 24);
            instructions.push_back(instruction);
            address++;
        } else {
            throw std::runtime_error("Invalid instruction format");
        }
    }

    resolveLabels(instructions);

    return instructions;
}

void Instructor::initializeOpcodeMap() {
    opcodeMap["NOP"] = 0x00;
    opcodeMap["MOV_REG"] = 0x01;
    opcodeMap["MOV_IMM16"] = 0x02;
    opcodeMap["MOV_IMM32"] = 0xE2;
    opcodeMap["LOAD"] = 0x03;
    opcodeMap["STORE"] = 0x04;
    opcodeMap["STORE_IMM32"] = 0xE4;
    opcodeMap["ADD"] = 0x05;
    opcodeMap["ADD_IMM16"] = 0xE5;
    opcodeMap["SUB"] = 0x06;
    opcodeMap["AND"] = 0x07;
    opcodeMap["OR"] = 0x08;
    opcodeMap["XOR"] = 0x09;
    opcodeMap["NOT"] = 0x0A;
    opcodeMap["CMP_IMM16"] = 0x10;
    opcodeMap["CMP_REG"] = 0x11;
    opcodeMap["JMP"] = 0x12;
    opcodeMap["JZ"] = 0x13;
    opcodeMap["JNZ"] = 0x14;
    opcodeMap["JL"] = 0x15;
    opcodeMap["JG"] = 0x16;
    opcodeMap["JLE"] = 0x17;
    opcodeMap["JGE"] = 0x18;
    opcodeMap["CALL"] = 0x30;
    opcodeMap["RET"] = 0x31;
    opcodeMap["IN"] = 0x40;
    opcodeMap["OUT"] = 0x41;
    opcodeMap["HLT"] = 0xFF;
}

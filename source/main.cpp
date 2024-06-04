#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <CPU32/CPU32.hpp>
#include <regex>
#include <algorithm>

// Function to convert a string to a uint32_t value, handling both hex and decimal formats
uint32_t stringToUInt32(const std::string& str) {
    uint32_t value;
    std::stringstream ss;

    // Check if the string is in hex format
    if (str.find("0x") == 0 || str.find("0X") == 0) {
        ss << std::hex << str.substr(2);
    } else {
        // Check if the string contains only hex characters (0-9, A-F, a-f)
        bool isHex = str.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos;
        if (isHex) {
            ss << std::hex << str;
        } else {
            ss << std::dec << str;
        }
    }

    ss >> value;
    return value;
}

// Function to convert a uint32_t value to a string with 0x prefix and uppercase letters
std::string uint32ToHexString(uint32_t value) {
    std::stringstream ss;
    ss << "0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << value;
    return ss.str();
}

void showHelp() {
    std::cout << "Commands:\n";
    std::cout << "  help                - Show this help message\n";
    std::cout << "  exit                - Exit the interpreter\n";
    std::cout << "  execute             - Execute the loaded program\n";
    std::cout << "  load <instructions> - Load a program (e.g., load 0x2 0x1 0x1111)\n";
    std::cout << "                         Or for HALT: load FF\n";
    std::cout << "  loadpgm <instructions> - Load a complete program (e.g., loadpgm 0x07010200, 0xFF000000)\n";
    std::cout << "  print               - Print the CPU state (registers, PC, flags)\n";
}

void printCPUState(const CPU32& cpu) {
    auto registers = cpu.GetRegisters();
    std::cout << "Registers:" << std::endl;
    for (size_t i = 0; i < registers.size(); ++i) {
        std::cout << "R" << i << ": " << uint32ToHexString(registers[i]->GetState()) << std::endl;
    }
    std::cout << "Program Counter (PC): " << uint32ToHexString(cpu.GetProgramCounter()->GetState()) << std::endl;
    std::cout << "Flags: " << uint32ToHexString(cpu.GetState()) << std::endl;
    std::cout << "Halted: " << (cpu.halted ? "True" : "False") << std::endl;
}

std::string getInstructionDescription(uint32_t opcode) {
    switch (opcode) {
        case 0x00: return "NOP";
        case 0x01: return "MOV (register)";
        case 0x02: return "MOV (immediate)";
        case 0x03: return "LOAD";
        case 0x04: return "STORE";
        case 0x05: return "ADD";
        case 0x06: return "SUB";
        case 0x07: return "AND";
        case 0x08: return "OR";
        case 0x09: return "XOR";
        case 0x0A: return "NOT";
        case 0x10: return "CMP (immediate)";
        case 0x11: return "CMP (register)";
        case 0x12: return "JMP";
        case 0x13: return "JZ";
        case 0x14: return "JNZ";
        case 0x15: return "JL";
        case 0x16: return "JG";
        case 0x17: return "JLE";
        case 0x18: return "JGE";
        case 0x30: return "CALL";
        case 0x31: return "RET";
        case 0x40: return "IN";
        case 0x41: return "OUT";
        case 0xE2: return "MOV (32-bit immediate)";
        case 0xFF: return "HALT";
        default: return "Unknown";
    }
}

int main() {
    CPU32 cpu(1024);
    std::string line;
    std::vector<uint32_t> program;

    std::cout << "CPU32 Interpreter. Type 'help' for a list of commands." << std::endl;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "help") {
            showHelp();
        } else if (command == "exit") {
            break;
        } else if (command == "execute") {
            cpu.loadProgram(program, 0);
            cpu.run();
            std::cout << "Program executed successfully." << std::endl; // Add success message
            std::cout << std::endl; // Add a newline to fix cursor position
        } else if (command == "load") {
            std::string token;
            uint32_t opcode = 0, reg1 = 0, reg2_or_immediate = 0;
            if (!(iss >> token)) {
                std::cerr << "No instruction provided." << std::endl;
                continue;
            }
            opcode = stringToUInt32(token);

            if (opcode == 0xFF) {
                uint32_t instruction = (opcode << 24);
                program.push_back(instruction);
                std::cout << "Loaded halt instruction: " << uint32ToHexString(instruction) << std::endl;
            } else if (opcode <= 0x0A || (opcode >= 0x10 && opcode <= 0x18) || opcode == 0x30 || opcode == 0x31 || opcode == 0x40 || opcode == 0xE2) {
                if (!(iss >> token)) {
                    std::cerr << "Invalid instruction format." << std::endl;
                    continue;
                }
                reg1 = stringToUInt32(token);

                if (opcode == 0x02 || opcode == 0x10) {
                    if (!(iss >> token)) {
                        std::cerr << "Invalid instruction format." << std::endl;
                        continue;
                    }
                    reg2_or_immediate = stringToUInt32(token);
                }

                if (opcode == 0xE2) {
                    // Handle 32-bit immediate value
                    if (!(iss >> token)) {
                        std::cerr << "Missing immediate value." << std::endl;
                        continue;
                    }
                    uint32_t immediate = stringToUInt32(token);
                    uint32_t instruction = (opcode << 24) | (reg1 << 16) | 0xFF;
                    program.push_back(instruction);
                    program.push_back(immediate);
                    std::cout << "Loaded instruction " << getInstructionDescription(opcode)
                              << " with 32-bit immediate value: "
                              << uint32ToHexString(instruction) << " "
                              << uint32ToHexString(immediate) << std::endl;
                } else if (opcode == 0x02 || opcode == 0x10) {
                    // Handle 16-bit immediate value
                    uint32_t instruction = (opcode << 24) | (reg1 << 16) | reg2_or_immediate;
                    program.push_back(instruction);
                    std::cout << "Loaded instruction " << getInstructionDescription(opcode)
                              << " with 16-bit immediate value: "
                              << uint32ToHexString(instruction) << std::endl;
                } else if (opcode >= 0x12 && opcode <= 0x18) {
                    // Handle jump instructions with address
                    uint32_t instruction = (opcode << 24) | reg1;
                    program.push_back(instruction);
                    std::cout << "Loaded jump instruction " << getInstructionDescription(opcode) << ": "
                              << uint32ToHexString(instruction) << std::endl;
                } else {
                    uint32_t instruction = (opcode << 24) | (reg1 << 16) | (reg2_or_immediate << 8);
                    program.push_back(instruction);
                    std::cout << "Loaded instruction " << getInstructionDescription(opcode) << ": "
                              << uint32ToHexString(instruction) << std::endl;
                }
            } else {
                std::cerr << "Unknown opcode." << std::endl;
            }
        } else if (command == "loadpgm") {
            std::string programHex;
            std::getline(iss, programHex);

            // Remove any whitespace characters around the commas
            programHex.erase(std::remove(programHex.begin(), programHex.end(), ' '), programHex.end());

            std::vector<uint32_t> newProgram;
            std::stringstream ss(programHex);
            std::string instructionHex;

            while (std::getline(ss, instructionHex, ',')) {
                uint32_t instruction = stringToUInt32(instructionHex);
                newProgram.push_back(instruction);
                std::cout << "Loaded instruction: " << uint32ToHexString(instruction) << std::endl;
            }

            program = newProgram;
            std::cout << "Program loaded successfully." << std::endl;
        } else if (command == "print") {
            printCPUState(cpu);
        } else {
            std::cerr << "Unknown command: " << command << std::endl;
        }
    }

    return 0;
}

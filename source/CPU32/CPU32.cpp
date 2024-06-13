//
// Created by John on 6/3/2024.
//

#include <CPU32/CPU32.hpp>
#include <iostream>

CPU32::CPU32(size_t memorySize)
        : instruction(0), immediateOperand(0), returnAddress(0), halted(false) {
    memory = std::make_shared<Memory32>(memorySize);
    clock = std::make_shared<Clock32>(1);
    programCounter = std::make_shared<Register32>();
    alu = std::make_shared<ALU32>();
    flagsRegister = std::make_shared<Flags32>();

    // Initialize 16 General Purpose Registers (We really don't need more than this!)
    for (int i = 0; i < 16; ++i) {
        registers.push_back(std::make_shared<Register32>());
    }

    // Initialize the stack pointer (register 15)
    stackPointer = new Register32();
    stackPointer->loadValue(memorySize);  // Stack pointer starts at the end of memory


    // Populate opcode map
    opcodeMap[0x00] = &CPU32::nop;
    opcodeMap[0x01] = &CPU32::movRegisterToRegister;
    opcodeMap[0x02] = &CPU32::movImmediateToRegister;
    opcodeMap[0x03] = &CPU32::load;
    opcodeMap[0x04] = &CPU32::store;
    opcodeMap[0x05] = &CPU32::add;
    opcodeMap[0x06] = &CPU32::sub;
    opcodeMap[0x07] = &CPU32::andOp;
    opcodeMap[0x08] = &CPU32::orOp;
    opcodeMap[0x09] = &CPU32::xorOp;
    opcodeMap[0x0A] = &CPU32::notOp;
    opcodeMap[0x10] = &CPU32::cmpImmediateToRegister;
    opcodeMap[0x11] = &CPU32::cmpRegisterToRegister;
    opcodeMap[0x12] = &CPU32::jmp;
    opcodeMap[0x13] = &CPU32::jz;
    opcodeMap[0x14] = &CPU32::jnz;
    opcodeMap[0x15] = &CPU32::jl;
    opcodeMap[0x16] = &CPU32::jg;
    opcodeMap[0x17] = &CPU32::jle;
    opcodeMap[0x18] = &CPU32::jge;
    opcodeMap[0x30] = &CPU32::call;
    opcodeMap[0x31] = &CPU32::ret;
    opcodeMap[0x32] = & CPU32::push;
    opcodeMap[0x33] = & CPU32::pop;
    opcodeMap[0x40] = &CPU32::inOp;
    opcodeMap[0x41] = &CPU32::outOp;
    opcodeMap[0xE2] = &CPU32::movImmediate32ToRegister;
    opcodeMap[0xE4] = &CPU32::storeImmediate32;
    opcodeMap[0xE5] = &CPU32::addImmediate;
    opcodeMap[0xFF] = &CPU32::hlt;
}

void CPU32::loadInstruction(uint32_t instruction, uint32_t immediate) {
    this->instruction = instruction;
    this->immediateOperand = immediate;
}

void CPU32::tickClock() {
    clock->tick();
    fetch();
    decodeExecute();
}

void CPU32::run() {
    while (!halted) {
        tickClock();
    }
}

void CPU32::loadProgram(const std::vector<uint32_t>& program, uint32_t startAddress) {
    for (size_t i = 0; i < program.size(); ++i) {
        memory->store(startAddress + i, program[i]);
    }
    programCounter->loadValue(startAddress);
}

std::vector<std::shared_ptr<Register32>> CPU32::GetRegisters() const {
    return registers;
}

std::shared_ptr<Register32> CPU32::GetProgramCounter() const {
    return programCounter;
}

std::shared_ptr<Memory32> CPU32::GetMemory() const {
    return memory;
}

std::shared_ptr<Flags32> CPU32::GetFlagsRegister() const {
    return flagsRegister;
}

void CPU32::fetch() {
    uint32_t pc = programCounter->GetState();
    instruction = memory->load(pc);

    // Set the program counter to the next instruction unless it's a jump or storeImmediate32
    uint8_t opcode = (instruction >> 24) & 0xFF;
    if (opcode != 0x12 && opcode != 0x13 && opcode != 0x14 && opcode != 0x15 && opcode != 0x16 && opcode != 0x17 && opcode != 0x18 && opcode != 0xE4) {
        programCounter->loadValue(pc + 1);
    }

    if (opcode == 0xE4) {
        // If the opcode is 0xE4, it signals two immediate values are next
        addressOperand = memory->load(programCounter->GetState() + 1); // PC is set to
        programCounter->loadValue(programCounter->GetState() + 2);
        immediateOperand = memory->load(programCounter->GetState());
        programCounter->loadValue(programCounter->GetState() + 1);
    } else if ((instruction & 0xFF) == 0xFF) {
        // If the last byte is 0xFF, it signals an immediate value is next
        immediateOperand = memory->load(programCounter->GetState());
        programCounter->loadValue(programCounter->GetState() + 1);
    } else {
        immediateOperand = 0;
        addressOperand = 0;
    }
}

void CPU32::decodeExecute() {
    uint8_t opcode = (instruction >> 24) & 0xFF;
    if (opcodeMap.find(opcode) != opcodeMap.end()) {
        (this->*opcodeMap[opcode])();
    } else {
        std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
        hlt();
    }
}

void CPU32::nop() {}

void CPU32::movRegisterToRegister() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint8_t reg2 = (instruction >> 8) & 0xFF;
    uint32_t value = registers[reg2]->GetState();
    registers[reg1]->loadValue(value);
}

void CPU32::movImmediateToRegister() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint32_t immediate = instruction & 0xFFFF;
    registers[reg1]->loadValue(immediate);
}

void CPU32::movImmediate32ToRegister() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    registers[reg1]->loadValue(immediateOperand);
}

void CPU32::load() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint8_t reg2 = (instruction >> 8) & 0xFF;
    uint32_t address = registers[reg2]->GetState();
    uint32_t value = memory->load(address);
    registers[reg1]->loadValue(value);
}

void CPU32::store() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint8_t reg2 = (instruction >> 8) & 0xFF;
    uint32_t address = registers[reg2]->GetState();
    uint32_t value = registers[reg1]->GetState();
    memory->store(address, value);
}

void CPU32::storeImmediate32() {
    uint32_t value = immediateOperand;
    uint32_t address = addressOperand;
    memory->store(address, value);
}

void CPU32::add() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint8_t reg2 = (instruction >> 8) & 0xFF;
    uint32_t value1 = registers[reg1]->GetState();
    uint32_t value2 = registers[reg2]->GetState();
    alu->setInputs(value1, value2);
    alu->setOperation(ALU32::ADD);
    uint32_t result = alu->GetState();
    registers[reg1]->loadValue(result);
    if (result == 0) {
        flagsRegister->setFlag(Flags32::ZERO);
    } else {
        flagsRegister->clearFlag(Flags32::ZERO);
    }
    if (static_cast<int32_t>(result) < 0) {
        flagsRegister->setFlag(Flags32::SIGN);
    } else {
        flagsRegister->clearFlag(Flags32::SIGN);
    }
}

void CPU32::addImmediate() {
    uint8_t regDest = (instruction >> 16) & 0x0F;
    uint32_t immediate = instruction & 0xFFFF;
    uint32_t currentValue = registers[regDest]->GetState();
    uint32_t result = currentValue + immediate;
    registers[regDest]->loadValue(result);

    // Update flags
    bool zeroFlag = (result == 0);
    if (zeroFlag)
    {
        flagsRegister->setFlag(Flags32::ZERO);
    }
    else
    {
        flagsRegister->clearFlag(Flags32::ZERO);
    }
    // Update overflow and carry flags as needed
}

void CPU32::sub() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint8_t reg2 = (instruction >> 8) & 0xFF;
    uint32_t value1 = registers[reg1]->GetState();
    uint32_t value2 = registers[reg2]->GetState();
    alu->setInputs(value1, value2);
    alu->setOperation(ALU32::SUB);
    uint32_t result = alu->GetState();
    registers[reg1]->loadValue(result);
    if (result == 0) {
        flagsRegister->setFlag(Flags32::ZERO);
    } else {
        flagsRegister->clearFlag(Flags32::ZERO);
    }
    if (static_cast<int32_t>(result) < 0) {
        flagsRegister->setFlag(Flags32::SIGN);
    } else {
        flagsRegister->clearFlag(Flags32::SIGN);
    }
}

void CPU32::andOp() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint8_t reg2 = (instruction >> 8) & 0xFF;
    uint32_t value1 = registers[reg1]->GetState();
    uint32_t value2 = registers[reg2]->GetState();
    alu->setInputs(value1, value2);
    alu->setOperation(ALU32::AND);
    uint32_t result = alu->GetState();
    registers[reg1]->loadValue(result);
    if (result == 0) {
        flagsRegister->setFlag(Flags32::ZERO);
    } else {
        flagsRegister->clearFlag(Flags32::ZERO);
    }
    if (static_cast<int32_t>(result) < 0) {
        flagsRegister->setFlag(Flags32::SIGN);
    } else {
        flagsRegister->clearFlag(Flags32::SIGN);
    }
}

void CPU32::orOp() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint8_t reg2 = (instruction >> 8) & 0xFF;
    uint32_t value1 = registers[reg1]->GetState();
    uint32_t value2 = registers[reg2]->GetState();
    alu->setInputs(value1, value2);
    alu->setOperation(ALU32::OR);
    uint32_t result = alu->GetState();
    registers[reg1]->loadValue(result);
    if (result == 0) {
        flagsRegister->setFlag(Flags32::ZERO);
    } else {
        flagsRegister->clearFlag(Flags32::ZERO);
    }
    if (static_cast<int32_t>(result) < 0) {
        flagsRegister->setFlag(Flags32::SIGN);
    } else {
        flagsRegister->clearFlag(Flags32::SIGN);
    }
}

void CPU32::xorOp() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint8_t reg2 = (instruction >> 8) & 0xFF;
    uint32_t value1 = registers[reg1]->GetState();
    uint32_t value2 = registers[reg2]->GetState();
    alu->setInputs(value1, value2);
    alu->setOperation(ALU32::XOR);
    uint32_t result = alu->GetState();
    registers[reg1]->loadValue(result);
    if (result == 0) {
        flagsRegister->setFlag(Flags32::ZERO);
    } else {
        flagsRegister->clearFlag(Flags32::ZERO);
    }
    if (static_cast<int32_t>(result) < 0) {
        flagsRegister->setFlag(Flags32::SIGN);
    } else {
        flagsRegister->clearFlag(Flags32::SIGN);
    }
}

void CPU32::notOp() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint32_t value = registers[reg1]->GetState();
    alu->setInputs(value, 0);
    alu->setOperation(ALU32::NOT);
    uint32_t result = alu->GetState();
    registers[reg1]->loadValue(result);
    if (result == 0) {
        flagsRegister->setFlag(Flags32::ZERO);
    } else {
        flagsRegister->clearFlag(Flags32::ZERO);
    }
    if (static_cast<int32_t>(result) < 0) {
        flagsRegister->setFlag(Flags32::SIGN);
    } else {
        flagsRegister->clearFlag(Flags32::SIGN);
    }
}

void CPU32::cmpImmediateToRegister() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint32_t immediate = instruction & 0xFFFF;
    uint32_t value = registers[reg1]->GetState();
    uint32_t result = value - immediate;
    if (result == 0) {
        flagsRegister->setFlag(Flags32::ZERO);
    } else {
        flagsRegister->clearFlag(Flags32::ZERO);
    }
    if (static_cast<int32_t>(result) < 0) {
        flagsRegister->setFlag(Flags32::SIGN);
    } else {
        flagsRegister->clearFlag(Flags32::SIGN);
    }
}

void CPU32::cmpRegisterToRegister() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint8_t reg2 = (instruction >> 8) & 0xFF;
    uint32_t value1 = registers[reg1]->GetState();
    uint32_t value2 = registers[reg2]->GetState();
    uint32_t result = value1 - value2;
    if (result == 0) {
        flagsRegister->setFlag(Flags32::ZERO);
    } else {
        flagsRegister->clearFlag(Flags32::ZERO);
    }
    if (static_cast<int32_t>(result) < 0) {
        flagsRegister->setFlag(Flags32::SIGN);
    } else {
        flagsRegister->clearFlag(Flags32::SIGN);
    }
}

void CPU32::jmp() {
    uint32_t address = instruction & 0xFFFF;
    programCounter->loadValue(address);
}

void CPU32::jz() {
    if (flagsRegister->isFlagSet(Flags32::ZERO)) {
        uint32_t address = instruction & 0xFFFF;
        programCounter->loadValue(address);
    }
}

void CPU32::jnz() {
    if (!flagsRegister->isFlagSet(Flags32::ZERO)) {
        uint32_t address = instruction & 0xFFFF;
        programCounter->loadValue(address);
    }
}

void CPU32::jl() {
    if (flagsRegister->isFlagSet(Flags32::SIGN)) {
        uint32_t address = instruction & 0xFFFF;
        programCounter->loadValue(address);
    }
}

void CPU32::jg() {
    if (!flagsRegister->isFlagSet(Flags32::SIGN) && !flagsRegister->isFlagSet(Flags32::ZERO)) {
        uint32_t address = instruction & 0xFFFF;
        programCounter->loadValue(address);
    }
}

void CPU32::jle() {
    if (flagsRegister->isFlagSet(Flags32::SIGN) || flagsRegister->isFlagSet(Flags32::ZERO)) {
        uint32_t address = instruction & 0xFFFF;
        programCounter->loadValue(address);
    }
}

void CPU32::jge() {
    if (!flagsRegister->isFlagSet(Flags32::SIGN) || flagsRegister->isFlagSet(Flags32::ZERO)) {
        uint32_t address = instruction & 0xFFFF;
        programCounter->loadValue(address);
    }
}

void CPU32::call() {
    uint32_t address = instruction & 0xFFFF;
    returnAddress = programCounter->GetState();
    stackPointer->loadValue(stackPointer->GetState() - 1);
    memory->store(stackPointer->GetState(), returnAddress);

    // Jump to the target address
    programCounter->loadValue(address);
}

void CPU32::ret() {
    returnAddress = memory->load(stackPointer->GetState());
    stackPointer->loadValue(stackPointer->GetState() + 1);
    programCounter->loadValue(returnAddress);
}

void CPU32::push() {
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    uint32_t value = registers[reg1]->GetState();
    if (stackPointer->GetState() == 0) {
        throw std::runtime_error("Stack overflow");
    }
    stackPointer->loadValue(stackPointer->GetState() - 1);
    memory->store(stackPointer->GetState(), value);
}

void CPU32::pop() {
    if (stackPointer->GetState() >= memory->getSize() - 1) {
        throw std::runtime_error("Stack underflow");
    }
    uint8_t reg1 = (instruction >> 16) & 0xFF;
    stackPointer->loadValue(stackPointer->GetState() + 1);
    uint32_t value = memory->load(stackPointer->GetState());
    registers[reg1]->loadValue(value);
}

void CPU32::inOp() {
    // Placeholder for input operation
}

void CPU32::outOp() {
    // Placeholder for output operation
}

void CPU32::hlt() {
    halted = true;
}

uint32_t CPU32::GetState() const {
    // Returning a combination of important states as a single uint32_t
    // Here we return the state of the program counter, zero flag, and halted status
    uint32_t state = 0;
    state |= (programCounter->GetState() & 0xFFFF);  // lower 16 bits for program counter
    state |= (flagsRegister->getFlags() << 16);  // 17th bit for flags
    state |= (halted << 17);    // 18th bit for halted flag
    return state;
}

void CPU32::Update(uint32_t state) {
    // Updating the state from a single uint32_t
    uint32_t pc = state & 0xFFFF;  // lower 16 bits for program counter
    uint32_t flags = (state >> 16) & 0xFFFF; // 17th bit for flags
    bool hFlag = (state >> 17) & 1; // 18th bit for halted flag

    programCounter->loadValue(pc);
    flagsRegister->loadValue(flags);
    halted = hFlag;
}

bool CPU32::GetZeroFlag() const {
    return flagsRegister->isFlagSet(Flags32::ZERO);
}

void CPU32::SetZeroFlag(bool zFlag) {
    if (zFlag) {
        flagsRegister->setFlag(Flags32::ZERO);
    } else {
        flagsRegister->clearFlag(Flags32::ZERO);
    }
}







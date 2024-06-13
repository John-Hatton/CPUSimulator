#include <string>
#include <vector>
#include <map>
#include <cstdint>

#ifndef CPUSIMULATOR_INSTRUCTOR_HPP
#define CPUSIMULATOR_INSTRUCTOR_HPP

class Instructor {
public:
    Instructor();

    std::vector<uint32_t> assemble(const std::string& code);

private:
    std::map<std::string, uint32_t> opcodeMap;
    std::map<std::string, uint32_t> labelMap;
    std::vector<std::pair<uint32_t, std::string>> labelUsages;

    std::vector<std::string> tokenize(const std::string& line);
    void initializeOpcodeMap();
    uint32_t parseRegister(const std::string& reg);
    uint32_t parseImmediate(const std::string& imm, int bitSize);
    void resolveLabels(std::vector<uint32_t>& instructions);
    uint32_t getOpcode(const std::vector<std::string>& tokens);
    bool isLabel(const std::string& token);
};

#endif //CPUSIMULATOR_INSTRUCTOR_HPP

#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <string>
#include <random>

class Chip8 {
public:
    static constexpr unsigned int MEMORY_SIZE = 4096;
    static constexpr unsigned int REGISTER_COUNT = 16;
    static constexpr unsigned int STACK_DEPTH = 16;
    static constexpr unsigned int GFX_WIDTH = 64;
    static constexpr unsigned int GFX_HEIGHT = 32;

    Chip8();
    bool loadROM(const std::string &filename);
    void cycle();
    void setKey(uint8_t key, bool pressed);
    void reset();

    // Display buffer (monochrome, 0 or 1)
    std::array<uint8_t, GFX_WIDTH * GFX_HEIGHT> gfx{};
    bool drawFlag = false;

    // Public keypad state
    std::array<uint8_t, 16> keypad{};

    // <-- Add this method to fix compile errors
    void updateTimers();

private:
    void decodeAndExecute(uint16_t opcode);

    // Core components
    std::array<uint8_t, MEMORY_SIZE> memory{};
    std::array<uint8_t, REGISTER_COUNT> V{}; // V0..VF
    uint16_t I = 0;
    uint16_t pc = 0x200; // programs start at 0x200
    std::array<uint16_t, STACK_DEPTH> stack{};
    uint8_t sp = 0;

    uint8_t delay_timer = 0;
    uint8_t sound_timer = 0;

    std::mt19937 rng;
    std::uniform_int_distribution<int> byte_dist{0, 255};

    // Opcode handlers
    void opcode_0x0000(uint16_t opcode);
    void opcode_0x8000(uint16_t opcode);
    void opcode_0xE000(uint16_t opcode);
    void opcode_0xF000(uint16_t opcode);
};
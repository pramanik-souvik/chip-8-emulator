#include "chip8.h"
#include <fstream>
#include <iostream>
#include <cstring>

// Standard Chip-8 font set (0-F), each character 5 bytes
static const uint8_t chip8_fontset[80] = {
  0xF0,0x90,0x90,0x90,0xF0, // 0
  0x20,0x60,0x20,0x20,0x70, // 1
  0xF0,0x10,0xF0,0x80,0xF0, // 2
  0xF0,0x10,0xF0,0x10,0xF0, // 3
  0x90,0x90,0xF0,0x10,0x10, // 4
  0xF0,0x80,0xF0,0x10,0xF0, // 5
  0xF0,0x80,0xF0,0x90,0xF0, // 6
  0xF0,0x10,0x20,0x40,0x40, // 7
  0xF0,0x90,0xF0,0x90,0xF0, // 8
  0xF0,0x90,0xF0,0x10,0xF0, // 9
  0xF0,0x90,0xF0,0x90,0x90, // A
  0xE0,0x90,0xE0,0x90,0xE0, // B
  0xF0,0x80,0x80,0x80,0xF0, // C
  0xE0,0x90,0x90,0x90,0xE0, // D
  0xF0,0x80,0xF0,0x80,0xF0, // E
  0xF0,0x80,0xF0,0x80,0x80  // F
};

Chip8::Chip8() : rng(std::random_device{}()) {
    reset();
}

void Chip8::reset() {
    memory.fill(0);
    V.fill(0);
    stack.fill(0);
    keypad.fill(0);
    gfx.fill(0);
    I = 0;
    pc = 0x200;
    sp = 0;
    delay_timer = 0;
    sound_timer = 0;
    drawFlag = false;

    // Load font set into memory starting at 0x50
    std::memcpy(&memory[0x50], chip8_fontset, sizeof(chip8_fontset));
}

bool Chip8::loadROM(const std::string &filename) {
    std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
    if (!ifs) return false;
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    if (size <= 0 || size > (int)(MEMORY_SIZE - 0x200)) return false;
    ifs.read(reinterpret_cast<char*>(&memory[0x200]), size);
    return true;
}

void Chip8::cycle() {
    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;
    decodeAndExecute(opcode);

    updateTimers();
}

void Chip8::updateTimers() {
    if (delay_timer > 0) --delay_timer;
    if (sound_timer > 0) --sound_timer;
}

void Chip8::setKey(uint8_t key, bool pressed) {
    if (key < 16) keypad[key] = pressed ? 1 : 0;
}

void Chip8::decodeAndExecute(uint16_t opcode) {
    uint16_t nnn = opcode & 0x0FFF;
    uint8_t n = opcode & 0x000F;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t kk = opcode & 0x00FF;

    switch (opcode & 0xF000) {
        case 0x0000: opcode_0x0000(opcode); break;
        case 0x1000: pc = nnn; break;
        case 0x2000: stack[sp++] = pc; pc = nnn; break;
        case 0x3000: if (V[x] == kk) pc += 2; break;
        case 0x4000: if (V[x] != kk) pc += 2; break;
        case 0x5000: if (V[x] == V[y]) pc += 2; break;
        case 0x6000: V[x] = kk; break;
        case 0x7000: V[x] += kk; break;
        case 0x8000: opcode_0x8000(opcode); break;
        case 0x9000: if (V[x] != V[y]) pc += 2; break;
        case 0xA000: I = nnn; break;
        case 0xB000: pc = nnn + V[0]; break;
        case 0xC000: V[x] = byte_dist(rng) & kk; break;
        case 0xD000: {
            uint8_t height = n;
            V[0xF] = 0;
            for (unsigned int row = 0; row < height; ++row) {
                uint8_t sprite = memory[I + row];
                for (unsigned int col = 0; col < 8; ++col) {
                    if ((sprite & (0x80 >> col)) != 0) {
                        uint16_t px = (V[x] + col) % GFX_WIDTH;
                        uint16_t py = (V[y] + row) % GFX_HEIGHT;
                        uint32_t idx = px + (py * GFX_WIDTH);
                        if (gfx[idx] == 1) V[0xF] = 1;
                        gfx[idx] ^= 1;
                    }
                }
            }
            drawFlag = true;
        } break;
        case 0xE000: opcode_0xE000(opcode); break;
        case 0xF000: opcode_0xF000(opcode); break;
        default:
            std::cerr << "Unknown opcode: 0x" << std::hex << opcode << std::dec << "\n";
    }
}

void Chip8::opcode_0x0000(uint16_t opcode) {
    switch (opcode & 0x00FF) {
        case 0x00E0: gfx.fill(0); drawFlag = true; break;
        case 0x00EE: if (sp > 0) pc = stack[--sp]; break;
        default: break;
    }
}

void Chip8::opcode_0x8000(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    switch (opcode & 0x000F) {
        case 0x0: V[x] = V[y]; break;
        case 0x1: V[x] |= V[y]; break;
        case 0x2: V[x] &= V[y]; break;
        case 0x3: V[x] ^= V[y]; break;
        case 0x4: { uint16_t sum = V[x] + V[y]; V[0xF] = sum > 0xFF; V[x] = sum & 0xFF; } break;
        case 0x5: V[0xF] = V[x] > V[y]; V[x] -= V[y]; break;
        case 0x6: V[0xF] = V[x] & 0x1; V[x] >>= 1; break;
        case 0x7: V[0xF] = V[y] > V[x]; V[x] = V[y] - V[x]; break;
        case 0xE: V[0xF] = (V[x] & 0x80) >> 7; V[x] <<= 1; break;
        default: break;
    }
}

void Chip8::opcode_0xE000(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    switch (opcode & 0x00FF) {
        case 0x9E: if (keypad[V[x]]) pc += 2; break;
        case 0xA1: if (!keypad[V[x]]) pc += 2; break;
        default: break;
    }
}

void Chip8::opcode_0xF000(uint16_t opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    switch (opcode & 0x00FF) {
        case 0x07: V[x] = delay_timer; break;
        case 0x0A: {
            bool pressed = false;
            for (int i = 0; i < 16; ++i) {
                if (keypad[i]) { V[x] = i; pressed = true; break; }
            }
            if (!pressed) pc -= 2;
        } break;
        case 0x15: delay_timer = V[x]; break;
        case 0x18: sound_timer = V[x]; break;
        case 0x1E: I += V[x]; break;
        case 0x29: I = 0x50 + (V[x] * 5); break;
        case 0x33: memory[I] = V[x] / 100; memory[I+1] = (V[x]/10)%10; memory[I+2] = V[x]%10; break;
        case 0x55: for (int i = 0; i <= x; ++i) memory[I + i] = V[i]; break;
        case 0x65: for (int i = 0; i <= x; ++i) V[i] = memory[I + i]; break;
        default: break;
    }
}
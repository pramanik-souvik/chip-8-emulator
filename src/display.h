#pragma once

#include "chip8.h"
#include <SDL2/SDL.h>
#include <string>
#include <cstdint>

class Display {
public:
    Display(int scale = 10);
    ~Display();

    bool init(const std::string &title);
    void render(const Chip8 &chip8);
    void pollEvents(Chip8 &chip8, bool &quit);
    void clear();
    void shutdown();   // <-- Added (main.cpp expects this)

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int scale;

    uint8_t keyMap[16];

    void setupKeyMap();
};
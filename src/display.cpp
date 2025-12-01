#include "display.h"
#include "chip8.h"
#include <iostream>

Display::Display(int scale) : scale(scale) {
    setupKeyMap();
}

Display::~Display() {
    shutdown();
}

bool Display::init(const std::string &title) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        Chip8::GFX_WIDTH * scale,
        Chip8::GFX_HEIGHT * scale,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "SDL window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

void Display::shutdown() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

void Display::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Display::render(const Chip8 &chip8) {
    clear();

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Render the 64x32 CHIP-8 display buffer
    for (int y = 0; y < Chip8::GFX_HEIGHT; y++) {
        for (int x = 0; x < Chip8::GFX_WIDTH; x++) {
            if (chip8.gfx[y * Chip8::GFX_WIDTH + x]) {  // <- updated here
                SDL_Rect pixel = {
                    x * scale,
                    y * scale,
                    scale,
                    scale
                };
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void Display::pollEvents(Chip8 &chip8, bool &quit) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {

            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYDOWN:
            case SDL_KEYUP: {
                bool pressed = (event.type == SDL_KEYDOWN);
                SDL_Keycode key = event.key.keysym.sym;

                // Translate SDL key → CHIP-8 keypad index
                for (int i = 0; i < 16; i++) {
                    if (key == keyMap[i]) {
                        chip8.keypad[i] = pressed ? 1 : 0;
                    }
                }

                // ESC exits the emulator
                if (key == SDLK_ESCAPE && pressed) {
                    quit = true;
                }
            }
        }
    }
}

void Display::setupKeyMap() {
    // Classic CHIP-8 keypad mapping:
    //
    // 1 2 3 C
    // 4 5 6 D
    // 7 8 9 E
    // A 0 B F

    keyMap[0x0] = SDLK_x;
    keyMap[0x1] = SDLK_1;
    keyMap[0x2] = SDLK_2;
    keyMap[0x3] = SDLK_3;
    keyMap[0x4] = SDLK_q;
    keyMap[0x5] = SDLK_w;
    keyMap[0x6] = SDLK_e;
    keyMap[0x7] = SDLK_a;
    keyMap[0x8] = SDLK_s;
    keyMap[0x9] = SDLK_d;
    keyMap[0xA] = SDLK_z;
    keyMap[0xB] = SDLK_c;
    keyMap[0xC] = SDLK_4;
    keyMap[0xD] = SDLK_r;
    keyMap[0xE] = SDLK_f;
    keyMap[0xF] = SDLK_v;
}
#include <iostream>
#include <filesystem>
#include <vector>
#include "chip8.h"
#include "display.h"
#include <SDL2/SDL.h>  // For SDL_Delay

namespace fs = std::filesystem;

std::string selectROM() {
    std::vector<std::string> roms;
    for (const auto& entry : fs::directory_iterator("../roms")) {
        if (entry.path().extension() == ".ch8")
            roms.push_back(entry.path().string());
    }

    if (roms.empty()) {
        std::cerr << "No ROMs found in ../roms\n";
        exit(1);
    }

    std::cout << "Available ROMs:\n";
    for (size_t i = 0; i < roms.size(); i++)
        std::cout << i << ": " << fs::path(roms[i]).filename().string() << "\n";

    std::cout << "Enter number: ";
    int choice;
    std::cin >> choice;
    if (choice < 0 || choice >= (int)roms.size()) {
        std::cerr << "Invalid selection!\n";
        exit(1);
    }

    return roms[choice];
}

int main(int argc, char** argv) {
    std::string romPath;
    if (argc == 2)
        romPath = argv[1];
    else
        romPath = selectROM();

    Chip8 chip8;
    Display display(10);

    if (!display.init("CHIP-8 Emulator")) {
        return 1;
    }

    if (!chip8.loadROM(romPath)) {
        std::cerr << "Failed to load ROM: " << romPath << std::endl;
        return 1;
    }

    bool running = true;
    const int cyclesPerFrame = 10;

    while (running) {
        display.pollEvents(chip8, running);

        for (int i = 0; i < cyclesPerFrame; i++) {
            chip8.cycle();  // execute one instruction, timers updated inside
        }

        if (chip8.drawFlag) {
            display.render(chip8);
            chip8.drawFlag = false;
        }

        SDL_Delay(16); // ~60 FPS
    }

    display.shutdown();
    return 0;
}
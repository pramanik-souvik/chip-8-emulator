# CHIP-8 Emulator

A CHIP-8 emulator written in **C++17** using **SDL2**. Play classic CHIP-8 games like Pong, Tetris, and Space Invaders directly on your computer.

![Space Invaders](doc/screenshots/invaders.png "Space Invaders")  
*Space Invaders*

![Pong](doc/screenshots/pong.png "Pong")  
*Pong*

CHIP-8 is an interpreted programming language developed by **Joseph Weisbecker** in the mid-1970s for 8-bit microcomputers such as the COSMAC VIP and Telmac 1800. It simplifies game programming by providing a virtual machine to run programs.

This emulator fully implements the CHIP-8 CPU (~35 opcodes), graphics, input, and timers using SDL2.

## Features

- Full CHIP-8 CPU emulation
- 64x32 monochrome display with SDL2
- 16-key hexadecimal input mapped to your keyboard
- Delay and sound timers
- Load and run `.ch8` ROM files
- Simple and modular C++17 code structure for easy extension

## Requirements

- **C++17** compiler  
- **SDL2**  
- **CMake**

### Installing SDL2 on macOS

```bash
brew install sdl2

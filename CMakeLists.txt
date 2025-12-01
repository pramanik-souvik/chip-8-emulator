cmake_minimum_required(VERSION 3.10)
project(chip8_emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use pkg-config to find SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# Include SDL2 headers
include_directories(${SDL2_INCLUDE_DIRS})

# Link SDL2 libraries
link_directories(${SDL2_LIBRARY_DIRS})

add_executable(chip8
    src/main.cpp
    src/chip8.cpp
    src/display.cpp
)

target_include_directories(chip8 PRIVATE src)

target_link_libraries(chip8 PRIVATE ${SDL2_LIBRARIES})
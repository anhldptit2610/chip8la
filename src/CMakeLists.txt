cmake_minimum_required(VERSION 3.22.1)
project(chip8la C)

set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-W -Wall -ggdb)

add_executable(chip8la
                chip8la.c
                chip8.c
                display.c)

target_link_libraries(chip8la PRIVATE SDL2)
target_link_libraries(chip8la PRIVATE SDL2_image)
target_link_libraries(chip8la PRIVATE m)
chip8la is a CHIP-8 virtual machine, which can run some CHIP-8 games.

This is my hobby project to learn about C, so if you want to use it to play games please try other emulators.

Dependencies:
1. cmake 3.22.1
2. sdl2 library(including sdl2_image)

How to install dependencies(on Ubuntu machine):

sudo apt update && sudo apt install cmake libsdl2-dev libsdl2-image-dev

How to build:

mkdir build

cd build && cmake ../src
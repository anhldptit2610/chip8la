#include <stdio.h>
#include <stdint.h>
#include "../include/chip8la.h"

int main(int argc, char *argv[])
{
    chip8_t chip8;
    sdl_t sdl;
    char *rom = argv[1];
    FILE *fp;

    if (sdl_init(&sdl))
        exit(EXIT_FAILURE);
    chip8_init(&chip8);
    fp = fopen(rom, "r");
    if (!fp) {
        fprintf(stderr, "can't open the rom!\n");
        exit(1);
    } else {
        printf("rom %s opened.\n", rom);
    }
    fread(&chip8.memory[0x200], sizeof(chip8.memory), 0x800, fp);

    while (1) {
        SDL_Event e;
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                exit(EXIT_FAILURE);
        }

        /* do things from here */
        chip8_fetch_instruction(&chip8);
        chip8_decode_and_execute_instruction(&chip8, &sdl);
        SDL_Delay(16);
        sdl_update_screen(&chip8, &sdl);
    }
    sdl_cleanup(&sdl);
    return 0;
}
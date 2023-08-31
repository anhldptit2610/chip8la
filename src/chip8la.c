#include <stdio.h>
#include <stdint.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>

#include "../include/chip8la.h"

void usage(void)
{
    fprintf(stderr, "usage: chip8la rom_path [scale_size]\n");
    fprintf(stderr, "       You could choose whether providing\n");
    fprintf(stderr, "       the scale_size argument or not. \n");
    fprintf(stderr, "       Default value is 10.\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    chip8_t chip8;
    sdl_t sdl;
    char *rom = argv[1];
    FILE *fp;
    uint16_t start_frame_time, end_frame_time;

    if (argc < 2)
        usage();
    else if (argc == 2)
        chip8.screen_scale_factor = 10;     // if user does not provide scale factor
    else
        chip8.screen_scale_factor = atoi(argv[2]);
    if (sdl_init(&sdl, chip8.screen_scale_factor))
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

    while (chip8.state != QUIT) {
        chip8_handle_user_input(&chip8);
        start_frame_time = SDL_GetPerformanceCounter();
        for (int i = 0; i < 8; i++)
            chip8_run_instruction(&chip8, &sdl);
        end_frame_time = SDL_GetPerformanceCounter();
        if (chip8.state == PAUSE)
                continue;
        float elapsed_ms = (end_frame_time - start_frame_time) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
        SDL_Delay(16.67f > elapsed_ms ? 16.67f - elapsed_ms : 0);
        sdl_update_screen(&chip8, &sdl);
        chip8_check_timers(&chip8);
    }
    sdl_cleanup(&sdl);
    return 0;
}
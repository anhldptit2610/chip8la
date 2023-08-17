#include "../include/chip8la.h"

int sdl_init(sdl_t *sdl)
{
    int error = 0;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        error = 1;
    }
    sdl->window = SDL_CreateWindow("lda_chip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    640, 320, SDL_WINDOW_SHOWN);
    if (!sdl->window) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        error = 1;
    }
    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_PRESENTVSYNC);
    if (!sdl->renderer) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        error = 1;
    }
    return error;
}

void sdl_cleanup(sdl_t *sdl)
{
    SDL_DestroyRenderer(sdl->renderer);
    sdl->renderer = NULL;
    SDL_DestroyWindow(sdl->window);
    sdl->window = NULL;
    SDL_Quit();
}

void sdl_draw(chip8_t *chip8, sdl_t *sdl, uint16_t vx, uint16_t vy, uint16_t n)
{
    uint16_t x = chip8_get_register_val(chip8, vx) % 64;
    uint16_t y = chip8_get_register_val(chip8, vy) % 32;
    uint16_t bit_coded_pos = chip8->i;

    /* loop through each col, find the row's data */
    for (int i = y; i < y + n; i++) {
        if (i >= SCREEN_HEIGHT)
            break;
        uint8_t bit_coded = chip8->memory[bit_coded_pos];
        for (int j = 7; j >= 0; j--) {
            if (x + 7 - j >= SCREEN_WIDTH)
                break;
            uint8_t bit = (bit_coded >> j) & 0x01;
            if (bit) {
                uint8_t flipper = chip8->pixels[x + 7 - j + i * 64] ^ 1;

                if (!flipper && chip8->pixels[x + 7 - j + i * 64])
                    chip8_set_register_to_val(chip8, F, 1);
                else
                    chip8_set_register_to_val(chip8, F, 0);
                chip8->pixels[x + 7 - j + i * 64] ^= 1;
            }
        }
        bit_coded_pos++;
    }    
}

void sdl_set_renderer_color(sdl_t *sdl, uint8_t color)
{
    switch (color) {
    case BLACK: 
        SDL_SetRenderDrawColor(sdl->renderer, 0x00, 0x00, 0x00, 0xff);
        break;
    case WHITE:
        SDL_SetRenderDrawColor(sdl->renderer, 0xff, 0xff, 0xff, 0xff);
        break; 
    default:
        break;
    }
}

void sdl_disp_clear(sdl_t *sdl)
{
    SDL_SetRenderDrawColor(sdl->renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(sdl->renderer);
}

void sdl_update_screen(chip8_t *chip8, sdl_t *sdl)
{
    /* loop through each pixel and draw based on the pixel table. */
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            SDL_Rect rect = {
                .w = 10,
                .h = 10,
                .x = j * 10,
                .y = i * 10,
            };
            uint8_t pixel = chip8->pixels[j + i * 64];
            if (!pixel)
                sdl_set_renderer_color(sdl, BLACK);
            else
                sdl_set_renderer_color(sdl, WHITE);
            SDL_RenderFillRect(sdl->renderer, &rect);
        }
    }
    SDL_RenderPresent(sdl->renderer);
}
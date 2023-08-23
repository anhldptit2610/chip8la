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

void chip8_draw(chip8_t *chip8, uint16_t vx, uint16_t vy, uint16_t n)
{
    uint16_t x = chip8->v[vx] % SCREEN_WIDTH;
    uint16_t y = chip8->v[vy] % SCREEN_HEIGHT;
    uint16_t orig_x = x;
    chip8->v[F] = 0;
    /* loop through each col, find the row's data */
    for (int i = 0; i < n; i++) {
        uint8_t bit_coded = chip8->memory[chip8->i + i];
        x = orig_x;
        for (int j = 7; j >= 0; j--) {
            bool *pixel = &chip8->pixels[x + y * SCREEN_WIDTH];
            uint8_t bit = (bit_coded >> j) & 0x01;

            if (bit & *pixel) 
                chip8->v[F] = 1;
            *pixel ^= bit;
            if (++x >= SCREEN_WIDTH)
                break;
        }
        if (++y >= SCREEN_HEIGHT)
            break;
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

void chip8_disp_clear(chip8_t *chip8, sdl_t *sdl)
{
    memset(chip8->pixels, 0, SCREEN_HEIGHT * SCREEN_WIDTH);
    SDL_SetRenderDrawColor(sdl->renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(sdl->renderer);
}

void sdl_update_screen(chip8_t *chip8, sdl_t *sdl)
{
    if (chip8->draw_flag) {
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
        chip8->draw_flag = 0;
    }
}
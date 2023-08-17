#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/chip8la.h"

uint16_t chip8_get_register_val(chip8_t *chip8, uint16_t reg_num)
{
    return chip8->gp_reg[reg_num];
}

void chip8_init(chip8_t *chip8)
{
    memset(chip8->pixels, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
    memset(chip8->memory, 0, MEM_SIZE);
    chip8->pc = 0x200;
}

void chip8_set_register_to_val(chip8_t *chip8, int x, uint16_t val)
{
    if (x == I) {
        chip8->i = val;
    } else {
        chip8->gp_reg[x] = val;
    }
}

void chip8_add_register_with_val(chip8_t *chip8, int x, uint16_t val)
{
    chip8->gp_reg[x] += val;
}

void chip8_fetch_instruction(chip8_t *chip8)
{
    uint16_t pc = chip8->pc;
    uint16_t opcode = (chip8->memory[pc] << 8) | chip8->memory[pc + 1];

    chip8->opcode = opcode;
    chip8->nnn = opcode & 0x0fff;
    chip8->nn = opcode & 0x00ff;
    chip8->n = opcode & 0x000f;
    chip8->x = (opcode & 0x0f00) >> 8;
    chip8->y = (opcode & 0x00f0) >> 4;

    chip8->pc += 2;
}

void handle_user_input(chip8_t *chip8, sdl_t *sdl)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            /* code */
            break;
           
        default:
            break;
        }
    }
}

void chip8_decode_and_execute_instruction(chip8_t *chip8, sdl_t *sdl)
{
    uint8_t first_nibble = (chip8->opcode & 0xf000) >> 12; 

    switch (first_nibble) {
    case 0:         /* 00e0 or 00ee */
        if (chip8->n == 0) {
            sdl_disp_clear(sdl);   
        } else if (chip8->n == E) {
            // TODO: implement 00ee - returns from a subroutine.
        }
        break;
    case 1:         /* 1nnn */
        chip8->pc = chip8->nnn;
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    case 5:
        break;
    case 6:         /* 6xnn */
        chip8_set_register_to_val(chip8, chip8->x, chip8->nn);
        break;
    case 7:         /* 7xnn */
        chip8_add_register_with_val(chip8, chip8->x, chip8->nn);
        break;
    case 8:
        break;
    case 9:
        break;
    case A:         /* annn */
        chip8_set_register_to_val(chip8, I, chip8->nnn);
        break;
    case B:
        break;
    case C:
        break;
    case D:         /* dxyn */
        sdl_draw(chip8, sdl, chip8->x, chip8->y, chip8->n);
        break;
    case E:
        break;
    case F:
        break;
    default:
        break;
    }
}


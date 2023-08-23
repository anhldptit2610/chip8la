#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "../include/chip8la.h"

uint8_t system_font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip8_init(chip8_t *chip8)
{
    memset(chip8->memory, 0, MEM_SIZE);
    memset(chip8->pixels, 0, SCREEN_HEIGHT * SCREEN_WIDTH);
    memset(chip8->keypad, false, 16);

    /* load font */
    memcpy(&chip8->memory[0], system_font, sizeof(system_font));

    chip8->pc = 0x200;
    chip8->state = RUNNING;
    chip8->key = 0xff;
    chip8->has_key_pressed = false;
}

void chip8_execute_8xyn_instructions(chip8_t *chip8)
{
    uint16_t vx = chip8->v[chip8->x];
    uint16_t vy = chip8->v[chip8->y];

    switch (chip8->n) {
    case 0:             /* 8xy0 */
        chip8->v[chip8->x] = chip8->v[chip8->y];
        break;
    case 1:             /* 8xy1 */
        chip8->v[F] = 0;
        chip8->v[chip8->x] = vx | vy;
        break;
    case 2:             /* 8xy2 */
        chip8->v[F] = 0;
        chip8->v[chip8->x] = vx & vy;
        break;
    case 3:             /* 8xy3 */
        chip8->v[F] = 0;
        chip8->v[chip8->x] = vx ^ vy;
        break;
    case 4:             /* 8xy4 */
        chip8->v[chip8->x] += chip8->v[chip8->y];
        chip8->v[F] = (vx + vy > 0xff) ? 1 : 0;
        break;
    case 5:             /* 8xy5 */
        chip8->v[chip8->x] -= chip8->v[chip8->y];
        chip8->v[F] = (vx < vy) ? 0 : 1;
        break;
    case 6:             /* 8xy6 */
        chip8->v[chip8->x] = chip8->v[chip8->y];
        vx = chip8->v[chip8->x];
        chip8->v[chip8->x] >>= 1;
        chip8->v[F] = vx & 0x01;
        break;
    case 7:             /* 8xy7 */
        chip8->v[chip8->x] = chip8->v[chip8->y] - chip8->v[chip8->x];
        chip8->v[F] = (vy < vx) ? 0 : 1;
        break;
    case E:             /* 8xye */
        chip8->v[chip8->x] = chip8->v[chip8->y];
        vx = chip8->v[chip8->x];
        chip8->v[chip8->x] <<= 1;
        chip8->v[F] = (vx & 0x80) >> 7;
        break;
    default:
        fprintf(stderr, "chip8la: opcode %04x is invalid.\n", chip8->opcode);
        break;
    }
}

void chip8_check_timers(chip8_t *chip8)
{
    if (chip8->delay_timer > 0)
        chip8->delay_timer--;
    
    if (chip8->sound_timer > 0) {
        chip8->sound_timer--;
        // TODO: make a sound.
    }
}

void chip8_execute_exnn_instructions(chip8_t *chip8)
{
    uint16_t vx = chip8->v[chip8->x];

    // TODO: implement ex9e and exa1
    switch (chip8->nn) {
    case 0x9E:
        if (chip8->keypad[vx] == true)
            chip8->pc += 2;
        break;
    case 0xA1:
        if (chip8->keypad[vx] == false)
            chip8->pc += 2;
        break; 
    default:
        break;
    }
}

void chip8_set_bcd(chip8_t *chip8)
{
    uint8_t vx = chip8->v[chip8->x];
    uint16_t i = chip8->i + 2;

    if (vx < 100)
        chip8->memory[i - 2] = 0;
    while (vx) {
        chip8->memory[i] = vx % 10;
        i--;
        vx /= 10;
    }
}

void chip8_execute_fxnn_instructions(chip8_t *chip8)
{
    switch (chip8->nn) {
    case 0x07:        /* fx07 */
        chip8->v[chip8->x] = chip8->delay_timer;
        break;
    case 0x0A:        /* fx0A */
        if (chip8->key != 0xff)
            return;
        for (int i = 0; chip8->key == 0xff && i < 16; i++) {
            if (chip8->keypad[i] == true) {
                chip8->has_key_pressed = true;
                chip8->key = i;
                break;
            }
        }        
        if (!chip8->has_key_pressed) {
            chip8->pc -= 2;
        } else {
            // if there is any key has been pressed, check if it is released or not.
            if (chip8->keypad[chip8->key]) {
                chip8->pc -= 2;
            } else {
                chip8->v[chip8->x] = chip8->key;
                chip8->key = 0xff;
                chip8->has_key_pressed = false;
            }
        }
        break;
    case 0x15:        /* fx15 */
        chip8->delay_timer = chip8->v[chip8->x];
        break;
    case 0x18:        /* fx18 */
        chip8->sound_timer = chip8->v[chip8->x];
        break;
    case 0x1e:        /* fx1e */
        chip8->i += chip8->v[chip8->x];
        break;
    case 0x29:        /* fx29 */
        chip8->i = 5 * chip8->v[chip8->x];
        break;
    case 0x33:        /* fx33 */
        chip8_set_bcd(chip8);
        break;
    case 0x55:        /* fx55 */
        for (int i = 0; i <= chip8->x; i++)
            chip8->memory[chip8->i++] = chip8->v[i];
        break;
    case 0x65:        /* fx65 */
        for (int i = 0; i <= chip8->x; i++)
            chip8->v[i] = chip8->memory[chip8->i++];
        break;
    default:
        break;
    }
}

void chip8_stack_push(chip8_t *chip8, uint16_t val)
{
    if (chip8->sp > 20)
        chip8->sp = 0;
    chip8->stack[chip8->sp++] = val;
}

uint16_t chip8_stack_pop(chip8_t *chip8)
{
    if (chip8->sp <= 0)
        chip8->sp = 1;
    chip8->sp--;
    return chip8->stack[chip8->sp];
}

void chip8_run_instruction(chip8_t *chip8, sdl_t *sdl)
{
    /* fetch the instruction */
    chip8->opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    chip8->pc += 2;

    /* extract opcode into parts */
    chip8->nnn = chip8->opcode & 0x0fff;
    chip8->nn = chip8->opcode & 0x00ff;
    chip8->n = chip8->opcode & 0x000f;
    chip8->x = (chip8->opcode & 0x0f00) >> 8;
    chip8->y = (chip8->opcode & 0x00f0) >> 4;

    /* run it. */
    switch ((chip8->opcode & 0xf000) >> 12) {
    case 0:
        if (chip8->n == 0) {     /* 00e0 */
            chip8_disp_clear(chip8, sdl);
            chip8->draw_flag = 1;
        } else if (chip8->n == E) {   /* 00ee */
            //TODO: implement 00ee instruction.
            chip8->pc = chip8_stack_pop(chip8);
        }
        break;
    case 1:         /* 1nnn */
        chip8->pc = chip8->nnn;
        break;
    case 2:         /* 2nnn */
        // TODO: implement 2nnn. 
        chip8_stack_push(chip8, chip8->pc);
        chip8->pc = chip8->nnn;
        break;
    case 3:         /* 3xnn */
        if (chip8->v[chip8->x] == chip8->nn)
            chip8->pc += 2;
        break;
    case 4:         /* 4xnn */
        if (chip8->v[chip8->x] != chip8->nn)
            chip8->pc += 2;
        break;
    case 5:         /* 5xy0 */
        if (chip8->v[chip8->x] == chip8->v[chip8->y])
            chip8->pc += 2;
        break;
    case 6:         /* 6xnn */
        chip8->v[chip8->x] = chip8->nn;
        break;
    case 7:         /* 7xnn */
        chip8->v[chip8->x] += chip8->nn;
        break;
    case 8:         
        chip8_execute_8xyn_instructions(chip8);
        break;
    case 9:         /* 9xy0 */
        if (chip8->v[chip8->x] != chip8->v[chip8->y])
            chip8->pc += 2;
        break;
    case A:         /* annn */
        chip8->i = chip8->nnn;
        break;
    case B:         /* bnnn */
        chip8->pc = chip8->v[0] + chip8->nnn;
        break;
    case C:         /* cxnn */
        chip8->v[chip8->x] = rand() & chip8->nn;
        break;
    case D:         /* dxyn */
        chip8_draw(chip8, chip8->x, chip8->y, chip8->n);
        chip8->draw_flag = 1;
        break;
    case E:
        chip8_execute_exnn_instructions(chip8);
        break;
    case F:
        chip8_execute_fxnn_instructions(chip8);
        break;
    default:
        fprintf("chip8la: opcode %04x is invalid.\n", chip8->opcode);
        break;
    }
}

void chip8_handle_user_input(chip8_t *chip8)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            chip8->state = QUIT;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_1: chip8->keypad[0x1] = true; break;
            case SDLK_2: chip8->keypad[0x2] = true; break;
            case SDLK_3: chip8->keypad[0x3] = true; break;
            case SDLK_4: chip8->keypad[0xc] = true; break;
            
            case SDLK_q: chip8->keypad[0x4] = true; break;
            case SDLK_w: chip8->keypad[0x5] = true; break;
            case SDLK_e: chip8->keypad[0x6] = true; break;
            case SDLK_r: chip8->keypad[0xd] = true; break;

            case SDLK_a: chip8->keypad[0x7] = true; break;
            case SDLK_s: chip8->keypad[0x8] = true; break;
            case SDLK_d: chip8->keypad[0x9] = true; break;
            case SDLK_f: chip8->keypad[0xe] = true; break;

            case SDLK_z: chip8->keypad[0xa] = true; break;
            case SDLK_x: chip8->keypad[0x0] = true; break;
            case SDLK_c: chip8->keypad[0xb] = true; break;
            case SDLK_v: chip8->keypad[0xf] = true; break;
            
            case SDLK_p:
                chip8->state = PAUSE;
                break;
            case SDLK_o:
                chip8->state = RUNNING;
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_1: chip8->keypad[0x1] = false; break;
            case SDLK_2: chip8->keypad[0x2] = false; break;
            case SDLK_3: chip8->keypad[0x3] = false; break;
            case SDLK_4: chip8->keypad[0xc] = false; break;
            
            case SDLK_q: chip8->keypad[0x4] = false; break;
            case SDLK_w: chip8->keypad[0x5] = false; break;
            case SDLK_e: chip8->keypad[0x6] = false; break;
            case SDLK_r: chip8->keypad[0xd] = false; break;

            case SDLK_a: chip8->keypad[0x7] = false; break;
            case SDLK_s: chip8->keypad[0x8] = false; break;
            case SDLK_d: chip8->keypad[0x9] = false; break;
            case SDLK_f: chip8->keypad[0xe] = false; break;

            case SDLK_z: chip8->keypad[0xa] = false; break;
            case SDLK_x: chip8->keypad[0x0] = false; break;
            case SDLK_c: chip8->keypad[0xb] = false; break;
            case SDLK_v: chip8->keypad[0xf] = false; break;

            default:
                break;
            }
            break;
        default:
            break;
        }
    }
}

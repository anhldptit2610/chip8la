#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define MEM_SIZE    0x1000
#define SCREEN_WIDTH    64
#define SCREEN_HEIGHT   32

enum COLOR {
    BLACK,
    WHITE,
};

typedef struct sdl {
    SDL_Window *window;
    SDL_Renderer *renderer;

} sdl_t;


enum REG {
    PC = 16,
    I = 17,
};

enum EMULATOR_STATE {
    RUNNING,
    PAUSE,
    QUIT
};

enum HEX {
    A = 10,
    B = 11,
    C = 12,
    D = 13,
    E = 14,
    F = 15,
};

typedef struct chip8 {
    /* chip8 memory */
    uint8_t memory[MEM_SIZE];

    /* registers */
    uint8_t gp_reg[16];
    uint16_t pc;
    uint16_t i;

    /* for fetching and decoding */
    uint16_t opcode;
    uint16_t nnn;       /* address */
    uint16_t nn;        /* 8-bit constant */
    uint16_t n;         /* 4-bit constant */
    uint16_t x;         /* 4-bit register identifier */
    uint16_t y;         /* 4-bit register identifier */
    uint16_t vn;

    /* for tracking set/unset pixels */
    uint8_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
} chip8_t;


/* chip8.c */
void chip8_decode_and_execute_instruction(chip8_t *chip8, sdl_t *sdl);
uint16_t chip8_get_register_val(chip8_t *chip8, uint16_t reg_num);
void chip8_init(chip8_t *chip8);
void chip8_set_register_to_val(chip8_t *chip8, int x, uint16_t val);
void chip8_add_register_with_val(chip8_t *chip8, int x, uint16_t val);
void chip8_fetch_instruction(chip8_t *chip8);
void handle_user_input(chip8_t *chip8, sdl_t *sdl);
void chip8_decode_and_execute_instruction(chip8_t *chip8, sdl_t *sdl);

/* display.c */
int sdl_init(sdl_t *sdl);
void sdl_cleanup(sdl_t *sdl);
void sdl_set_renderer_color(sdl_t *sdl, uint8_t color);
void sdl_disp_clear(sdl_t *sdl);
void sdl_update_screen(chip8_t *chip8, sdl_t *sdl);

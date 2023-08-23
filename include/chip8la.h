#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#define MEM_SIZE    0x1000
#define SCREEN_WIDTH    64
#define SCREEN_HEIGHT   32
#define STACK_ENTRIES   20

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

typedef enum BITOP_AND_MATH {
    ADD,
    OR,
    AND,
    XOR,
    SUB,
    LSHIFT,
    RSHIFT,
    Y_MINUS_X,
} bitop_and_math_t;

typedef enum EMULATOR_STATE {
    RUNNING,
    PAUSE,
    QUIT
} chip8_state_t;

enum HEX {
    A = 10,
    B = 11,
    C = 12,
    D = 13,
    E = 14,
    F = 15,
};

typedef struct keyboard_symbol {
    SDL_KeyCode code;
    uint8_t val;
} keyboard_symbol_t;

typedef struct chip8 {
    /* chip8 memory */
    uint8_t memory[MEM_SIZE];

    /* registers */
    uint8_t v[16];
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

    /* stack */
    uint16_t stack[STACK_ENTRIES];
    uint8_t sp;

    /* for tracking set/unset pixels and drawing */
    uint8_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    uint8_t draw_flag;

    /* state of the machine */
    chip8_state_t state;

    /* timers */
    uint8_t sound_timer;
    uint8_t delay_timer;

    /* for handling user input */
    bool keypad[16];
    uint8_t key;
    bool has_key_pressed;
} chip8_t;


/* chip8.c */
void chip8_init(chip8_t *chip8);
void chip8_run_instruction(chip8_t *chip8, sdl_t *sdl);
void chip8_set_register_to_val(chip8_t *chip8, int x, uint16_t val);
void chip8_add_register_with_val(chip8_t *chip8, int x, uint16_t val);
void chip8_handle_user_input(chip8_t *chip8);
void chip8_decode_and_execute_instruction(chip8_t *chip8, sdl_t *sdl);
void chip8_execute_fxnn_instructions(chip8_t *chip8);
uint8_t chip8_get_register_val(chip8_t *chip8, uint16_t reg_num);
void chip8_check_timers(chip8_t *chip8);

/* display.c */
int sdl_init(sdl_t *sdl);
void chip8_disp_clear(chip8_t *chip8, sdl_t *sdl);
void chip8_draw(chip8_t *chip8, uint16_t vx, uint16_t vy, uint16_t n);

void sdl_cleanup(sdl_t *sdl);
void sdl_set_renderer_color(sdl_t *sdl, uint8_t color);
void sdl_disp_clear(chip8_t *chip8);
void sdl_update_screen(chip8_t *chip8, sdl_t *sdl);

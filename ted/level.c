// crafted with love by Austin Adams, 2k17

#include "myLib.h"
#include "level.h"
#include "assets/tile_bg.h"
#include "assets/tile_text1.h"
#include "assets/palette_bg.h"

void level_load_global(level *l, int palette_index, int tile_index) {
    dma32(&bg_palette_mem[palette_index],   palette_bg,           sizeof (PALETTE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_bg_wall,         sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_bg_ground,       sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_bg_nasty,        sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_bg_upper_finish, sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_bg_upper_open,   sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_bg_finish,       sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_bg_open,         sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_bg_roof,         sizeof (TILE));

    // Mark where text tiles begin
    l->text_start = tile_index;

    // Text
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_0,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_1,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_2,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_3,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_4,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_5,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_6,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_7,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_8,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_9,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_M,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_O,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_V,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_E,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_S,          sizeof (TILE));
    dma32(&tile_mem[TILE_BG][tile_index++], tile_text_slash,      sizeof (TILE));
}

void level_new(level *l, int screenblock_index) {
    l->screenblock_index = screenblock_index;
}

void level_set_map(level *l, const map *m) {
    l->map = m;
}

void level_load(level *l) {
    dma32(&bg_screen_entry_mem[l->screenblock_index], l->map->screenblock, sizeof (SCREENBLOCK));
}

void level_update_score(level *l, unsigned int score) {
    // Copy the first row in from the level
    volatile unsigned short first_row[LEVEL_WIDTH];
    dma32(first_row, l->map->screenblock, sizeof first_row);

    int start_col = 10;
    int digit0 = score % 10;
    int digit1 = score / 10 % 10;
    int digit2 = score / 100 % 10;

    // Write the number of moves.
    if (digit2 != 0) {
        first_row[LEVEL_WIDTH - start_col] = l->text_start + digit2;
    }
    start_col--;
    if (digit2 != 0 || digit1 != 0) {
        first_row[LEVEL_WIDTH - start_col] = l->text_start + digit1;
    }
    start_col--;
    first_row[LEVEL_WIDTH - start_col--] = l->text_start + digit0;

    // Now write " MOVES"
    start_col--;
    first_row[LEVEL_WIDTH - start_col--] = l->text_start + 10 + 0; // M
    first_row[LEVEL_WIDTH - start_col--] = l->text_start + 10 + 1; // O
    first_row[LEVEL_WIDTH - start_col--] = l->text_start + 10 + 2; // V
    first_row[LEVEL_WIDTH - start_col--] = l->text_start + 10 + 3; // E
    first_row[LEVEL_WIDTH - start_col--] = l->text_start + 10 + 4; // S

    // Now that we've doctored it up as needed, copy it back to the screenblock
    dma32(&bg_screen_entry_mem[l->screenblock_index], first_row, 30 * 2);
}

void level_open_door(level *l) {
    unsigned short *sb = (unsigned short *) &bg_screen_entry_mem[l->screenblock_index];
    sb[l->map->finish_x + SCREENBLOCK_WIDTH * l->map->finish_y] += 1;
    sb[l->map->finish_x + SCREENBLOCK_WIDTH * (l->map->finish_y + 1)] += 1;
}

void level_close_door(level *l) {
    unsigned short *sb = (unsigned short *) &bg_screen_entry_mem[l->screenblock_index];
    sb[l->map->finish_x + SCREENBLOCK_WIDTH * l->map->finish_y] -= 1;
    sb[l->map->finish_x + SCREENBLOCK_WIDTH * (l->map->finish_y + 1)] -= 1;
}

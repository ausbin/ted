// crafted with love by Austin Adams, 2k17

#ifndef LEVEL_H
#define LEVEL_H

#define SCREENBLOCK_WIDTH 32
#define SCREENBLOCK_HEIGHT 32
#define LEVEL_WIDTH (240 / 8)
#define LEVEL_HEIGHT (160 / 8)

enum map {
    MAP_COLLIDE = 1 << 0,
    MAP_KILL    = 1 << 1
};

typedef struct {
    unsigned int start_x;
    unsigned int start_y;
    unsigned int finish_x;
    unsigned int finish_y;
    unsigned short screenblock[SCREENBLOCK_WIDTH * SCREENBLOCK_HEIGHT];
    int map[LEVEL_WIDTH][LEVEL_HEIGHT];
} map;

typedef struct {
    const map *map;
    int text_start;
    int screenblock_index;
} level;

void level_load_global(level *l, int palette_index, int tile_index);
void level_new(level *l, int screenblock_index);
void level_load(level *l);
void level_update_score(level *l, unsigned int score);
void level_open_door(level *l);
void level_close_door(level *l);
void level_set_map(level *l, const map *m);

#endif

// crafted with love by Austin Adams, 2k17

#include "level.h"

#define TED_NECK_MAX 8

// These correspond to lut_sprite_rot's entries (in particular, they are CCW
// from the y-axis)
enum ted_rot {
    TED_ROT_0DEG,
    TED_ROT_90DEG,
    TED_ROT_180DEG,
    TED_ROT_270DEG
};

enum ted_rot_dir {
    TED_ROT_LEFT,
    TED_ROT_RIGHT
};

typedef struct {
    // Tile indices
    unsigned int x;
    unsigned int y;
    // Range 0 - 8
    unsigned int neck;
    // CCW rotation
    enum ted_rot rot;
    int tile_index;
    int palette_index;
    int oam_index;
    OBJ_ATTR oam_head;
    OBJ_ATTR oam_body;
    OBJ_ATTR oam_neck;
} ted;

void ted_new(ted *t, int tile_index, int palette_index, int oam_index);
void ted_reset(ted *t, const map *m);
void ted_load(ted *t);
void ted_update_affine(ted *t);
void ted_draw(ted *t);
int ted_can_rotate(ted *t, enum ted_rot_dir dir, const int map[LEVEL_WIDTH][LEVEL_HEIGHT]);
void ted_rotate(ted *t, enum ted_rot_dir dir, const int map[LEVEL_WIDTH][LEVEL_HEIGHT]);
int ted_can_extend(ted *t, const int map[LEVEL_WIDTH][LEVEL_HEIGHT]);
int ted_can_retract(ted *t);
void ted_extend(ted *t);
void ted_retract(ted *t);
int ted_dead(ted *t, const int map[LEVEL_WIDTH][LEVEL_HEIGHT]);
void ted_kill(ted *t);
int ted_victory(ted *t, unsigned int finish_x, unsigned int finish_y);
void ted_hide(ted *t);

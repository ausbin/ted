// crafted with love by Austin Adams, 2k17

#include "myLib.h"
#include "ted.h"
#include "luts.h"
#include "level.h"
#include "assets/tile_ted.h"
#include "assets/palette_ted.h"

// How much each body segment should be offset, indexed by enum ted_rot
// (0deg,90deg,180deg,270deg)
static const int rot_segment_offset_x[] = {0, 8, 0, -8};
static const int rot_segment_offset_y[] = {8, 0, -8, 0};

void ted_new(ted *t, int tile_index, int palette_index, int oam_index) {
    t->x = 0;
    t->y = 0;
    t->neck = 0;
    t->rot = TED_ROT_0DEG;
    t->tile_index = tile_index;
    t->palette_index = palette_index;
    t->oam_index = oam_index;

    // Create OAM
    unsigned short attr0 = ATTR0_SQUARE | ATTR0_16COLOR | ATTR0_HIDE;
    unsigned short attr1 = ATTR1_SQUARE;
    unsigned short attr2 = t->tile_index;
    t->oam_head.attr0 = attr0;
    t->oam_head.attr1 = attr1;
    t->oam_head.attr2 = attr2;
    t->oam_body.attr0 = attr0;
    t->oam_body.attr1 = attr1;
    t->oam_body.attr2 = attr2 + 2; // Skip body_neck
    t->oam_neck.attr0 = attr0;
    t->oam_neck.attr1 = attr1;
    t->oam_neck.attr2 = attr2 + 3; // Skip to neck_mid
}

void ted_load(ted *t) {
    dma32(&sprite_palette_mem[t->palette_index], palette_ted, sizeof (PALETTE)/2);

    // Load all the good ol' tiles
    int tile_index = t->tile_index;
    dma32(&tile_mem[TILE_SPRITE_LOW][tile_index++], tile_ted_head,      sizeof (TILE));
    dma32(&tile_mem[TILE_SPRITE_LOW][tile_index++], tile_ted_body_neck, sizeof (TILE));
    dma32(&tile_mem[TILE_SPRITE_LOW][tile_index++], tile_ted_body,      sizeof (TILE));
    dma32(&tile_mem[TILE_SPRITE_LOW][tile_index++], tile_ted_neck_mid,  sizeof (TILE));
    dma32(&tile_mem[TILE_SPRITE_LOW][tile_index++], tile_ted_neck_tip,  sizeof (TILE));
    dma32(&tile_mem[TILE_SPRITE_LOW][tile_index++], tile_ted_head_dead, sizeof (TILE));

    ted_update_affine(t);
}

void ted_reset(ted *t, const map *m) {
    t->rot = TED_ROT_0DEG;
    t->x = m->start_x;
    t->y = m->start_y;
}

// Once rot changes, update affine
void ted_update_affine(ted *t) {
    // Create affine attr for head
    OBJ_AFFINE aff;
    lut_entry_sprite_rot rot = lut_sprite_rot[t->rot];
    aff.pa = rot.pa;
    aff.pb = rot.pb;
    aff.pc = rot.pc;
    aff.pd = rot.pd;
    affine_write(affine_mem, &aff);
}

// Once position (x,y) or neck has changed, redraw ol' ted
void ted_draw(ted *t) {
    int base_x = 8 * t->x;
    int base_y = 8 * t->y;
    lut_entry_sprite_rot rot = lut_sprite_rot[t->rot];
    // How much each body segment should be offset
    int segment_offset_x = rot_segment_offset_x[t->rot];
    int segment_offset_y = rot_segment_offset_y[t->rot];
    unsigned short attr0 = (t->oam_head.attr0 & ~ATTR0_HIDE & ~ATTR0_Y_MASK) | ATTR0_AFFINE_DOUBLE;
    unsigned short attr1 = (t->oam_head.attr1 & ~ATTR1_X_MASK);

    // Reposition head
    t->oam_head.attr0 = attr0 | (base_y + rot.y_offset);
    t->oam_head.attr1 = attr1 | (base_x + rot.x_offset);
    oam_write(&oam_mem[t->oam_index], &t->oam_head);

    // Reposition body
    t->oam_body.attr0 = attr0 | (base_y + rot.y_offset + (t->neck + 1) * segment_offset_y);
    t->oam_body.attr1 = attr1 | (base_x + rot.x_offset + (t->neck + 1) * segment_offset_x);
    oam_write(&oam_mem[t->oam_index + 1], &t->oam_body);

    // Reposition neck
    for (unsigned int i = 0; i < t->neck; i++) {
        t->oam_neck.attr0 = attr0 | (base_y + rot.y_offset + (i + 1) * segment_offset_y);
        t->oam_neck.attr1 = attr1 | (base_x + rot.x_offset + (i + 1) * segment_offset_x);
        oam_write(&oam_mem[t->oam_index + i + 2], &t->oam_neck);
    }
    // Hide rest of neck
    for (unsigned int i = 0; i < TED_NECK_MAX - t->neck; i++) {
        t->oam_neck.attr0 = ATTR0_HIDE;
        oam_write(&oam_mem[t->oam_index + i + t->neck + 2], &t->oam_neck);
    }
}

// Change Ted's head to the dead head
void ted_kill(ted *t) {
    t->oam_head.attr2 = (t->oam_head.attr2 & ~ATTR2_TID_MASK) | (t->tile_index + 5);
    oam_write(&oam_mem[t->oam_index], &t->oam_head);
}

// Collision detection
int ted_can_rotate(ted *t, enum ted_rot_dir dir, const int map[LEVEL_WIDTH][LEVEL_HEIGHT]) {
    int height = t->neck + 2;
    int upright = t->rot == TED_ROT_0DEG || t->rot == TED_ROT_180DEG;

    // These vary based on the direction and direction
    int size;
    int check_x;
    int check_y;
    int x_sign = 1;
    int y_sign = 1;

    if (upright && dir == TED_ROT_RIGHT) {
        size = height;
        check_x = t->x + 1;
        if (t->rot == TED_ROT_0DEG) {
            check_y = t->y;
        } else { // t->rot == TED_ROT_180DEG
            check_y = t->y - height + 1;
        }
    } else if (upright && dir == TED_ROT_LEFT) {
        x_sign = -1;
        size = height;
        check_x = t->x - 1;
        if (t->rot == TED_ROT_0DEG) {
            check_y = t->y;
        } else { // t->rot == TED_ROT_180DEG
            check_y = t->y - height + 1;
        }
    } else if (!upright && dir == TED_ROT_RIGHT) {
        x_sign = -1;
        size = height - 1;
        check_y = t->y - height + 1;
        if (t->rot == TED_ROT_90DEG) {
            check_x = t->x + height - 1;
        } else { // t->rot == TED_ROT_270DEG
            check_x = t->x;
        }
    } else { // !upright && dir == TED_ROT_LEFT
        size = height - 1;
        check_y = t->y - height + 1;
        if (t->rot == TED_ROT_90DEG) {
            check_x = t->x;
        } else { // t->rot == TED_ROT_270DEG
            check_x = t->x - height + 1;
        }
    }

    for (int cy = 0; cy < size; cy++) {
        for (int cx = 0; cx <= cy; cx++) {
            int x = check_x + x_sign * cx;
            int y = check_y + y_sign * cy;
            if (x >= LEVEL_WIDTH || y >= LEVEL_HEIGHT || map[x][y] & MAP_COLLIDE) {
                return 0;
            }
        }
    }

    // Extra check for left/right side
    if (!upright) {
        if (dir == TED_ROT_RIGHT) {
            check_x += 1;
        } else { // dir == TED_ROT_LEFT
            check_x -= 1;
        }

        for (int cy = 0; cy < height; cy++) {
            int x = check_x;
            int y = check_y + cy;
            if (x >= LEVEL_WIDTH || y >= LEVEL_HEIGHT || map[x][y] & MAP_COLLIDE) {
                return 0;
            }
        }
    }

    // Finally, check that Ted would be resting on something if he's upright
    // and flipping sideways
    if (upright) {
        int test_left_x, test_y;

        if (dir == TED_ROT_RIGHT) {
            test_left_x = t->x + 1;
        } else { // t->rot == TED_ROT_LEFT
            test_left_x = t->x - height;
        }
        if (t->rot == TED_ROT_0DEG) {
            test_y = t->y + height;
        } else { // t->rot == TED_ROT_180DEG
            test_y = t->y + 1;
        }

        // Can't rotate sideways if there's nothing for Ted's body to rest on!
        if (!(map[test_left_x][test_y] & MAP_COLLIDE) ||
            !(map[test_left_x + height - 1][test_y] & MAP_COLLIDE)) {
            return 0;
        }
    }

    return 1;
}

void ted_rotate(ted *t, enum ted_rot_dir dir, const int map[LEVEL_WIDTH][LEVEL_HEIGHT]) {
    int height = t->neck + 2;
    int new_x = t->x;
    int new_y = t->y;
    enum ted_rot new_rot;

    // Upright
    if (t->rot == TED_ROT_0DEG && dir == TED_ROT_RIGHT) {
        new_x += height;
        new_y += height - 1;
        new_rot = TED_ROT_270DEG;
    } else if (t->rot == TED_ROT_180DEG && dir == TED_ROT_RIGHT) {
        new_x += 1;
        new_rot = TED_ROT_90DEG;
    } else if (t->rot == TED_ROT_0DEG && dir == TED_ROT_LEFT) {
        new_x -= height;
        new_y += height - 1;
        new_rot = TED_ROT_90DEG;
    } else if (t->rot == TED_ROT_180DEG && dir == TED_ROT_LEFT) {
        new_x -= 1;
        new_rot = TED_ROT_270DEG;
    // Sideways
    } else if (t->rot == TED_ROT_90DEG && dir == TED_ROT_RIGHT) {
        new_x += height;
        new_y -= height - 1;
        new_rot = TED_ROT_0DEG;
    } else if (t->rot == TED_ROT_270DEG && dir == TED_ROT_RIGHT) {
        new_x += 1;
        new_rot = TED_ROT_180DEG;
    } else if (t->rot == TED_ROT_90DEG && dir == TED_ROT_LEFT) {
        new_x -= 1;
        new_rot = TED_ROT_180DEG;
    } else { // t->rot == TED_ROT_270DEG && dir == TED_ROT_LEFT
        new_x -= height;
        new_y -= height - 1;
        new_rot = TED_ROT_0DEG;
    }

    // If Ted will now be upright, gravity him!
    if (new_rot == TED_ROT_0DEG || new_rot == TED_ROT_180DEG) {
        int resting_y;
        if (new_rot == TED_ROT_0DEG) {
            resting_y = new_y + height;
        } else { // new_rot == TED_ROT_180DEG
            resting_y = new_y + 1;
        }

        while (resting_y < LEVEL_HEIGHT && !(map[new_x][resting_y] & MAP_COLLIDE)) {
            resting_y++;
        }

        if (new_rot == TED_ROT_0DEG) {
            new_y = resting_y - height;
        } else { // new_rot == TED_ROT_180DEG
            new_y = resting_y -  1;
        }
    }

    t->x = new_x;
    t->y = new_y;
    t->rot = new_rot;
    ted_update_affine(t);
}

int ted_can_extend(ted *t, const int map[LEVEL_WIDTH][LEVEL_HEIGHT]) {
    return t->neck < TED_NECK_MAX &&
           ((t->rot == TED_ROT_0DEG && !(map[t->x][t->y - 1] & MAP_COLLIDE)) ||
            (t->rot == TED_ROT_180DEG && !(map[t->x][t->y - (t->neck + 2)] & MAP_COLLIDE)));
}

int ted_can_retract(ted *t) {
    return t->neck > 0 && (t->rot == TED_ROT_0DEG || t->rot == TED_ROT_180DEG);
}

void ted_extend(ted *t) {
    t->neck++;

    if (t->rot == TED_ROT_0DEG) {
        t->y--;
    }
}

void ted_retract(ted *t) {
    t->neck--;

    if (t->rot == TED_ROT_0DEG) {
        t->y++;
    }
}

int ted_dead(ted *t, const int map[LEVEL_WIDTH][LEVEL_HEIGHT]) {
    int body_x = t->x + (t->neck + 1) * (rot_segment_offset_x[t->rot] / 8);
    int body_y = t->y + (t->neck + 1) * (rot_segment_offset_y[t->rot] / 8);
    return t->y >= LEVEL_HEIGHT - 1 ||
           body_y >= LEVEL_HEIGHT - 1 || // If Ted has fallen off the map
           map[t->x][t->y] & MAP_KILL ||
           map[body_x][body_y] & MAP_KILL;
}

int ted_victory(ted *t, unsigned int finish_x, unsigned int finish_y) {
    return t->neck == 0 && t->rot == TED_ROT_0DEG && t->x == finish_x && t->y == finish_y;
}

// XXX make this work in more cases than just !!ted_victory()
void ted_hide(ted *t) {
    t->oam_head.attr0 = (t->oam_head.attr0 & ~ATTR0_AFFINE_DOUBLE) | ATTR0_HIDE;
    oam_write(&oam_mem[t->oam_index], &t->oam_head);
    t->oam_body.attr0 = (t->oam_body.attr0 & ~ATTR0_AFFINE_DOUBLE) | ATTR0_HIDE;
    oam_write(&oam_mem[t->oam_index + 1], &t->oam_body);
}

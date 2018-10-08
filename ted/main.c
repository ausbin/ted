// crafted with love by Austin Adams, 2k17

#include "myLib.h"
#include "ted.h"
#include "victory.h"
#include "assets/map_intro.h"
#include "assets/map_gravity.h"
#include "assets/map_finale.h"
#include "assets/image_title.h"
#include "assets/image_gameover.h"

enum state {
    STATE_START,
    STATE_TITLE,
    STATE_NEXT_LEVEL,
    STATE_LEVEL,
    STATE_ANIMATION,
    STATE_GAMEOVER,
    STATE_VICTORY
};

enum animation {
    ANIM_DEATH,
    ANIM_VICTORY
};

static const map *maps[] = {&map_intro, &map_gravity, &map_finale};
static const unsigned int map_count = sizeof maps / sizeof maps[0];

int main() {
    ted t;
    level l;
    unsigned int map_i;
    unsigned int moves;
    unsigned int anim_start;
    enum animation anim;

    unsigned int down = 0;
    unsigned int vblanks = 0;
    enum state state = STATE_START;

    while (1) {
        waitForVblank();
        vblanks++;

        // Note when keys are released
        if (!KEY_DOWN_NOW(KEY_SELECT)) down &= ~KEY_SELECT;
        if (!KEY_DOWN_NOW(KEY_L))      down &= ~KEY_L;
        if (!KEY_DOWN_NOW(KEY_R))      down &= ~KEY_R;
        if (!KEY_DOWN_NOW(KEY_UP))     down &= ~KEY_UP;
        if (!KEY_DOWN_NOW(KEY_DOWN))   down &= ~KEY_DOWN;

        switch (state) {
        case STATE_START:
            REG_DISPCNT = DCNT_MODE_3 | DCNT_BG2_EN;
            drawImage3(0, 0, image_width_title, image_height_title, image_title);
            state = STATE_TITLE;
            break;
        case STATE_TITLE:
            if (KEY_PRESSED(KEY_SELECT, down)) {
                state = STATE_NEXT_LEVEL;

                map_i = 0;
                moves = 0;

                // Setup level
                int screenblock_index = 8;
                REG_DISPCNT = DCNT_MODE_0 | DCNT_BG0_EN | DCNT_OBJ | DCNT_OBJ_1D;
                REG_BG0CNT = BG_REG_32x32 | (screenblock_index << 8);
                level_new(&l, screenblock_index);
                level_load_global(&l, 0, 0);
                oam_clear_all();
                ted_new(&t, 0, 0, 0);
                ted_load(&t);
            }
            break;
        case STATE_NEXT_LEVEL:
            if (KEY_PRESSED(KEY_SELECT, down)) {
                state = STATE_START;
            } else {
                state = STATE_LEVEL;

                level_set_map(&l, maps[map_i]);
                // Load the screenblock
                level_load(&l);
                // Update the screenblock with the score
                level_update_score(&l, moves);
                // Reset ted
                ted_reset(&t, l.map);
                ted_draw(&t);
            }
            break;
        case STATE_LEVEL: ;
            int dead = 0;
            int extend = 0;
            int retract = 0;
            int rotate = 0;
            enum ted_rot_dir dir;

            if (KEY_PRESSED(KEY_SELECT, down)) {
                state = STATE_START;
            }
            if (KEY_PRESSED(KEY_L, down)) {
                rotate = 1;
                dir = TED_ROT_LEFT;
            }
            if (KEY_PRESSED(KEY_R, down)) {
                rotate = 1;
                dir = TED_ROT_RIGHT;
            }
            if (KEY_PRESSED(KEY_UP, down)) {
                extend = 1;
            }
            if (KEY_PRESSED(KEY_DOWN, down)) {
                retract = 1;
            }

            if (rotate) {
                if (ted_can_rotate(&t, dir, l.map->map)) {
                    ted_rotate(&t, dir, l.map->map);
                    ted_draw(&t);

                    moves++;
                    if (moves == 1000) {
                        dead = 1;
                    } else {
                        level_update_score(&l, moves);
                    }
                }
            } else if (extend) {
                if (ted_can_extend(&t, l.map->map)) {
                    ted_extend(&t);
                    ted_draw(&t);
                }
            } else if (retract) {
                if (ted_can_retract(&t)) {
                    ted_retract(&t);
                    ted_draw(&t);
                }
            }

            if (dead || ted_dead(&t, l.map->map)) {
                state = STATE_ANIMATION;
                anim = ANIM_DEATH;
                anim_start = vblanks;
            } else if (ted_victory(&t, l.map->finish_x, l.map->finish_y)) {
                state = STATE_ANIMATION;
                anim = ANIM_VICTORY;
                anim_start = vblanks;
            }

            break;
        case STATE_ANIMATION:
            if (KEY_PRESSED(KEY_SELECT, down)) {
                state = STATE_START;
            } else {
                unsigned int delta = vblanks - anim_start;

                // Account for overflow. If anim_start is large, vblanks could
                // have overflowed. I estimate this would require running the
                // game for about two years before triggering an animation
                if (vblanks < anim_start) {
                    delta = -delta;
                }

                // Account for the vblanks++ at the top of the loop between the
                // assignment in the last state, and the first time we enter
                // this state block. We want delta to measure the number of
                // vblanks since the animation was supposed to begin, not since
                // the number of vblanks since it was triggered
                delta -= 1;

                switch (anim) {
                    case ANIM_DEATH:
                        switch (delta) {
                            case DECISECONDS(0): ted_kill(&t); break;
                            case DECISECONDS(10):
                                REG_DISPCNT = DCNT_MODE_3 | DCNT_BG2_EN;
                                drawImage3(0, 0, image_width_gameover, image_height_gameover, image_gameover);
                                state = STATE_GAMEOVER;
                                break;
                        }
                        break;
                    case ANIM_VICTORY:
                        switch (delta) {
                            case DECISECONDS(0):  level_open_door(&l);  break;
                            case DECISECONDS(5):  ted_hide(&t);         break;
                            case DECISECONDS(10): level_close_door(&l); break;
                            case DECISECONDS(15):
                                if (++map_i == map_count) {
                                    state = STATE_VICTORY;

                                    REG_DISPCNT = DCNT_MODE_3 | DCNT_BG2_EN;
                                    victory_draw(moves);
                                } else {
                                    state = STATE_NEXT_LEVEL;
                                }
                                break;
                        }
                        break;
                }
            }
            break;
        case STATE_GAMEOVER:
        case STATE_VICTORY:
            if (KEY_PRESSED(KEY_SELECT, down)) {
                state = STATE_START;
            }
            break;
        }
    }

    return 0;
}

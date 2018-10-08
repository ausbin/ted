// crafted with love by Austin Adams, 2k17

#include "luts.h"

// Lookup tables

// The GBA does not affine sprites around their true center, which means
// sprites will be offset in weird ways when rotated. So this table provides
// the x,y offsets needed to fix the positioning of rotated sprites.
// (Currently, Ted's body parts)
const lut_entry_sprite_rot lut_sprite_rot[] = {
    // deg CCW  x,y offset    affine matrix
    /* 0 */     {-4, -4,     1 << 8,    0,
                               0   ,  1 << 8 },

    /* 90 */    {-4, -5,       0   , -1 << 8,
                             1 << 8,    0    },

    /* 180 */   {-5, -5,    -1 << 8,    0,
                               0   , -1 << 8 },

    /* 270 */   {-5, -4,       0   ,  1 << 8,
                            -1 << 8,    0    }
};

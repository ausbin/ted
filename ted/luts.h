// crafted with love by Austin Adams, 2k17

// Lookup tables

typedef struct {
    // Offset on sprite position to re-center sprite
    int x_offset;
    int y_offset;
    // The affine matrix
    int pa; int pb;
    int pc; int pd;
} lut_entry_sprite_rot;

const lut_entry_sprite_rot lut_sprite_rot[4];

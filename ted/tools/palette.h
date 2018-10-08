// crafted with love by Austin Adams, 2k17

#include <stdio.h>
#include <stdint.h>

typedef struct {
    unsigned int size;
    uint16_t colors[15];
} palette;

palette *palette_new();
void palette_free(palette *p);
int palette_lookup(palette *p, uint8_t red, uint8_t green, uint8_t blue);
int palette_push(palette *p, uint8_t red, uint8_t green, uint8_t blue);
int palette_load(palette *p, FILE *fp);

// crafted with love by Austin Adams, 2k17

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "palette.h"

palette *palette_new() {
    palette *p = malloc(sizeof (palette));

    if (p == NULL) {
        return NULL;
    }

    p->size = 0;

    return p;
}

void palette_free(palette *p) {
    free(p);
}

static uint16_t rgb_to_gba(uint8_t red, uint8_t green, uint8_t blue) {
    red = red >> 3;
    green = green >> 3;
    blue = blue >> 3;
    return (blue << 10) | (green << 5) | red;
}

int palette_lookup(palette *p, uint8_t red, uint8_t green, uint8_t blue) {
    uint16_t color = rgb_to_gba(red, green, blue);
    int i;
    for (i = 0; i < p->size && p->colors[i] != color; i++);

    if (i == p->size) {
        return -1;
    } else {
        return i;
    }
}

// Add/lookup color. Returns new index.
// Remember to add one to the result, since palette index 0 is transparent
int palette_push(palette *p, uint8_t red, uint8_t green, uint8_t blue) {
    int loc = palette_lookup(p, red, green, blue);

    // Add the color only if not already present
    if (loc == -1) {
        // Full
        if (p->size == sizeof p->colors / sizeof p->colors[0]) {
            // XXX Don't set errno, what the heck man
            errno = EFBIG;
            return 1;
        }

        p->colors[p->size++] = rgb_to_gba(red, green, blue);
    }

    return 0;
}

int palette_load(palette *p, FILE *fp) {
    char *line;
    size_t n;
    ssize_t len;

    // getline() dereferences pointers passed in(!), so these need to be
    // initialized
    line = NULL;
    n = 0;

    while ((len = getline(&line, &n, fp)) != -1) {
        unsigned int red, green, blue;

        red = green = blue = 255;

        if (sscanf(line, "%02x%02x%02x", &red, &green, &blue) != 3) {
            return 1;
        }

        if (palette_push(p, red, green, blue)) {
            return 1;
        }
    }

    return 0;
}

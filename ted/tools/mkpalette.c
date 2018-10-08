// crafted with love by Austin Adams, 2k17

#include <stdio.h>
#include <stdlib.h>
#include "palette.h"

int main(int argc, char **argv) {
    if (argc - 1 != 1 + 1) {
        fprintf(stderr, "usage: %s <pal.palette> <palettename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *pal_file = fopen(argv[1], "r");

    if (pal_file == NULL) {
        perror("fopen on palette file");
        return EXIT_FAILURE;
    }

    palette *pal = palette_new();

    if (palette_load(pal, pal_file)) {
        perror("reading palette file");
        return EXIT_FAILURE;
    }

    printf("const unsigned short palette_%s[] = {\n\t", argv[2]);
    for (int i = 0; i < 16; i++) {
        uint16_t color;

        // Include an inaccessible dummy value as the first element, and pad the rest
        if (i == 0 || i > pal->size) {
            // Filler value
            color = 0xbeef;
        } else {
            color = pal->colors[i - 1];
        }

        printf("0x%04x%s", color, (i < 15)? ", " : "");
    }
    printf("\n};\n");

    return EXIT_SUCCESS;
}

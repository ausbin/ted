// crafted with love by Austin Adams, 2k17

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "specfile.h"

#define BG_PREFIX ("bg_")
#define LEVEL_WIDTH (240 / 8)
#define LEVEL_HEIGHT (160 / 8)
#define PADDED_WIDTH 32
#define PADDED_HEIGHT 32

int main(int argc, char **argv) {
    if (argc - 1 != 3) {
        fprintf(stderr, "usage: %s <levelname> <x.level> <y.spec>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *level_file = fopen(argv[2], "r");

    if (level_file == NULL) {
        perror("fopen on level file");
        return EXIT_FAILURE;
    }

    FILE *spec_file = fopen(argv[3], "r");

    if (spec_file == NULL) {
        perror("fopen on spec file");
        return EXIT_FAILURE;
    }

    spec_collection *sc = spec_collection_new();

    if (sc == NULL) {
        perror("malloc for specfile");
        return EXIT_FAILURE;
    }

    if (spec_collection_load(sc, spec_file)) {
        perror("reading specfile");
        return EXIT_FAILURE;
    }

    int start_x = 0, start_y = 0, finish_x = 0, finish_y = 0;
    int sprites[LEVEL_WIDTH][LEVEL_HEIGHT];
    int backgrounds[LEVEL_WIDTH][LEVEL_HEIGHT];
    int map[LEVEL_WIDTH][LEVEL_HEIGHT];
    memset(map, 0, sizeof map);

    char *line = NULL;
    size_t n = 0;
    ssize_t len;
    int x, y;

    y = 0;
    while ((len = getline(&line, &n, level_file)) != -1) {
        if (len != 2 * LEVEL_WIDTH + 1) {
            free(line);
            fprintf(stderr, "%s: line %d is too short. %ld < %d\n", argv[0],
                            y, len, LEVEL_WIDTH * 2);
            return EXIT_FAILURE;
        }

        for (x = 0; x < LEVEL_WIDTH; x++) {
            int start = 2 * x;
            char sprite, bg;

            sprite = line[start];
            bg = line[start + 1];

            int bg_index;
            if (bg == ' ') {
                // Assume the first index is transparent
                bg_index = 0;
            } else {
                // Caps means collision
                if (bg >= 'A' && bg <= 'Z') {
                    bg += 'a' - 'A';
                    map[x][y] |= 1;
                }

                if ((bg_index = spec_collection_lookup(sc, BG_PREFIX, bg)) == -1) {
                    free(line);
                    fprintf(stderr, "%s: unknown bg tile %c\n", argv[0], bg);
                    return EXIT_FAILURE;
                }
            }

            backgrounds[x][y] = bg_index;

            if (sprite == '*') {
                sprite = ' ';
                start_x = x;
                start_y = y;
            } else if (sprite == 'X') {
                sprite = ' ';
                map[x][y] |= 1 << 1;
            } else if (sprite == '+') {
                sprite = ' ';
                finish_x = x;
                finish_y = y;
            }
            sprites[x][y] = sprite;
        }

        y++;
        free(line);
        line = NULL;
        n = 0;
    }

    // Now, print the results.
    printf("#include \"../level.h\"\n\n");
    printf("const map map_%s = {\n\t%d, %d, %d, %d,\n", argv[1],
           start_x, start_y, finish_x, finish_y);
    printf("\t{");

    for (y = 0; y < PADDED_HEIGHT; y++) {
        for (x = 0; x < PADDED_WIDTH; x++) {
            unsigned short entry;
            int sprite;

            if (x >= LEVEL_WIDTH || y >= LEVEL_HEIGHT) {
                entry = 0;
                sprite = ' ';
            } else {
                entry = backgrounds[x][y];
                sprite = sprites[x][y];
            }

            printf("0x%04x /* %c */%s", entry, sprite,
                   (y == PADDED_HEIGHT - 1 && x == PADDED_WIDTH - 1)? "" : ", ");
        }
    }
    printf("},\n");

    printf("\t{");
    for (x = 0; x < LEVEL_WIDTH; x++) {
        printf("{");
        for (y = 0; y < LEVEL_HEIGHT; y++) {
            printf("%d%s", map[x][y], (y == LEVEL_HEIGHT - 1)? "" : ", ");
        }
        printf("}%s", (x == LEVEL_WIDTH - 1)? "" : ", ");
    }
    printf("}\n};\n");

    return EXIT_SUCCESS;
}

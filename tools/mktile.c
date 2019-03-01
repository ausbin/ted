// crafted with love by Austin Adams, 2k17

// Converts a png+specfile to a tile

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include "specfile.h"
#include "palette.h"

int main(int argc, char **argv) {
    if (argc - 1 != 1 + 1 + 1 + 1) {
        fprintf(stderr, "usage: %s <tileset.png> <tilesetname> <tileset.spec> <pal.palette>\n", argv[0]);
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

    FILE *pal_file = fopen(argv[4], "r");

    if (pal_file == NULL) {
        perror("fopen on palette file");
        return EXIT_FAILURE;
    }

    palette *pal = palette_new();

    if (pal == NULL) {
        perror("malloc for palette");
        return EXIT_FAILURE;
    }

    if (palette_load(pal, pal_file)) {
        perror("reading palette file");
        return EXIT_FAILURE;
    }

    FILE *src_file; 
    
    if (!(src_file = fopen(argv[1], "rb"))) {
        fprintf(stderr, "%s: could not open `%s': %s\n", argv[0], argv[1],
                strerror(errno));
        return EXIT_FAILURE;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                                 NULL, NULL);

    if (!png_ptr) {
        fprintf(stderr, "%s: could not allocate png_struct\n", argv[0]);
        return EXIT_FAILURE;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (!info_ptr) {
        fprintf(stderr, "%s: could not allocate png_info\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Need to be volatile since they're modified after setjmp() is called
    // below.
    unsigned char tiles[16][32];
    memset(tiles, 0, sizeof tiles);

    volatile png_bytep row = NULL;

    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
        png_uint_32 width, height;
        int bit_depth, color_type, interlace_method, compression_method,
            filter_method;

        // Associate png_ptr with src_file
        png_init_io(png_ptr, src_file);
        // Read PNG header
        png_read_info(png_ptr, info_ptr);

        row = png_malloc(png_ptr, png_get_rowbytes(png_ptr, info_ptr));

        if (!png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
                          &color_type, &interlace_method, &compression_method,
                          &filter_method)) {
            png_error(png_ptr, "png_get_IHDR failed");
        }

        if (bit_depth != 4 && bit_depth != 8) {
            png_error(png_ptr, "Sorry, I understand only bit depths 4 and 8.");
        }

        if (color_type != PNG_COLOR_TYPE_PALETTE) {
            png_error(png_ptr, "Please use a palette color type, not RGBA etc.");
        }

        if (interlace_method != PNG_INTERLACE_NONE) {
            png_error(png_ptr, "Please don't use interlacing; I'm too lazy to handle it.");
        }

        png_start_read_image(png_ptr);

        // Read palette
        png_colorp palette = NULL;
        int palette_size = 0;

        if (png_get_PLTE(png_ptr, info_ptr, &palette, &palette_size)
            != PNG_INFO_PLTE || palette == NULL) {
            png_error(png_ptr, "Couldn't read palette");
        }

        // Check for transparency
        png_bytep trans = NULL;
        int trans_size = 0;

        png_get_tRNS(png_ptr, info_ptr, &trans, &trans_size, NULL);

        for (png_uint_32 py = 0; py < height; py++) {
            png_read_row(png_ptr, row, NULL);

            for (png_uint_32 px = 0; px < width; px++) {
                // Index in color palette
                int index;

                if (bit_depth == 4) {
                    index = row[px / 2] >> 4 * !(px % 2) & 0xf;
                } else { // bit_depth == 8
                    index = row[px];
                }

                png_byte alpha = (trans_size > 0 && index < trans_size)? trans[index] : 255;
                int color_i = 0;

                if (alpha) {
                    color_i = palette_lookup(pal, palette[index].red,
                                                  palette[index].green,
                                                  palette[index].blue);

                    if (color_i == -1) {
                        png_error(png_ptr, "Color not found in palette");
                    }

                    // Remember: if a tile pixel = 0, it's transparent. So account for this offset.
                    color_i++;
                }

                int tile_num = px / 8 + py / 8 * 4;
                int pixel_num = px % 8 + py % 8 * 8;

                tiles[tile_num][pixel_num / 2] |= color_i << 4 * (pixel_num % 2);
            }
        }

        row = NULL;
        png_free(png_ptr, row);

    } else {
        if (row != NULL) {
            // Just in case
            png_bytep row_tmp = row;
            row = NULL;
            png_free(png_ptr, row_tmp);
        }

        return EXIT_FAILURE;
    }

    printf("const unsigned int tile_count_%s = %d;\n\n", argv[2],
           spec_collection_nonempty_cells(sc));

    // Write tiles
    for (int i = 0; i < 16; i++) {
        char *name = spec_collection_get(sc, i);

        // Don't write tiles where the specfile has a .
        if (name == NULL) {
            continue;
        }

        printf("const unsigned char tile_%s[] = {\n\t", name);

        for (int j = 0; j < 32; j++) {
            printf("0x%02x%s", tiles[i][j], (j < 31)? ", " : "");
        }

        printf("\n};\n\n");
    }

    return EXIT_SUCCESS;
}

// crafted with love by Austin Adams, 2k17

// Converts a png to a mode 3 image

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <stdint.h>

int main(int argc, char **argv) {
    if (argc - 1 != 1 + 1 + 1) {
        fprintf(stderr, "usage: %s <image name> <image.png> <dest.h>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *header_file;

    if (!(header_file = fopen(argv[3], "w"))) {
        fprintf(stderr, "%s: could not open header file `%s': %s\n", argv[0], argv[3],
                strerror(errno));
        return EXIT_FAILURE;
    }

    FILE *src_file;

    if (!(src_file = fopen(argv[2], "rb"))) {
        fprintf(stderr, "%s: could not open `%s': %s\n", argv[0], argv[2],
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

    // Need to be volatile since it's modified after setjmp() is called below
    volatile uint16_t image[240][160];
    volatile int image_width = 0, image_height = 0;
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

        image_width = width;
        image_height = height;

        if (bit_depth != 8) {
            png_error(png_ptr, "Sorry, I understand only bit depth 8.");
        }

        if (color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGB_ALPHA) {
            png_error(png_ptr, "Please use a palette color type, not RGBA etc.");
        }

        if (interlace_method != PNG_INTERLACE_NONE) {
            png_error(png_ptr, "Please don't use interlacing; I'm too lazy to handle it.");
        }

        png_start_read_image(png_ptr);

        for (png_uint_32 py = 0; py < height; py++) {
            png_read_row(png_ptr, row, NULL);

            for (png_uint_32 px = 0; px < width; px++) {
                int channels;
                unsigned int red, green, blue;

                if (color_type == PNG_COLOR_TYPE_RGB) {
                    channels = 3;
                } else { // color_type == PNG_COLOR_TYPE_RGB_ALPHA
                    channels = 4;
                }

                red   = row[channels * px + 0] >> 3;
                green = row[channels * px + 1] >> 3;
                blue  = row[channels * px + 2] >> 3;

                image[px][py] = blue << 10 | green << 5 | red;
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

    printf("const unsigned int image_width_%s = %d;\n", argv[1], image_width);
    printf("const unsigned int image_height_%s = %d;\n", argv[1], image_height);
    printf("const unsigned short image_%s[] = {\n\t", argv[1]);
    for (int py = 0; py < image_height; py++) {
        for (int px = 0; px < image_width; px++) {
            printf("0x%04x%s", image[px][py], (px < image_width-1 || py < image_height-1)? ", " : "");
        }
    }
    printf("\n};\n");

    fprintf(header_file, "const unsigned int image_width_%s;\n", argv[1]);
    fprintf(header_file, "const unsigned int image_height_%s;\n", argv[1]);
    fprintf(header_file, "const unsigned short image_%s[%d];\n", argv[1], image_width * image_height);
    fclose(header_file);

    return EXIT_SUCCESS;
}

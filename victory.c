// crafted with love by Austin Adams, 2k17

#include "myLib.h"
#include "assets/image_victory.h"
#include "assets/image_0.h"
#include "assets/image_1.h"
#include "assets/image_2.h"
#include "assets/image_3.h"
#include "assets/image_4.h"
#include "assets/image_5.h"
#include "assets/image_6.h"
#include "assets/image_7.h"
#include "assets/image_8.h"
#include "assets/image_9.h"

static const unsigned short *victory_digit_image(int digit) {
    switch (digit) {
    case 9: return image_9;
    case 8: return image_8;
    case 7: return image_7;
    case 6: return image_6;
    case 5: return image_5;
    case 4: return image_4;
    case 3: return image_3;
    case 2: return image_2;
    case 1: return image_1;
    case 0: default: return image_0;
    }
}

void victory_draw(unsigned int moves) {
    drawImage3(0, 0, image_width_victory, image_height_victory, image_victory);

    // Sadly, much of this is hardcoded from eyeballing the image
    int start_x = 40 + (160 - image_width_0 * 3) / 2;
    int start_y = 95;
    int digit0 = moves % 10;
    int digit1 = moves / 10 % 10;
    int digit2 = moves / 100 % 10;

    drawImage3(start_x, start_y, image_width_0, image_height_0, victory_digit_image(digit2));
    start_x += image_width_0;
    drawImage3(start_x, start_y, image_width_0, image_height_0, victory_digit_image(digit1));
    start_x += image_width_0;
    drawImage3(start_x, start_y, image_width_0, image_height_0, victory_digit_image(digit0));
}

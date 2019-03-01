// crafted with love by Austin Adams, 2k17

#include <stddef.h>
#include "myLib.h"

// Miscellaneous GBA stuff

unsigned short *videoBuffer = (unsigned short*) 0x6000000;

void waitForVblank() {
    while(SCANLINECOUNTER >= 160);
    while(SCANLINECOUNTER < 160);
}

// Set every sprite in OAM to hidden, 32 bits at a time.
// Prevents excess sprites from appearing in the upper left.
// WARNING: This _will_ nuke any affine thingies (attr3)
void oam_clear_all() {
    // This approach is a little iffy, but I wanted to use DMA
    unsigned int dma_cleared = ATTR0_HIDE;
    dma32_fixedsrc(oam_mem, &dma_cleared, 128 * sizeof (OBJ_ATTR));
}

void oam_write(volatile OBJ_ATTR *dest, OBJ_ATTR *source) {
    // We don't want to overwrite attribute 3 (which is used by affine
    // thingies), so be careful to write only attributes 0-2 here.
    dma16(dest, source, sizeof (short) * 3);
}

void affine_write(volatile OBJ_AFFINE *dest, OBJ_AFFINE *source) {
    // Similarly to above, we don't want to overwrite OAM while updating affine
    // matrices.
    short *ptr = (short *) dest;

    // We want to skip:
    // ((num of matrix entries to skip) + 3*(num of OAMs to skip)) shorts
    // Look at the OBJ_AFFINE struct in myLib.h if this doesn't make sense
    ptr[0 + 3*1] = source->pa;
    ptr[1 + 3*2] = source->pb;
    ptr[2 + 3*3] = source->pc;
    ptr[3 + 3*4] = source->pd;
}

void dma32(volatile void *dst, volatile const void *src, size_t n) {
    DMA[3].src = src;
    DMA[3].dst = dst;
    DMA[3].cnt = DMA_ON | DMA_NOW | DMA_32 | (n / 4);
}

void dma16(volatile void *dst, volatile const void *src, size_t n) {
    DMA[3].src = src;
    DMA[3].dst = dst;
    DMA[3].cnt = DMA_ON | DMA_NOW | DMA_16 | (n / 2);
}

void dma32_fixedsrc(volatile void *dst, volatile const void *src, size_t n) {
    DMA[3].src = src;
    DMA[3].dst = dst;
    DMA[3].cnt = DMA_ON | DMA_NOW | DMA_32 | DMA_SOURCE_FIXED | (n / 4);
}

void dma16_fixedsrc(volatile void *dst, volatile const void *src, size_t n) {
    DMA[3].src = src;
    DMA[3].dst = dst;
    DMA[3].cnt = DMA_ON | DMA_NOW | DMA_16 | DMA_SOURCE_FIXED | (n / 2);
}

void drawImage3(int x, int y, int width, int height, const unsigned short *image) {
    if (x == 0 && y == 0 && width == 240 && height == 160) {
        dma32(videoBuffer, image, 240 * 160 * 2);
    } else {
        for (int py = 0; py < height; py++) {
            dma16(videoBuffer + x + (y + py) * 240, image + py * width, width * 2);
        }
    }
}

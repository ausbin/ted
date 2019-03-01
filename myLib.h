// crafted with love by Austin Adams, 2k17

#include <stddef.h>
#include "dma.h"

// Meta
#define ALIGN4 __attribute__((aligned(4)))

// Registers
#define REG_DISPCNT *(volatile unsigned short *) 0x4000000
#define REG_BG0CNT *(volatile unsigned short *) 0x4000008
#define REG_BG1CNT *(volatile unsigned short *) 0x400000A
#define REG_BG2CNT *(volatile unsigned short *) 0x400000C
#define REG_BG3CNT *(volatile unsigned short *) 0x400000E
#define REG_BG0X *(volatile unsigned short *) 0x4000010
#define REG_BG0Y *(volatile unsigned short *) 0x4000012
#define REG_BG1X *(volatile unsigned short *) 0x4000014
#define REG_BG1Y *(volatile unsigned short *) 0x4000016
#define REG_BG2X *(volatile unsigned short *) 0x4000018
#define REG_BG2Y *(volatile unsigned short *) 0x400001A
#define REG_BG3X *(volatile unsigned short *) 0x400001C
#define REG_BG3Y *(volatile unsigned short *) 0x400001E
#define REG_KEYINPUT *(volatile unsigned short *) 0x04000130
#define REG_KEYCNT *(volatile unsigned short *) 0x04000132
#define SCANLINECOUNTER *(volatile unsigned short *) 0x4000006
// Useful. The approximate number of vblanks in ds deciseconds
#define DECISECONDS(ds) (6 * (ds))

// REG_DISPCNT values
#define DCNT_MODE_0 0
#define DCNT_MODE_3 3
#define DCNT_BG0_EN (1 << 8)
#define DCNT_BG1_EN (1 << 9)
#define DCNT_BG2_EN (1 << 10)
#define DCNT_BG3_EN (1 << 11)
#define DCNT_OBJ (1 << 12)
#define DCNT_OBJ_1D (1 << 6)

// Background values
#define BG_REG_32x32 0

// OAM attribute values
#define ATTR0_HIDE (1 << 9)
#define ATTR0_AFFINE (1 << 8)
#define ATTR0_AFFINE_DOUBLE (3 << 8)
#define ATTR0_SQUARE 0
#define ATTR0_16COLOR 0
#define ATTR0_256COLOR (1 << 13)
#define ATTR0_Y_MASK 0xff
#define ATTR1_SQUARE 0
#define ATTR1_X_MASK 0x1ff
#define ATTR2_TID_MASK 0x3ff

// Keys
#define KEY_A 0x0001
#define KEY_B 0x0002
#define KEY_SELECT 0x0004
#define KEY_START 0x0008
#define KEY_RIGHT 0x0010
#define KEY_LEFT 0x0020
#define KEY_UP 0x0040
#define KEY_DOWN 0x0080
#define KEY_R 0x0100
#define KEY_L 0x0200
// Useful for checking if keys are pressed
#define KEY_DOWN_NOW(key) (~(REG_KEYINPUT) & key)
#define KEY_PRESSED(key, var) (!(var & key) && KEY_DOWN_NOW(key) && (var |= key))

// Types
typedef struct { unsigned int data[8]; } ALIGN4 TILE;
typedef TILE CHARBLOCK[512];
typedef struct {
    unsigned short attr0;
    unsigned short attr1;
    unsigned short attr2;
    short fill;
} ALIGN4 OBJ_ATTR;
typedef struct {
    unsigned short fill0[3];
    short pa;
    unsigned short fill1[3];
    short pb;
    unsigned short fill2[3];
    short pc;
    unsigned short fill3[3];
    short pd;
} ALIGN4 OBJ_AFFINE;
typedef unsigned short BG_SCREEN_ENTRY;
typedef struct { BG_SCREEN_ENTRY entries[1024]; } ALIGN4 SCREENBLOCK;
typedef struct { unsigned short colors[16]; } ALIGN4 PALETTE;

// Easily-indexed memory locations
#define tile_mem ((volatile CHARBLOCK *) 0x06000000)
#define bg_palette_mem ((volatile PALETTE *) 0x5000000)
#define sprite_palette_mem ((volatile PALETTE *) 0x5000200)
#define oam_mem ((volatile OBJ_ATTR *) 0x07000000)
#define affine_mem ((volatile OBJ_AFFINE *) 0x07000000)
#define bg_screen_entry_mem ((volatile SCREENBLOCK *) 0x06000000)

// Used for easy indexing
#define TILE_BG 0
#define TILE_SPRITE_LOW 4

// Helper variables
unsigned short *videoBuffer;

// Helper functions
void waitForVblank();
void oam_clear_all();
void oam_write(volatile OBJ_ATTR *dest, OBJ_ATTR *source);
void affine_write(volatile OBJ_AFFINE *dest, OBJ_AFFINE *source);
void dma16(volatile void *dst, volatile const void *src, size_t n);
void dma32(volatile void *dst, volatile const void *src, size_t n);
void dma16_fixedsrc(volatile void *dst, volatile const void *src, size_t n);
void dma32_fixedsrc(volatile void *dst, volatile const void *src, size_t n);
void drawImage3(int x, int y, int width, int height, const unsigned short *image);

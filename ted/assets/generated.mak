# crafted with love by Austin Adams, 2k17

BG_TILES = bg
TILES = ted bg text1
PALETTES = ted bg
LEVELS = intro gravity finale
IMAGES = title gameover victory 0 1 2 3 4 5 6 7 8 9

ASSET_CFILES = $(patsubst %,tile_%.c,$(TILES)) \
               $(patsubst %,map_%.c,$(LEVELS)) \
			   $(patsubst %,image_%.c,$(IMAGES)) \
			   $(patsubst %,palette_%.c,$(PALETTES))

ASSET_OFILES = $(patsubst %.c,%.o,$(ASSET_CFILES))
ASSET_HFILES = $(patsubst %.c,%.h,$(ASSET_CFILES))

# crafted with love by Austin Adams, 2k17

# Generates C and header files from assets
include generated.mak

# Tile/level variables
TOOLSDIR = ../tools
MKTILE = $(TOOLSDIR)/mktile
MKTILEH = $(TOOLSDIR)/mktileh
MKLEVEL = $(TOOLSDIR)/mklevel
MKLEVELH = $(TOOLSDIR)/mklevelh
MKIMAGE = $(TOOLSDIR)/mkimage
MKPALETTE = $(TOOLSDIR)/mkpalette
MKPALETTEH = $(TOOLSDIR)/mkpaletteh
TILESDIR = tiles
LEVELSDIR = levels
IMAGESDIR = images

.PHONY: all clean tools tile-clean level-clean image-clean

all: $(ASSET_CFILES) $(ASSET_HFILES)

clean: tile-clean level-clean image-clean

###############
##   TOOLS   ##
###############

tools:
	$(MAKE) -C $(TOOLSDIR)

###############
##   TILES   ##
###############

palette_%.c: tools $(TILESDIR)/%.palette
	$(MKPALETTE) $(word 2,$^) $* >$@

palette_%.h: tools
	$(MKPALETTEH) $* >$@

tile_%.c: tools $(TILESDIR)/%.png $(TILESDIR)/%.spec
	$(MKTILE) $(word 2,$^) $* $(word 3,$^) $(patsubst %.png,%.palette,$(word 2,$^)) >$@

tile_%.h: $(TILESDIR)/%.spec
	$(MKTILEH) $* $(shell cat $<) >$@

tile-clean:
	rm -f tile_*.* palette_*.*

################
##   LEVELS   ##
################

map_%.c: tools $(LEVELSDIR)/%.map $(patsubst %,$(TILESDIR)/%.spec,$(BG_TILES))
	$(MKLEVEL) $* $(filter-out tools,$^) >$@

map_%.h:
	$(MKLEVELH) $* >$@

level-clean:
	rm -f map_*.*

################
##   IMAGES   ##
################

image_%.c image_%.h: tools $(IMAGESDIR)/%.png
	$(MKIMAGE) $* $(filter-out tools,$^) image_$*.h >image_$*.c

image-clean:
	rm -f image_*.*

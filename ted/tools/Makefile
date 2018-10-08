# crafted with love by Austin Adams, 2k17

.PHONY: all clean

CC = gcc
#CFLAGS = -g -Wall -Werror -std=gnu99
CFLAGS = -g -Wall -std=gnu99
OFILES = mklevel.o mktile.o mkimage.o mkpalette.o specfile.o palette.o

all: mktile mklevel mkimage mkpalette

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

mktile: $(OFILES)
	$(CC) $(CFLAGS) mktile.o specfile.o palette.o -o $@ -lpng

mklevel: $(OFILES)
	$(CC) $(CFLAGS) mklevel.o specfile.o -o $@

mkimage: $(OFILES)
	$(CC) $(CFLAGS) mkimage.o -o $@ -lpng

mkpalette: $(OFILES)
	$(CC) $(CFLAGS) mkpalette.o palette.o -o $@

clean:
	rm -f *.o mktile mklevel mkimage mkpalette

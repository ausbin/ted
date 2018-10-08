My Gameboy Game
===============

This is the Gameboy Advance Game I made as a student in CS 2110. It uses
some relatively advanced GBA features such as tiled backgrounds,
paletted colors, sprites, and affine sprites.

To build it, you will probably need the following Debian/Ubuntu
packages: gcc-arm-none-eabi libnewlib-arm-none-eabi to do ARM
cross-compilation. To run it, you will need a GBA emulator such as
Visual Boy Advance (recommended) or Mednafen. Building and running goes
something like:

    $ make
    $ vba ted.gba # or whatever emulator you want

For the controls, see readme.txt. To play in a browser, visit:
https://ausbin.github.io/cs2110-gba/launcher.html#ted

The code is not beautiful since it was made in a week, but it works. In
particular, I was stubborn and wrote my own image converter and level
generators in C in tools/ which worked just well enough to finish the
homework. They only support a subset of inputs (e.g. some PNG features)
and leak memory like as stuck pig, but in all, the asset generation is
pretty nice and prevents hardcoding level data in hand-written C.

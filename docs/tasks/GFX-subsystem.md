# GFX subsystem

## Source code

- [`rom/graphics/`](https://github.com/ApolloTeam-dev/AROS/tree/master/rom/graphics/)

## Documentation from AROS

- http://www.aros.org/documentation/developers/app-dev/graphics-library.php
- http://www.aros.org/documentation/developers/sys-dev/index.php
This does not cover SAGA.

Documentation from the [Amiga Developer CD v2.1 (OS 3.5)](http://amigadev.elowar.com/):

- “[Amiga® RMK Libraries: Graphics Libraries]”(http://amigadev.elowar.com/read/ADCD_2.1/Libraries_Manual_guide/node0004.html)

## Issues

Amiga OS RTG subsystems usually have conversion routines allowing them to blit from any format to any format: blit from 8bit to 16bit, blit from 16bit to 24bit, etc.

Aros lacks some of those and falls back to “for each pixel, read it single and cast it single to new format” which is about 1000 times slower than the normal bulk blit routines you would expect on Amiga RTG.

It’s visually easy to see if you hit this on Aros because they render from bottom to top and not from top to bottom as usual.

Fixing these render limits would make a major improvement to Aros... as Aros with an 68020 would sometimes take basically *minutes* (68080 is roughly 100 times faster than A1200) to refresh a frame.

You can trigger this easily:
1. Boot without startup sequence: go in early boot, select boot with no startup.
2. In CLI type `workbook`.
3. Click on some HD icon so that the window appears.
4. Open a few folder windows and move them around a little, see how fast it is.

High priority bugs for the GFX subsystem are listed under:
https://github.com/ApolloTeam-dev/AROS/issues/8

## Compiling and testing
Workbook / `workbench.library` is in ROM.

- With the [`master`](https://github.com/ApolloTeam-dev/AROS) branch, which is the default, use `build_all.sh`.
- If you are using the [`mkapollo.sh`](https://github.com/ApolloTeam-dev/AROS/blob/master/mkapollo.sh) script, `./mkapollo.sh kernel` suffices to build it. 
- The ROM is written to `./aros.rom`, and it must be under 1MB total or 512KB for each of `rom` and `ext`. `mkapollo.sh` will tell you after compilation whether it fits or is too big.
- Copy `./aros.rom` to the V4SA, and load it with `C:VControl MAPROM=aros.rom`: the V4SA will reboot with that ROM. To go back to the previous ROM, just power off the machine. More details at: https://github.com/flype44/VControl/blob/master/DOCUMENTATION.md#vcontrol-maprom

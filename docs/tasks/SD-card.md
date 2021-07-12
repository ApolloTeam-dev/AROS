# SD card

Source code:
[`rom/devs/sdcard/`](https://github.com/ApolloTeam-dev/AROS/tree/master/rom/devs/sdcard)

The SD card can only be used for reading files, not writing.

The empty functions where the write operations should go are in
[`rom/devs/sdcard/sdcard_ioops.c`](https://github.com/ApolloTeam-dev/AROS/tree/master/rom/devs/sdcard/sdcard_ioops.c)

Documentation from AROS:

- http://www.aros.org/documentation/developers/app-dev/dos-library.php
- http://www.aros.org/documentation/developers/sys-dev/index.php

Documentation from the [Amiga Developer CD v2.1 (OS 3.5)](http://amigadev.elowar.com/):

- “[Amiga® RKM Libraries: Exec Device I/O](http://amigadev.elowar.com/read/ADCD_2.1/Libraries_Manual_guide/node0299.html)”
- “[Amiga® RKM Devices: Contents](http://amigadev.elowar.com/read/ADCD_2.1/Devices_Manual_guide/node0000.html)”

## Compiling and testing
The SD card support is in ROM.

- With the [`master`](https://github.com/ApolloTeam-dev/AROS) branch, which is the default, use `build_all.sh`.
- If you are using the [`mkapollo.sh`](https://github.com/ApolloTeam-dev/AROS/blob/master/mkapollo.sh) script, `./mkapollo.sh kernel` suffices to build it.
- The ROM is written to `./aros.rom`, and it must be under 1MB total or 512KB for each of `rom` and `ext`. `mkapollo.sh` will tell you after compilation whether it fits or is too big.
- Copy `./aros.rom` to the V4SA, and load it with `C:VControl MAPROM=aros.rom`: the V4SA will reboot with that ROM. To go back to the previous ROM, just power off the machine. More details at: https://github.com/flype44/VControl/blob/master/DOCUMENTATION.md#vcontrol-maprom

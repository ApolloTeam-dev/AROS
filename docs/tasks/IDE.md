# IDE controller

Source code:

- [`rom/devs/ata/`](https://github.com/ApolloTeam-dev/AROS/tree/master/rom/devs/ata)
- [`arch/m68k-all/`](https://github.com/ApolloTeam-dev/AROS/tree/master/arch/m68k-all)
- [`arch/m68k-amiga/`](https://github.com/ApolloTeam-dev/AROS/tree/master/arch/m68k-amiga)

IDE is part of the kernel (the Kickstart) and today the IDE is split in several pieces:

- the general ATA logic. 
- the low level function to poke Amiga register.
- the probing part which finds out what IDE is there at all.

This is more abstracted that it was on Amiga OS.
On Amiga OS the kickstart included the SCSI.device and this knew exactly what HW is in the Amiga. It would not probe around, but rather talk directly to the defined Amiga registers.

The Aros counterpart does not do this, but has pointers to functions and these pointers will get replaced by the probing unit, and it will point them to function that then talk to the register directly.

Documentation from AROS:

- http://www.aros.org/documentation/developers/app-dev/dos-library.php
- http://www.aros.org/documentation/developers/sys-dev/index.php

Documentation from the [Amiga Developer CD v2.1 (OS 3.5)](http://amigadev.elowar.com/):

- “[Amiga® RKM Libraries: Exec Device I/O](http://amigadev.elowar.com/read/ADCD_2.1/Libraries_Manual_guide/node0299.html)”
- “[Amiga® RKM Devices: Contents](http://amigadev.elowar.com/read/ADCD_2.1/Devices_Manual_guide/node0000.html)”

## Compiling and testing
IDE is in ROM.

- With the [`master`](https://github.com/ApolloTeam-dev/AROS) branch, which is the default, use `build_all.sh`.
- If you are using the [`mkapollo.sh`](https://github.com/ApolloTeam-dev/AROS/blob/v4-alynna/mkapollo.sh) script, `./mkapollo.sh kernel` suffices to build it.
- The ROM is written to `./aros.rom`, and it must be under 1MB total or 512KB for each of `rom` and `ext`. `mkapollo.sh` will tell you after compilation whether it fits or is too big.
- Copy `./aros.rom` to the V4SA, and load it with `C:VControl MAPROM=aros.rom`: the V4SA will reboot with that ROM. To go back to the previous ROM, just power off the machine. More details at: https://github.com/flype44/VControl/blob/master/DOCUMENTATION.md#vcontrol-maprom

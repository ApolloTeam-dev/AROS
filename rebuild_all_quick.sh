#!/bin/bash
./make_dist_config.sh "bin/amiga-m68k/gen"

CPU_COUNT=$(grep processor /proc/cpuinfo | wc -l)
THREADS=${CPU_COUNT}

#Some how, running more than 8 tasks doesn't succeed every time
if [ ${THREADS} -gt  8 ]
then
	THREADS=8
fi

args=("$@")
if [ "${args[ 0 ]}" == "-h" ]
then
	echo "Options are:"
	echo "-h				prints this help text"
	echo "--with-nonvampire-support	building for non-vampire platforms"
	exit
fi

echo "##############################################"
echo "# Rebuilding all"
echo "# CPU Count: ${CPU_COUNT}"
echo "# Rebuilding with ${THREADS} parallel tasks"
if [ $# -gt 0 ]; then echo "# Building with $@"; fi
echo "##############################################"
sleep 3

make -j${THREADS} distfiles
cat bin/amiga-m68k/gen/boot/aros-amiga-m68k-ext.bin bin/amiga-m68k/gen/boot/aros-amiga-m68k-rom.bin > aros.rom
ls -lah aros.rom


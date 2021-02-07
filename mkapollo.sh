#!/bin/bash
shopt -s extglob

## BEGIN Configuration ## Place your favorite configuration here.
JOBS="$(nproc)"
CLEAN=0
GITCLEAN=0
DL=0
WORK="apollo-os"
DISTRONAME="ApolloOS"
DISTROVERSION="$(cat version)"
DISTRODATE="$(date +%Y-%m-%d)"
AMIGADATE="$(date +"%-d.%-m.%Y")"

if [ -e ".git" ]; then
	BRANCH="$(git branch --show-current)"
	REMOTE=$(git status -sb | sed "s/\#\#\ ${BRANCH}\.\.\.//g" | sed "s/\/${BRANCH}//g" | head -n 1)
	REPO="$(git remote get-url "${REMOTE}")"
else
	REPO="https://github.com/ApolloTeam-dev/AROS"
	BRANCH="v4-alynna"
fi
SRC=""
REZ=640x256x4
CPU=68040
FPU=68881
OPT=s
VAMP=1
CONF=0
DEBUG=0
CONFO=""
MAKEO=""
EXCLUDE=0
## END Configuration ##

## BEGIN Beauty Variables ##
BOLD="$(tput bold)"
BOLDEND="$(tput sgr0)"
RED="$(tput setaf 1)"
GREEN="$(tput setaf 2)"
YELLOW="$(tput setaf 3)"
NC="$(tput sgr0)"
ARROWS="${BOLD}${YELLOW}>>>${NC}"
## BEGIN Beauty Variables ##

## BEGIN Variables ##
setvars () {
	DIR="$(pwd)"
	if [ -e "${DIR}/.git" ]; then
		SRC="${DIR}/"
	else
		SRC="${DIR}/${WORK}/src"
	fi
	TEST_CFLAGS="" #-I${DIR}/${WORK}"
	PORTS="${DIR}/${WORK}/prt"
	BIN="${DIR}/${WORK}/bin"
	CONFOPTS="--target=amiga-m68k --with-optimization=-O${OPT} --with-aros-prefs=classic --with-resolution=${REZ} --with-cpu=${CPU} --with-fpu=${FPU} --disable-mmu --with-portssources=${PORTS}"
	if [ ${VAMP} = 0 ];		then CONFOPTS="${CONFOPTS} --with-nonvampire-support"; fi
	if [ ${DEBUG} = 1 ];	then CONFOPTS="${CONFOPTS} --enable-debug --with-serial-debug"; fi
	MAKEOPTS="-j${JOBS}"
	PKGS="git gcc g++ make cmake gawk bison flex bzip2 netpbm autoconf automake libx11-dev libxext-dev libc6-dev liblzo2-dev libxxf86vm-dev libpng-dev libsdl1.2-dev byacc python3-mako libxcursor-dev gcc-multilib"

	mkdir -p "${DIR}/${WORK}"
	VERSION_FILE="${DIR}/${WORK}/dist_config.h"

	printf "#ifndef AROS_DIST_CONFIG_H\n#define AROS_DIST_CONFIG_H\n\n" > "${VERSION_FILE}"
	# shellcheck disable=SC2129
	printf "#define __DISTRONAME__\t\t\"%s\"\n" "${DISTRONAME}" >> "${VERSION_FILE}"
	printf "#define __DISTROVERSION__\t\"%s\"\n" "${DISTROVERSION}" >> "${VERSION_FILE}"
	printf "#define __DISTRODATE__\t\t\"%s\"\n" "${DISTRODATE}" >> "${VERSION_FILE}"
	printf "#define __AMIGADATE__\t\t\"%s\"\n" "${AMIGADATE}" >> "${VERSION_FILE}"
	printf "\n#endif //AROS_DIST_CONFIG_H\n" >> "${VERSION_FILE}"

	export CONFOPTS MAKEOPTS SRC PORTS BIN DIR PKGS
}

freevars () { unset CONFOPTS MAKEOPTS SRC PORTS BIN DIR BRANCH CLEAN DL CONF CONFO GITCLEAN JOBS MAKEO REZ VAMP WORK DEBUG CPU FPU OPT; }
## END Variables ##

## BEGIN FUNCTIONS ##
defaults () {
              echo -n "--branch=${BRANCH} -c${CLEAN} -d${DL} -f${CONF} -g${GITCLEAN} --jobs=${JOBS} -v${VAMP} "
              if [ $REZ = "640x200x4" ]; then echo -n "-n"; else echo -n "-p"; fi
              echo " --work=${WORK} --conf=\"${CONFO}\" --make=\"${MAKEO}\" --cpu=${CPU} --fpu=${FPU} --opt=${OPT}"
}

deposit-rom () {
		 if [ -e "${BIN}/bin/amiga-m68k/gen/boot/bootdisk-amiga-m68k.adf" ]; then
                  cp "${BIN}/bin/amiga-m68k/gen/boot/bootdisk-amiga-m68k.adf" ${WORK}/
                  print_bold_nl "${ARROWS} ${BOLD}${GREEN}ADF RESULT:${NC} $(du --apparent-size -h ${WORK}/bootdisk-amiga-m68k.adf)"
                 fi

		 if [ -e "${BIN}/bin/amiga-m68k/gen/boot/aros-amiga-m68k-rom.bin" ]; then
                  cat "${BIN}/bin/amiga-m68k/gen/boot/aros-amiga-m68k-ext.bin" "${BIN}/bin/amiga-m68k/gen/boot/aros-amiga-m68k-rom.bin" > "${WORK}/AROS.ROM"
                  print_bold "${ARROWS} ${BOLD}${GREEN}ROM RESULT:${NC} "
                  read -r ROMSIZE ROMSIZETYPE ROMFILEPATH <<< "$(du --apparent-size -h ${WORK}/AROS.ROM |sed -re "s|([0-9\.]+)([a-Z])\t([-/a-Z\.]+)|\1 \2 \3|g")"

                  # shellcheck disable=SC2072
                  MAXROMSIZE=1.0
                  printf "%s%s\t%s" "${ROMSIZE}" "${ROMSIZETYPE}" "${ROMFILEPATH}"

                  if [[ ((${ROMSIZE} > ${MAXROMSIZE})) ]] && [ "${ROMSIZETYPE}" == "M" ]; then
                   print_bold_nl " -- ${RED}BAD ROM SIZE${NC}"
                  else
                   print_bold_nl " -- ${GREEN}ROM SIZE OK${NC}"
                  fi
                 fi

		 if [ -e "${BIN}/distfiles/aros-amiga-m68k.iso" ]; then
                  cp "${BIN}/distfiles/aros-amiga-m68k.iso" ${WORK}/
                  echo "${ARROWS} ${BOLD}${GREEN}ISO RESULT:${NC} $(du --apparent-size -h ${WORK}/aros-amiga-m68k.iso)"
                 fi

		 if [ -e "${BIN}/bin/amiga-m68k/AROS.HUNK/Devs/sagasd.device" ]; then
                  cp "${BIN}/bin/amiga-m68k/AROS.HUNK/Devs/sagasd.device" ${WORK}/
                  echo "${ARROWS} ${BOLD}${GREEN}SD0 RESULT:${NC} $(du --apparent-size -h ${WORK}/sagasd.device)"
                 fi
}

update-distro-files () {
	touch "${SRC}/rom/dosboot/menu.c";
	touch "${SRC}/rom/dos/boot.c";
	touch "${SRC}/rom/dos/banner.c";
}

makeclean () { cd "${BIN}" || exit; make clean; cd "${DIR}" || exit; }
gitclean  () { cd "${SRC}" || exit; git clean -df; cd "${DIR}" || exit; }
pkgcheck  () { if [ $(dpkg-query -W -f '${Binary:Package} ${Status}\n' $PKGS | wc -l) -eq $(echo $PKGS | wc -w) ]; then return 0; else return 1; fi; }
download  () { cd "${WORK}" || exit; if [ ! -d $SRC ]; then git clone --recursive ${REPO} --branch=${BRANCH} ${SRC}; cd ${DIR}; else git checkout ${REMOTE}/${BRANCH} --recurse-submodules -f; fi }
# shellcheck disable=SC2086
configure () { cd "${BIN}" || exit; ${SRC}/configure ${CONFOPTS} ${CONFO}; cd "${DIR}" || exit; }
# shellcheck disable=SC2086
compile () { cd "${BIN}" || exit; update-distro-files; make ${1} ${MAKEOPTS} ${MAKEO}; cd "${DIR}" || exit; }

# shellcheck disable=SC2120
valid-cpu () {
	if [ "$1" = "" ]; then
		echo "68000 68010 68020 68030 68040 68060"
		return 0
	else
		if [ "$1" == "680[12346]0" ]; then return 1; else return 0; fi
	fi
}

# shellcheck disable=SC2120
valid-fpu () {
	if [ "$1" = "" ]; then
		echo "68881 soft-float hard-float"
	else
		if [ "$1" == "68881|soft-float|hard-float" ]; then return 1; else return 0; fi
	fi
}

loop-through-opts () {
for i in "$@"; do
 case $i in
  -b=*|--branch=*)
   BRANCH="${i#*=}"
   shift
  ;;
  -b0|--no-branch)
   BRANCH=""
   shift
  ;;
  -c|-c1|--clean)
   CLEAN=1
   shift
  ;;
  -c0|--no-clean)
   CLEAN=0
   shift
  ;;
  -d|-d1|--download)
   DL=1
   shift
  ;;
  -d0|--no-download)
   DL=0
   shift
  ;;
  -f|-f1|--conf)
   CONF=1
   shift
  ;;
  -f0|--no-conf)
   CONF=0
   shift
  ;;
  -f=*|--conf=*)
   CONFO="${i#*=}"
   shift
  ;;
  -g|-g1|--gitclean)
   GITCLEAN=1
   shift
  ;;
  -g0|--no-gitclean)
   GITCLEAN=0
   shift
  ;;
  -j=*|--jobs=*)
   JOBS="${i#*=}"
   shift
  ;;
  -j)
   JOBS=$(nproc)
   shift
  ;;
  -m=*|--make=*)
   MAKEO="${i#*=}"
   shift
  ;;
  -m0|--no-make)
   MAKEO=""
   shift
  ;;
  -n|--ntsc)
   REZ="640x200x4"
   shift
  ;;
  -p|--pal)
   REZ="640x256x4"
   shift
  ;;
  -v|-v1|--vamp)
   VAMP=1
   shift
  ;;
  -v0|--no-vamp)
   VAMP=0
   shift
  ;;
  -w=*|--work=*)
   WORK="${i#*=}"
   shift
  ;;
  -x|-x1|--exclude)
   EXCLUDE=1
   shift
  ;;
  -x0|--no-exclude)
   EXCLUDE=0
   shift
  ;;
  --cpu=*)
   CPU="${i#*=}"
   shift
  ;;
  --fpu=*)
   FPU="${i#*=}"
   shift
  ;;
  -o=*|--opt=*)
   OPT="${i#*=}"
   shift
  ;;
  --debug|--debug=1)
   DEBUG=1
   shift
  ;;
  --no-debug|--debug=0)
   DEBUG=0
   shift
  ;;
  *)
   if [ "${CMD}" = "" ]; then CMD=${i}; shift;
   else (echo "!!! There seems to be an issue with your Options." >&2); shift; exit 2
   fi
 esac
 export BRANCH CLEAN DL CONF CONFO GITCLEAN JOBS MAKEO REZ VAMP WORK CMD CPU FPU OPT DEBUG EXCLUDE
done
}

help () {
cat << EOF
${BOLD}mkapollo.sh -- Roll your own ApolloOS image and ROM${NC}
 (C) 2021 Alynna Trypnotk, License APL 1.1 & (C) 2021 Marlon Beijer (marlon@amigadev.com), License APL 1.1:
 https://github.com/ApolloTeam-dev/AROS/blob/master-new/LICENSE

 It does what NintenDon't.
 ${BOLD}Syntax:${NC} mkapollo.sh [options] <command> [args]

 ${BOLD}Commands:${NC}
  all                         Run configure, make distfiles and ROM (Start here!)
  dist                        Just make distfiles
  kernel                      Just make ROM
  deposit-rom                 List paths to built files
  list-rom-contents           List the contents of the built rom
  wipe                        Starts from near fresh (leaves ports)
  null                        Just parse options

 ${BOLD}Options:${NC}
  -b,--branch=<branch>        Select different branch
  -c,--clean                  Make clean first
  -d,--download               Delete source and redownload
  -f,--conf                   Run configure first
  --conf=<opts>               Add configure options
  -g,--gitclean               Clean for git committing (after other operations)
  --jobs=#                    Specify number of compile threads
  -j                          One compile thread for each cpu
  --make=<opts>               Add make options
  -n,--ntsc                   Select NTSC build
  -o,--opt=#                  Select optimization level
  -p,--pal                    Select PAL build
  -v,--vamp                   Compile for True Vampires
  -w,--work=<dir>             Work directory (will be created if not present)
  -x,--exclude                When wiping, do not save cross compile tools
  --cpu=<cpu>                 Specify target CPU
  --fpu=<fpu>                 Specify target FPU
  --debug                     Enable debugging

  Defaults can be changed at the top of this file.
  Nearly all options take a boolean 0/1 to override defaults, as well as --no-<option>

  Valid optimization levels: s 0 1 2 3
EOF
echo "  Valid CPUs               : $(valid-cpu)"
echo "  Valid FPUs               : $(valid-fpu)"
cat << EOF

 It will automatically install the packages needed for Vampire Goodness,
 assuming that you use a Debian based distribution.
 Note that sources will be downloaded if it is not found at the location.
 If you just want your ROM, just do:  mkapollo.sh all
 Then go get a coffee. Or a meal.

 The current defaults are:
EOF
printf "%s\n\n" "$(defaults)"
print_bold_nl "Please Amiga responsibly."
}

check-deps () {
	if [ ${CONF} = 1 ] || [ ! -e "${BIN}/config.status" ];								then configure;													fi
	if [ ! -e "${BIN}/bin/linux-x86_64/tools/crosstools/m68k-aros-gcc" ];	then compile tools-crosstools-gcc;			fi
	if [ ! -e "${BIN}/bin/linux-x86_64/tools/mmake" ];										then compile mmake;											fi
	if [ ! -e "${BIN}/bin/linux-x86_64/tools/sfdc" ];											then compile sfdc;											fi
	if [ ! -e "${BIN}/bin/amiga-m68k/gen/include/zconf.h" ];							then compile workbench-libs-z-includes;	fi
}

list-rom-contents () {
	print_bold_nl ""
	print_bold_nl "${YELLOW} ---- ${GREEN}Start ROM Contents: ${YELLOW}----${NC}"
	readarray a <<< $(strings "${WORK}/AROS.ROM" | grep "\$VER" | sed -re 's|.*\$VER\:\s*([\.\-\_a-Z0-9]+)\s*([-a-Z0-9\_]*)\s([\.0-9]+)\s([0-9\(\)\.]+)(.*)|\1 \"\2\" \3 \4|g')

	for element in "${a[@]}"; do
		readarray -td ' ' b  <<< "$(printf "%s" "${element}"| tr -d "\n")"; printf "${BOLD}${YELLOW}%s${NC} %s %s\n" ${b[0]} ${b[2]} ${b[3]};
	done

	print_bold_nl "${YELLOW} ----- ${GREEN}End ROM Contents: ${YELLOW}-----${NC}"
	print_bold_nl ""
}

print_bold () {
	# shellcheck disable=SC2059
	printf "${BOLD}${1}${BOLDEND}"
}

print_bold_nl () {
	# shellcheck disable=SC2059
	print_bold "${1}\n"
}
## END FUNCTIONS ##

## BEGIN MAIN ##
# shellcheck disable=SC2068
loop-through-opts $@
if [ "$CMD" = "" ]; 	then help; exit 1; 	fi

setvars
print_bold "${YELLOW}Making ApolloOS: "
printf "%s %s\n" "${CMD}" "$(defaults)"

print_bold "${YELLOW}Configuration:   "
printf "%s\n" "${CONFOPTS}"

print_bold "${YELLOW}Make options:    "
printf "%s\n" "${MAKEOPTS}"

print_bold "${YELLOW}Work directory:  "
printf "%s\n" "${WORK}"

print_bold "${YELLOW}Starting in:     "
echo -n "3..."; sleep 1; echo -n "2..."; sleep 1; echo -n "1..."; sleep 1; print_bold_nl "${GREEN}Go!";

mkdir -p "${BIN}"

if [ $(pkgcheck; echo $?) = 1 ]; then
 print_bold_nl "${ARROWS} ${BOLD}${RED}You are missing required packages to build ApolloOS!  ${GREEN}Attempting to install...${NC}"
 sudo apt -y update
 sudo apt -y install ${PKGS}
fi
if [ $DL = 1 ];                 then print_bold_nl "${ARROWS} Source wipe requested, ${RED}deleting${NC}.";     rm -rf "${SRC}"; fi
if [ ! -e "${SRC}/configure" ]; then print_bold_nl "${ARROWS} Source not detected, ${GREEN}downloading${NC}.";  download;	       fi
if [ $CLEAN = 1 ];              then print_bold_nl "${ARROWS} Fresh build requested, ${YELLOW}cleaning${NC}.";  makeclean;       fi

case $CMD in
 dist)
  check-deps
  compile distfiles
 ;;
 kernel)
  check-deps
  compile kernel
  deposit-rom
 ;;
 list-rom-contents)
  check-deps
  if [ ! -e "${WORK}/AROS.ROM" ]; then compile kernel; fi
  list-rom-contents
 ;;
 all)
  check-deps
  compile distfiles
  deposit-rom
 ;;
 wipe)
  print_bold_nl "!!! CTRL-AMIGA-AMIGA Pressed !!!"
  if [ ! -e ".git" ]; then
   print_bold_nl "${ARROWS} Removing sources"
   rm -rf "${SRC}"
  fi
  if [ "$EXCLUDE" = "0" ]; then
   print_bold_nl "${ARROWS} Preserving Crosstools (saves serious recompile time)"
   mv ${BIN}/bin/linux* ${PORTS}/
  fi
  print_bold_nl "${ARROWS} Removing Binaries (Things in the work dir root are preserved)"
  rm -rf "${BIN}"

  if [ "${EXCLUDE}" = "0" ]; then
   print_bold_nl "${ARROWS} Restoring Crosstools"
   mkdir -p ${BIN}/bin/
   mv ${PORTS}/linux* ${BIN}/bin/
  fi
  print_bold_nl "${ARROWS} Done.  You may now redownload or even choose another branch."
 ;;
 deposit-rom)
  deposit-rom
 ;;
 i2c)
  compile workbench-c-i2c-quick
 ;;
 *)
  help
  exit 1
 ;;
esac
if [ ${GITCLEAN} = 1 ]; then print_bold_nl "${ARROWS} Cleaning git artifacts."; gitclean; fi
freevars
print_bold_nl "Please Amiga responsibly!"
## END MAIN ##

#!/bin/bash
DISTRONAME="$(cat distname)"
DISTROVERSION="$(cat version)"
DISTRODATE="$(date +%Y-%m-%d)"
AMIGADATE="$(date +"%-d.%-m.%Y")"

mkdir -p "${1}"
VERSION_FILE="${1}/dist_config.h"

printf "#ifndef AROS_DIST_CONFIG_H\n#define AROS_DIST_CONFIG_H\n\n" > "${VERSION_FILE}"
# shellcheck disable=SC2129
printf "#define __DISTRONAME__\t\t\"%s\"\n" "${DISTRONAME}" >> "${VERSION_FILE}"
printf "#define __DISTROVERSION__\t\"%s\"\n" "${DISTROVERSION}" >> "${VERSION_FILE}"
printf "#define __DISTRODATE__\t\t\"%s\"\n" "${DISTRODATE}" >> "${VERSION_FILE}"
printf "#define __AMIGADATE__\t\t\"%s\"\n" "${AMIGADATE}" >> "${VERSION_FILE}"
printf "\n#endif //AROS_DIST_CONFIG_H\n" >> "${VERSION_FILE}"

printf "%s (%s, %s)\n" "${DISTRONAME}" "${DISTROVERSION}" "${DISTRODATE}"
echo "dist_config.h created!"

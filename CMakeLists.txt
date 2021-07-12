cmake_minimum_required(VERSION 3.18)

project(aros)

# <aros-setup>
set(AROS_ROOT ON)
include("cmake/aros-setup.cmake")
include(aros-support-includes)
# </aros-setup>

# <asm.h>
include(build-asm-h)
# </asm.h>

add_subdirectory(rom)

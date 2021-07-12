file(READ "${AROS_ROOT_DIR}/config/config.h.in" AROS_CONFIG_H)

set(M68K_CPU 68040)
set(M68K_FPU hard)

add_definitions(-D__AROS__ -DISODATE="CHANGEME")

string(REPLACE "-m68000" "-m${M68K_CPU}" CMAKE_ASM_FLAGS ${CMAKE_ASM_FLAGS})

string(REPLACE "-m68000" "-m${M68K_CPU}" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
string(REPLACE "-msoft-float" "-m${M68K_FPU}-float" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
string(REPLACE "-mcrt=nix20" "-nostdlib" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
message(STATUS "C flags: ${CMAKE_C_FLAGS}")

string(REPLACE "-mcrt=nix20" "-nostdlib" CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS})
message(STATUS "LD flags: ${CMAKE_EXE_LINKER_FLAGS}")


string(REPLACE "\@aros_nominal_width\@" "640" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@aros_nominal_height\@" "256" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@aros_nominal_depth\@" "4" AROS_CONFIG_H ${AROS_CONFIG_H})

string(REPLACE "\@aros_flavour_uc\@" "(AROS_FLAVOUR_STANDALONE|AROS_FLAVOUR_BINCOMPAT)" AROS_CONFIG_H ${AROS_CONFIG_H})

string(REPLACE "\@aros_enable_mmu\@" "0" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@aros_amigaos_compliance\@" "0" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@aros_mungwall_debug\@" "0" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@aros_stack_debug\@" "0" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@aros_modules_debug\@" "0" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@ENABLE_DBUS\@" "0" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@aros_nesting_supervisor\@" "0" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@aros_palm_debug_hack\@" "0" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@aros_serial_debug\@" "0" AROS_CONFIG_H ${AROS_CONFIG_H})

string(REPLACE "\@ENABLE_X11\@" "" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@DO_XSHM_SUPPORT\@" "" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@DO_VIDMODE_SUPPORT\@" "" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@PLATFORM_EXECSMP\@" "" AROS_CONFIG_H ${AROS_CONFIG_H})
string(REPLACE "\@ENABLE_EXECSMP\@" "" AROS_CONFIG_H ${AROS_CONFIG_H})

file(WRITE "${CMAKE_BINARY_DIR}/CMakeFiles/include/aros/config.h" ${AROS_CONFIG_H})

include(aros-functions)
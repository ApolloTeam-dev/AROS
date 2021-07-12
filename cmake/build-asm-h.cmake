message(STATUS "Generating arch-specific asm.h")

add_library(asm OBJECT "${AROS_ROOT_DIR}/compiler/include/asm.c")
aros_request_includes(asm exec)
aros_request_includes(asm kernel)
target_include_directories(asm PRIVATE
               $<BUILD_INTERFACE:${AROS_ROOT_DIR}/rom/exec>
               $<INSTALL_INTERFACE:rom/exec>
               )
target_include_directories(asm PRIVATE
               $<BUILD_INTERFACE:${AROS_ROOT_DIR}/rom/kernel>
               $<INSTALL_INTERFACE:rom/exec>
               )
target_include_directories(asm PRIVATE
               $<BUILD_INTERFACE:${AROS_ROOT_DIR}/arch/m68k-all/kernel>
               $<INSTALL_INTERFACE:rom/exec>
               )

target_compile_options(asm PRIVATE -S)

add_custom_target(asm_h COMMAND ${CMAKE_COMMAND} -P "${AROS_ROOT_DIR}/cmake/generate-asm-h.cmake" DEPENDS asm)

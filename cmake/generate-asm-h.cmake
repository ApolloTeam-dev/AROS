file(READ ${CMAKE_BINARY_DIR}/CMakeFiles/asm.dir/compiler/include/asm.c.obj contents)

# Get the version from the config file

# Convert file contents into a CMake list (where each element in the list
# is one line of the file)
#
string(REGEX REPLACE ";" "\\\\;" contents "${contents}")
string(REGEX REPLACE "\n" ";" contents "${contents}")

set(ASM_OUTPUT_REGEX ".asciz\ \"(.+)\"$")

file(WRITE "${CMAKE_BINARY_DIR}/CMakeFiles/include/aros/m68k/asm.h" "")

foreach(item ${contents})

	if(item MATCHES ${ASM_OUTPUT_REGEX})
		STRING(REGEX REPLACE
				${ASM_OUTPUT_REGEX}
				"\\1" ASM_DEFINE ${item})

		file(APPEND "${CMAKE_BINARY_DIR}/CMakeFiles/include/aros/m68k/asm.h" "${ASM_DEFINE}\n")
	endif()
endforeach()
message(STATUS "Generated asm.h!")
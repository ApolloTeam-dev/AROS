include(aros-config)

# Fetch AROS support include files to the cmake build directory
file(GLOB AROS_SUPPORT_INCLUDES "${AROS_ROOT_DIR}/compiler/arossupport/include/*.h")

foreach(support_include ${AROS_SUPPORT_INCLUDES})
	get_filename_component(support_include_filename ${support_include} NAME)
	configure_file(${support_include} "${CMAKE_BINARY_DIR}/CMakeFiles/include/aros/${support_include_filename}" COPYONLY)
endforeach()

# Fetch AROS m68k support include files to the cmake build directory
file(GLOB AROS_M68K_SUPPORT_INCLUDES "${AROS_ROOT_DIR}/arch/m68k-all/include/aros/*.h" "${AROS_ROOT_DIR}/arch/m68k-amiga/include/aros/*.h")

foreach(m68k_include ${AROS_M68K_SUPPORT_INCLUDES})
	get_filename_component(m68k_include_filename ${m68k_include} NAME)
	configure_file(${m68k_include} "${CMAKE_BINARY_DIR}/CMakeFiles/include/aros/m68k/${m68k_include_filename}" COPYONLY)
endforeach()

configure_file("${AROS_ROOT_DIR}/compiler/arossupport/include/asmcall.h" "${CMAKE_BINARY_DIR}/CMakeFiles/include/aros/m68k/asmcall.h" COPYONLY)
configure_file("${AROS_ROOT_DIR}/compiler/arossupport/include/libcall.h" "${CMAKE_BINARY_DIR}/CMakeFiles/include/aros/m68k/libcall.h" COPYONLY)
configure_file("${AROS_ROOT_DIR}/compiler/autoinit/autoinit.h" "${CMAKE_BINARY_DIR}/CMakeFiles/include/aros/autoinit.h" COPYONLY)

include_directories(SYSTEM "${CMAKE_BINARY_DIR}/CMakeFiles/include")
include_directories(SYSTEM "${AROS_ROOT_DIR}/compiler")
include_directories(SYSTEM "${AROS_ROOT_DIR}/compiler/include")
include_directories(SYSTEM "${AROS_ROOT_DIR}/compiler/stdc/include/")
include_directories(SYSTEM "${AROS_ROOT_DIR}/compiler/stdc/include/aros/stdc")
include_directories(SYSTEM "${AROS_ROOT_DIR}/compiler/posixc/include")
include_directories(SYSTEM "${AROS_ROOT_DIR}/compiler/posixc/include/aros/posixc")
include_directories(SYSTEM "${AROS_ROOT_DIR}/arch/m68k-all/include")
include_directories(SYSTEM "${AROS_ROOT_DIR}/arch/m68k-amiga/include")


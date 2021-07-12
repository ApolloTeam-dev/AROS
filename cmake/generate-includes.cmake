file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/CMakeFiles/include/${PROJECT_NAME}/")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/CMakeFiles/include/${PROJECT_NAME}/defines")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/CMakeFiles/include/${PROJECT_NAME}/inline")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/CMakeFiles/include/${PROJECT_NAME}/clib")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/CMakeFiles/include/${PROJECT_NAME}/proto")


if(OS_LIBRARY)
	set(OS_PROJECT_TYPE library)
elseif(OS_DEVICE)
	set(OS_PROJECT_TYPE device)
endif()

add_custom_target(${PROJECT_NAME}_includes COMMAND ${AROS_ROOT_DIR}/apollo-os/bin/bin/linux-x86_64/tools/genmodule -c ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.conf -d ${CMAKE_BINARY_DIR}/CMakeFiles/include/${PROJECT_NAME} writeincludes ${PROJECT_NAME} ${OS_PROJECT_TYPE})
add_custom_target(${PROJECT_NAME}_libdefs COMMAND ${AROS_ROOT_DIR}/apollo-os/bin/bin/linux-x86_64/tools/genmodule -c ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}.conf -d ${CMAKE_BINARY_DIR}/CMakeFiles/include writelibdefs ${PROJECT_NAME} ${OS_PROJECT_TYPE})

target_include_directories(${PROJECT_BINARY_FILENAME} PUBLIC
		$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/CMakeFiles/include/${PROJECT_NAME}>
		$<INSTALL_INTERFACE:include/${PROJECT_NAME}>)

target_compile_definitions(${PROJECT_BINARY_FILENAME} PRIVATE "-DLC_LIBDEFS_FILE=\"${PROJECT_NAME}_libdefs.h\"")

add_dependencies(${PROJECT_BINARY_FILENAME} ${PROJECT_NAME}_includes ${PROJECT_NAME}_libdefs)
cmake_minimum_required(VERSION 3.17)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(ERROR [[In-source build detected, please eg. create a new directory and use `cmake ..`]])
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	set(AROS_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../")

	get_filename_component(AROS_ROOT_DIR "${AROS_ROOT_DIR}"
			REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")

	message(STATUS "AROS root directory: ${AROS_ROOT_DIR}")
else()
	set(AROS_ROOT_DIR "${CMAKE_SOURCE_DIR}/")
endif()


list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${AROS_ROOT_DIR}/cmake")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

option(DEBUG "Enable debug mode in engine" OFF)
option(APOLLO "Enable AC68080 specifics" ON)
option(DEBUG_RELEASE "Enable options for debug release" OFF)

set(PROJECT_NAME "dos")

if(DEBUG_RELEASE)
	set(CMAKE_BUILD_TYPE "Debug")
	set(DEBUG ON)
endif()

include(aros-component-version)

# Project specifics
project(${PROJECT_NAME}
		VERSION ${PROJECT_VERSION}
		LANGUAGES C ASM)

# Set to true/ON if it's a library
set(OS_LIBRARY ON)

set(PROJECT_BINARY_FILENAME "${PROJECT_NAME}")
if(OS_LIBRARY)
	list(APPEND PROJECT_BINARY_FILENAME ".library")
	list(JOIN PROJECT_BINARY_FILENAME "" PROJECT_BINARY_FILENAME)
endif()

# Make version variables
string(REPLACE "." ";" VERSION_LIST ${PROJECT_VERSION})
list(GET VERSION_LIST 0 PROJECT_MAJOR_VERSION)
list(GET VERSION_LIST 1 PROJECT_MINOR_VERSION)

# Print out which project we're compiling
message(STATUS "${PROJECT_BINARY_FILENAME} version: ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}")

# Lowercase project name for binaries and packaging
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWER)
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} SYSTEM_ARCH_LOWER)

include(GNUInstallDirs)

# Lets include the files to compile
file(GLOB PROJECT_SOURCE_FILES "*.c" "*.h" "*.asm")

#set(INIT_FILES "exec_init.c;prepareexecbase.c")

if(OS_LIBRARY)
	add_executable(${PROJECT_BINARY_FILENAME} ${INIT_FILES} ${PROJECT_SOURCE_FILES})
else()
	add_executable(${PROJECT_BINARY_FILENAME} ${PROJECT_SOURCE_FILES})
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	# We're running standalone guys!
	include(aros-support-includes)
else()
	# We're building as part of the full ApolloOS experience! We don't need to do anything
endif()

aros_request_includes(${PROJECT_BINARY_FILENAME} kernel)
aros_request_includes(${PROJECT_BINARY_FILENAME} debug)
aros_request_includes(${PROJECT_BINARY_FILENAME} exec)
aros_request_includes(${PROJECT_BINARY_FILENAME} utility)
aros_request_includes(${PROJECT_BINARY_FILENAME} intuition)
aros_request_includes(${PROJECT_BINARY_FILENAME} timer)
aros_request_includes(${PROJECT_BINARY_FILENAME} graphics)

if(OS_LIBRARY)
	include(generate-includes)
	configure_file("${AROS_ROOT_DIR}/compiler/include/exec/execbase.inc" "${CMAKE_BINARY_DIR}/CMakeFiles/include/exec/execbase.h" COPYONLY)
endif()

# We're compiling for Amiga computers, so add the proper define for this
add_definitions("-DAROS_ARCH_amiga")


include_directories(${PROJECT_SOURCE_DIR} "${PROJECT_BINARY_DIR}/CMakeFiles/include")

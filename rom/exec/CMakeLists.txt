cmake_minimum_required(VERSION 3.17)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(ERROR [[In-source build detected, please eg. create a new directory and use `cmake ..`]])
endif()

include("../../cmake/aros-setup.cmake")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif()

option(DEBUG "Enable debug mode in engine" OFF)
option(APOLLO "Enable AC68080 specifics" ON)
option(DEBUG_RELEASE "Enable options for debug release" OFF)

set(PROJECT_NAME "exec")

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
file(GLOB PROJECT_SOURCE_FILES "*.c" "*.h" "*.asm" )
file(GLOB PROJECT_ARCH_SOURCE_FILES "${AROS_ROOT_DIR}/arch/m68k-all/exec/*.c" "${AROS_ROOT_DIR}/arch/m68k-all/exec/*.S")

string(REPLACE "${PROJECT_SOURCE_DIR}/stackswap.c\n" "" PROJECT_SOURCE_FILES ${PROJECT_SOURCE_FILES})

set(ALL_FUNCTIONS
		abortio.c
		adddevice.c
		addhead.c
		addintserver.c
		addlibrary.c
		addmemhandler.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/addmemlist.c
		addport.c
		addresource.c
		addsemaphore.c
		addtail.c
		addtask.c
		alert.c
		alertstrings.c
		allocabs.c
		allocate.c
		allocentry.c
		allocmem.c
		allocpooled.c
		allocsignal.c
		alloctrap.c
		allocvec.c
		attemptsemaphore.c
		attemptsemaphoreshared.c
		availmem.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/cachecleare.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/cachecleare_.S
		${AROS_ROOT_DIR}/arch/m68k-all/exec/cacheclearu.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/cacheclearu_.S
		${AROS_ROOT_DIR}/arch/m68k-all/exec/cachecontrol.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/cachecontrol_.S
		${AROS_ROOT_DIR}/arch/m68k-all/exec/cachepostdma.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/cachepostdma_.S
		${AROS_ROOT_DIR}/arch/m68k-all/exec/cachepredma.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/cachepostdma_.S
		cause.c
		checkio.c
		childfree.c
		childorphan.c
		childstatus.c
		childwait.c
		closedevice.c
		closelibrary.c
		${AROS_ROOT_DIR}/arch/m68k-amiga/exec/coldreboot.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/copymem.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/copymem_.S
		${AROS_ROOT_DIR}/arch/m68k-all/exec/copymemquick.S
		createiorequest.c
		createmsgport.c
		createpool.c
		deallocate.c
		debug.c
		deleteiorequest.c
		deletemsgport.c
		deletepool.c
		${AROS_ROOT_DIR}/arch/m68k-amiga/exec/disable.S
		${AROS_ROOT_DIR}/arch/m68k-all/exec/dispatch.S
		doio.c
		${AROS_ROOT_DIR}/arch/m68k-amiga/exec/enable.S
		enqueue.c
		exception.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/exitintr.S
		findname.c
		findport.c
		findresident.c
		findsemaphore.c
		findtask.c
		findtaskbypid.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/forbid.S
		freeentry.c
		freemem.c
		freepooled.c
		freesignal.c
		freetrap.c
		freevec.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/getcc.c
		getmsg.c
		initcode.c
		initresident.c
		initsemaphore.c
		initstruct.c
		insert.c
		makefunctions.c
		makelibrary.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/newstackswap.S
		${AROS_ROOT_DIR}/arch/m68k-all/exec/obtainquickvector.S
		obtainsemaphore.c
		obtainsemaphorelist.c
		obtainsemaphoreshared.c
		oldopenlibrary.c
		opendevice.c
		openlibrary.c
		openresource.c
		permit.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/preparecontext.c
		procure.c
		putmsg.c
		rawdofmt.c
		rawioinit.c
		rawmaygetchar.c
		rawputchar.c
		readgayle.c
		releasesemaphore.c
		releasesemaphorelist.c
		remdevice.c
		remhead.c
		remintserver.c
		remlibrary.c
		remmemhandler.c
		remove.c
		remport.c
		remresource.c
		remsemaphore.c
		remtail.c
		remtask.c
		replymsg.c
		reschedule.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/schedule.S
		sendio.c
		setexcept.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/setfunction.c
		setintvector.c
		setsignal.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/setsr.S
		settaskpri.c
		signal.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/stackswap.S
		sumkickdata.c
		sumlibrary.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/superstate.S
		${AROS_ROOT_DIR}/arch/m68k-all/exec/supervisor.S
		${AROS_ROOT_DIR}/arch/m68k-all/exec/switch.S
		taggedopenlibrary.c
		typeofmem.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/userstate.S
		vacate.c
		wait.c
		waitio.c
		waitport.c
		allocvecpooled.c
		freevecpooled.c
		newallocentry.c
		newaddtask.c
		newminlist.c
		avl.c
		vnewrawdofmt.c
		${AROS_ROOT_DIR}/arch/m68k-amiga/exec/shutdowna.c
		useralert.c
		addresetcallback.c
		remresetcallback.c
		doresetcallbacks.c
		newcreatetaska.c
		alloctaskstorageslot.c
		freetaskstorageslot.c
		savetaskstorage.c
		restoretaskstorage.c
		settaskstorageslot.c
		gettaskstorageslot.c
		getparenttaskstorageslot)

set(INIT_FILES
		exec_init.c
		prepareexecbase.c)

set(FILES
		alertextra.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/alert_cpu.c
		systemalert.c
		initkicktags.c
		intservers.c
		intserver_vblank.c
		memory.c
		memory_nommu.c
		mungwall.c
		semaphores.c
		service.c
		traphandler.c
		exec_debug.c
		${AROS_ROOT_DIR}/arch/m68k-amiga/exec/exec_globals.c
		${AROS_ROOT_DIR}/arch/m68k-all/exec/exec_platform.c
		exec_util.c
		exec_locks.c
		supervisoralert.c)


add_library(${PROJECT_BINARY_FILENAME} ${INIT_FILES} ${ALL_FUNCTIONS} ${FILES})

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	# We're running standalone guys!
	include(aros-support-includes)
else()
	# We're building as part of the full ApolloOS experience! We don't need to do anything
endif()

target_compile_options(${PROJECT_BINARY_FILENAME} PRIVATE -Wno-discarded-qualifiers)

aros_request_includes(${PROJECT_BINARY_FILENAME} kernel)
aros_request_includes(${PROJECT_BINARY_FILENAME} debug)
aros_request_includes(${PROJECT_BINARY_FILENAME} dos)
aros_request_includes(${PROJECT_BINARY_FILENAME} intuition)


add_dependencies(${PROJECT_BINARY_FILENAME} asm_h)

if(OS_LIBRARY)
	include(generate-includes)
	configure_file("${AROS_ROOT_DIR}/compiler/include/exec/execbase.inc" "${CMAKE_BINARY_DIR}/CMakeFiles/include/exec/execbase.h" COPYONLY)
endif()

# We're compiling for Amiga computers, so add the proper define for this
add_definitions("-DAROS_ARCH_amiga")


include_directories(${PROJECT_SOURCE_DIR} "${PROJECT_BINARY_DIR}/CMakeFiles/include")

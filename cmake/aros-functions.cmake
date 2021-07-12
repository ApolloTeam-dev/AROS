function(aros_request_includes TARGET DEPENDENCY)
	#message(ERROR "HI! ${TARGET}")

	target_include_directories(${TARGET} PUBLIC
			$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/CMakeFiles/include/${DEPENDENCY}>
			$<INSTALL_INTERFACE:include/${DEPENDENCY}>
			)

	add_dependencies(${TARGET} ${DEPENDENCY}_includes)
endfunction()

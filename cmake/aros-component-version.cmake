# Get the version from the config file
file(READ "${PROJECT_NAME}.conf" contents)
# Convert file contents into a CMake list (where each element in the list
# is one line of the file)
#
string(REGEX REPLACE ";" "\\\\;" contents "${contents}")
string(REGEX REPLACE "\n" ";" contents "${contents}")

set(VERSION_REGEX "version\ ([0-9]+).([0-9]+)$")

foreach(item ${contents})
	if(item MATCHES ${VERSION_REGEX})
		STRING(REGEX REPLACE
				${VERSION_REGEX}
				"\\1.\\2" PROJECT_VERSION ${item})
	endif()
endforeach()
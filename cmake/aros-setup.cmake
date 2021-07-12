if(NOT AROS_ROOT AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        set(AROS_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../")

        get_filename_component(AROS_ROOT_DIR "${AROS_ROOT_DIR}"
                        REALPATH BASE_DIR "${CMAKE_BINARY_DIR}")

        message(STATUS "AROS root directory: ${AROS_ROOT_DIR}")
else()
        set(AROS_ROOT_DIR "${CMAKE_SOURCE_DIR}/")
endif()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${AROS_ROOT_DIR}/cmake")

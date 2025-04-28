cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(TestAPIHeaders
        LANGUAGES C
        )

include(CheckIncludeFile)

set(CMAKE_REQUIRED_INCLUDES "${INCLUDE_DIRS}")

foreach(INCLUDE_HEADER ${INCLUDE_FILES})
    cmake_path(GET INCLUDE_HEADER FILENAME INCLUDE_FILENAME)
    string(REPLACE "." "_" INCLUDE_FILENAME "${INCLUDE_FILENAME}")
    check_include_file(${INCLUDE_HEADER} PROJECTM_${INCLUDE_FILENAME}_INCLUDE_OK)
    if(NOT PROJECTM_${INCLUDE_FILENAME}_INCLUDE_OK)
        message(FATAL_ERROR "projectM API include file ${INCLUDE_HEADER} does not compile on its own!\nSee logs in ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/ for additional information.")
    endif()
endforeach()

file(TOUCH ${OUTPUT_FILE})

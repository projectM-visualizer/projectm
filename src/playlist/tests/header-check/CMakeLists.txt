# CMakeLists.txt in tests/header-check
cmake_minimum_required(VERSION 3.15)
project(PublicHeaderCheck C)

# Set this to your install prefix or build tree path
set(PM_INCLUDE_DIR "/usr/local/include")

# List your public headers here
set(PUBLIC_HEADERS
        "${PM_INCLUDE_DIR}/projectM-4/playlist.h"
)

# Generate dummy .c files that just include the headers
foreach(header IN LISTS PUBLIC_HEADERS)
    get_filename_component(header_file "${header}" NAME)
    set(test_source "${CMAKE_BINARY_DIR}/check_${header_file}.c")

    file(WRITE "${test_source}" "#include \"${header}\"\nint main(void) { return 0; }\n")
    add_executable("check_${header_file}" "${test_source}")
    target_include_directories("check_${header_file}" PRIVATE "${PM_INCLUDE_DIR}")
endforeach()

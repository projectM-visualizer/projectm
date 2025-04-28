cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

# Compile-test C API headers
macro(TEST_API_HEADERS _target _include_dirs _includes)
    set(_test_project_dir "${CMAKE_CURRENT_BINARY_DIR}/${_target}")
    set(HEADER_TEST_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_target}.done)

    add_custom_command(OUTPUT ${HEADER_TEST_OUTPUT}
            # Create test project directory
            COMMAND ${CMAKE_COMMAND}
            ARGS
            -E make_directory "${_test_project_dir}"

            # Copy project CMake file into it
            COMMAND ${CMAKE_COMMAND}
            ARGS
            -E copy_if_different "${CMAKE_SOURCE_DIR}/cmake/TestAPIHeadersProject.cmake" "${_test_project_dir}/CMakeLists.txt"

            # Delete any existing build dir to re-run all checks
            COMMAND ${CMAKE_COMMAND}
            ARGS
            -E rm -Rf "${_test_project_dir}/build"

            # Configure the test project
            COMMAND ${CMAKE_COMMAND}
            ARGS
            -S ${_test_project_dir}
            -B ${_test_project_dir}/build
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
            -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
            -DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}
            "-DINCLUDE_FILES=${_includes}"
            "-DINCLUDE_DIRS=${_include_dirs}"
            -DOUTPUT_FILE=${HEADER_TEST_OUTPUT}

            DEPENDS
            ${CMAKE_SOURCE_DIR}/cmake/TestAPIHeaders.cmake
            ${CMAKE_SOURCE_DIR}/cmake/TestAPIHeadersProject.cmake
            ${_includes}

            VERBATIM
            COMMENT "Testing C API headers: ${_target}"
            )

    add_custom_target(${_target} ALL
            DEPENDS ${HEADER_TEST_OUTPUT}
            )

    unset(_test_project_dir)

endmacro()

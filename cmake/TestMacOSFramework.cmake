# TestMacOSFramework.cmake
# CI test functions for macOS framework validation
#
# This module provides functions to validate that macOS frameworks are
# correctly built with proper structure, headers, and linkability.

# Validate the structure of a macOS framework
#
# Usage:
#   test_framework_structure(
#       FRAMEWORK_PATH <path-to-framework>
#       FRAMEWORK_NAME <name>
#   )
#
# Returns FATAL_ERROR if validation fails.
#
function(test_framework_structure)
    set(options "")
    set(oneValueArgs FRAMEWORK_PATH FRAMEWORK_NAME)
    set(multiValueArgs "")
    cmake_parse_arguments(TF "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT TF_FRAMEWORK_PATH)
        message(FATAL_ERROR "test_framework_structure: FRAMEWORK_PATH is required")
    endif()
    if(NOT TF_FRAMEWORK_NAME)
        message(FATAL_ERROR "test_framework_structure: FRAMEWORK_NAME is required")
    endif()

    message(STATUS "Validating framework structure: ${TF_FRAMEWORK_PATH}")

    # Check framework directory exists
    if(NOT EXISTS "${TF_FRAMEWORK_PATH}")
        message(FATAL_ERROR "Framework does not exist: ${TF_FRAMEWORK_PATH}")
    endif()

    # Check Versions directory
    if(NOT EXISTS "${TF_FRAMEWORK_PATH}/Versions")
        message(FATAL_ERROR "Framework missing Versions directory")
    endif()

    # Check Version A directory
    if(NOT EXISTS "${TF_FRAMEWORK_PATH}/Versions/A")
        message(FATAL_ERROR "Framework missing Versions/A directory")
    endif()

    # Check Current symlink
    if(NOT EXISTS "${TF_FRAMEWORK_PATH}/Versions/Current")
        message(FATAL_ERROR "Framework missing Versions/Current symlink")
    endif()

    # Check binary
    if(NOT EXISTS "${TF_FRAMEWORK_PATH}/Versions/A/${TF_FRAMEWORK_NAME}")
        message(FATAL_ERROR "Framework missing binary: Versions/A/${TF_FRAMEWORK_NAME}")
    endif()

    # Check Headers directory
    if(NOT EXISTS "${TF_FRAMEWORK_PATH}/Versions/A/Headers")
        message(FATAL_ERROR "Framework missing Headers directory")
    endif()

    # Check Resources directory
    if(NOT EXISTS "${TF_FRAMEWORK_PATH}/Versions/A/Resources")
        message(FATAL_ERROR "Framework missing Resources directory")
    endif()

    # Check Info.plist
    if(NOT EXISTS "${TF_FRAMEWORK_PATH}/Versions/A/Resources/Info.plist")
        message(FATAL_ERROR "Framework missing Info.plist")
    endif()

    # Check top-level symlinks
    if(NOT EXISTS "${TF_FRAMEWORK_PATH}/${TF_FRAMEWORK_NAME}")
        message(FATAL_ERROR "Framework missing top-level binary symlink")
    endif()
    if(NOT EXISTS "${TF_FRAMEWORK_PATH}/Headers")
        message(FATAL_ERROR "Framework missing top-level Headers symlink")
    endif()
    if(NOT EXISTS "${TF_FRAMEWORK_PATH}/Resources")
        message(FATAL_ERROR "Framework missing top-level Resources symlink")
    endif()

    message(STATUS "Framework structure validation passed")
endfunction()

# Validate that expected headers exist in a framework
#
# Usage:
#   test_framework_headers(
#       FRAMEWORK_PATH <path-to-framework>
#       FRAMEWORK_NAME <name>
#       EXPECTED_HEADERS <list of expected header paths relative to Headers/<name>/>
#   )
#
function(test_framework_headers)
    set(options "")
    set(oneValueArgs FRAMEWORK_PATH FRAMEWORK_NAME)
    set(multiValueArgs EXPECTED_HEADERS)
    cmake_parse_arguments(TF "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT TF_FRAMEWORK_PATH)
        message(FATAL_ERROR "test_framework_headers: FRAMEWORK_PATH is required")
    endif()
    if(NOT TF_FRAMEWORK_NAME)
        message(FATAL_ERROR "test_framework_headers: FRAMEWORK_NAME is required")
    endif()

    message(STATUS "Validating framework headers: ${TF_FRAMEWORK_PATH}")

    set(_headers_dir "${TF_FRAMEWORK_PATH}/Versions/A/Headers/${TF_FRAMEWORK_NAME}")

    # Check that headers directory exists
    if(NOT EXISTS "${_headers_dir}")
        message(FATAL_ERROR "Framework headers directory does not exist: ${_headers_dir}")
    endif()

    # Check each expected header
    foreach(_header ${TF_EXPECTED_HEADERS})
        if(NOT EXISTS "${_headers_dir}/${_header}")
            message(FATAL_ERROR "Framework missing expected header: ${_header}")
        endif()
    endforeach()

    message(STATUS "Framework headers validation passed")
endfunction()

# Test that a framework can be linked against
#
# Usage:
#   test_framework_linkability(
#       FRAMEWORK_PATH <path-to-framework>
#       FRAMEWORK_NAME <name>
#       TEST_SOURCE <source code to compile>
#       [INCLUDE_SUBDIR <subdirectory under Headers for includes>]
#   )
#
function(test_framework_linkability)
    set(options "")
    set(oneValueArgs FRAMEWORK_PATH FRAMEWORK_NAME TEST_SOURCE INCLUDE_SUBDIR)
    set(multiValueArgs "")
    cmake_parse_arguments(TF "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT TF_FRAMEWORK_PATH)
        message(FATAL_ERROR "test_framework_linkability: FRAMEWORK_PATH is required")
    endif()
    if(NOT TF_FRAMEWORK_NAME)
        message(FATAL_ERROR "test_framework_linkability: FRAMEWORK_NAME is required")
    endif()
    if(NOT TF_TEST_SOURCE)
        message(FATAL_ERROR "test_framework_linkability: TEST_SOURCE is required")
    endif()

    message(STATUS "Testing framework linkability: ${TF_FRAMEWORK_PATH}")

    # Get framework parent directory for -F flag
    get_filename_component(_framework_parent "${TF_FRAMEWORK_PATH}" DIRECTORY)

    # Determine include path
    if(TF_INCLUDE_SUBDIR)
        set(_include_path "${TF_FRAMEWORK_PATH}/Headers/${TF_INCLUDE_SUBDIR}")
    else()
        set(_include_path "${TF_FRAMEWORK_PATH}/Headers/${TF_FRAMEWORK_NAME}")
    endif()

    # Write test source to a temporary file
    set(_test_file "${CMAKE_CURRENT_BINARY_DIR}/framework_link_test.c")
    file(WRITE "${_test_file}" "${TF_TEST_SOURCE}")

    # Try to compile and link
    set(_test_output "${CMAKE_CURRENT_BINARY_DIR}/framework_link_test")
    execute_process(
        COMMAND ${CMAKE_C_COMPILER}
            -F "${_framework_parent}"
            -I "${_include_path}"
            -framework ${TF_FRAMEWORK_NAME}
            "${_test_file}"
            -o "${_test_output}"
        RESULT_VARIABLE _result
        OUTPUT_VARIABLE _output
        ERROR_VARIABLE _error
    )

    if(NOT _result EQUAL 0)
        message(FATAL_ERROR "Framework linkability test failed:\n${_error}")
    endif()

    # Clean up test files
    file(REMOVE "${_test_file}" "${_test_output}")

    message(STATUS "Framework linkability validation passed")
endfunction()

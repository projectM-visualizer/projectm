# MacOSFramework.cmake
# Build macOS framework bundles from scratch, with proper header hierarchy support.
#
# This module provides functions to create macOS framework bundles manually,
# bypassing CMake's built-in FRAMEWORK support which doesn't preserve header
# directory structures.

# Create a macOS framework bundle from a shared library target.
#
# Usage:
#   create_macos_framework(
#       TARGET <target>
#       FRAMEWORK_NAME <name>
#       IDENTIFIER <bundle-identifier>
#       VERSION <version>
#       C_HEADERS <list of C header files>
#       [CXX_HEADERS <list of C++ header files with subdirs>]
#       [HEADER_BASE_DIR <base directory to strip from header paths>]
#       [HEADER_SUBDIR <subdirectory name under Headers, defaults to FRAMEWORK_NAME>]
#   )
#
# The framework will be created in the same directory as the target's output.
# C_HEADERS are installed flat under Headers/<header-subdir>/
# CXX_HEADERS preserve their directory structure relative to HEADER_BASE_DIR.
#
function(create_macos_framework)
    set(options "")
    set(oneValueArgs TARGET FRAMEWORK_NAME IDENTIFIER VERSION HEADER_BASE_DIR HEADER_SUBDIR)
    set(multiValueArgs C_HEADERS CXX_HEADERS)
    cmake_parse_arguments(FW "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT FW_TARGET)
        message(FATAL_ERROR "create_macos_framework: TARGET is required")
    endif()
    if(NOT FW_FRAMEWORK_NAME)
        message(FATAL_ERROR "create_macos_framework: FRAMEWORK_NAME is required")
    endif()
    if(NOT FW_IDENTIFIER)
        message(FATAL_ERROR "create_macos_framework: IDENTIFIER is required")
    endif()
    if(NOT FW_VERSION)
        message(FATAL_ERROR "create_macos_framework: VERSION is required")
    endif()

    # Use a concrete output directory (CMAKE_CURRENT_BINARY_DIR) for the framework
    # This avoids issues with generator expressions in OUTPUT paths
    set(_framework_dir "${CMAKE_CURRENT_BINARY_DIR}/${FW_FRAMEWORK_NAME}.framework")
    set(_versions_dir "${_framework_dir}/Versions")
    set(_version_a_dir "${_versions_dir}/A")

    # Headers are placed in Headers/<header-subdir>/ to support existing include patterns.
    # By default, header-subdir is the framework name, but can be overridden with HEADER_SUBDIR.
    # Users should add -I <framework>/Headers to their include path.
    if(FW_HEADER_SUBDIR)
        set(_header_subdir "${FW_HEADER_SUBDIR}")
    else()
        set(_header_subdir "${FW_FRAMEWORK_NAME}")
    endif()
    set(_headers_dir "${_version_a_dir}/Headers/${_header_subdir}")
    set(_resources_dir "${_version_a_dir}/Resources")

    # Marker file to track framework creation
    set(_framework_marker "${CMAKE_CURRENT_BINARY_DIR}/${FW_FRAMEWORK_NAME}.framework.marker")

    # Generate Info.plist content
    set(_info_plist_content "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">
<plist version=\"1.0\">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleExecutable</key>
    <string>${FW_FRAMEWORK_NAME}</string>
    <key>CFBundleIdentifier</key>
    <string>${FW_IDENTIFIER}</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>${FW_FRAMEWORK_NAME}</string>
    <key>CFBundlePackageType</key>
    <string>FMWK</string>
    <key>CFBundleShortVersionString</key>
    <string>${FW_VERSION}</string>
    <key>CFBundleVersion</key>
    <string>${FW_VERSION}</string>
</dict>
</plist>")

    # Write Info.plist to build directory (will be copied by custom command)
    set(_info_plist_file "${CMAKE_CURRENT_BINARY_DIR}/${FW_FRAMEWORK_NAME}_Info.plist")
    file(WRITE "${_info_plist_file}" "${_info_plist_content}")

    # Build list of commands to copy headers
    set(_header_copy_commands "")
    set(_header_dependencies "")

    # Process C headers (flat structure)
    foreach(_header ${FW_C_HEADERS})
        get_filename_component(_header_name "${_header}" NAME)
        list(APPEND _header_copy_commands
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_header}" "${_headers_dir}/${_header_name}"
        )
        list(APPEND _header_dependencies "${_header}")
    endforeach()

    # Process C++ headers (preserve directory structure)
    if(FW_CXX_HEADERS AND FW_HEADER_BASE_DIR)
        foreach(_header ${FW_CXX_HEADERS})
            # Get relative path from base directory
            file(RELATIVE_PATH _rel_path "${FW_HEADER_BASE_DIR}" "${_header}")
            get_filename_component(_rel_dir "${_rel_path}" DIRECTORY)

            if(_rel_dir)
                list(APPEND _header_copy_commands
                    COMMAND ${CMAKE_COMMAND} -E make_directory "${_headers_dir}/${_rel_dir}"
                )
            endif()
            list(APPEND _header_copy_commands
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_header}" "${_headers_dir}/${_rel_path}"
            )
            list(APPEND _header_dependencies "${_header}")
        endforeach()
    elseif(FW_CXX_HEADERS)
        # No base dir specified, install flat
        foreach(_header ${FW_CXX_HEADERS})
            get_filename_component(_header_name "${_header}" NAME)
            list(APPEND _header_copy_commands
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_header}" "${_headers_dir}/${_header_name}"
            )
            list(APPEND _header_dependencies "${_header}")
        endforeach()
    endif()

    # Create custom command to build the framework
    # Note: We use a marker file as OUTPUT since the actual output path depends on the target
    add_custom_command(
        OUTPUT "${_framework_marker}"
        # Clean up any existing framework directory to ensure clean symlinks
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${_framework_dir}"

        # Create directory structure
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_version_a_dir}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_headers_dir}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_resources_dir}"

        # Copy the dylib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${FW_TARGET}>" "${_version_a_dir}/${FW_FRAMEWORK_NAME}"

        # Copy Info.plist
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_info_plist_file}" "${_resources_dir}/Info.plist"

        # Copy headers
        ${_header_copy_commands}

        # Create symlinks (Current -> A)
        COMMAND ${CMAKE_COMMAND} -E create_symlink "A" "${_versions_dir}/Current"

        # Create top-level symlinks
        COMMAND ${CMAKE_COMMAND} -E create_symlink "Versions/Current/${FW_FRAMEWORK_NAME}" "${_framework_dir}/${FW_FRAMEWORK_NAME}"
        COMMAND ${CMAKE_COMMAND} -E create_symlink "Versions/Current/Headers" "${_framework_dir}/Headers"
        COMMAND ${CMAKE_COMMAND} -E create_symlink "Versions/Current/Resources" "${_framework_dir}/Resources"

        # Create marker file
        COMMAND ${CMAKE_COMMAND} -E touch "${_framework_marker}"

        DEPENDS ${FW_TARGET} ${_header_dependencies}
        COMMENT "Building ${FW_FRAMEWORK_NAME}.framework"
        VERBATIM
    )

    # Create a target that depends on the framework being built
    add_custom_target(${FW_TARGET}_framework ALL
        DEPENDS "${_framework_marker}"
    )

    # Set properties on the framework target for use by install commands
    set_target_properties(${FW_TARGET} PROPERTIES
        MACOS_FRAMEWORK_LOCATION "${_framework_dir}"
        MACOS_FRAMEWORK_NAME "${FW_FRAMEWORK_NAME}"
    )
endfunction()

# Install a framework created by create_macos_framework
#
# Usage:
#   install_macos_framework(
#       TARGET <target>
#       DESTINATION <install-dir>
#       [COMPONENT <component>]
#   )
#
function(install_macos_framework)
    set(options "")
    set(oneValueArgs TARGET DESTINATION COMPONENT)
    set(multiValueArgs "")
    cmake_parse_arguments(FW "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT FW_TARGET)
        message(FATAL_ERROR "install_macos_framework: TARGET is required")
    endif()
    if(NOT FW_DESTINATION)
        message(FATAL_ERROR "install_macos_framework: DESTINATION is required")
    endif()

    get_target_property(_framework_name ${FW_TARGET} MACOS_FRAMEWORK_NAME)
    if(NOT _framework_name)
        message(FATAL_ERROR "install_macos_framework: Target ${FW_TARGET} does not have MACOS_FRAMEWORK_NAME property. Did you call create_macos_framework first?")
    endif()

    # Install the entire framework directory
    set(_component_arg "")
    if(FW_COMPONENT)
        set(_component_arg COMPONENT ${FW_COMPONENT})
    endif()

    install(
        DIRECTORY "$<TARGET_FILE_DIR:${FW_TARGET}>/${_framework_name}.framework"
        DESTINATION "${FW_DESTINATION}"
        ${_component_arg}
        USE_SOURCE_PERMISSIONS
    )
endfunction()

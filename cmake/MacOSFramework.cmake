# MacOSFramework.cmake
# Build Apple framework bundles from scratch, with proper header hierarchy support.
#
# On macOS the bundle is versioned (Versions/A with symlinks). On iOS, tvOS, watchOS and visionOS
# it is shallow, as those platforms require: the binary, Info.plist and Headers sit at the bundle
# root, and the C headers go directly into Headers/ so that clients include them as
# <FRAMEWORK_NAME/header.h>. In both layouts the library's install name points into the bundle.
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
# The framework will be created in CMAKE_CURRENT_BINARY_DIR.
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

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        set(_shallow FALSE)
    else()
        set(_shallow TRUE)
    endif()

    # Headers are placed in Headers/<header-subdir>/ to support existing include patterns.
    # By default, header-subdir is the framework name, but can be overridden with HEADER_SUBDIR.
    # Users should add -I <framework>/Headers to their include path.
    if(FW_HEADER_SUBDIR)
        set(_header_subdir "${FW_HEADER_SUBDIR}")
    else()
        set(_header_subdir "${FW_FRAMEWORK_NAME}")
    endif()
    if(_shallow)
        # Shallow bundle: headers flat in Headers/, Info.plist and binary at the root.
        set(_bundle_dir "${_framework_dir}")
        set(_headers_dir "${_framework_dir}/Headers")
        set(_resources_dir "${_framework_dir}")
        set(_install_name "@rpath/${FW_FRAMEWORK_NAME}.framework/${FW_FRAMEWORK_NAME}")
    else()
        set(_bundle_dir "${_version_a_dir}")
        set(_headers_dir "${_version_a_dir}/Headers/${_header_subdir}")
        set(_resources_dir "${_version_a_dir}/Resources")
        set(_install_name "@rpath/${FW_FRAMEWORK_NAME}.framework/Versions/A/${FW_FRAMEWORK_NAME}")
    endif()

    # The library's install name must point into the bundle, not at the plain dylib name.
    set_target_properties(${FW_TARGET} PROPERTIES NO_SONAME ON)
    target_link_options(${FW_TARGET} PRIVATE "LINKER:-install_name,${_install_name}")

    # Keys that iOS-family bundles need in addition to the macOS ones.
    set(_platform_plist_keys "")
    if(_shallow)
        # CFBundleSupportedPlatforms uses the SDK platform names (iPhoneOS, AppleTVSimulator, ...).
        set(_platform_names_iOS "iPhone")
        set(_platform_names_tvOS "AppleTV")
        set(_platform_names_watchOS "Watch")
        set(_platform_names_visionOS "XR")
        set(_supported_platform "${_platform_names_${CMAKE_SYSTEM_NAME}}")
        if(NOT _supported_platform)
            set(_supported_platform "${CMAKE_SYSTEM_NAME}")
        endif()
        if(CMAKE_OSX_SYSROOT MATCHES "[Ss]imulator")
            string(APPEND _supported_platform "Simulator")
        else()
            string(APPEND _supported_platform "OS")
        endif()
        set(_platform_plist_keys "
    <key>CFBundleSupportedPlatforms</key>
    <array>
        <string>${_supported_platform}</string>
    </array>")
        if(CMAKE_OSX_DEPLOYMENT_TARGET)
            string(APPEND _platform_plist_keys "
    <key>MinimumOSVersion</key>
    <string>${CMAKE_OSX_DEPLOYMENT_TARGET}</string>")
        endif()
    endif()

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
    <string>${FW_VERSION}</string>${_platform_plist_keys}
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

    # Layout-specific steps: macOS gets Versions/A plus symlinks; shallow bundles need none.
    if(_shallow)
        set(_layout_commands "")
    else()
        set(_layout_commands
            # Create symlinks (Current -> A)
            COMMAND ${CMAKE_COMMAND} -E create_symlink "A" "${_versions_dir}/Current"

            # Create top-level symlinks
            COMMAND ${CMAKE_COMMAND} -E create_symlink "Versions/Current/${FW_FRAMEWORK_NAME}" "${_framework_dir}/${FW_FRAMEWORK_NAME}"
            COMMAND ${CMAKE_COMMAND} -E create_symlink "Versions/Current/Headers" "${_framework_dir}/Headers"
            COMMAND ${CMAKE_COMMAND} -E create_symlink "Versions/Current/Resources" "${_framework_dir}/Resources"
        )
    endif()

    # Create custom command to build the framework
    # Note: We use a marker file as OUTPUT since the actual output path depends on the target
    add_custom_command(
        OUTPUT "${_framework_marker}"
        # Clean up any existing framework directory to ensure clean symlinks
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${_framework_dir}"

        # Create directory structure
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_bundle_dir}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_headers_dir}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_resources_dir}"

        # Copy the dylib
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${FW_TARGET}>" "${_bundle_dir}/${FW_FRAMEWORK_NAME}"

        # Copy Info.plist
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${_info_plist_file}" "${_resources_dir}/Info.plist"

        # Copy headers
        ${_header_copy_commands}

        ${_layout_commands}

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
        MACOS_FRAMEWORK_OUTPUT_DIR "${_framework_dir}"
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

    get_target_property(_framework_dir ${FW_TARGET} MACOS_FRAMEWORK_OUTPUT_DIR)
    install(
        DIRECTORY "${_framework_dir}"
        DESTINATION "${FW_DESTINATION}"
        ${_component_arg}
        USE_SOURCE_PERMISSIONS
    )
endfunction()

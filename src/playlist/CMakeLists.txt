if(NOT ENABLE_PLAYLIST)
    return()
endif()

set(PROJECTM_PLAYLIST_EXPORT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/projectM-4/projectM_playlist_export.h")

set(PROJECTM_PLAYLIST_PUBLIC_HEADERS
        "${PROJECTM_PLAYLIST_EXPORT_HEADER}"
        "${CMAKE_CURRENT_SOURCE_DIR}/api/projectM-4/playlist.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/api/projectM-4/playlist_callbacks.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/api/projectM-4/playlist_core.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/api/projectM-4/playlist_filter.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/api/projectM-4/playlist_items.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/api/projectM-4/playlist_memory.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/api/projectM-4/playlist_playback.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/api/projectM-4/playlist_types.h"
        )

add_library(projectM_playlist_main OBJECT
        ${PROJECTM_PLAYLIST_PUBLIC_HEADERS}
        Filter.cpp
        Filter.hpp
        Item.cpp
        Item.hpp
        Playlist.cpp
        Playlist.hpp
        PlaylistCWrapper.cpp
        PlaylistCWrapper.hpp
        )

target_include_directories(projectM_playlist_main
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        )

target_link_libraries(projectM_playlist_main
        PRIVATE
        libprojectM::API
        ${PROJECTM_FILESYSTEM_LIBRARY}
        )

add_library(projectM_playlist
        ${PROJECTM_DUMMY_SOURCE_FILE} # CMake needs at least one "real" source file.
        $<TARGET_OBJECTS:projectM_playlist_main>
        )

set_target_properties(projectM_playlist PROPERTIES
        VERSION "${PROJECTM_LIB_VERSION}"
        SOVERSION "${PROJECTM_SO_VERSION}"
        EXPORT_NAME playlist
        FOLDER libprojectM
        OUTPUT_NAME ${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}-playlist
        PUBLIC_HEADER "${PROJECTM_PLAYLIST_PUBLIC_HEADERS}"
        )

target_include_directories(projectM_playlist
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/api>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        "$<INSTALL_INTERFACE:${PROJECTM_INCLUDE_DIR}>"
        )

target_link_libraries(projectM_playlist
        PUBLIC
        libprojectM::projectM
        ${PROJECTM_FILESYSTEM_LIBRARY}
        )

if(BUILD_SHARED_LIBS)
    target_compile_definitions(projectM_playlist
            PRIVATE
            projectM_api_EXPORTS
            )

    target_link_libraries(projectM_playlist
            PUBLIC
            ${CMAKE_DL_LIBS}
            )

    if(ENABLE_MACOS_FRAMEWORK)
        include(MacOSFramework)

        create_macos_framework(
                TARGET projectM_playlist
                FRAMEWORK_NAME ${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}-playlist
                IDENTIFIER com.github.projectm-visualizer.projectM-playlist
                VERSION "${PROJECT_VERSION}"
                C_HEADERS ${PROJECTM_PLAYLIST_PUBLIC_HEADERS}
                # Headers use #include "projectM-4/..." so we need to match that path
                HEADER_SUBDIR "projectM-4"
        )
    endif()
else()
    target_compile_definitions(projectM_playlist_main
            PUBLIC
            PROJECTM_STATIC_DEFINE
            )
    target_compile_definitions(projectM_playlist
            PUBLIC
            PROJECTM_STATIC_DEFINE
            )

    set_target_properties(projectM_playlist PROPERTIES
            OUTPUT_NAME ${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}-playlist
            FOLDER libprojectM
            )
endif()

include(GenerateExportHeader)

generate_export_header(projectM_playlist
        BASE_NAME projectM_playlist
        EXPORT_FILE_NAME "${PROJECTM_PLAYLIST_EXPORT_HEADER}"
        )

add_library(libprojectM::playlist ALIAS projectM_playlist)

if(ENABLE_INSTALL)

    if(ENABLE_MACOS_FRAMEWORK)
        # Install the manually-built framework
        install_macos_framework(
                TARGET projectM_playlist
                DESTINATION "${PROJECTM_LIB_DIR}"
                COMPONENT Runtime
        )
        # Frameworks are used directly without CMake find_package, so skip target exports
    else()
        install(TARGETS projectM_playlist
                EXPORT libprojectMPlaylist
                LIBRARY DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Runtime
                RUNTIME DESTINATION "${PROJECTM_RUNTIME_DIR}" COMPONENT Runtime
                ARCHIVE DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Devel
                PUBLIC_HEADER DESTINATION "${PROJECTM_INCLUDE_DIR}/projectM-4" COMPONENT Devel
                )

        # For use from an installed package (system install, vcpkg, homebrew etc.)
        include(CMakePackageConfigHelpers)

        write_basic_package_version_file(
                "${CMAKE_CURRENT_BINARY_DIR}/libprojectMPlaylist/projectM4PlaylistConfigVersion.cmake"
                VERSION ${PROJECT_VERSION}
                COMPATIBILITY AnyNewerVersion
                )

        configure_package_config_file(projectM4PlaylistConfig.cmake.in
                "${CMAKE_CURRENT_BINARY_DIR}/libprojectMPlaylist/projectM4PlaylistConfig.cmake"
                INSTALL_DESTINATION "${PROJECTM_LIB_DIR}/cmake/projectM4Playlist"
                PATH_VARS PROJECTM_BIN_DIR PROJECTM_INCLUDE_DIR
                )

        install(FILES
                "${CMAKE_CURRENT_BINARY_DIR}/libprojectMPlaylist/projectM4PlaylistConfigVersion.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/libprojectMPlaylist/projectM4PlaylistConfig.cmake"
                DESTINATION "${PROJECTM_LIB_DIR}/cmake/projectM4Playlist"
                COMPONENT Devel
                )

        install(EXPORT libprojectMPlaylist
                FILE projectM4PlaylistTargets.cmake
                NAMESPACE libprojectM::
                DESTINATION "${PROJECTM_LIB_DIR}/cmake/projectM4Playlist"
                COMPONENT Devel
                )
    endif()

    # pkg-config export, only supported on UNIX systems (not for frameworks).
    if(UNIX AND NOT ENABLE_MACOS_FRAMEWORK)
        include(GeneratePkgConfigFiles)

        if(ENABLE_BOOST_FILESYSTEM)
            # Since there are many different CMake scripts out there to find Boost, e.g. Gentoo has its own Find module,
            # it's very hard to extract the proper library names and paths from the Boost package targets and convert them
            # into pkgconfig's expected format.
            # We will just assume the build uses Boost from the default location (e.g. not a custom CMAKE_PREFIX_PATH) and
            # the library name is correct.
            set(PKGCONFIG_LIBS "${PKGCONFIG_LIBS} -lboost_filesystem")
        endif()

        set(PKGCONFIG_PACKAGE_NAME "${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}-playlist")
        set(PKGCONFIG_PACKAGE_DESCRIPTION "projectM Playlist Library")
        set(PKGCONFIG_PACKAGE_REQUIREMENTS_RELEASE "${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}")
        set(PKGCONFIG_PACKAGE_REQUIREMENTS_DEBUG "${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}-debug")

        generate_pkg_config_files(projectM_playlist ${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}-playlist)
    endif()

endif()

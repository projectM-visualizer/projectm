if(NOT ENABLE_PLAYLIST)
    return()
endif()

add_library(projectM_playlist_main OBJECT
        Filter.cpp
        Filter.hpp
        Item.cpp
        Item.hpp
        Playlist.cpp
        Playlist.hpp
        PlaylistCWrapper.cpp
        PlaylistCWrapper.hpp
        api/projectM-4/playlist.h
        api/projectM-4/playlist_callbacks.h
        api/projectM-4/playlist_core.h
        api/projectM-4/playlist_filter.h
        api/projectM-4/playlist_items.h
        api/projectM-4/playlist_memory.h
        api/projectM-4/playlist_playback.h
        api/projectM-4/playlist_types.h
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
else()
    target_compile_definitions(projectM_playlist
            PUBLIC
            PROJECTM_STATIC_DEFINE
            )

    set_target_properties(projectM_playlist PROPERTIES
            OUTPUT_NAME $<IF:$<PLATFORM_ID:Windows>,lib${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}-playlist,${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}-playlist>
            FOLDER libprojectM
            )
endif()

include(GenerateExportHeader)

set(PROJECTM_PLAYLIST_EXPORT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/projectM-4/projectM_playlist_export.h")

generate_export_header(projectM_playlist
        BASE_NAME projectM_playlist
        EXPORT_FILE_NAME "${PROJECTM_PLAYLIST_EXPORT_HEADER}"
        )

# Adds fallback support to boost if std::filesystem is unavailable.
include(FilesystemSupport.cmake)

add_library(libprojectM::playlist ALIAS projectM_playlist)

if(ENABLE_INSTALL)

    install(TARGETS projectM_playlist
            EXPORT libprojectMPlaylist
            LIBRARY DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Runtime
            RUNTIME DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Runtime
            ARCHIVE DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Devel
            PUBLIC_HEADER DESTINATION "${PROJECTM_INCLUDE_DIR}/projectM-4" COMPONENT Devel
            )

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/include/projectM-4/projectM_playlist_export.h"
            DESTINATION "${PROJECTM_INCLUDE_DIR}/projectM-4"
            COMPONENT Devel
            )

    install(DIRECTORY api/projectM-4
            DESTINATION "${PROJECTM_INCLUDE_DIR}"
            COMPONENT Devel
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

    # pkg-config export, only supported on UNIX systems.
    if(UNIX)
        include(GeneratePkgConfigFiles)

        set(PKGCONFIG_PACKAGE_NAME "${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}-playlist")
        set(PKGCONFIG_PACKAGE_DESCRIPTION "projectM Playlist Library")
        set(PKGCONFIG_PACKAGE_REQUIREMENTS_RELEASE "${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}")
        set(PKGCONFIG_PACKAGE_REQUIREMENTS_DEBUG "${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}-debug")

        generate_pkg_config_files(projectM_playlist ${PROJECTM_LIBRARY_BASE_OUTPUT_NAME}-playlist)
    endif()

endif()

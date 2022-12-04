if(NOT ENABLE_PLAYLIST)
    return()
endif()

add_library(projectM_playlist STATIC
        Filter.cpp
        Filter.hpp
        Item.cpp
        Item.hpp
        Playlist.cpp
        Playlist.hpp
        PlaylistCWrapper.cpp
        PlaylistCWrapper.h
        playlist.h
        )

target_include_directories(projectM_playlist
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        )

target_link_libraries(projectM_playlist
        PUBLIC
        projectM::API
        )

set_target_properties(projectM_playlist PROPERTIES
        EXPORT_NAME playlist
        FOLDER libprojectM
        )

# Adds fallback support to boost if std::filesystem is unavailable.
include(FilesystemSupport.cmake)

install(TARGETS projectM_playlist
        EXPORT libprojectMPlaylist
        LIBRARY DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Runtime
        RUNTIME DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Runtime
        ARCHIVE DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Devel
        PUBLIC_HEADER DESTINATION "${PROJECTM_INCLUDE_DIR}/libprojectM" COMPONENT Devel
        )

install(FILES
        playlist.h
        DESTINATION "${PROJECTM_INCLUDE_DIR}/libprojectM"
        COMPONENT Devel
        )


# For use from an installed package (system install, vcpkg, homebrew etc.)
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/libprojectMPlaylist/libprojectMPlaylistConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(libprojectMPlaylistConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/libprojectMPlaylist/libprojectMPlaylistConfig.cmake"
        INSTALL_DESTINATION "${PROJECTM_LIB_DIR}/cmake/libprojectMPlaylist"
        PATH_VARS PROJECTM_BIN_DIR PROJECTM_INCLUDE_DIR
        )

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/libprojectMPlaylist/libprojectMPlaylistConfigVersion.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/libprojectMPlaylist/libprojectMPlaylistConfig.cmake"
        DESTINATION "${PROJECTM_LIB_DIR}/cmake/libprojectMPlaylist"
        COMPONENT Devel
        )

install(EXPORT libprojectMPlaylist
        FILE libprojectMPlaylistTargets.cmake
        NAMESPACE libprojectM::
        DESTINATION "${PROJECTM_LIB_DIR}/cmake/libprojectMPlaylist"
        COMPONENT Devel
        )

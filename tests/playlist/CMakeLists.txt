if(NOT TARGET projectM_playlist)
    return()
endif()

find_package(GTest 1.10 REQUIRED NO_MODULE)

# Compile-test C API headers
set(INCLUDE_FILES
        ${CMAKE_SOURCE_DIR}/src/playlist/api/projectM-4/playlist_types.h

        ${CMAKE_SOURCE_DIR}/src/playlist/api/projectM-4/playlist_callbacks.h
        ${CMAKE_SOURCE_DIR}/src/playlist/api/projectM-4/playlist_core.h
        ${CMAKE_SOURCE_DIR}/src/playlist/api/projectM-4/playlist_filter.h
        ${CMAKE_SOURCE_DIR}/src/playlist/api/projectM-4/playlist_items.h
        ${CMAKE_SOURCE_DIR}/src/playlist/api/projectM-4/playlist_memory.h
        ${CMAKE_SOURCE_DIR}/src/playlist/api/projectM-4/playlist_playback.h

        # Convenience header last, so it doesn't obscure issues with a single header above.
        ${CMAKE_SOURCE_DIR}/src/playlist/api/projectM-4/playlist.h
        )

get_target_property(API_HEADER_INCLUDE_DIRS_MAIN libprojectM::API INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(API_HEADER_INCLUDE_DIRS_PLAYLIST projectM_playlist_main INTERFACE_INCLUDE_DIRECTORIES)

include(TestAPIHeaders)
test_api_headers(TestPlaylistAPIHeaders
        "${API_HEADER_INCLUDE_DIRS_MAIN};${API_HEADER_INCLUDE_DIRS_PLAYLIST}"
        "${INCLUDE_FILES}"
        )

add_executable(projectM-playlist-unittest
        $<TARGET_OBJECTS:projectM_playlist_main>
        APITest.cpp
        ItemTest.cpp
        PlaylistCWrapperMock.h
        PlaylistTest.cpp
        ProjectMAPIMocks.cpp
        FilterTest.cpp
        )

if(BUILD_SHARED_LIBS)
    set_source_files_properties(ProjectMAPIMocks.cpp PROPERTIES
            COMPILE_DEFINITIONS projectM_api_EXPORTS
            )
else()
    set_source_files_properties(ProjectMAPIMocks.cpp PROPERTIES
            COMPILE_DEFINITIONS PROJECTM_STATIC_DEFINE
            )
endif()

target_compile_definitions(projectM-playlist-unittest
        PRIVATE
        PROJECTM_PLAYLIST_TEST_DATA_DIR="${CMAKE_CURRENT_LIST_DIR}/data"
        )

target_link_libraries(projectM-playlist-unittest
        PRIVATE
        projectM_playlist_main
        libprojectM::API
        GTest::gmock_main
        )

add_test(NAME projectM-playlist-unittest COMMAND projectM-playlist-unittest)

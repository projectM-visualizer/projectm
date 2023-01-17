if(NOT TARGET projectM_playlist)
    return()
endif()

find_package(GTest 1.10 REQUIRED NO_MODULE)

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

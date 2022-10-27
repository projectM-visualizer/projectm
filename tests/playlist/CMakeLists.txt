if(NOT TARGET projectM_playlist)
    return()
endif()

find_package(GTest 1.10 REQUIRED NO_MODULE)

add_executable(projectM-playlist-unittest
        APITest.cpp
        ItemTest.cpp
        PlaylistCWrapperMock.cpp
        PlaylistCWrapperMock.h
        PlaylistTest.cpp
        )

target_link_libraries(projectM-playlist-unittest
        PRIVATE
        projectM_playlist
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        )

add_test(NAME projectM-playlist-unittest COMMAND projectM-playlist-unittest)

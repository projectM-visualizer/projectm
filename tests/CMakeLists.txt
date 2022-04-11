find_package(GTest 1.10 REQUIRED NO_MODULE)

add_executable(projectM-unittest
        ConfigFile.cpp
        ConfigFile.h
        getConfigFilename.cpp
        getConfigFilename.h
        projectM-unittest.cpp
        )

target_compile_definitions(projectM-unittest
        PRIVATE
        PROJECTM_TEST_DATA_DIR="${CMAKE_CURRENT_LIST_DIR}/data"
        )

# Test includes a header file from libprojectM with its full path in the source dir.
target_include_directories(projectM-unittest
        PRIVATE
        "${CMAKE_SOURCE_DIR}"
        )

target_link_libraries(projectM-unittest
        PRIVATE
        projectM_static
        GTest::gtest
        GTest::gtest_main
        )

add_test(NAME projectM-unittest COMMAND projectM-unittest)

find_package(GTest 1.10 REQUIRED NO_MODULE)

add_executable(projectM-unittest
        PCMTest.cpp
        PresetFileParserTest.cpp

        $<TARGET_OBJECTS:MilkdropPreset>
        $<TARGET_OBJECTS:Renderer>
        $<TARGET_OBJECTS:hlslparser>
        $<TARGET_OBJECTS:SOIL2>
        $<TARGET_OBJECTS:projectM_main>
        )

target_compile_definitions(projectM-unittest
        PRIVATE
        PROJECTM_TEST_DATA_DIR="${CMAKE_CURRENT_LIST_DIR}/data"
        )

# Test includes a header file from libprojectM with its full path in the source dir.
target_include_directories(projectM-unittest
        PRIVATE
        "${PROJECTM_SOURCE_DIR}/src/libprojectM"
        "${PROJECTM_SOURCE_DIR}"
        )

target_link_libraries(projectM-unittest
        PRIVATE
        projectM_main
        GTest::gtest
        GTest::gtest_main
        )

add_test(NAME projectM-unittest COMMAND projectM-unittest)

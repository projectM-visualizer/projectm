find_package(GTest 1.10 REQUIRED NO_MODULE)

# Compile-test C API headers
set(INCLUDE_FILES
        ${CMAKE_SOURCE_DIR}/src/api/include/projectM-4/types.h

        ${CMAKE_SOURCE_DIR}/src/api/include/projectM-4/audio.h
        ${CMAKE_SOURCE_DIR}/src/api/include/projectM-4/callbacks.h
        ${CMAKE_SOURCE_DIR}/src/api/include/projectM-4/core.h
        ${CMAKE_SOURCE_DIR}/src/api/include/projectM-4/debug.h
        ${CMAKE_SOURCE_DIR}/src/api/include/projectM-4/memory.h
        ${CMAKE_SOURCE_DIR}/src/api/include/projectM-4/parameters.h
        ${CMAKE_SOURCE_DIR}/src/api/include/projectM-4/render_opengl.h
        ${CMAKE_SOURCE_DIR}/src/api/include/projectM-4/touch.h
        ${CMAKE_SOURCE_DIR}/src/api/include/projectM-4/user_sprites.h

        # Convenience header last, so it doesn't obscure issues with a single header above.
        ${CMAKE_SOURCE_DIR}/src/api/include/projectM-4/projectM.h
        )

get_target_property(API_HEADER_INCLUDE_DIRS libprojectM::API INTERFACE_INCLUDE_DIRECTORIES)

include(TestAPIHeaders)
test_api_headers(TestMainAPIHeaders
        "${API_HEADER_INCLUDE_DIRS}"
        "${INCLUDE_FILES}"
        )

add_executable(projectM-unittest
        HLSLParserTest.cpp
        LoggingTest.cpp
        PresetFileParserTest.cpp
        WaveformAlignerTest.cpp

        $<TARGET_OBJECTS:Audio>
        $<TARGET_OBJECTS:MilkdropPreset>
        $<TARGET_OBJECTS:Renderer>
        $<TARGET_OBJECTS:UserSprites>
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
        "${PROJECTM_SOURCE_DIR}/vendor/hlslparser/src"
        )

target_link_libraries(projectM-unittest
        PRIVATE
        projectM_main
        GTest::gtest
        GTest::gtest_main
        )

add_test(NAME projectM-unittest COMMAND projectM-unittest)

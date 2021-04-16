add_compile_definitions(
        DATADIR_PATH="${PROJECTM_DATADIR_PATH}"
        GL_SILENCE_DEPRECATION
)

add_subdirectory(MilkdropPresetFactory)
add_subdirectory(NativePresetFactory)
add_subdirectory(Renderer)

# CMake cannot combine multiple static libraries using target_link_libraries.
# This syntax will pull in the compiled object files into the final library.
add_library(projectM_static STATIC
        $<TARGET_OBJECTS:MilkdropPresetFactory>
        $<TARGET_OBJECTS:NativePresetFactory>
        $<TARGET_OBJECTS:Renderer>
        $<TARGET_OBJECTS:hlslparser>
        $<TARGET_OBJECTS:SOIL2>
        Common.hpp
        ConfigFile.cpp
        ConfigFile.h
        dlldefs.h
        event.h
        fatal.h
        fftsg.cpp
        fftsg.h
        FileScanner.cpp
        FileScanner.hpp
        glError.h
        gltext.h
        HungarianMethod.hpp
        IdleTextures.hpp
        KeyHandler.cpp
        KeyHandler.hpp
        lvtoprojectM.h
        PCM.cpp
        PCM.hpp
        PipelineMerger.cpp
        PipelineMerger.hpp
        Preset.cpp
        Preset.hpp
        PresetChooser.cpp
        PresetChooser.hpp
        PresetFactory.cpp
        PresetFactory.hpp
        PresetFactoryManager.cpp
        PresetFactoryManager.hpp
        PresetLoader.cpp
        PresetLoader.hpp
        projectM.cpp
        projectM.hpp
        projectM-opengl.h
        RandomNumberGenerators.hpp
        resource.h
        sdltoprojectM.h
        TestRunner.cpp
        TestRunner.hpp
        TimeKeeper.cpp
        TimeKeeper.hpp
        timer.cpp
        timer.h
        wipemalloc.cpp
        wipemalloc.h
        )

set_target_properties(projectM_static PROPERTIES
        OUTPUT_NAME projectM
        )

target_include_directories(projectM_static
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

target_link_libraries(projectM_static
        PUBLIC
        MilkdropPresetFactory
        NativePresetFactory
        Renderer
        )

add_library(projectM_shared SHARED
        $<TARGET_OBJECTS:projectM_static>
        )

set_target_properties(projectM_shared PROPERTIES
        OUTPUT_NAME projectM
        VERSION "${PROJECTM_LIB_VERSION}"
        SOVERSION "${PROJECTM_SO_VERSION}"
        )

target_link_libraries(projectM_shared
        PUBLIC
        projectM_static
        ${CMAKE_DL_LIBS}
        )

enable_target_linker_flags_if_supported(projectM_shared PRIVATE --no-undefined)

install(TARGETS projectM_static projectM_shared
        LIBRARY DESTINATION "${PROJECTM_LIB_DIR}"
        RUNTIME DESTINATION "${PROJECTM_LIB_DIR}"
        ARCHIVE DESTINATION "${PROJECTM_LIB_DIR}"
        COMPONENT Runtime
        )

install(FILES
        Common.hpp
        PCM.hpp
        dlldefs.h
        event.h
        fatal.h
        projectM.hpp
        DESTINATION "${PROJECTM_INCLUDE_DIR}/libprojectM"
        COMPONENT Devel
        )

configure_file(config.inp.cmake.in "${CMAKE_BINARY_DIR}/config.inp" @ONLY)

install(FILES "${CMAKE_BINARY_DIR}/config.inp"
        DESTINATION "${PROJECTM_DATADIR_PATH}/"
        COMPONENT Runtime
        )

install(DIRECTORY "${CMAKE_SOURCE_DIR}/fonts"
        DESTINATION "${PROJECTM_DATADIR_PATH}"
        COMPONENT Runtime
        )

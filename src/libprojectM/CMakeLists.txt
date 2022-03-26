add_compile_definitions(
        DATADIR_PATH="${PROJECTM_DATADIR_PATH}"
        GL_SILENCE_DEPRECATION
        USE_TEXT_MENU
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # dirent.h support
    set(MSVC_EXTRA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/msvc")
    include_directories("${MSVC_EXTRA_INCLUDE_DIR}")
    
    add_library(dlfcn OBJECT
            msvc/dlfcn.c
            msvc/dlfcn.h
            )
            
    set_target_properties(dlfcn PROPERTIES
            FOLDER libprojectM
            )

    # Additional preprocessor definitions for Windows builds
    add_compile_definitions(
            USE_FTGL
            USE_NATIVE_GLEW
            STBI_NO_DDS
            projectM_FONT_MENU="${CMAKE_SOURCE_DIR}/fonts/VeraMono.ttf"
            projectM_FONT_TITLE="${CMAKE_SOURCE_DIR}/fonts/Vera.ttf"
            )
endif()

# List of targets used in export().
# Optional libraries will be added to the list as configured in the build options.
set(EXPORTED_TARGETS
        MilkdropPresetFactory
        Renderer
        hlslparser
        SOIL2
        )

add_subdirectory(MilkdropPresetFactory)
add_subdirectory(NativePresetFactory)
add_subdirectory(Renderer)

set(PROJECTM_EXPORT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/libprojectM/projectM_export.h")

# CMake cannot combine multiple static libraries using target_link_libraries.
# This syntax will pull in the compiled object files into the final library.
add_library(projectM_main OBJECT
        "${PROJECTM_EXPORT_HEADER}"
        Common.hpp
        ConfigFile.cpp
        ConfigFile.h
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
        wipemalloc.cpp
        wipemalloc.h
        )

target_include_directories(projectM_main
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/Renderer"
        "${CMAKE_CURRENT_SOURCE_DIR}/Renderer/hlslparser/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/MilkdropPresetFactory"
        "${CMAKE_CURRENT_SOURCE_DIR}/NativePresetFactory"
        "${MSVC_EXTRA_INCLUDE_DIR}"
        )

target_link_libraries(projectM_main
        PRIVATE
        GLM::GLM
        PUBLIC
        ${PROJECTM_OPENGL_LIBRARIES}
        )

include(GenerateExportHeader)

generate_export_header(projectM_main
        BASE_NAME projectM
        EXPORT_FILE_NAME "${PROJECTM_EXPORT_HEADER}"
        )

if (ENABLE_STATIC_LIB)
    include(libprojectM_static.cmake)
endif()

if(ENABLE_SHARED_LIB)
    include(libprojectM_shared.cmake)
endif()

if(ENABLE_SHARED_LINKING)
    add_library(projectM::libprojectM ALIAS projectM_shared)
else()
    add_library(projectM::libprojectM ALIAS projectM_static)
endif()

install(FILES
        event.h
        projectM.h
        "${CMAKE_CURRENT_BINARY_DIR}/include/libprojectM/projectM_export.h"
        DESTINATION "${PROJECTM_INCLUDE_DIR}/libprojectM"
        COMPONENT Devel
        )

if(ENABLE_CXX_INTERFACE)
    install(FILES
            Common.hpp
            PCM.hpp
            fatal.h
            projectM.hpp
            DESTINATION "${PROJECTM_INCLUDE_DIR}/libprojectM"
            COMPONENT Devel
            )
endif()


# CMake target exports

# For use from a local projectM build tree (without installing)
export(TARGETS
        ${EXPORTED_TARGETS}
        NAMESPACE libprojectM::
        FILE projectM-exports.cmake
        )


# For use from an installed package (system install, vcpkg, homebrew etc.)
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/libprojectM/libprojectMConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(libprojectMConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/libprojectM/libprojectMConfig.cmake"
        INSTALL_DESTINATION "${PROJECTM_LIB_DIR}/cmake/libprojectM"
        PATH_VARS PROJECTM_BIN_DIR PROJECTM_INCLUDE_DIR
        )

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/libprojectM/libprojectMConfigVersion.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/libprojectM/libprojectMConfig.cmake"
        DESTINATION "${PROJECTM_LIB_DIR}/cmake/libprojectM"
        COMPONENT Devel
        )

install(EXPORT libprojectMTargets
        FILE libprojectMTargets.cmake
        NAMESPACE libprojectM::
        DESTINATION "${PROJECTM_LIB_DIR}/cmake/libprojectM"
        COMPONENT Devel
        )


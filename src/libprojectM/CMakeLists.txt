add_compile_definitions(
        DATADIR_PATH="${PROJECTM_DATADIR_PATH}"
        GL_SILENCE_DEPRECATION
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # dirent.h support
    set(MSVC_EXTRA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/msvc")
    include_directories("${MSVC_EXTRA_INCLUDE_DIR}")

    # Additional preprocessor definitions for Windows builds
    add_compile_definitions(
            NOMINMAX
            WIN32_LEAN_AND_MEAN
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
add_subdirectory(Renderer)

add_library(projectM_main OBJECT
        "${PROJECTM_EXPORT_HEADER}"
        Common.hpp
        HungarianMethod.hpp
        IdleTextures.hpp
        PCM.cpp
        PCM.hpp
        PipelineMerger.cpp
        PipelineMerger.hpp
        Preset.hpp
        PresetFactory.cpp
        PresetFactory.hpp
        PresetFactoryManager.cpp
        PresetFactoryManager.hpp
        ProjectM.cpp
        ProjectM.hpp
        ProjectMCWrapper.cpp
        ProjectMCWrapper.hpp
        RandomNumberGenerators.hpp
        TimeKeeper.cpp
        TimeKeeper.hpp
        fatal.h
        fftsg.cpp
        fftsg.h
        glError.h
        gltext.h
        projectM-opengl.h
        resource.h
        wipemalloc.cpp
        wipemalloc.h
        )

target_link_libraries(projectM_main
        PUBLIC
        MilkdropPresetFactory
        Renderer
        hlslparser
        SOIL2
        libprojectM::API
        )

target_include_directories(projectM_main
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/Renderer"
        "${CMAKE_CURRENT_SOURCE_DIR}/Renderer/hlslparser/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/MilkdropPresetFactory"
        "${MSVC_EXTRA_INCLUDE_DIR}"
        )

# CMake cannot combine multiple static libraries using target_link_libraries.
# This syntax will pull in the compiled object files into the final library.
add_library(projectM
        ${PROJECTM_DUMMY_SOURCE_FILE} # CMake needs at least one "real" source file.
        $<TARGET_OBJECTS:MilkdropPresetFactory>
        $<TARGET_OBJECTS:Renderer>
        $<TARGET_OBJECTS:hlslparser>
        $<TARGET_OBJECTS:SOIL2>
        $<TARGET_OBJECTS:projectM_main>
        )

target_include_directories(projectM
        PUBLIC
        "$<INSTALL_INTERFACE:${PROJECTM_INCLUDE_DIR}>"
        )

target_link_libraries(projectM
        PUBLIC
        ${PROJECTM_OPENGL_LIBRARIES}
        libprojectM::API
        )

if(ENABLE_LLVM)
    target_link_libraries(projectM
            PUBLIC
            LLVM::LLVM
            )
endif()

if(ENABLE_OPENMP)
    target_link_libraries(projectM
            PUBLIC
            OpenMP::OpenMP_CXX
            )
endif()

set_target_properties(projectM PROPERTIES
        VERSION "${PROJECTM_LIB_VERSION}"
        SOVERSION "${PROJECTM_SO_VERSION}"
        FOLDER libprojectM
        )

if(BUILD_SHARED_LIBS)
    target_compile_definitions(projectM
            PRIVATE
            projectM_api_EXPORTS
            )

    target_link_libraries(projectM
            PUBLIC
            ${CMAKE_DL_LIBS}
            )
else()
    target_compile_definitions(projectM
            PUBLIC
            PROJECTM_STATIC_DEFINE
            )

    set_target_properties(projectM PROPERTIES
            OUTPUT_NAME $<IF:$<PLATFORM_ID:Windows>,libprojectM,projectM>
            FOLDER libprojectM
            )
endif()

add_library(libprojectM::projectM ALIAS projectM)

install(TARGETS projectM
        EXPORT libprojectMTargets
        LIBRARY DESTINATION "${PROJECTM_LIB_DIR}"
        RUNTIME DESTINATION "${PROJECTM_LIB_DIR}"
        ARCHIVE DESTINATION "${PROJECTM_LIB_DIR}"
        COMPONENT Runtime
        )

if(ENABLE_CXX_INTERFACE)
    install(FILES
            Common.hpp
            PCM.hpp
            fatal.h
            ProjectM.hpp
            DESTINATION "${PROJECTM_INCLUDE_DIR}/libprojectM"
            COMPONENT Devel
            )
endif()


# CMake target exports

# For use from a local projectM build tree (without installing)
export(TARGETS
        projectM_api
        projectM
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

if(NOT ENABLE_EMSCRIPTEN AND ENABLE_GLES)
    install(FILES
            "${CMAKE_SOURCE_DIR}/cmake/gles/FindOpenGL.cmake"
            DESTINATION "${PROJECTM_LIB_DIR}/cmake/libprojectM"
            COMPONENT Devel
            )
endif()

if(ENABLE_LLVM)
    install(FILES
            "${CMAKE_SOURCE_DIR}/cmake/gles/FindLLVM.cmake"
            DESTINATION "${PROJECTM_LIB_DIR}/cmake/libprojectM"
            COMPONENT Devel
            )
endif()


install(EXPORT libprojectMTargets
        FILE libprojectMTargets.cmake
        NAMESPACE libprojectM::
        DESTINATION "${PROJECTM_LIB_DIR}/cmake/libprojectM"
        COMPONENT Devel
        )

# pkg-config export, only supports static library on UNIX systems.
if(UNIX AND NOT BUILD_SHARED_LIBS)
    macro(set_pkg_config_path varname path)
        if(IS_ABSOLUTE "${path}")
            set(${varname} "${path}")
        else()
            set(${varname} "\${prefix}/${path}")
        endif()
    endmacro()

    set(PKGCONFIG_PREFIX "${CMAKE_INSTALL_PREFIX}")
    set_pkg_config_path(PKGCONFIG_LIB_DIR "${PROJECTM_LIB_DIR}")
    set_pkg_config_path(PKGCONFIG_INCLUDE_DIR "${PROJECTM_INCLUDE_DIR}")
    set_pkg_config_path(PKGCONFIG_DATADIR_PATH "${PROJECTM_DATADIR_PATH}")

    configure_file(libprojectM.pc.cmake.in "${CMAKE_BINARY_DIR}/libprojectM.pc" @ONLY)
    install(FILES "${CMAKE_BINARY_DIR}/libprojectM.pc"
            DESTINATION "${PROJECTM_LIB_DIR}/pkgconfig"
            COMPONENT Devel
            )
endif()

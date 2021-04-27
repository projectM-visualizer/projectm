add_compile_definitions(
        DATADIR_PATH="${PROJECTM_DATADIR_PATH}"
        GL_SILENCE_DEPRECATION
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
            USE_TEXT_MENU
            USE_FTGL
            USE_NATIVE_GLEW
            STBI_NO_DDS
            projectM_FONT_MENU="${CMAKE_SOURCE_DIR}/fonts/VeraMono.ttf"
            projectM_FONT_TITLE="${CMAKE_SOURCE_DIR}/fonts/Vera.ttf"
            )
endif()

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
        $<$<PLATFORM_ID:Windows>:$<TARGET_OBJECTS:dlfcn>>
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
        EXPORT_NAME static
        FOLDER libprojectM
        )

target_include_directories(projectM_static
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Renderer>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Renderer/hlslparser/src>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/MilkdropPresetFactory>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/NativePresetFactory>"
        "$<BUILD_INTERFACE:${MSVC_EXTRA_INCLUDE_DIR}>"
        "$<INSTALL_INTERFACE:${PROJECTM_INCLUDE_DIR}>"
        )

target_link_libraries(projectM_static
        PRIVATE
        GLM::GLM
        PUBLIC
        ${PROJECTM_OPENGL_LIBRARIES}
        )

if(ENABLE_THREADING)
    target_link_libraries(projectM_static
            PUBLIC
            Threads::Threads
            )
endif()

if(ENABLE_LLVM)
    target_link_libraries(projectM_static
            PUBLIC
            LLVM::LLVM
            )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_link_libraries(projectM_static
        PUBLIC
        "-framework CoreFoundation"
        )
endif()

install(TARGETS projectM_static
        EXPORT libprojectMTargets
        LIBRARY DESTINATION "${PROJECTM_LIB_DIR}"
        RUNTIME DESTINATION "${PROJECTM_LIB_DIR}"
        ARCHIVE DESTINATION "${PROJECTM_LIB_DIR}"
        COMPONENT Runtime
        )

# Building projectM as a Windows DLL is not (yet) supported.
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_library(projectM_shared SHARED
            $<TARGET_OBJECTS:MilkdropPresetFactory>
            $<TARGET_OBJECTS:NativePresetFactory>
            $<TARGET_OBJECTS:Renderer>
            $<TARGET_OBJECTS:hlslparser>
            $<TARGET_OBJECTS:SOIL2>
            $<TARGET_OBJECTS:projectM_static>
            )

    set_target_properties(projectM_shared PROPERTIES
            OUTPUT_NAME projectM
            VERSION "${PROJECTM_LIB_VERSION}"
            SOVERSION "${PROJECTM_SO_VERSION}"
            EXPORT_NAME shared
            FOLDER libprojectM
            )

    target_include_directories(projectM_shared
            PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Renderer>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Renderer/hlslparser/src>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/MilkdropPresetFactory>"
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/NativePresetFactory>"
            "$<BUILD_INTERFACE:${MSVC_EXTRA_INCLUDE_DIR}>"
            "$<INSTALL_INTERFACE:${PROJECTM_INCLUDE_DIR}>"
            )

    target_link_libraries(projectM_shared
            PRIVATE
            GLM::GLM
            PUBLIC
            ${PROJECTM_OPENGL_LIBRARIES}
            ${CMAKE_DL_LIBS}
            )

    if(ENABLE_THREADING)
        target_link_libraries(projectM_shared
                PUBLIC
                Threads::Threads
                )
    endif()

    if(ENABLE_LLVM)
        target_link_libraries(projectM_shared
                PUBLIC
                LLVM::LLVM
                )
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        target_link_libraries(projectM_shared
            PUBLIC
            "-framework CoreFoundation"
            )
    endif()

    enable_target_linker_flags_if_supported(projectM_shared PRIVATE --no-undefined)

    install(TARGETS projectM_shared
            EXPORT libprojectMTargets
            LIBRARY DESTINATION "${PROJECTM_LIB_DIR}"
            RUNTIME DESTINATION "${PROJECTM_LIB_DIR}"
            ARCHIVE DESTINATION "${PROJECTM_LIB_DIR}"
            COMPONENT Runtime
            )

    set(EXPORT_SHARED_LIB_TARGET projectM_shared)
endif()

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



# CMake target exports

# For use from a local projectM build tree (without installing)
export(TARGETS
        MilkdropPresetFactory
        NativePresetFactory
        Renderer
        hlslparser
        SOIL2
        projectM_static
        ${EXPORT_SHARED_LIB_TARGET}
        NAMESPACE libprojectM::
        FILE projectM-exports.cmake
        )

export(PACKAGE libprojectM)



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
        PATH_VARS PROJECTM_BIN_DIR PROJECTM_INCLUDE_DIR PROJECTM_DATADIR_PATH
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



# pkgconfig export
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



# If the path is not absolute, need to add the prefix path (and hope the result is absolute).
if(NOT IS_ABSOLUTE PROJECTM_DATADIR_PATH)
    set(PROJECTM_DATADIR_PATH_ABSOLUTE "${CMAKE_INSTALL_PREFIX}/${PROJECTM_DATADIR_PATH}")
else()
    set(PROJECTM_DATADIR_PATH_ABSOLUTE "${PROJECTM_DATADIR_PATH}")
endif()

configure_file(config.inp.cmake.in "${CMAKE_BINARY_DIR}/config.inp" @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/config.inp"
        DESTINATION "${PROJECTM_DATADIR_PATH}/"
        COMPONENT Runtime
        )

install(DIRECTORY "${CMAKE_SOURCE_DIR}/fonts"
        DESTINATION "${PROJECTM_DATADIR_PATH}"
        COMPONENT Runtime
        )


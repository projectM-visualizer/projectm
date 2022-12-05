add_library(projectM_static STATIC
        ProjectMCWrapper.cpp
        ProjectMCWrapper.hpp
        $<TARGET_OBJECTS:projectM_main>
        $<TARGET_OBJECTS:MilkdropPresetFactory>
        $<TARGET_OBJECTS:Renderer>
        $<TARGET_OBJECTS:hlslparser>
        $<TARGET_OBJECTS:SOIL2>
        )

target_compile_definitions(projectM_static
        PUBLIC
        PROJECTM_STATIC_DEFINE
        )

set_target_properties(projectM_static PROPERTIES
        OUTPUT_NAME $<IF:$<PLATFORM_ID:Windows>,libprojectM,projectM>
        EXPORT_NAME static
        FOLDER libprojectM
        )

target_include_directories(projectM_static
        PUBLIC
        "$<INSTALL_INTERFACE:${PROJECTM_INCLUDE_DIR}>"
        )

target_link_libraries(projectM_static
        PUBLIC
        projectM::API
        ${PROJECTM_OPENGL_LIBRARIES}
        )

if(ENABLE_OPENMP)
    target_link_libraries(projectM_static
            PUBLIC
            OpenMP::OpenMP_CXX
            )
endif()

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

list(APPEND EXPORTED_TARGETS projectM_static)


# pkg-config export, only supports static library on UNIX systems.
if(UNIX)
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

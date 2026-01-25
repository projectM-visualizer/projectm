add_library(projectM_api INTERFACE)

configure_file(version.h.in "${CMAKE_CURRENT_BINARY_DIR}/include/projectM-4/version.h" @ONLY)

include(GenerateExportHeader)

set(PROJECTM_EXPORT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/projectM-4/projectM_export.h")

generate_export_header(projectM_api
        BASE_NAME projectM
        EXPORT_FILE_NAME "${PROJECTM_EXPORT_HEADER}"
        )

# Always generate the header, but we only install it if the C++ interface is actually enabled.
set(PROJECTM_CXX_EXPORT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/projectM-4/projectM_cxx_export.h")

generate_export_header(projectM_api
        BASE_NAME projectM_CXX
        EXPORT_FILE_NAME "${PROJECTM_CXX_EXPORT_HEADER}"
        )

set(PROJECTM_PUBLIC_HEADERS
        "${PROJECTM_EXPORT_HEADER}"
        "${CMAKE_CURRENT_BINARY_DIR}/include/projectM-4/version.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/projectM-4/audio.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/projectM-4/callbacks.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/projectM-4/core.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/projectM-4/debug.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/projectM-4/logging.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/projectM-4/memory.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/projectM-4/parameters.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/projectM-4/projectM.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/projectM-4/render_opengl.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/projectM-4/touch.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/projectM-4/types.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/projectM-4/user_sprites.h"
        )

if(ENABLE_CXX_INTERFACE)
    list(APPEND PROJECTM_PUBLIC_HEADERS ${PROJECTM_CXX_EXPORT_HEADER})
endif()

# Make available to parent scope for framework builds
set(PROJECTM_PUBLIC_HEADERS "${PROJECTM_PUBLIC_HEADERS}" PARENT_SCOPE)

target_sources(projectM_api
        PRIVATE
        ${PROJECTM_PUBLIC_HEADERS}
        )

set_target_properties(projectM_api PROPERTIES
        EXPORT_NAME API
        FOLDER libprojectM
        PUBLIC_HEADER "${PROJECTM_PUBLIC_HEADERS}"
        )

target_include_directories(projectM_api
        INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "$<INSTALL_INTERFACE:${PROJECTM_INCLUDE_DIR}>"
        )

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(projectM_api
            INTERFACE
            PROJECTM_STATIC_DEFINE
            PROJECTM_CXX_STATIC_DEFINE
            )
endif()

add_library(libprojectM::API ALIAS projectM_api)


if(ENABLE_INSTALL)

    install(TARGETS projectM_api
            EXPORT libprojectMTargets
            LIBRARY DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Runtime
            RUNTIME DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Runtime
            ARCHIVE DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Devel
            PUBLIC_HEADER DESTINATION "${PROJECTM_INCLUDE_DIR}/projectM-4" COMPONENT Devel
            )

endif()
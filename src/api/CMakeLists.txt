add_library(projectM_api INTERFACE)

target_sources(projectM_api
        PRIVATE
        "${PROJECTM_EXPORT_HEADER}"
        include/libprojectM/audio.h
        include/libprojectM/callbacks.h
        include/libprojectM/debug.h
        include/libprojectM/memory.h
        include/libprojectM/projectM.h
        include/libprojectM/types.h
        )

set_target_properties(projectM_api PROPERTIES
        EXPORT_NAME API
        FOLDER libprojectM
        )

target_include_directories(projectM_api
        INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "$<INSTALL_INTERFACE:${PROJECTM_INCLUDE_DIR}>"
        )

configure_file(version.h.in "${CMAKE_CURRENT_BINARY_DIR}/include/libprojectM/version.h" @ONLY)

include(GenerateExportHeader)

set(PROJECTM_EXPORT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/libprojectM/projectM_export.h")

generate_export_header(projectM_api
        BASE_NAME projectM
        EXPORT_FILE_NAME "${PROJECTM_EXPORT_HEADER}"
        )

add_library(libprojectM::API ALIAS projectM_api)


install(TARGETS projectM_api
        EXPORT libprojectMTargets
        LIBRARY DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Runtime
        RUNTIME DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Runtime
        ARCHIVE DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Devel
        PUBLIC_HEADER DESTINATION "${PROJECTM_INCLUDE_DIR}/libprojectM" COMPONENT Devel
        )

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/include/libprojectM/projectM_export.h"
        "${CMAKE_CURRENT_BINARY_DIR}/include/libprojectM/version.h"
        DESTINATION "${PROJECTM_INCLUDE_DIR}/libprojectM"
        COMPONENT Devel
        )

install(DIRECTORY include/libprojectM
        DESTINATION "${PROJECTM_INCLUDE_DIR}"
        COMPONENT Devel
        )

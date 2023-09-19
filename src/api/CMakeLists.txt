add_library(projectM_api INTERFACE)

target_sources(projectM_api
        PRIVATE
        "${PROJECTM_EXPORT_HEADER}"
        include/projectM-4/audio.h
        include/projectM-4/callbacks.h
        include/projectM-4/debug.h
        include/projectM-4/memory.h
        include/projectM-4/projectM.h
        include/projectM-4/types.h
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

configure_file(version.h.in "${CMAKE_CURRENT_BINARY_DIR}/include/projectM-4/version.h" @ONLY)

include(GenerateExportHeader)

set(PROJECTM_EXPORT_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/projectM-4/projectM_export.h")

generate_export_header(projectM_api
        BASE_NAME projectM
        EXPORT_FILE_NAME "${PROJECTM_EXPORT_HEADER}"
        )

add_library(libprojectM::API ALIAS projectM_api)


if(ENABLE_INSTALL)

    install(TARGETS projectM_api
            EXPORT libprojectMTargets
            LIBRARY DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Runtime
            RUNTIME DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Runtime
            ARCHIVE DESTINATION "${PROJECTM_LIB_DIR}" COMPONENT Devel
            PUBLIC_HEADER DESTINATION "${PROJECTM_INCLUDE_DIR}/projectM-4" COMPONENT Devel
            )

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/include/projectM-4/projectM_export.h"
            "${CMAKE_CURRENT_BINARY_DIR}/include/projectM-4/version.h"
            DESTINATION "${PROJECTM_INCLUDE_DIR}/projectM-4"
            COMPONENT Devel
            )

    install(DIRECTORY include/projectM-4
            DESTINATION "${PROJECTM_INCLUDE_DIR}"
            COMPONENT Devel
            )

endif()
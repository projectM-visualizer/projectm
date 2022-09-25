add_library(projectM_shared SHARED
        ProjectMCWrapper.cpp
        ProjectMCWrapper.hpp
        $<TARGET_OBJECTS:projectM_main>
        $<TARGET_OBJECTS:MilkdropPresetFactory>
        $<TARGET_OBJECTS:Renderer>
        $<TARGET_OBJECTS:hlslparser>
        $<TARGET_OBJECTS:SOIL2>
        )

target_compile_definitions(projectM_shared
        PRIVATE
        projectM_main_EXPORTS
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
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Renderer>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Renderer/hlslparser/src>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/MilkdropPresetFactory>"
        "$<BUILD_INTERFACE:${MSVC_EXTRA_INCLUDE_DIR}>"
        "$<INSTALL_INTERFACE:${PROJECTM_INCLUDE_DIR}>"
        )

target_link_libraries(projectM_shared
        PUBLIC
        projectM::API
        ${PROJECTM_OPENGL_LIBRARIES}
        ${CMAKE_DL_LIBS}
        )

if(ENABLE_OPENMP)
    target_link_libraries(projectM_shared
            PUBLIC
            OpenMP::OpenMP_CXX
            )
endif()

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

# --no-undefined doesn't make sense for Windows as all symbols are always required to be defined
if (NOT WIN32)
    enable_target_linker_flags_if_supported(projectM_shared PRIVATE --no-undefined)
endif()

install(TARGETS projectM_shared
        EXPORT libprojectMTargets
        LIBRARY DESTINATION "${PROJECTM_LIB_DIR}"
        RUNTIME DESTINATION "${PROJECTM_LIB_DIR}"
        ARCHIVE DESTINATION "${PROJECTM_LIB_DIR}"
        COMPONENT Runtime
        )

list(APPEND EXPORTED_TARGETS projectM_shared)

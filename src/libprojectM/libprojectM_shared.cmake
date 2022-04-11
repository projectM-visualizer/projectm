add_library(projectM_shared SHARED
        projectM.h
        ProjectMCWrapper.cpp
        ProjectMCWrapper.hpp

        $<TARGET_OBJECTS:projectM_main>
        $<TARGET_OBJECTS:MilkdropPresetFactory>
        $<$<TARGET_EXISTS:NativePresetFactory>:$<TARGET_OBJECTS:NativePresetFactory>>
        $<TARGET_OBJECTS:Renderer>
        $<TARGET_OBJECTS:hlslparser>
        $<TARGET_OBJECTS:SOIL2>
        $<$<PLATFORM_ID:Windows>:$<TARGET_OBJECTS:dlfcn>>
        )

target_compile_options(projectM_shared
        PRIVATE
        -DprojectM_main_EXPORTS
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
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/libprojectM>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Renderer>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Renderer/hlslparser/src>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/MilkdropPresetFactory>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/NativePresetFactory>"
        "$<BUILD_INTERFACE:${MSVC_EXTRA_INCLUDE_DIR}>"
        "$<INSTALL_INTERFACE:${PROJECTM_INCLUDE_DIR}>"
        )

target_link_libraries(projectM_shared
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

list(APPEND EXPORTED_TARGETS projectM_shared)

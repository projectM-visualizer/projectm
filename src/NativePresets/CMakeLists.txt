if(NOT ENABLE_PRESETS OR NOT ENABLE_NATIVE_PRESETS)
    return()
endif()

# Using a macro to reduce repetition.
macro(add_native_preset name)
    if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
        add_library(${name}_static STATIC ${name}.cpp)

        set_target_properties(${name}_static PROPERTIES
                OUTPUT_NAME ${name}
                )

        target_link_libraries(${name}_static
                PRIVATE
                projectM_static
                GLM::GLM
                )
    
        install(TARGETS ${name}_static
                LIBRARY DESTINATION "${PROJECTM_DATADIR_PATH}/presets"
                RUNTIME DESTINATION "${PROJECTM_DATADIR_PATH}/presets"
                ARCHIVE DESTINATION "${PROJECTM_DATADIR_PATH}/presets"
                COMPONENT Presets
                )
    endif()

    add_library(${name}_shared SHARED ${name}.cpp)

    target_compile_definitions(${name}_shared
            PRIVATE
            ${name}_EXPORTS
            )

    set_target_properties(${name}_shared PROPERTIES
            OUTPUT_NAME ${name}
            )

    # On Windows, libprojectM is only built statically.
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_link_libraries(${name}_shared
            PRIVATE
            projectM_static
            )
    else()
        target_link_libraries(${name}_shared
            PRIVATE
            projectM_shared
            )
    endif()

    target_link_libraries(${name}_shared
            PRIVATE
            GLM::GLM
            )

    install(TARGETS ${name}_shared
            LIBRARY DESTINATION "${PROJECTM_DATADIR_PATH}/presets"
            RUNTIME DESTINATION "${PROJECTM_DATADIR_PATH}/presets"
            ARCHIVE DESTINATION "${PROJECTM_DATADIR_PATH}/presets"
            COMPONENT Presets
            )
endmacro()

add_native_preset(MstressJuppyDancer)
add_native_preset(RLGFractalDrop7c)
add_native_preset(RovastarDarkSecret)
add_native_preset(RovastarDriftingChaos)
add_native_preset(RovastarFractalSpiral)
add_native_preset(RovastarFractopiaFrantic)

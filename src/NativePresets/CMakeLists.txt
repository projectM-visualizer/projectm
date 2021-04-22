if(NOT ENABLE_PRESETS OR NOT ENABLE_NATIVE_PRESETS)
    return()
endif()

# Using a macro to reduce repetition.
macro(add_native_preset name)
    add_library(${name}_static STATIC ${name}.cpp)
    add_library(${name}_shared SHARED ${name}.cpp)

    set_target_properties(${name}_static PROPERTIES
            OUTPUT_NAME ${name}
            )
    set_target_properties(${name}_shared PROPERTIES
            OUTPUT_NAME ${name}
            )

    target_link_libraries(${name}_static
            PRIVATE
            projectM_static
            )

    target_link_libraries(${name}_shared
            PRIVATE
            projectM_shared
            )

    install(TARGETS ${name}_static ${name}_shared
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

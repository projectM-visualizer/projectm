if(NOT ENABLE_NATIVE_PRESETS)
    return()
endif()

add_library(NativePresetFactory OBJECT
        MilkdropCompatability.hpp
        NativePreset.hpp
        NativePresetFactory.cpp
        NativePresetFactory.hpp
        )
        
target_include_directories(NativePresetFactory
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )
        
target_link_libraries(NativePresetFactory
        PUBLIC
        GLM::GLM
        ${PROJECTM_OPENGL_LIBRARIES}
        ${CMAKE_DL_LIBS}
        )
        
set_target_properties(NativePresetFactory PROPERTIES
        FOLDER libprojectM
        )

set(EXPORTED_TARGETS ${EXPORTED_TARGETS} NativePresetFactory PARENT_SCOPE)

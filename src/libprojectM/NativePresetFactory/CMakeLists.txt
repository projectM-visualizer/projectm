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

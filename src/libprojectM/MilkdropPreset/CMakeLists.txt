# Debugging options, for development purposes.
option(ENABLE_DEBUG_MILKDROP_PRESET "Enable STDERR debug output in Milkdrop preset code (Debug builds only)" OFF)

add_library(MilkdropPreset OBJECT
        BlurTexture.cpp
        BlurTexture.hpp
        Border.cpp
        Border.hpp
        Constants.hpp
        CustomShape.cpp
        CustomShape.hpp
        CustomWaveform.cpp
        CustomWaveform.hpp
        DarkenCenter.cpp
        DarkenCenter.hpp
        EvalLibMutex.cpp
        Filters.cpp
        Filters.hpp
        IdlePreset.cpp
        IdlePreset.hpp
        MilkdropNoise.cpp
        MilkdropNoise.hpp
        MilkdropPreset.cpp
        MilkdropPreset.hpp
        MilkdropPresetExceptions.hpp
        MilkdropPresetFactory.cpp
        MilkdropPresetFactory.hpp
        MilkdropShader.cpp
        MilkdropShader.hpp
        MotionVectors.cpp
        MotionVectors.hpp
        PerFrameContext.cpp
        PerFrameContext.hpp
        PerPixelContext.cpp
        PerPixelContext.hpp
        PerPixelMesh.cpp
        PerPixelMesh.hpp
        PresetFileParser.cpp
        PresetFileParser.hpp
        PresetState.cpp
        PresetState.hpp
        ShapePerFrameContext.cpp
        ShapePerFrameContext.hpp
        Transformation.hpp
        VideoEcho.cpp
        VideoEcho.hpp
        Waveform.cpp
        Waveform.hpp
        WaveformPerFrameContext.cpp
        WaveformPerFrameContext.hpp
        WaveformPerPointContext.cpp
        WaveformPerPointContext.hpp
        )

target_include_directories(MilkdropPreset
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

target_link_libraries(MilkdropPreset
        PRIVATE
        projectM::Eval
        libprojectM::API # For export header
        PUBLIC
        hlslparser
        GLM::GLM
        ${PROJECTM_OPENGL_LIBRARIES}
        )

if(ENABLE_DEBUG_MILKDROP_PRESET)
    target_compile_definitions(MilkdropPreset
            PRIVATE
            MILKDROP_PRESET_DEBUG=1
            )
endif()

set_target_properties(MilkdropPreset PROPERTIES
        FOLDER libprojectM
        )

if(ENABLE_LLVM)
    target_link_libraries(MilkdropPreset
            PUBLIC
            LLVM::LLVM
            )
endif()

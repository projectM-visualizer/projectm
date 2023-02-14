add_library(MilkdropPresetFactory OBJECT
        Constants.hpp
        FileParser.cpp
        FileParser.hpp
        IdlePreset.cpp
        IdlePreset.hpp
        MilkdropPreset.cpp
        MilkdropPreset.hpp
        MilkdropPresetFactory.cpp
        MilkdropPresetFactory.hpp
        PerFrameContext.cpp
        PerFrameContext.hpp
        PerPixelContext.cpp
        PerPixelContext.hpp
        PresetFrameIO.cpp
        PresetFrameIO.hpp
        PresetState.cpp
        PresetState.hpp
        Shape.cpp
        Shape.hpp
        Waveform.cpp
        Waveform.hpp
        WaveformPerFrameContext.cpp
        WaveformPerFrameContext.hpp
        WaveformPerPointContext.cpp
        WaveformPerPointContext.hpp
        ShapePerFrameContext.cpp ShapePerFrameContext.hpp)

target_include_directories(MilkdropPresetFactory
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

target_link_libraries(MilkdropPresetFactory
        PRIVATE
        projectM::Eval
        libprojectM::API # For export header
        PUBLIC
        omptl
        GLM::GLM
        ${PROJECTM_OPENGL_LIBRARIES}
        )

set_target_properties(MilkdropPresetFactory PROPERTIES
        FOLDER libprojectM
        )

if(ENABLE_LLVM)
    target_link_libraries(MilkdropPresetFactory
            PUBLIC
            LLVM::LLVM
            )
endif()

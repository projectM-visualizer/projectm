set(SHADER_FILES
        Shaders/Blur1FragmentShaderGlsl330.frag
        Shaders/Blur2FragmentShaderGlsl330.frag
        Shaders/BlurVertexShaderGlsl330.vert
        Shaders/PresetCompVertexShaderGlsl330.vert
        Shaders/PresetMotionVectorsVertexShaderGlsl330.vert
        Shaders/PresetShaderHeaderGlsl330.inc
        Shaders/PresetWarpFragmentShaderGlsl330.frag
        Shaders/PresetWarpVertexShaderGlsl330.vert
        Shaders/TexturedDrawFragmentShaderGlsl330.frag
        Shaders/TexturedDrawVertexShaderGlsl330.vert
        Shaders/UntexturedDrawFragmentShaderGlsl330.frag
        Shaders/UntexturedDrawVertexShaderGlsl330.vert
        )

string(REPLACE ";" "\\;" SHADER_FILES_ARG "${SHADER_FILES}")
add_custom_command(OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/MilkdropStaticShaders.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/MilkdropStaticShaders.hpp

        COMMAND ${CMAKE_COMMAND}

        ARGS
        -D SHADER_FILES="${SHADER_FILES_ARG}"
        -D OUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/GenerateStaticShaders.cmake

        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS # Files watched for changes:
        ${SHADER_FILES}
        MilkdropStaticShaders.cpp.in
        MilkdropStaticShaders.hpp.in
        GenerateStaticShaders.cmake
        )

add_library(MilkdropPreset OBJECT
        ${SHADER_FILES}
        ${CMAKE_CURRENT_BINARY_DIR}/MilkdropStaticShaders.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/MilkdropStaticShaders.hpp
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
        Factory.cpp
        Factory.hpp
        Filters.cpp
        Filters.hpp
        FinalComposite.cpp
        FinalComposite.hpp
        IdlePreset.cpp
        IdlePreset.hpp
        MilkdropPreset.cpp
        MilkdropPreset.hpp
        MilkdropPresetExceptions.hpp
        MilkdropShader.cpp
        MilkdropShader.hpp
        MilkdropStaticShaders.cpp.in
        MilkdropStaticShaders.hpp.in
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
        VideoEcho.cpp
        VideoEcho.hpp
        Waveform.cpp
        Waveform.hpp
        WaveformMode.hpp
        WaveformPerFrameContext.cpp
        WaveformPerFrameContext.hpp
        WaveformPerPointContext.cpp
        WaveformPerPointContext.hpp
        Waveforms/CenteredSpiro.cpp
        Waveforms/CenteredSpiro.hpp
        Waveforms/Circle.cpp
        Waveforms/Circle.hpp
        Waveforms/DerivativeLine.cpp
        Waveforms/DerivativeLine.hpp
        Waveforms/DoubleLine.cpp
        Waveforms/DoubleLine.hpp
        Waveforms/ExplosiveHash.cpp
        Waveforms/ExplosiveHash.hpp
        Waveforms/Factory.cpp
        Waveforms/Factory.hpp
        Waveforms/Line.cpp
        Waveforms/Line.hpp
        Waveforms/LineBase.cpp
        Waveforms/LineBase.hpp
        Waveforms/Milkdrop2077Wave11.cpp
        Waveforms/Milkdrop2077Wave11.hpp
        Waveforms/Milkdrop2077Wave9.cpp
        Waveforms/Milkdrop2077Wave9.hpp
        Waveforms/Milkdrop2077WaveFlower.cpp
        Waveforms/Milkdrop2077WaveFlower.hpp
        Waveforms/Milkdrop2077WaveLasso.cpp
        Waveforms/Milkdrop2077WaveLasso.hpp
        Waveforms/Milkdrop2077WaveSkewed.cpp
        Waveforms/Milkdrop2077WaveSkewed.hpp
        Waveforms/Milkdrop2077WaveStar.cpp
        Waveforms/Milkdrop2077WaveStar.hpp
        Waveforms/Milkdrop2077WaveX.cpp
        Waveforms/Milkdrop2077WaveX.hpp
        Waveforms/SpectrumLine.cpp
        Waveforms/SpectrumLine.hpp
        Waveforms/WaveformMath.cpp
        Waveforms/WaveformMath.hpp
        Waveforms/XYOscillationSpiral.cpp
        Waveforms/XYOscillationSpiral.hpp
        )

target_include_directories(MilkdropPreset
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_BINARY_DIR}"
        )

target_link_libraries(MilkdropPreset
        PRIVATE
        projectM::Eval
        libprojectM::API # For export header
        PUBLIC
        hlslparser
        GLM::GLM
        )

if(BUILD_SHARED_LIBS)
    if(ENABLE_CXX_INTERFACE)
        target_compile_definitions(MilkdropPreset
                PRIVATE
                projectM_api_EXPORTS
                )
    else()
        target_compile_definitions(MilkdropPreset
                PRIVATE
                PROJECTM_CXX_STATIC_DEFINE
                )
    endif()
else()
    target_compile_definitions(MilkdropPreset
            PRIVATE
            PROJECTM_STATIC_DEFINE
            PROJECTM_CXX_STATIC_DEFINE
            )
endif()

set_target_properties(MilkdropPreset PROPERTIES
        FOLDER libprojectM
        )

# Debugging options, for development purposes.
option(ENABLE_DEBUG_MILKDROP_PRESET "Enable STDERR debug output in Milkdrop preset code (Debug builds only)" OFF)

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
        Filters.cpp
        Filters.hpp
        FinalComposite.cpp
        FinalComposite.hpp
        FlipTexture.cpp
        FlipTexture.hpp
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
        "${CMAKE_CURRENT_BINARY_DIR}"
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

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(MilkdropPreset
        PRIVATE
        PROJECTM_STATIC_DEFINE
        )
endif()

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

set(TRANSITION_SHADER_FILES
        TransitionShaders/TransitionShaderBuiltInCircleGlsl330.frag
        TransitionShaders/TransitionShaderBuiltInPlasmaGlsl330.frag
        TransitionShaders/TransitionShaderBuiltInSimpleBlendGlsl330.frag
        TransitionShaders/TransitionShaderBuiltInSweepGlsl330.frag
        TransitionShaders/TransitionShaderBuiltInWarpGlsl330.frag
        TransitionShaders/TransitionShaderBuiltInZoomBlurGlsl330.frag
        TransitionShaders/TransitionVertexShaderGlsl330.vert
        TransitionShaders/TransitionShaderHeaderGlsl330.frag
        TransitionShaders/TransitionShaderMainGlsl330.frag
        )

string(REPLACE ";" "\\;" TRANSITION_SHADER_FILES_ARG "${TRANSITION_SHADER_FILES}")
add_custom_command(OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/BuiltInTransitionsResources.hpp

        COMMAND ${CMAKE_COMMAND}

        ARGS
        -D SHADER_FILES="${TRANSITION_SHADER_FILES_ARG}"
        -D OUTPUT_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/GenerateBuiltInTransitionsResources.cmake

        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS # Files watched for changes:
        ${TRANSITION_SHADER_FILES}
        BuiltInTransitionsResources.hpp.in
        GenerateBuiltInTransitionsResources.cmake
        )

add_library(Renderer OBJECT
        ${CMAKE_CURRENT_BINARY_DIR}/BuiltInTransitionsResources.hpp
        BuiltInTransitionsResources.hpp.in
        CopyTexture.cpp
        CopyTexture.hpp
        FileScanner.cpp
        FileScanner.hpp
        Framebuffer.cpp
        Framebuffer.hpp
        IdleTextures.hpp
        MilkdropNoise.cpp
        MilkdropNoise.hpp
        PresetTransition.cpp
        PresetTransition.hpp
        RenderContext.hpp
        RenderItem.cpp
        RenderItem.hpp
        Sampler.cpp
        Sampler.hpp
        Shader.cpp
        Shader.hpp
        Texture.cpp
        Texture.hpp
        TextureAttachment.cpp
        TextureAttachment.hpp
        TextureManager.cpp
        TextureManager.hpp
        TextureSamplerDescriptor.cpp
        TextureSamplerDescriptor.hpp
        TransitionShaderManager.cpp
        TransitionShaderManager.hpp
        )

target_include_directories(Renderer
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        "${CMAKE_CURRENT_BINARY_DIR}"
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

target_link_libraries(Renderer
        PRIVATE
        libprojectM::API # For export header
        PUBLIC
        GLM::GLM
        hlslparser
        SOIL2
        )

set_target_properties(Renderer PROPERTIES
        FOLDER libprojectM
        )

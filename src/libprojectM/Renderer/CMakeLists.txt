add_subdirectory(hlslparser)
add_subdirectory(SOIL2)

add_library(Renderer OBJECT
        BeatDetect.cpp
        BeatDetect.hpp
        Filters.cpp
        Filters.hpp
        MenuText.cpp
        MenuText.h
        MilkdropNoise.cpp
        MilkdropNoise.hpp
        MilkdropWaveform.cpp
        MilkdropWaveform.hpp
        PerPixelMesh.cpp
        PerPixelMesh.hpp
        Pipeline.cpp
        Pipeline.hpp
        PipelineContext.cpp
        PipelineContext.hpp
        RenderItemDistanceMetric.cpp
        RenderItemDistanceMetric.hpp
        RenderItemMatcher.cpp
        RenderItemMatcher.hpp
        RenderItemMergeFunction.hpp
        Renderable.cpp
        Renderable.hpp
        Renderer.cpp
        Renderer.hpp
        Shader.cpp
        Shader.hpp
        ShaderEngine.cpp
        ShaderEngine.hpp
        StaticGlShaders.cpp
        Texture.cpp
        Texture.hpp
        TextureManager.cpp
        TextureManager.hpp
        Transformation.hpp
        VideoEcho.cpp
        VideoEcho.hpp
        Waveform.cpp
        Waveform.hpp
        )

target_include_directories(Renderer
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

target_link_libraries(Renderer
        PUBLIC
        GLM::GLM
        hlslparser
        SOIL2
        )
        
set_target_properties(Renderer PROPERTIES
        FOLDER libprojectM
        )

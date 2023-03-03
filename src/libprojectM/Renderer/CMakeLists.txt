add_library(Renderer OBJECT
        BeatDetect.cpp
        BeatDetect.hpp
        Border.cpp
        Border.hpp
        DarkenCenter.cpp
        DarkenCenter.hpp
        FileScanner.cpp
        FileScanner.hpp
        Filters.cpp
        Filters.hpp
        MilkdropNoise.cpp
        MilkdropNoise.hpp
        MilkdropWaveform.cpp
        MilkdropWaveform.hpp
        MotionVectors.cpp
        MotionVectors.hpp
        PerPixelMesh.cpp
        PerPixelMesh.hpp
        Pipeline.cpp
        Pipeline.hpp
        PipelineContext.cpp
        PipelineContext.hpp
        RenderContext.hpp
        RenderItem.cpp
        RenderItem.hpp
        Renderer.cpp
        Renderer.hpp
        Shader.cpp
        Shader.hpp
        ShaderEngine.cpp
        ShaderEngine.hpp
        Shape.cpp
        Shape.hpp
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
        PRIVATE
        libprojectM::API # For export header
        PUBLIC
        GLM::GLM
        hlslparser
        SOIL2
        )

if(ENABLE_OPENMP)
    target_link_libraries(Renderer
            PUBLIC
            OpenMP::OpenMP_CXX
            )
endif()

set_target_properties(Renderer PROPERTIES
        FOLDER libprojectM
        )

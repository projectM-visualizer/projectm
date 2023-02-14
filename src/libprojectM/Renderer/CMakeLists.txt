add_library(Renderer OBJECT
        FileScanner.cpp
        FileScanner.hpp
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
        StaticGlShaders.cpp
        Texture.cpp
        Texture.hpp
        TextureManager.cpp
        TextureManager.hpp
        Transformation.hpp
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

add_library(Renderer OBJECT
        FileScanner.cpp
        FileScanner.hpp
        Framebuffer.cpp
        Framebuffer.hpp
        IdleTextures.hpp
        RenderContext.hpp
        RenderItem.cpp
        RenderItem.hpp
        Renderer.cpp
        Renderer.hpp
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
        )

target_include_directories(Renderer
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

target_link_libraries(Renderer
        PRIVATE
        omptl
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

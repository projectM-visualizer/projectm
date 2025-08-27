#pragma once

#include "Renderer/Framebuffer.hpp"
#include "Renderer/RenderItem.hpp"
#include "Renderer/Shader.hpp"

namespace libprojectM {
namespace Renderer {


class CopyTexture : public RenderItem
{
public:
    CopyTexture();

    void InitVertexAttrib() override;


    void Draw(const std::shared_ptr<class Texture>& originalTexture,
              bool flipVertical = false, bool flipHorizontal = false);


    void Draw(const std::shared_ptr<class Texture>& originalTexture, const std::shared_ptr<class Texture>& targetTexture = {},
              bool flipVertical = false, bool flipHorizontal = false);


    void Draw(const std::shared_ptr<class Texture>& originalTexture, Framebuffer& framebuffer, int framebufferIndex,
              bool flipVertical = false, bool flipHorizontal = false);


    auto Texture() -> std::shared_ptr<class Texture>;

private:

    void UpdateTextureSize(int width, int height);

    void Copy(bool flipVertical, bool flipHorizontal) const;

    Shader m_shader;
    Framebuffer m_framebuffer{1};
    Sampler m_sampler{GL_CLAMP_TO_EDGE, GL_NEAREST};

    int m_width{};
    int m_height{};
};

}
}

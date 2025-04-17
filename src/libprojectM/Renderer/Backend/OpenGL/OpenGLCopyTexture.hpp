#pragma once

#include <Renderer/CopyTexture.hpp>
#include <Renderer/Framebuffer.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Backend/OpenGL/OpenGLRenderItem.hpp>

namespace libprojectM {
namespace Renderer {
namespace Backend {
namespace OpenGL {

class OpenGLCopyTexture : public CopyTexture, public OpenGLRenderItem
{
public:
    OpenGLCopyTexture();
    ~OpenGLCopyTexture() override = default;

    void InitVertexAttrib() override;

    void Draw(const std::shared_ptr<class Texture>& originalTexture,
              bool flipVertical = false, bool flipHorizontal = false) override;

    void Draw(const std::shared_ptr<class Texture>& originalTexture, const std::shared_ptr<class Texture>& targetTexture,
              bool flipVertical = false, bool flipHorizontal = false) override;

    void Draw(const std::shared_ptr<class Texture>& originalTexture, Framebuffer& framebuffer, int framebufferIndex,
              bool flipVertical = false, bool flipHorizontal = false) override;

    auto Texture() -> std::shared_ptr<class Texture> override;

private:
    void UpdateTextureSize(int width, int height);
    void Copy(bool flipVertical, bool flipHorizontal) const;

    Shader m_shader;
    Framebuffer m_framebuffer{1};
    Sampler m_sampler{GL_CLAMP_TO_EDGE, GL_NEAREST};

    int m_width{};
    int m_height{};
};

} // namespace OpenGL
} // namespace Backend
} // namespace Renderer
} // namespace libprojectM


#pragma once

#include <Renderer/Framebuffer.hpp>
#include <Renderer/RenderContext.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/TextureSamplerDescriptor.hpp>

#include <array>
#include <memory>

namespace libprojectM {
namespace MilkdropPreset {

class PerFrameContext;
class PresetState;


class BlurTexture
{
public:
    using Values = std::array<float, 3>;


    enum class BlurLevel : int
    {
        None,
        Blur1,
        Blur2,
        Blur3
    };


    BlurTexture();


    ~BlurTexture();


    void Initialize(const Renderer::RenderContext& renderContext);


    void SetRequiredBlurLevel(BlurLevel level);


    auto GetDescriptorsForBlurLevel(BlurLevel blurLevel) const -> std::vector<Renderer::TextureSamplerDescriptor>;


    void Update(const Renderer::Texture& sourceTexture, const PerFrameContext& perFrameContext);


    void Bind(GLint& unit, Renderer::Shader& shader) const;


    static void GetSafeBlurMinMaxValues(const PerFrameContext& perFrameContext,
                                        Values& blurMin,
                                        Values& blurMax);

private:
    static constexpr int NumBlurTextures = 6;


    void AllocateTextures(const Renderer::Texture& sourceTexture);

    GLuint m_vboBlur;
    GLuint m_vaoBlur;

    std::weak_ptr<Renderer::Shader> m_blur1Shader;
    std::weak_ptr<Renderer::Shader> m_blur2Shader;

    int m_sourceTextureWidth{};
    int m_sourceTextureHeight{};

    Renderer::Framebuffer m_blurFramebuffer;
    std::shared_ptr<Renderer::Sampler> m_blurSampler;
    std::array<std::shared_ptr<Renderer::Texture>, NumBlurTextures> m_blurTextures;
    BlurLevel m_blurLevel{BlurLevel::None};
};

}
}

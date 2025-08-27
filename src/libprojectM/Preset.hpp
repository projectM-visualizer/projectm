#pragma once

#include <Audio/FrameAudioData.hpp>

#include <Renderer/RenderContext.hpp>
#include <Renderer/Texture.hpp>

#include <memory>
#include <string>

namespace libprojectM {

class Preset
{
public:
    virtual ~Preset() = default;


    virtual void Initialize(const Renderer::RenderContext& renderContext) = 0;


    virtual void RenderFrame(const libprojectM::Audio::FrameAudioData& audioData,
                             const Renderer::RenderContext& renderContext) = 0;


    virtual auto OutputTexture() const -> std::shared_ptr<Renderer::Texture> = 0;


    virtual void DrawInitialImage(const std::shared_ptr<Renderer::Texture>& image,
                                  const Renderer::RenderContext& renderContext) = 0;


    virtual void BindFramebuffer() = 0;

    inline void SetFilename(const std::string& filename)
    {
        m_filename = filename;
    }

    inline auto Filename() const -> const std::string&
    {
        return m_filename;
    }

private:
    std::string m_filename;
};

}

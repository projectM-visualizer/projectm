#pragma once

#include "UserSprites/SpriteException.hpp"

#include <Preset.hpp>

#include <Audio/FrameAudioData.hpp>

#include <Renderer/RenderContext.hpp>
#include <Renderer/RenderItem.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace libprojectM {
namespace UserSprites {

class Sprite : public Renderer::RenderItem
{
public:
    using Ptr = std::unique_ptr<Sprite>;
    using PresetList = std::vector<std::reference_wrapper<const std::unique_ptr<Preset>>>;


    virtual void Init(const std::string& spriteData, const Renderer::RenderContext& renderContext) = 0;


    virtual void Draw(const Audio::FrameAudioData& audioData,
                      const Renderer::RenderContext& renderContext,
                      uint32_t outputFramebufferObject,
                      PresetList presets) = 0;


    virtual auto Done() const -> bool = 0;
};

}
}

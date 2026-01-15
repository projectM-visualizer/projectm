#pragma once

#include <Preset.hpp>

#include <Audio/FrameAudioData.hpp>

#include <Renderer/RenderContext.hpp>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace libprojectM {
namespace UserSprites {

class Sprite
{
public:
    using Ptr = std::unique_ptr<Sprite>;
    using PresetList = std::vector<std::reference_wrapper<const std::unique_ptr<Preset>>>;

    virtual ~Sprite() = default;

    /**
     * @brief Initializes the sprite instance for rendering.
     * @param spriteData The data for the sprite type.
     * @param renderContext The current frame's rendering context.
     */
    virtual void Init(const std::string& spriteData, const Renderer::RenderContext& renderContext) = 0;

    /**
     * @brief Draws the sprite onto the current framebuffer.
     * @param audioData The frame audio data structure.
     * @param renderContext The current frame's rendering context.
     * @param outputFramebufferObject Framebuffer object the sprite will be rendered to.
     * @param presets The active preset, plus eventually the transitioning one. Used for the burn-in effect.
     */
    virtual void Draw(const Audio::FrameAudioData& audioData,
                      const Renderer::RenderContext& renderContext,
                      uint32_t outputFramebufferObject,
                      PresetList presets) = 0;

    /**
     * @brief Returns if the sprite has finished rendering and should be deleted.
     * @return true if the sprite should be deleted, false if not.
     */
    virtual auto Done() const -> bool = 0;
};

} // namespace UserSprites
} // namespace libprojectM

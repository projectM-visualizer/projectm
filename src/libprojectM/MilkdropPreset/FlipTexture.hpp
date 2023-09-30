#pragma once

#include "PresetState.hpp"

#include <Renderer/Framebuffer.hpp>
#include <Renderer/RenderItem.hpp>

/**
 * @class FlipTexture
 * @brief Flips the given input texture upside-down.
 *
 * Milkdrop uses HLSL, so the input and output UV coordinates in the draw call are upside-down because
 * DirectX has the origin at the top-left while OpenGL/Vulkan use the bottom-left.
 *
 * Some presets need the input, the calculated UVs and the output to be properly aligned, so at some
 * point, input textures must be flipped for the next rendering step. This class uses a simple draw
 * call with a pass-through shader to draw the same image 1:1, but vertically flipped.
 */
class FlipTexture : public RenderItem
{
public:
    FlipTexture() = delete;
    explicit FlipTexture(const PresetState& presetState);

    void InitVertexAttrib();

    /**
     * @brief Flips the original texture either into the object's internal framebuffer or a given target texture.
     * The original and target textures must not be the same.
     * @param originalTexture The texture to be flipped.
     * @param targetTexture Optional target texture to draw onto.
     */
    void Draw(const std::shared_ptr<Texture>& originalTexture, const std::shared_ptr<Texture>& targetTexture = {});

    /**
     * @brief Flips the texture bound the given framebuffer's first color attachment.
     * This is done by drawing into a second framebuffer, then swapping the textures, so the original texture
     * can be the current color attachment of targetFramebuffer.
     * @param originalTexture The texture to be flipped.
     * @param targetFramebuffer Optional target texture to draw onto.
     * @param framebufferIndex The index of the framebuffer to use.
     */
    void Draw(const std::shared_ptr<Texture>& originalTexture, Framebuffer& framebuffer, int framebufferIndex);

    /**
     * @brief Returns the flipped texture.
     *
     * @return The flipped texture.
     */
    auto FlippedTexture() -> std::shared_ptr<Texture>;

private:
    /**
     * Updates the mesh
     */
    void UpdateTextureSize();

    void Flip() const;

    const PresetState& m_presetState; //!< The global preset state.

    Framebuffer m_framebuffer{1};                    //!< Framebuffer for drawing the flipped texture
    Sampler m_sampler{GL_CLAMP_TO_EDGE, GL_NEAREST}; //!< Texture sampler settings

    int m_viewportWidth{};  //!< Last known viewport width
    int m_viewportHeight{}; //!< Last known viewport height
};

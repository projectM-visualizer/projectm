#pragma once

#include "Renderer/Framebuffer.hpp"
#include "Renderer/RenderItem.hpp"
#include "Renderer/Shader.hpp"

namespace libprojectM {
namespace Renderer {

/**
 * @class CopyTexture
 * @brief Copies the given input texture to an output texture or framebuffer.
 *
 * Optionally flips the image horizontally or vertically during the copy operation.
 */
class CopyTexture : public RenderItem
{
public:
    CopyTexture();

    void InitVertexAttrib() override;

    /**
     * @brief Copies the original texture into the currently bound framebuffer.
     * @param originalTexture The texture to be copied.
     * @param flipVertical Flip image on the y axis when copying.
     * @param flipHorizontal Flip image on the x axis when copying.
     */
    void Draw(const std::shared_ptr<class Texture>& originalTexture,
              bool flipVertical = false, bool flipHorizontal = false);

    /**
     * @brief Copies the original texture either into the object's internal framebuffer or a given target texture.
     * The original and target textures must not be the same.
     * @param originalTexture The texture to be copied.
     * @param targetTexture Optional target texture to draw onto.
     * @param flipVertical Flip image on the y axis when copying.
     * @param flipHorizontal Flip image on the x axis when copying.
     */
    void Draw(const std::shared_ptr<class Texture>& originalTexture, const std::shared_ptr<class Texture>& targetTexture = {},
              bool flipVertical = false, bool flipHorizontal = false);

    /**
     * @brief Copies the texture bound the given framebuffer's first color attachment.
     * This is done by drawing into a second framebuffer, then swapping the textures, so the original texture
     * can be the current color attachment of targetFramebuffer.
     * @param originalTexture The texture to be copied.
     * @param targetFramebuffer Optional target texture to draw onto.
     * @param framebufferIndex The index of the framebuffer to use.
     * @param flipVertical Flip image on the y axis when copying.
     * @param flipHorizontal Flip image on the x axis when copying.
     */
    void Draw(const std::shared_ptr<class Texture>& originalTexture, Framebuffer& framebuffer, int framebufferIndex,
              bool flipVertical = false, bool flipHorizontal = false);

    /**
     * @brief Returns the flipped texture.
     *
     * @return The flipped texture.
     */
    auto Texture() -> std::shared_ptr<class Texture>;

private:
    /**
     * Updates the mesh
     */
    void UpdateTextureSize(int width, int height);

    void Copy(bool flipVertical, bool flipHorizontal) const;

    Shader m_shader;                                 //!< Simple textured shader
    Framebuffer m_framebuffer{1};                    //!< Framebuffer for drawing the flipped texture
    Sampler m_sampler{GL_CLAMP_TO_EDGE, GL_NEAREST}; //!< Texture sampler settings

    int m_width{};  //!< Last known framebuffer/texture width
    int m_height{}; //!< Last known framebuffer/texture height
};

} // namespace Renderer
} // namespace libprojectM

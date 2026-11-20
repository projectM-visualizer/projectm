/**
 * @file FlashLimiter.hpp
 * @brief Limits how fast the colors of the output image can change between frames.
 */
#pragma once

#include "Renderer/Framebuffer.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Sampler.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/ShaderCache.hpp"

#include <memory>

namespace libprojectM {
namespace Renderer {

/**
 * @class FlashLimiter
 * @brief Limits how fast each color channel of the output image can change between frames.
 *
 * The frame is first drawn into an internal image. Draw() then moves each color channel of each
 * pixel from the previously shown frame towards the new one, but by no more than the given
 * amount, and draws the result to the target framebuffer. Fast flashes and strobing are reduced
 * in contrast this way, while slow changes pass unchanged.
 *
 * Each channel is limited on its own, so cycling through red, green and blue is limited as well,
 * even though the overall brightness doesn't change.
 */
class FlashLimiter
{
public:
    FlashLimiter();

    /**
     * @brief Binds the framebuffer the frame must be drawn into and sets its size.
     *
     * If the size changes, the previously shown frame is reset to black, so the next frames
     * fade in.
     *
     * @param width The frame width in pixels.
     * @param height The frame height in pixels.
     */
    void BindInput(int width, int height);

    /**
     * @brief Draws the limited frame into the given framebuffer and keeps it for the next frame.
     * @param shaderCache The shader cache to store the limiter shader in.
     * @param maxChange The maximum change of each color channel since the last frame, in 0..1.
     * @param targetFramebufferObject The framebuffer to draw the limited frame to.
     */
    void Draw(ShaderCache& shaderCache, float maxChange, uint32_t targetFramebufferObject);

    /**
     * @brief Resets the previously shown frame to black.
     */
    void Reset();

private:
    auto BindShader(ShaderCache& shaderCache) -> std::shared_ptr<Shader>;

    void DrawLimited(Shader& shader, float maxChange);

    static constexpr int InputBuffer = 0; //!< Framebuffer index of the new, unlimited frame.

    Mesh m_mesh;                                     //!< Fullscreen quad.
    Framebuffer m_framebuffer{3};                    //!< Input image and two images of the shown frame (current and previous).
    Sampler m_sampler{GL_CLAMP_TO_EDGE, GL_NEAREST}; //!< Sampler for all images.
    std::weak_ptr<Shader> m_shader;                  //!< The limiter shader.

    int m_previousBuffer{1}; //!< Framebuffer index holding the previously shown frame.
    bool m_reset{true};      //!< If true, the previously shown frame must be cleared first.
};

} // namespace Renderer
} // namespace libprojectM

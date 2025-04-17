#pragma once

#include "Renderer/Framebuffer.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/ShaderCache.hpp"

namespace libprojectM {
namespace Renderer {

/**
 * @class CopyTexture
 * @brief Copies the given input texture to an output texture or framebuffer.
 *
 * Optionally flips the image horizontally or vertically during the copy operation.
 */
class CopyTexture
{
public:
    virtual ~CopyTexture() = default;

    virtual void Draw(const std::shared_ptr<class Texture>& originalTexture,
                     bool flipVertical = false, bool flipHorizontal = false) = 0;

    virtual void Draw(const std::shared_ptr<class Texture>& originalTexture, const std::shared_ptr<class Texture>& targetTexture = {},
                     bool flipVertical = false, bool flipHorizontal = false) = 0;

    virtual void Draw(const std::shared_ptr<class Texture>& originalTexture, Framebuffer& framebuffer, int framebufferIndex,
                     bool flipVertical = false, bool flipHorizontal = false) = 0;

    virtual auto Texture() -> std::shared_ptr<class Texture> = 0;
};

} // namespace Renderer
} // namespace libprojectM

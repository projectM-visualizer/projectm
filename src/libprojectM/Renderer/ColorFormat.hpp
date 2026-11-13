/**
 * @file ColorFormat.hpp
 * @brief Selects the texture format of the internal render targets.
 */
#pragma once

#include "Renderer/OpenGL.h"

namespace libprojectM {
namespace Renderer {

/**
 * @brief Describes the format of a color texture as passed to glTexImage2D().
 */
struct ColorFormat {
    GLint internalFormat{GL_RGBA}; //!< OpenGL internal format, e.g. GL_RGBA8
    GLenum format{GL_RGBA};        //!< OpenGL color format, e.g. GL_RGBA
    GLenum type{GL_UNSIGNED_BYTE}; //!< OpenGL component storage type, e.g. GL_UNSIGNED_BYTE

    auto operator==(const ColorFormat& other) const -> bool
    {
        return internalFormat == other.internalFormat && format == other.format && type == other.type;
    }

    auto operator!=(const ColorFormat& other) const -> bool
    {
        return !(*this == other);
    }
};

/**
 * @brief Returns the format to use for internal render targets.
 *
 * If the framebuffer the application draws to has more than 8 bits per color channel, the
 * internal images use 10 bits per channel, so the additional precision is not lost before the
 * frame gets there. GL_RGB10_A2 is color-renderable in OpenGL 3.3 and OpenGL ES 3.0 and, like
 * the 8-bit format, stores values clamped to the 0..1 range.
 *
 * @param targetColorBits Bits per color channel of the framebuffer the frame is drawn to.
 * @return The color format for the internal render targets.
 */
inline auto RenderTargetColorFormat(int targetColorBits) -> ColorFormat
{
    if (targetColorBits > 8)
    {
        return {GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV};
    }

    return {};
}

} // namespace Renderer
} // namespace libprojectM

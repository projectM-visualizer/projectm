#pragma once

#include <functional>
#include <string>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Structure containing texture data provided by a callback.
 */
struct TextureLoadData {
    const unsigned char* data{nullptr}; //!< Pointer to raw pixel data (RGBA/RGB format).
    unsigned int width{0};              //!< Width of the texture in pixels.
    unsigned int height{0};             //!< Height of the texture in pixels.
    unsigned int channels{0};           //!< Number of color channels (3 for RGB, 4 for RGBA).
    unsigned int textureId{0};          //!< An existing OpenGL texture ID to use.
};

/**
 * @brief Callback function type for loading textures.
 * @param textureName The name of the texture being requested.
 * @param[out] data Structure to fill with texture data.
 */
using TextureLoadCallback = std::function<void(const std::string& textureName, TextureLoadData& data)>;

} // namespace Renderer
} // namespace libprojectM

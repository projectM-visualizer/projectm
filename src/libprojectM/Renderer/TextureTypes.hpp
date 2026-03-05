#pragma once

#include <functional>
#include <string>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Callback function type for loading textures.
 *
 * Called when projectM needs a texture. The callback should use the TextureManager's
 * LoadExternal* methods to provide the texture data, then return true.
 * Return false to fall back to filesystem loading.
 *
 * @param textureName The name of the texture being requested.
 * @return true if the texture was loaded by the callback, false to fall back to filesystem.
 */
using TextureLoadCallback = std::function<bool(const std::string& textureName)>;

/**
 * @brief Callback function type for unloading textures.
 * @param textureName The name of the texture being unloaded.
 */
using TextureUnloadCallback = std::function<void(const std::string& textureName)>;

} // namespace Renderer
} // namespace libprojectM

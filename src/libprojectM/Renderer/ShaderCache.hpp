#pragma once

#include "Renderer/Shader.hpp"

#include <map>
#include <memory>
#include <string>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Instance shader cache.
 *
 * Used to store shaders which only need to be compiled once per projectM instance.
 * Removes the need for the previously used static shader provider, and reduces
 * shader recompilation times.
 *
 * Shaders are accessed by arbitrary strings as keys.
 *
 * All cached shader programs will be properly deleted when the projectM instance is destroyed.
 * Classes storing a reference to a cached shader should ideally use an std::weak_ptr to do so.
 */
class ShaderCache
{
public:
    /**
     * @brief Adds a new shader to the cache.
     * If the shader already exists, the existing cache entry it NOT replaced.
     * @param key The key to store the shader with.
     * @param shader A shared pointer to the shader program to be cached.
     */
    void Insert(const std::string& key, const std::shared_ptr<Shader>& shader);

    /**
     * @brief Adds a new shader to the cache.
     * If the shader already exists, the existing cache entry it NOT replaced.
     * @param key The key to store the shader with.
     * @param shader A shared pointer to the shader program to be cached.
     */
    void Insert(const std::string& key, std::shared_ptr<Shader>&& shader);

    /**
     * @brief Removes a shader from the cache.
     * If the key does not exist in the cache, this function will not do anything.
     * @param key The key of the sahder to be removed.
     */
    void Remove(const std::string& key);

    /**
     * @brief Returns a cached shader.
     * @param key the key of the cached shader to be returned.
     * @return A shared pointer to the cached shader program, or nullptr if the key wasn't found.
     */
    auto Get(const std::string& key) const -> std::shared_ptr<Shader>;

private:
    std::map<std::string, std::shared_ptr<Shader>> m_cachedShaders;
};

} // namespace Renderer
} // namespace libprojectM

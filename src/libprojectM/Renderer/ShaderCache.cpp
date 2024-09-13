#include "ShaderCache.hpp"

#include <utility>

namespace libprojectM {
namespace Renderer {

void ShaderCache::Insert(const std::string& key, const std::shared_ptr<Shader>& shader)
{
    m_cachedShaders.emplace(key, shader);
}

void ShaderCache::Insert(const std::string& key, std::shared_ptr<Shader>&& shader)
{
    m_cachedShaders.emplace(key, std::move(shader));
}

void ShaderCache::Remove(const std::string& key)
{
    m_cachedShaders.erase(key);
}

auto ShaderCache::Get(const std::string& key) const -> std::shared_ptr<Shader>
{
    if (m_cachedShaders.find(key) != m_cachedShaders.end())
    {
        return m_cachedShaders.at(key);
    }

    return {};
}

} // namespace Renderer
} // namespace libprojectM

#pragma once

#include "Renderer/Shader.hpp"

#include <map>
#include <memory>
#include <string>

namespace libprojectM {
namespace Renderer {


class ShaderCache
{
public:

    void Insert(const std::string& key, const std::shared_ptr<Shader>& shader);


    void Insert(const std::string& key, std::shared_ptr<Shader>&& shader);


    void Remove(const std::string& key);


    auto Get(const std::string& key) const -> std::shared_ptr<Shader>;

private:
    std::map<std::string, std::shared_ptr<Shader>> m_cachedShaders;
};

}
}

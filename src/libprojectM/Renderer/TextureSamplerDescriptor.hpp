#pragma once

#include "Renderer/Shader.hpp"

namespace libprojectM {
namespace Renderer {

class TextureManager;


class TextureSamplerDescriptor
{
public:
    TextureSamplerDescriptor() = default;


    TextureSamplerDescriptor(const std::shared_ptr<class Texture>& texture,
                             const std::shared_ptr<class Sampler>& sampler,
                             std::string samplerName,
                             std::string sizeName);

    TextureSamplerDescriptor(const TextureSamplerDescriptor& other) = default;
    TextureSamplerDescriptor(TextureSamplerDescriptor&& other) = default;
    auto operator=(const TextureSamplerDescriptor& other) -> TextureSamplerDescriptor& = default;
    auto operator=(TextureSamplerDescriptor&& other) -> TextureSamplerDescriptor& = default;


    auto Empty() const -> bool;


    void Bind(GLint unit, const Shader& shader) const;


    void Unbind(GLint unit);


    auto Texture() const -> std::shared_ptr<class Texture>;


    void Texture(std::weak_ptr<class Texture> texture);


    auto Sampler() const -> std::shared_ptr<class Sampler>;


    auto SamplerDeclaration() const -> std::string;


    auto TexSizeDeclaration() const -> std::string;


    void TryUpdate(TextureManager& textureManager);

private:
    std::weak_ptr<class Texture> m_texture;
    std::weak_ptr<class Sampler> m_sampler;
    std::string m_samplerName;
    std::string m_sizeName;
    bool m_updateFailed{false};
};

}
}

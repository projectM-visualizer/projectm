#pragma once

#include "Texture.hpp"
#include "Sampler.hpp"
#include "Shader.hpp"

class TextureManager;

/**
 * @brief A pair of a texture and a sampler for use in shaders.
 *
 * This class uses weak pointers to store the information. This allows implementations to
 * store the descriptor while allowing to check if it's necessary to retrieve a new one from
 * the texture manager, for example if the texture search paths were changed, a texture has
 * been purged or the manager has been cleaned/recreated.
 *
 * For this to work properly, it's recommended to only store the descriptor, and not a shared
 * pointer to any of the stored resources.
 */
class TextureSamplerDescriptor
{
public:
    TextureSamplerDescriptor() = default;

    /**
     * @brief Constructor. Creates a new descriptor for a texture and an associated sampler.
     * @param texture A pointer to a texture.
     * @param sampler A pointer to a sampler.
     * @param samplerName The name of the original sampler, e.g. "mytex_pw".
     * @param sizeName The name of the texsize uniform, e.g. "mytex".
     */
    TextureSamplerDescriptor(const std::shared_ptr<class Texture>& texture,
                             const std::shared_ptr<class Sampler>& sampler,
                             std::string samplerName,
                             std::string sizeName);

    TextureSamplerDescriptor(const TextureSamplerDescriptor& other) = default;
    TextureSamplerDescriptor(TextureSamplerDescriptor&& other) = default;
    auto operator=(const TextureSamplerDescriptor& other) -> TextureSamplerDescriptor& = default;
    auto operator=(TextureSamplerDescriptor&& other) -> TextureSamplerDescriptor& = default;

    /**
     * @brief Returns if the descriptor contains a valid texture and sampler.
     * @return true if both texture and sampler are valid, false if at least one is invalid or nullptr.
     */
    auto Empty() const -> bool;

    /**
     * @brief Binds the texture, sampler and  in the descriptor to the given texture unit.
     * @param unit The texture unit to bind the descriptor's contents to.
     * @param shader The shader to register the sampler/texsize uniforms with. Must already be bound.
     */
    void Bind(GLint unit, const Shader& shader) const;

    /**
     * @brief Unbinds the texture and sampler in the descriptor from the given texture unit.
     * @param unit The texture unit to undbind the descriptor's contents from.
     */
    void Unbind(GLint unit);

    /**
     * @brief Returns a pointer to the stored texture.
     * @return A shared pointer to the stored texture, or nullptr if the texture is invalid or empty.
     */
    auto Texture() const -> std::shared_ptr<class Texture>;

    /**
     * @brief Updates the internal texture with a new one.
     * @param texture A weak pointer to the new texture.
     */
    void Texture(std::weak_ptr<class Texture> texture);

    /**
     * @brief Returns a pointer to the stored sampler.
     * @return A shared pointer to the stored sampler, or nullptr if the sampler is invalid or empty.
     */
    auto Sampler() const -> std::shared_ptr<class Sampler>;

    /**
     * @brief Returns the shader sampler HLSL declaration.
     * @return The sampler declaration for use in the preset HLSL shaders.
     */
    auto SamplerDeclaration() const -> std::string;

    /**
     * @brief Returns the shader texsize HLSL declaration.
     * @return The texsize declaration for use in the preset HLSL shaders.
     */
    auto TexSizeDeclaration() const -> std::string;

    /**
     * @brief Tries to update the texture and sampler from the given texture manager if invalid.
     * @param textureManager The texture manager to retrieve the new data from.
     */
    void TryUpdate(TextureManager& textureManager);

private:
    std::weak_ptr<class Texture> m_texture; //!< A weak reference to the texture.
    std::weak_ptr<class Sampler> m_sampler; //!< A weak reference to the sampler.
    std::string m_samplerName; //!< The name of the texture sampler as referenced in the shader.
    std::string m_sizeName; //!< The name of the "texsize_" uniform as referenced in the shader.
    bool m_updateFailed{false}; //!< Set to true if the update try failed, e.g. texture could not be loaded.
};

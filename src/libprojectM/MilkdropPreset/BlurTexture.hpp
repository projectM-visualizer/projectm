/**
 * @file BlurTexture.hpp
 * @brief Blurs a given texture in multiple passes and stores the results.
 */
#pragma once

#include <Renderer/Framebuffer.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/TextureSamplerDescriptor.hpp>

#include <array>
#include <memory>

class PerFrameContext;
class PresetState;

/**
 * @brief Blurs a given texture in multiple passes and stores the results.
 *
 * Blur textures are not stored in the texture manager to enable independent blur textures
 * for each loaded preset, e.g. during blending.
 */
class BlurTexture
{
public:
    using Values = std::array<float, 3>;

    /**
     * Maximum main texture blur level used in the shader
     */
    enum class BlurLevel : int
    {
        None,  //!< No blur used.
        Blur1, //!< First blur level (2 passes)
        Blur2, //!< Second blur level (4 passes)
        Blur3  //!< Third blur level (6 passes)
    };

    /**
     * Constructor.
     */
    BlurTexture();

    /**
     * Destructor.
     */
    ~BlurTexture();

    /**
     * @brief Sets the minimum required blur level.
     * If the current level isn't high enough, it'll be increased.
     * @param level The minim blur level.
     */
    void SetRequiredBlurLevel(BlurLevel level);

    /**
     * @brief Returns a list of descriptors for the given blur level.
     * The blur textures don't need to be present and can be empty placeholders.
     * @param blurLevel The blur level.
     */
    auto GetDescriptorsForBlurLevel(BlurLevel blurLevel) const -> std::vector<TextureSamplerDescriptor>;

    /**
     * @brief Renders the required blur passes on the given texture.
     * @param presetState The preset state with initial values and the main texture.
     * @param perFrameContext The per-frame variables.
     */
    void Update(const PresetState& presetState, const PerFrameContext& perFrameContext);

    /**
     * @brief Binds the user-readable blur textures to the texture slots starting with the given index.
     * The shader must already be bound.
     * @param[in,out] unit The first texture unit to bind the blur textures from. Returns the next
     *                     free unit, which can be the same as the input slot if no blur textures
     *                     are used.
     */
    void Bind(GLint& unit, Shader& shader) const;

    /**
     * @brief Returns properly scaled and clamped vlur values from the given context.
     * @param perFrameContext The per-frame context to retrieve the initial values from.
     * @param blurMin The calculated min values.
     * @param blurMax The calculated max values.
     */
    static void GetSafeBlurMinMaxValues(const PerFrameContext& perFrameContext,
                                        Values& blurMin,
                                        Values& blurMax);

private:
    static constexpr int NumBlurTextures = 6; //!< Maximum number of blur passes/textures.

    /**
     * Allocates the blur textures.
     * @param sourceTexture The source texture.
     */
    void AllocateTextures(const Texture& sourceTexture);

    GLuint m_vboBlur; //!< Vertex buffer object for the fullscreen blur quad.
    GLuint m_vaoBlur; //!< Vertex array object for the fullscreen blur quad.

    Shader m_blur1Shader; //!< The shader used on the first blur pass.
    Shader m_blur2Shader; //!< The shader used for subsequent blur passes after the initial pass.

    int m_sourceTextureWidth{}; //!< Width of the source texture used to create the blur textures.
    int m_sourceTextureHeight{}; //!< Height of the source texture used to create the blur textures.

    Framebuffer m_blurFramebuffer;                                        //!< The framebuffer used to draw the blur textures.
    std::shared_ptr<Sampler> m_blurSampler;                               //!< The blur sampler.
    std::array<std::shared_ptr<Texture>, NumBlurTextures> m_blurTextures; //!< The blur textures for each pass.
    BlurLevel m_blurLevel{BlurLevel::None};                               //!< Current blur level.
};

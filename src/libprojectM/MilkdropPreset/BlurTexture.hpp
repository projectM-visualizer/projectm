/**
 * @file BlurTexture.hpp
 * @brief Blurs a given texture in multiple passes and stores the results.
 */
#pragma once

#include <Renderer/Shader.hpp>
#include <Renderer/TextureSamplerDescriptor.hpp>

#include <array>
#include <memory>

/**
 * @brief Blurs a given texture in multiple passes and stores the results.
 *
 * Blur textures are not stored in the texture manager to enable independent blur textures
 * for each loaded preset, e.g. during blending.
 */
class BlurTexture
{
public:
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
     * @brief Parameters which control the blur look & feel.
     */
    struct BlurParameters {
        BlurLevel level{BlurLevel::None}; //!< Requested blur level.
        float blur1Min{0.0f};             //!< Minimum blur precision on the first pass.
        float blur1Max{1.0f};             //!< Maximum blur precision on the first pass.
        float blur2Min{0.0f};             //!< Minimum blur precision on the second pass.
        float blur2Max{1.0f};             //!< Maximum blur precision on the second pass.
        float blur3Min{0.0f};             //!< Minimum blur precision on the third pass.
        float blur3Max{1.0f};             //!< Maximum blur precision on the third pass.
        float blur1EdgeDarken{0.25f};     //!< Center darken factor on the first blur pass.
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
     * @brief Renders the required blur passes on the given texture.
     * @param parameters The blur parameters.
     * @param sourceTexture The main/source texture to blur.
     */
    void UpdateFromTexture(const BlurParameters& parameters, const Texture& sourceTexture);

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

    std::array<std::unique_ptr<TextureSamplerDescriptor>, NumBlurTextures> m_blurTextures; //!< The blur textures for each pass.
};

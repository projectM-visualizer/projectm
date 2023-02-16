/**
 * @file MilkdropShader
 * @brief Holds a warp or composite shader of Milkdrop presets.
 */
#pragma once

#include "Renderer/Shader.hpp"
#include "Renderer/TextureManager.hpp"

/**
 * @brief Holds a warp or composite shader of Milkdrop presets.
 * Also does the required shader translation from HLSL to GLSL using hlslparser.
 */
class MilkdropShader
{
public:
    enum class ShaderType
    {
        WarpShader, //!< Warp shader
        CompositeShader //!< Composite shader
    };

    /**
     * Maximum main texture blur level used in the shader
     */
    enum class BlurLevel : int
    {
        None, //!< No blur used.
        Blur1, //!< First blur level (2 passes)
        Blur2, //!< Second blur level (4 passes)
        Blur3 //!< Third blur level (6 passes)
    };

    /**
     * constructor.
     * @param type The preset shader type.
     */
    explicit MilkdropShader(ShaderType type);

    /**
     * @brief Translates and compiles the shader code.
     * @param presetShaderCode The preset shader code.
     */
    void LoadCode(std::string presetShaderCode);

    /**
     * @brief Loads the required texture references into the shader.
     */
    void LoadTextures(TextureManager& textureManager);

private:
    /**
     * @brief Prepares the shader code to be translated into GLSL.
     * @param program The program code to work on.
     */
    void PreprocessPresetShader(std::string& program);

    /**
     * @brief Searches for sampler references in the program and stores them in m_samplerNames.
     * @param program The program code to work on.
     */
    void GetReferencedSamplers(const std::string& program);

    ShaderType m_type{ShaderType::WarpShader}; //!< Type of this shader.
    std::string m_presetShaderCode; //!< The original preset shader code.

    std::vector<std::string> m_samplerNames; //!< Names of all referenced samplers in the shader code.

    Shader m_shader;
};

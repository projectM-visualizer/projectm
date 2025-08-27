
#pragma once

#include "BlurTexture.hpp"

#include <Renderer/Shader.hpp>
#include <Renderer/TextureManager.hpp>

#include <array>
#include <set>

namespace libprojectM {
namespace MilkdropPreset {

class PerFrameContext;
class PresetState;


class MilkdropShader
{
public:
    enum class ShaderType
    {
        WarpShader,
        CompositeShader
    };


    explicit MilkdropShader(ShaderType type);


    void LoadCode(const std::string& presetShaderCode);


    void LoadTexturesAndCompile(PresetState& presetState);


    void LoadVariables(const PresetState& presetState, const PerFrameContext& perFrameContext);


    auto Shader() -> Renderer::Shader&;

private:

    void PreprocessPresetShader(std::string& program);


    void GetReferencedSamplers(const std::string& program);


    void TranspileHLSLShader(const PresetState& presetState, std::string& program);


    void UpdateMaxBlurLevel(BlurTexture::BlurLevel requestedLevel);

    ShaderType m_type{ShaderType::WarpShader};
    std::string m_fragmentShaderCode;
    std::string m_preprocessedCode;

    std::set<std::string> m_samplerNames;
    std::vector<Renderer::TextureSamplerDescriptor> m_mainTextureDescriptors;
    std::vector<Renderer::TextureSamplerDescriptor> m_textureSamplerDescriptors;
    BlurTexture::BlurLevel m_maxBlurLevelRequired{BlurTexture::BlurLevel::None};

    std::array<float, 4> m_randValues{};
    std::array<glm::vec3, 20> m_randTranslation{};
    std::array<glm::vec3, 20> m_randRotationCenters{};
    std::array<glm::vec3, 20> m_randRotationSpeeds{};

    Renderer::Shader m_shader;
};

}
}

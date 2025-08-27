#pragma once

#include "Renderer/RenderItem.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/TextureSamplerDescriptor.hpp"

#include <Preset.hpp>

#include <glm/glm.hpp>

#include <random>

namespace libprojectM {
namespace Renderer {


class PresetTransition : public RenderItem
{
public:
    PresetTransition() = delete;


    explicit PresetTransition(const std::shared_ptr<Shader>& transitionShader,
                              double durationSeconds,
                              double transitionStartTime);

    void InitVertexAttrib() override;


    auto IsDone(double currentFrameTime) const -> bool;


    auto Progress(double currentFrameTime) const -> double;


    void Draw(const Preset& oldPreset,
              const Preset& newPreset,
              const RenderContext& context,
              const libprojectM::Audio::FrameAudioData& audioData,
              double currentFrameTime);

private:
    std::vector<std::string> m_noiseTextureNames{"noise_lq",
                                                 "pw_noise_lq",
                                                 "noise_mq",
                                                 "pw_noise_mq",
                                                 "noise_hq",
                                                 "pw_noise_hq",
                                                 "noisevol_lq",
                                                 "pw_noisevol_lq",
                                                 "noisevol_hq",
                                                 "pw_noisevol_hq"};

    std::shared_ptr<Shader> m_transitionShader;
    std::shared_ptr<Sampler> m_presetSampler{std::make_shared<Sampler>(GL_CLAMP_TO_EDGE, GL_LINEAR)};

    double m_durationSeconds{3.0};
    double m_transitionStartTime{};
    double m_lastFrameTime{};

    glm::ivec4 m_staticRandomValues{};

    std::random_device m_randomDevice;
};

}
}

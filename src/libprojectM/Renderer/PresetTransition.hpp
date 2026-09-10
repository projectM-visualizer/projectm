#pragma once

#include "Renderer/Mesh.hpp"
#include "Renderer/Shader.hpp"

#include <Preset.hpp>

#include <glm/glm.hpp>

#include <random>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Implements the shader and rendering logic to blend two presets into each other.
 */
class PresetTransition
{
public:
    PresetTransition() = delete;

    explicit PresetTransition(const std::shared_ptr<Shader>& transitionShader,
                              double durationSeconds,
                              double transitionStartTime);

    virtual ~PresetTransition() = default;

    auto IsDone(double currentFrameTime) const -> bool;
    auto Progress(double currentFrameTime) const -> double;

    virtual void Draw(const Preset& oldPreset,
                     const Preset& newPreset,
                     const RenderContext& context,
                     const libprojectM::Audio::FrameAudioData& audioData,
                     double currentFrameTime);

protected:
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

} // namespace Renderer
} // namespace libprojectM

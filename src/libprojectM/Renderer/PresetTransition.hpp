#pragma once

#include "Renderer/RenderItem.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/TextureSamplerDescriptor.hpp"

#include <Preset.hpp>

#include <glm/glm.hpp>

#include <random>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Implements the shader and rendering logic to blend two presets into each other.
 */
class PresetTransition : public RenderItem
{
public:
    PresetTransition() = delete;

    /**
     * Constructor.
     * @param transitionShader The transition shader program.
     * @param durationSeconds Transition duration in seconds.
     * @param transitionStartTime The time in seconds since start of projectM.
     */
    explicit PresetTransition(const std::shared_ptr<Shader>& transitionShader,
                              double durationSeconds,
                              double transitionStartTime);

    void InitVertexAttrib() override;

    /**
     * @brief Returns true if the transition is done.
     * @param currentFrameTime The time in seconds since start of the current frame.
     * @return false if the transition is still in progress, true if it's done.
     */
    auto IsDone(double currentFrameTime) const -> bool;

    /**
     * @brief Updates the transition variables and renders the shader quad to the current FBO.
     * @param oldPreset A reference to the old (fading out) preset.
     * @param newPreset A reference to the new (fading in) preset.
     * @param context The rendering context used to render the presets.
     * @param audioData Current audio data and beat detection values.
     * @param currentFrameTime The time in seconds since start of the current frame.
     */
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
                                                 "pw_noisevol_hq"}; //!< Names of noise textures to retrieve from TextureManager.

    std::shared_ptr<Shader> m_transitionShader;                                                       //!< The compiled shader used for this transition.
    std::shared_ptr<Sampler> m_presetSampler{std::make_shared<Sampler>(GL_CLAMP_TO_EDGE, GL_LINEAR)}; //!< Sampler for preset textures. Uses bilinear interpolation and no repeat.

    double m_durationSeconds{3.0};  //!< Transition duration in seconds.
    double m_transitionStartTime{}; //!< Start time of this transition. Duration is measured from this point.
    double m_lastFrameTime{};       //!< Time when the previous frame was rendered.

    glm::ivec4 m_staticRandomValues{}; //!< Four random integers, remaining static during the whole transition.

    std::random_device m_randomDevice; //!< Seed for the random number generator
};

} // namespace Renderer
} // namespace libprojectM

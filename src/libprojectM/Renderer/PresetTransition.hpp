#pragma once

#include "Renderer/Framebuffer.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Shader.hpp"

#include <Preset.hpp>

#include <glm/glm.hpp>

#include <random>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Easing curve types for transition progress.
 */
enum class EasingType : int
{
    Linear = 0,   //!< No easing — raw linear progress.
    Smoothstep,   //!< Smoothstep (3t² - 2t³) — recommended default.
    EaseIn,       //!< Quadratic ease-in.
    EaseOut,      //!< Quadratic ease-out.
    Count         //!< Number of easing types (not a valid selection).
};

/**
 * @brief Blending modes for preset transitions.
 */
enum class TransitionBlendMode : int
{
    Alpha = 0,        //!< Default mix (current behavior).
    Additive,         //!< Old + New * progress.
    Multiplicative,   //!< Old * (1-progress) + (Old * New) * progress.
    Screen,           //!< Screen blending.
    Masked,           //!< Future: use a third mask texture.
    Count             //!< Number of blend modes (not a valid selection).
};

/**
 * @brief Implements the shader and rendering logic to blend two presets into each other.
 */
class PresetTransition
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

    /**
     * @brief Returns true if the transition is done.
     * @param currentFrameTime The time in seconds since start of the current frame.
     * @return false if the transition is still in progress, true if it's done.
     */
    auto IsDone(double currentFrameTime) const -> bool;

    /**
     * @brief Returns the current linear blending progress according to the given frame time.
     * @param currentFrameTime The time in seconds since start of the current frame.
     * @return The linear blending progress from 0.0 to 1.0.
     */
    auto Progress(double currentFrameTime) const -> double;

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
              const Audio::FrameAudioData& audioData,
              double currentFrameTime);

    /**
     * @brief Sets the number of render passes for this transition.
     *
     * 1 = single-pass (default). 2 = multi-pass (pass 0 renders to an
     * intermediate FBO, pass 1 samples from it via iLastPassTex).
     */
    void SetPassCount(int passCount);

    /**
     * @brief Returns the current pass count.
     */
    auto PassCount() const -> int;

    /**
     * @brief Begins a render pass.
     *
     * Binds the appropriate framebuffer, sets the viewport, and updates
     * the iPass uniform. Must be paired with EndPass().
     *
     * @param passNumber The pass index (0 = first pass, 1 = second pass).
     * @param viewportWidth The viewport width in pixels.
     * @param viewportHeight The viewport height in pixels.
     */
    void BeginPass(int passNumber, int viewportWidth, int viewportHeight);

    /**
     * @brief Ends the current render pass.
     *
     * Resets internal pass state. For multi-pass transitions, pass 0
     * will restore the original draw framebuffer so pass 1 can safely
     * sample the intermediate texture.
     */
    void EndPass();

    /**
     * @brief Returns the current pass index, or -1 if no pass is active.
     */
    auto GetCurrentPass() const -> int;

    /**
     * @brief Returns the output texture of a completed pass.
     *
     * Only valid for pass 0 of a multi-pass transition after EndPass().
     *
     * @param passNumber The pass index to query.
     * @return The pass output texture, or nullptr if unavailable.
     */
    auto GetPassTexture(int passNumber) const -> std::shared_ptr<class Texture>;

    /**
     * @brief Sets the blending mode used by this transition.
     */
    void SetBlendMode(TransitionBlendMode mode);

    /**
     * @brief Returns the current blending mode.
     */
    auto GetBlendMode() const -> TransitionBlendMode;

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

    Mesh m_mesh;                                                                                      //!< The mesh used to draw the transition effect.
    std::shared_ptr<Shader> m_transitionShader;                                                       //!< The compiled shader used for this transition.
    std::shared_ptr<Sampler> m_presetSampler{std::make_shared<Sampler>(GL_CLAMP_TO_EDGE, GL_LINEAR)}; //!< Sampler for preset textures. Uses bilinear interpolation and no repeat.

    double m_durationSeconds{3.0};  //!< Transition duration in seconds.
    double m_transitionStartTime{}; //!< Start time of this transition. Duration is measured from this point.
    double m_lastFrameTime{};       //!< Time when the previous frame was rendered.

    EasingType m_easingType{EasingType::Smoothstep}; //!< Easing curve applied to linear progress in GLSL.

    int m_passCount{1}; //!< Number of render passes (1 = single-pass, 2 = multi-pass).
    int m_currentPass{-1}; //!< Currently active pass index (-1 = none).

    TransitionBlendMode m_blendMode{TransitionBlendMode::Alpha}; //!< Blending mode for this transition.

    std::shared_ptr<Framebuffer> m_passFramebuffer; //!< Intermediate FBO for multi-pass transitions (pass 0 output).
    GLint m_originalDrawFbo{0}; //!< Original draw framebuffer saved before pass 0 binds the intermediate FBO.

    glm::ivec4 m_staticRandomValues{}; //!< Four random integers, remaining static during the whole transition.

    std::random_device m_randomDevice; //!< Seed for the random number generator
};

} // namespace Renderer
} // namespace libprojectM

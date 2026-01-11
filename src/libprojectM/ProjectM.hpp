/*
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2007 projectM Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * See 'LICENSE.txt' included within this release
 *
 */
#pragma once

#include <projectM-4/projectM_export.h>

#include <Renderer/RenderContext.hpp>

#include <Audio/PCM.hpp>

#include <memory>
#include <string>
#include <vector>

namespace libprojectM {

namespace Renderer {
class CopyTexture;
class PresetTransition;
class Renderer;
class TextureManager;
class ShaderCache;
class TransitionShaderManager;

namespace Platform {
class GLResolver;
}

} // namespace Renderer

namespace UserSprites {
class SpriteManager;
} // namespace UserSprites

class Preset;
class PresetFactoryManager;
class TimeKeeper;

class PROJECTM_EXPORT ProjectM
{
public:
    ProjectM(const std::shared_ptr<Renderer::Platform::GLResolver>& glResolver);

    virtual ~ProjectM();

    /**
     * @brief Callback for notifying the integrating app that projectM wants to switch to a new preset.
     *
     * It is safe to call LoadPreset() from inside the callback. The app can decide when to actually
     * call the function or even ignore the request completely.
     *
     * @param isHardCut True if the switch event was caused by a hard cut, false if it is a soft cut.
     */
    virtual void PresetSwitchRequestedEvent(bool isHardCut) const;

    /**
     * @brief Callback for notifying the integrating app that the requested preset file couldn't be loaded.
     * @param presetFilename The filename of the preset that failed to load. Empty if loaded from a stream.
     * @param message The error message with the failure reason.
     */
    virtual void PresetSwitchFailedEvent(const std::string& presetFilename, const std::string& message) const;

    /**
     * @brief Loads the given preset file and performs a smooth or immediate transition.
     * @param presetFilename The preset filename to load.
     * @param smoothTransition If set to true, old and new presets will be blended over smoothly.
     *                         If set to false, the new preset will be rendered immediately.
     */
    void LoadPresetFile(const std::string& presetFilename, bool smoothTransition);

    /**
     * @brief Loads the given preset data and performs a smooth or immediate transition.
     *
     * This function assumes the data to be in Milkdrop format.
     *
     * @param presetData The preset data stream to load from.
     * @param smoothTransition If set to true, old and new presets will be blended over smoothly.
     *                         If set to false, the new preset will be rendered immediately.
     */
    void LoadPresetData(std::istream& presetData, bool smoothTransition);

    void SetWindowSize(uint32_t width, uint32_t height);

    /**
     * @brief Sets the texture paths used to find images for presets.
     *
     * Setting new texture paths will clear the texture manager cache and reload textures.
     * This can cause lags in rendering.
     *
     * @param texturePaths A list of paths projectM will look for texture images, in order.
     */
    void SetTexturePaths(std::vector<std::string> texturePaths);

    void ResetTextures();

    void RenderFrame(uint32_t targetFramebufferObject = 0);

    /**
     * @brief Sets a user-specified time for rendering the next frame
     * Negative values will make projectM use the system clock instead.
     * @param secondsSinceStart Fractional seconds since rendering the first frame.
     */
    void SetFrameTime(double secondsSinceStart);

    /**
     * @brief Gets the time of the last frame rendered.
     * @note This will not return the value set with SetFrameTime, but the actual time used to render the last frame.
     *       If a user-specified frame time was set, this value is returned. Otherwise, the frame time measured via the
     *       system clock will be returned.
     * @return Seconds elapsed rendering the last frame since starting projectM.
     */
    double GetFrameTime();

    void SetBeatSensitivity(float sensitivity);

    auto GetBeatSensitivity() const -> float;

    auto SoftCutDuration() const -> double;

    void SetSoftCutDuration(double seconds);

    auto HardCutDuration() const -> double;

    void SetHardCutDuration(double seconds);

    auto HardCutEnabled() const -> bool;

    void SetHardCutEnabled(bool enabled);

    auto HardCutSensitivity() const -> float;

    void SetHardCutSensitivity(float sensitivity);

    /**
     * @brief Returns the currently set preset duration in seconds.
     * @return The currently set preset duration in seconds.
     */
    auto PresetDuration() const -> double;

    void SetPresetDuration(double seconds);

    /**
     * @brief Returns the current frames per second value.
     * @return The current frames per second value.
     */
    auto TargetFramesPerSecond() const -> int32_t;

    /**
     * @brief Sets a new current frames per second value.
     * @param fps The new frames per second value.
     */
    void SetTargetFramesPerSecond(int32_t fps);

    auto AspectCorrection() const -> bool;

    void SetAspectCorrection(bool enabled);

    auto EasterEgg() const -> float;

    void SetEasterEgg(float value);

    void MeshSize(uint32_t& meshResolutionX, uint32_t& meshResolutionY) const;

    void SetMeshSize(uint32_t meshResolutionX, uint32_t meshResolutionY);

    void TexelOffsets(float& texelOffsetX, float& texelOffsetY) const;

    void SetTexelOffsets(float texelOffsetX, float texelOffsetY);

    void Touch(float touchX, float touchY, int pressure, int touchType);

    void TouchDrag(float touchX, float touchY, int pressure);

    void TouchDestroy(float touchX, float touchY);

    void TouchDestroyAll();

    /// Turn on or off a lock that prevents projectM from switching to another preset
    void SetPresetLocked(bool locked);

    /// Returns true if the active preset is locked
    auto PresetLocked() const -> bool;

    auto PCM() -> Audio::PCM&;

    auto WindowWidth() -> int;

    auto WindowHeight() -> int;

    /**
     * @brief Creates a new user sprite.
     * @param type The type of the sprite to create.
     * @param spriteData The type-dependent sprite data.
     * @return A unique, non-zero sprite identifier if the sprite was created successfully,
     *         or zero if any error occurred or the sprite limit was reached.
     */
    auto AddUserSprite(const std::string& type, const std::string& spriteData) -> uint32_t;

    /**
     * @brief Destroys a single sprite using its identifier.
     * @param spriteIdentifier The identifier of the sprite to destroy.
     */
    void DestroyUserSprite(uint32_t spriteIdentifier);

    /**
     * @brief Destroys all active user sprites.
     */
    void DestroyAllUserSprites();

    /**
     * @brief Returns the current user sprite count.
     * @return The number of currently active user sprites.
     */
    auto UserSpriteCount() const -> uint32_t;

    /**
     * @brief Sets a new limit for user sprites.
     * Any sprites above the new limit will be removed in order, oldest first.
     * @param maxSprites The new sprite limit. 0 disables sprites, 16 is the default.
     */
    void SetUserSpriteLimit(uint32_t maxSprites);

    /**
     * @brief Returns the current user sprite display limit.
     * @return The current user sprite limit.
     */
    auto UserSpriteLimit() const -> uint32_t;

    /**
     * @brief Returns a list of identifiers for all active user sprites.
     * @return A list of all active user sprite identifiers, ordered by age (oldest first).
     */
    auto UserSpriteIdentifiers() const -> std::vector<uint32_t>;

    /**
     * @brief Draws the given texture on the active preset's main texture to get a "burn-in" effect.
     * @param openGlTextureId The OpenGL texture to draw onto the active preset(s).
     * @param left Left coordinate in pixels on the destination texture.
     * @param top Top coordinate in pixels on the destination texture.
     * @param width Width of the final image on the destination texture in pixels, can be negative to flip it horizontally.
     * @param height Height of the final image on the destination texture in pixels, can be negative to flip it vertically.
     */
    void BurnInTexture(uint32_t openGlTextureId, int left, int top, int width, int height);

private:
    void Initialize();

    void CheckGLSLVersion();

    void StartPresetTransition(std::unique_ptr<Preset>&& preset, bool hardCut);

    void LoadIdlePreset();

    auto GetRenderContext() -> Renderer::RenderContext;

    uint32_t m_meshX{32};            //!< Per-point mesh horizontal resolution.
    uint32_t m_meshY{24};            //!< Per-point mesh vertical resolution.
    uint32_t m_targetFps{35};        //!< Target frames per second.
    uint32_t m_windowWidth{0};       //!< EvaluateFrameData window width. If 0, nothing is rendered.
    uint32_t m_windowHeight{0};      //!< EvaluateFrameData window height. If 0, nothing is rendered.
    double m_presetDuration{30.0};   //!< Preset duration in seconds.
    double m_softCutDuration{3.0};   //!< Soft cut transition time.
    double m_hardCutDuration{20.0};  //!< Time after which a hard cut can happen at the earliest.
    bool m_hardCutEnabled{false};    //!< If true, hard cuts based on beat detection are enabled.
    float m_hardCutSensitivity{2.0}; //!< Loudness sensitivity value for hard cuts.
    float m_beatSensitivity{1.0};    //!< General beat sensitivity modifier for presets.
    bool m_aspectCorrection{true};   //!< If true, corrects aspect ratio for non-rectangular windows.
    float m_easterEgg{1.0};          //!< Random preset duration modifier. See TimeKeeper class.
    float m_previousFrameVolume{};   //!< Volume in previous frame, used for hard cuts.
    float m_texelOffsetX{0.0};       //!< Horizontal warp shader texel offset
    float m_texelOffsetY{0.0};       //!< Vertical warp shader texel offset

    std::vector<std::string> m_textureSearchPaths; ///!< List of paths to search for texture files

    /** Timing information */
    int m_frameCount{0}; //!< Rendered frame count since start

    bool m_presetLocked{false};         //!< If true, the preset change event will not be sent.
    bool m_presetChangeNotified{false}; //!< Stores whether the user has been notified that projectM wants to switch the preset.

    std::shared_ptr<Renderer::Platform::GLResolver> m_glResolver;                 //!< Function resolver for accessing GL backend. Stored for keeping ref count, may be null.
    std::unique_ptr<PresetFactoryManager> m_presetFactoryManager; //!< Provides access to all available preset factories.

    Audio::PCM m_audioStorage;                                                    //!< Audio data buffer and analyzer instance.
    std::unique_ptr<Renderer::TextureManager> m_textureManager;                   //!< The texture manager.
    std::unique_ptr<Renderer::ShaderCache> m_shaderCache;                         //!< The global shader cache.
    std::unique_ptr<Renderer::TransitionShaderManager> m_transitionShaderManager; //!< The transition shader manager.
    std::unique_ptr<Renderer::CopyTexture> m_textureCopier;                       //!< Class that copies textures 1:1 to another texture or framebuffer.
    std::unique_ptr<Preset> m_activePreset;                                       //!< Currently loaded preset.
    std::unique_ptr<Preset> m_transitioningPreset;                                //!< Destination preset when smooth preset switching.
    std::unique_ptr<Renderer::PresetTransition> m_transition;                     //!< Transition effect used for blending.
    std::unique_ptr<TimeKeeper> m_timeKeeper;                                     //!< Keeps the different timers used to render and switch presets.
    std::unique_ptr<UserSprites::SpriteManager> m_spriteManager;                  //!< Manages all types of user sprites.
};

} // namespace libprojectM

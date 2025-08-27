
#pragma once

#include <projectM-4/projectM_export.h>

#include <Renderer/RenderContext.hpp>

#include <Audio/PCM.hpp>

#include <memory>
#include <set>
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
}

namespace UserSprites {
class SpriteManager;
}

class Preset;
class PresetFactoryManager;
class TimeKeeper;

class PROJECTM_EXPORT ProjectM
{
public:
    ProjectM();

    virtual ~ProjectM();


    virtual void PresetSwitchRequestedEvent(bool isHardCut) const;


    virtual void PresetSwitchFailedEvent(const std::string& presetFilename, const std::string& message) const;


    void LoadPresetFile(const std::string& presetFilename, bool smoothTransition);


    void LoadPresetData(std::istream& presetData, bool smoothTransition);

    void SetWindowSize(uint32_t width, uint32_t height);


    void SetTexturePaths(std::vector<std::string> texturePaths);

    void ResetTextures();

    void RenderFrame(uint32_t targetFramebufferObject = 0);


    void SetFrameTime(double secondsSinceStart);


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


    auto PresetDuration() const -> double;

    void SetPresetDuration(double seconds);


    auto TargetFramesPerSecond() const -> int32_t;


    void SetTargetFramesPerSecond(int32_t fps);

    auto AspectCorrection() const -> bool;

    void SetAspectCorrection(bool enabled);

    auto EasterEgg() const -> float;

    void SetEasterEgg(float value);

    void MeshSize(uint32_t& meshResolutionX, uint32_t& meshResolutionY) const;

    void SetMeshSize(uint32_t meshResolutionX, uint32_t meshResolutionY);

    void Touch(float touchX, float touchY, int pressure, int touchType);

    void TouchDrag(float touchX, float touchY, int pressure);

    void TouchDestroy(float touchX, float touchY);

    void TouchDestroyAll();


    void SetPresetLocked(bool locked);


    auto PresetLocked() const -> bool;

    auto PCM() -> Audio::PCM&;

    auto WindowWidth() -> int;

    auto WindowHeight() -> int;


    auto AddUserSprite(const std::string& type, const std::string& spriteData) -> uint32_t;


    void DestroyUserSprite(uint32_t spriteIdentifier);


    void DestroyAllUserSprites();


    auto UserSpriteCount() const -> uint32_t;


    void SetUserSpriteLimit(uint32_t maxSprites);


    auto UserSpriteLimit() const -> uint32_t;


    auto UserSpriteIdentifiers() const -> std::vector<uint32_t>;

private:
    void Initialize();

    void StartPresetTransition(std::unique_ptr<Preset>&& preset, bool hardCut);

    void LoadIdlePreset();

    auto GetRenderContext() -> Renderer::RenderContext;

    uint32_t m_meshX{32};
    uint32_t m_meshY{24};
    uint32_t m_targetFps{35};
    uint32_t m_windowWidth{0};
    uint32_t m_windowHeight{0};
    double m_presetDuration{30.0};
    double m_softCutDuration{3.0};
    double m_hardCutDuration{20.0};
    bool m_hardCutEnabled{false};
    float m_hardCutSensitivity{2.0};
    float m_beatSensitivity{1.0};
    bool m_aspectCorrection{true};
    float m_easterEgg{1.0};
    float m_previousFrameVolume{};

    std::vector<std::string> m_textureSearchPaths;


    int m_frameCount{0};

    bool m_presetLocked{false};
    bool m_presetChangeNotified{false};

    std::unique_ptr<PresetFactoryManager> m_presetFactoryManager;

    Audio::PCM m_audioStorage;
    std::unique_ptr<Renderer::TextureManager> m_textureManager;
    std::unique_ptr<Renderer::ShaderCache> m_shaderCache;
    std::unique_ptr<Renderer::TransitionShaderManager> m_transitionShaderManager;
    std::unique_ptr<Renderer::CopyTexture> m_textureCopier;
    std::unique_ptr<Preset> m_activePreset;
    std::unique_ptr<Preset> m_transitioningPreset;
    std::unique_ptr<Renderer::PresetTransition> m_transition;
    std::unique_ptr<TimeKeeper> m_timeKeeper;
    std::unique_ptr<UserSprites::SpriteManager> m_spriteManager;
};

}

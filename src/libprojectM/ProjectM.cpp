

#include "ProjectM.hpp"

#include "Preset.hpp"
#include "PresetFactoryManager.hpp"
#include "TimeKeeper.hpp"

#include <Audio/PCM.hpp>

#include <Renderer/CopyTexture.hpp>
#include <Renderer/PresetTransition.hpp>
#include <Renderer/ShaderCache.hpp>
#include <Renderer/TextureManager.hpp>
#include <Renderer/TransitionShaderManager.hpp>

#include <UserSprites/SpriteManager.hpp>

namespace libprojectM {

ProjectM::ProjectM()
    : m_presetFactoryManager(std::make_unique<PresetFactoryManager>())
{
    Initialize();
}

ProjectM::~ProjectM()
{

}

void ProjectM::PresetSwitchRequestedEvent(bool) const
{
}

void ProjectM::PresetSwitchFailedEvent(const std::string&, const std::string&) const
{
}

void ProjectM::LoadPresetFile(const std::string& presetFilename, bool smoothTransition)
{
    try
    {
        m_textureManager->PurgeTextures();
        StartPresetTransition(m_presetFactoryManager->CreatePresetFromFile(presetFilename), !smoothTransition);
    }
    catch (const std::exception& ex)
    {
        PresetSwitchFailedEvent(presetFilename, ex.what());
    }
}

void ProjectM::LoadPresetData(std::istream& presetData, bool smoothTransition)
{
    try
    {
        m_textureManager->PurgeTextures();
        StartPresetTransition(m_presetFactoryManager->CreatePresetFromStream(".milk", presetData), !smoothTransition);
    }
    catch (const std::exception& ex)
    {
        PresetSwitchFailedEvent("", ex.what());
    }
}

void ProjectM::SetTexturePaths(std::vector<std::string> texturePaths)
{
    m_textureSearchPaths = std::move(texturePaths);
    m_textureManager = std::make_unique<Renderer::TextureManager>(m_textureSearchPaths);
}

void ProjectM::ResetTextures()
{
    m_textureManager = std::make_unique<Renderer::TextureManager>(m_textureSearchPaths);
}

void ProjectM::RenderFrame(uint32_t targetFramebufferObject )
{

    if (m_windowWidth == 0 || m_windowHeight == 0)
    {
        return;
    }


    m_timeKeeper->UpdateTimers();


    m_audioStorage.UpdateFrameAudioData(m_timeKeeper->SecondsSinceLastFrame(), m_frameCount);
    auto audioData = m_audioStorage.GetFrameAudioData();


    if (!m_presetChangeNotified)
    {

        if (m_timeKeeper->PresetProgressA() >= 1.0 && !m_timeKeeper->IsSmoothing())
        {
            m_presetChangeNotified = true;
            PresetSwitchRequestedEvent(false);
        }
        else if (m_hardCutEnabled &&
                 m_frameCount > 50 &&
                 (audioData.vol - m_previousFrameVolume > m_hardCutSensitivity) &&
                 m_timeKeeper->CanHardCut())
        {
            m_presetChangeNotified = true;
            PresetSwitchRequestedEvent(true);
        }
    }


    if (!m_activePreset)
    {
        LoadIdlePreset();
        if (!m_activePreset)
        {
            return;
        }

        m_activePreset->Initialize(GetRenderContext());
    }

    if (m_timeKeeper->IsSmoothing() && m_transitioningPreset != nullptr)
    {


        if (m_timeKeeper->SmoothRatio() >= 1.0)
        {
            m_timeKeeper->EndSmoothing();
        }
    }

    auto renderContext = GetRenderContext();

    if (m_transition != nullptr && m_transitioningPreset != nullptr)
    {
        if (m_transition->IsDone(m_timeKeeper->GetFrameTime()))
        {
            m_activePreset = std::move(m_transitioningPreset);
            m_transitioningPreset.reset();
            m_transition.reset();
        }
        else
        {
            m_transitioningPreset->RenderFrame(audioData, renderContext);
        }
    }



    m_activePreset->RenderFrame(audioData, renderContext);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(targetFramebufferObject));

    if (m_transition != nullptr && m_transitioningPreset != nullptr)
    {
        m_transition->Draw(*m_activePreset, *m_transitioningPreset, renderContext, audioData, m_timeKeeper->GetFrameTime());
    }
    else
    {
        m_textureCopier->Draw(m_activePreset->OutputTexture(), false, false);
    }


    m_spriteManager->Draw(audioData, renderContext, targetFramebufferObject, {m_activePreset, m_transitioningPreset});

    m_frameCount++;
    m_previousFrameVolume = audioData.vol;
}

void ProjectM::Initialize()
{

    m_timeKeeper = std::make_unique<TimeKeeper>(m_presetDuration,
                                                m_softCutDuration,
                                                m_hardCutDuration,
                                                m_easterEgg);





    m_textureManager = std::make_unique<Renderer::TextureManager>(m_textureSearchPaths);
    m_shaderCache = std::make_unique<Renderer::ShaderCache>();

    m_transitionShaderManager = std::make_unique<Renderer::TransitionShaderManager>();

    m_textureCopier = std::make_unique<Renderer::CopyTexture>();

    m_spriteManager = std::make_unique<UserSprites::SpriteManager>();

    m_presetFactoryManager->initialize();


    srand(time(nullptr));

    LoadIdlePreset();

    m_timeKeeper->StartPreset();
}

void ProjectM::LoadIdlePreset()
{
    LoadPresetFile("idle:
    assert(m_activePreset);
}

void ProjectM::SetWindowSize(uint32_t width, uint32_t height)
{

    m_windowWidth = width;
    m_windowHeight = height;
}

void ProjectM::StartPresetTransition(std::unique_ptr<Preset>&& preset, bool hardCut)
{
    m_presetChangeNotified = m_presetLocked;

    if (preset == nullptr)
    {
        return;
    }

    preset->Initialize(GetRenderContext());


    if (m_transitioningPreset != nullptr)
    {
        m_activePreset = std::move(m_transitioningPreset);
        m_transition.reset();
    }

    if (m_activePreset)
    {
        preset->DrawInitialImage(m_activePreset->OutputTexture(), GetRenderContext());
    }

    if (hardCut)
    {
        m_activePreset = std::move(preset);
        m_timeKeeper->StartPreset();
    }
    else
    {
        m_transitioningPreset = std::move(preset);
        m_timeKeeper->StartSmoothing();
        m_transition = std::make_unique<Renderer::PresetTransition>(m_transitionShaderManager->RandomTransition(), m_softCutDuration, m_timeKeeper->GetFrameTime());
    }
}

auto ProjectM::WindowWidth() -> int
{
    return m_windowWidth;
}

auto ProjectM::WindowHeight() -> int
{
    return m_windowHeight;
}

auto ProjectM::AddUserSprite(const std::string& type, const std::string& spriteData) -> uint32_t
{
    return m_spriteManager->Spawn(type, spriteData, GetRenderContext());
}

void ProjectM::DestroyUserSprite(uint32_t spriteIdentifier)
{
    m_spriteManager->Destroy(spriteIdentifier);
}

void ProjectM::DestroyAllUserSprites()
{
    m_spriteManager->DestroyAll();
}

auto ProjectM::UserSpriteCount() const -> uint32_t
{
    return m_spriteManager->ActiveSpriteCount();
}

void ProjectM::SetUserSpriteLimit(uint32_t maxSprites)
{
    m_spriteManager->SpriteSlots(maxSprites);
}

auto ProjectM::UserSpriteLimit() const -> uint32_t
{
    return m_spriteManager->SpriteSlots();
}

auto ProjectM::UserSpriteIdentifiers() const -> std::vector<uint32_t>
{
    return m_spriteManager->ActiveSpriteIdentifiers();
}

void ProjectM::SetPresetLocked(bool locked)
{


    m_presetLocked = locked;
    m_presetChangeNotified = locked;
}

auto ProjectM::PresetLocked() const -> bool
{
    return m_presetLocked;
}

void ProjectM::SetFrameTime(double secondsSinceStart)
{
    m_timeKeeper->SetFrameTime(secondsSinceStart);
}

double ProjectM::GetFrameTime()
{
    return m_timeKeeper->GetFrameTime();
}

void ProjectM::SetBeatSensitivity(float sensitivity)
{
    m_beatSensitivity = std::min(std::max(0.0f, sensitivity), 2.0f);
}

auto ProjectM::GetBeatSensitivity() const -> float
{
    return m_beatSensitivity;
}

auto ProjectM::SoftCutDuration() const -> double
{
    return m_softCutDuration;
}

void ProjectM::SetSoftCutDuration(double seconds)
{
    m_softCutDuration = seconds;
    m_timeKeeper->ChangeSoftCutDuration(seconds);
}

auto ProjectM::HardCutDuration() const -> double
{
    return m_hardCutDuration;
}

void ProjectM::SetHardCutDuration(double seconds)
{
    m_hardCutDuration = static_cast<int>(seconds);
    m_timeKeeper->ChangeHardCutDuration(seconds);
}

auto ProjectM::HardCutEnabled() const -> bool
{
    return m_hardCutEnabled;
}

void ProjectM::SetHardCutEnabled(bool enabled)
{
    m_hardCutEnabled = enabled;
}

auto ProjectM::HardCutSensitivity() const -> float
{
    return m_hardCutSensitivity;
}

void ProjectM::SetHardCutSensitivity(float sensitivity)
{
    m_hardCutSensitivity = sensitivity;
}

void ProjectM::SetPresetDuration(double seconds)
{
    m_timeKeeper->ChangePresetDuration(seconds);
}

auto ProjectM::PresetDuration() const -> double
{
    return m_timeKeeper->PresetDuration();
}

auto ProjectM::TargetFramesPerSecond() const -> int32_t
{
    return m_targetFps;
}

void ProjectM::SetTargetFramesPerSecond(int32_t fps)
{
    m_targetFps = fps;
}

auto ProjectM::AspectCorrection() const -> bool
{
    return m_aspectCorrection;
}

void ProjectM::SetAspectCorrection(bool enabled)
{
    m_aspectCorrection = enabled;
}

auto ProjectM::EasterEgg() const -> float
{
    return m_easterEgg;
}

void ProjectM::SetEasterEgg(float value)
{
    m_easterEgg = value;
    m_timeKeeper->ChangeEasterEgg(value);
}

void ProjectM::MeshSize(uint32_t& meshResolutionX, uint32_t& meshResolutionY) const
{
    meshResolutionX = m_meshX;
    meshResolutionY = m_meshY;
}

void ProjectM::SetMeshSize(uint32_t meshResolutionX, uint32_t meshResolutionY)
{
    m_meshX = meshResolutionX;
    m_meshY = meshResolutionY;


    if (m_meshX % 2 == 1)
    {
        m_meshX++;
    }

    if (m_meshY % 2 == 1)
    {
        m_meshY++;
    }


    m_meshX = std::max(8u, std::min(400u, m_meshX));
    m_meshY = std::max(8u, std::min(400u, m_meshY));
}

auto ProjectM::PCM() -> libprojectM::Audio::PCM&
{
    return m_audioStorage;
}

void ProjectM::Touch(float, float, int, int)
{

}

void ProjectM::TouchDrag(float, float, int)
{

}

void ProjectM::TouchDestroy(float, float)
{

}

void ProjectM::TouchDestroyAll()
{

}

auto ProjectM::GetRenderContext() -> Renderer::RenderContext
{
    Renderer::RenderContext ctx{};
    ctx.viewportSizeX = m_windowWidth;
    ctx.viewportSizeY = m_windowHeight;
    ctx.time = static_cast<float>(m_timeKeeper->GetRunningTime());
    ctx.progress = static_cast<float>(m_timeKeeper->PresetProgressA());
    ctx.fps = static_cast<float>(m_targetFps);
    ctx.frame = m_frameCount;
    ctx.aspectX = (m_windowHeight > m_windowWidth) ? static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight) : 1.0f;
    ctx.aspectY = (m_windowWidth > m_windowHeight) ? static_cast<float>(m_windowHeight) / static_cast<float>(m_windowWidth) : 1.0f;
    ctx.invAspectX = 1.0f / ctx.aspectX;
    ctx.invAspectY = 1.0f / ctx.aspectY;
    ctx.perPixelMeshX = static_cast<int>(m_meshX);
    ctx.perPixelMeshY = static_cast<int>(m_meshY);
    ctx.textureManager = m_textureManager.get();
    ctx.shaderCache = m_shaderCache.get();

    if (m_transition)
    {
        ctx.blendProgress = m_transition->Progress(ctx.time);
    }
    else
    {
        ctx.blendProgress = 0.0;
    }

    return ctx;
}

}

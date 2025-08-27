
#pragma once

#include "Border.hpp"
#include "CustomShape.hpp"
#include "CustomWaveform.hpp"
#include "DarkenCenter.hpp"
#include "Filters.hpp"
#include "FinalComposite.hpp"
#include "MotionVectors.hpp"
#include "PerFrameContext.hpp"
#include "PerPixelContext.hpp"
#include "PerPixelMesh.hpp"
#include "Preset.hpp"
#include "Waveform.hpp"

#include <Renderer/CopyTexture.hpp>
#include <Renderer/Framebuffer.hpp>

#include <cassert>
#include <map>
#include <memory>
#include <string>

namespace libprojectM {
class PresetFileParser;

namespace MilkdropPreset {

class Factory;

class MilkdropPreset : public ::libprojectM::Preset
{

public:

    MilkdropPreset(const std::string& absoluteFilePath);


    MilkdropPreset(std::istream& presetData);


    void Initialize(const Renderer::RenderContext& renderContext) override;


    void RenderFrame(const libprojectM::Audio::FrameAudioData& audioData,
                     const Renderer::RenderContext& renderContext) override;

    auto OutputTexture() const -> std::shared_ptr<Renderer::Texture> override;

    void DrawInitialImage(const std::shared_ptr<Renderer::Texture>& image, const Renderer::RenderContext& renderContext) override;

    void BindFramebuffer() override;

private:
    void PerFrameUpdate();

    void Load(const std::string& pathname);

    void Load(std::istream& stream);

    void InitializePreset(::libprojectM::PresetFileParser& parsedFile);

    void CompileCodeAndRunInitExpressions();


    void LoadShaderCode();

    auto ParseFilename(const std::string& filename) -> std::string;

    std::string m_absoluteFilePath;
    std::string m_absolutePath;

    Renderer::Framebuffer m_framebuffer{2};
    int m_currentFrameBuffer{0};
    int m_previousFrameBuffer{1};
    std::shared_ptr<Renderer::TextureAttachment> m_motionVectorUVMap;

    PresetState m_state;
    PerFrameContext m_perFrameContext;
    PerPixelContext m_perPixelContext;

    PerPixelMesh m_perPixelMesh;

    MotionVectors m_motionVectors;
    Waveform m_waveform;
    std::array<std::unique_ptr<CustomWaveform>, CustomWaveformCount> m_customWaveforms;
    std::array<std::unique_ptr<CustomShape>, CustomShapeCount> m_customShapes;
    DarkenCenter m_darkenCenter;
    Border m_border;
    Renderer::CopyTexture m_flipTexture;

    FinalComposite m_finalComposite;

    bool m_isFirstFrame{true};
};

}
}



#include "MilkdropPreset.hpp"

#include "Factory.hpp"
#include "MilkdropPresetExceptions.hpp"
#include "PresetFileParser.hpp"

#ifdef MILKDROP_PRESET_DEBUG
#include <iostream>
#endif

namespace libprojectM {
namespace MilkdropPreset {

MilkdropPreset::MilkdropPreset(const std::string& absoluteFilePath)
    : m_absoluteFilePath(absoluteFilePath)
    , m_perFrameContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_perPixelContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_motionVectors(m_state)
    , m_waveform(m_state)
    , m_darkenCenter(m_state)
    , m_border(m_state)
{
    Load(absoluteFilePath);
}

MilkdropPreset::MilkdropPreset(std::istream& presetData)
    : m_perFrameContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_perPixelContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_motionVectors(m_state)
    , m_waveform(m_state)
    , m_darkenCenter(m_state)
    , m_border(m_state)
{
    Load(presetData);
}

void MilkdropPreset::Initialize(const Renderer::RenderContext& renderContext)
{
    assert(renderContext.textureManager);
    m_state.renderContext = renderContext;
    m_state.blurTexture.Initialize(renderContext);
    m_state.LoadShaders();


    CompileCodeAndRunInitExpressions();


    m_framebuffer.SetSize(renderContext.viewportSizeX, renderContext.viewportSizeY);
    m_motionVectorUVMap->SetSize(renderContext.viewportSizeX, renderContext.viewportSizeY);
    if (m_state.mainTexture.expired())
    {
        m_state.mainTexture = m_framebuffer.GetColorAttachmentTexture(1, 0);
    }

    m_perPixelMesh.CompileWarpShader(m_state);
    m_finalComposite.CompileCompositeShader(m_state);
}

void MilkdropPreset::RenderFrame(const libprojectM::Audio::FrameAudioData& audioData, const Renderer::RenderContext& renderContext)
{
    m_state.audioData = audioData;
    m_state.renderContext = renderContext;


    if (m_framebuffer.SetSize(renderContext.viewportSizeX, renderContext.viewportSizeY))
    {
        m_motionVectorUVMap->SetSize(renderContext.viewportSizeX, renderContext.viewportSizeY);
        m_isFirstFrame = true;
    }

    m_state.mainTexture = m_framebuffer.GetColorAttachmentTexture(m_previousFrameBuffer, 0);


    PerFrameUpdate();

    glViewport(0, 0, renderContext.viewportSizeX, renderContext.viewportSizeY);

    m_framebuffer.Bind(m_previousFrameBuffer);


    if (!m_isFirstFrame)
    {
        m_motionVectors.Draw(m_perFrameContext, m_motionVectorUVMap->Texture());
    }


    m_flipTexture.Draw(m_framebuffer.GetColorAttachmentTexture(m_previousFrameBuffer, 0), nullptr, true, false);
    m_state.mainTexture = m_flipTexture.Texture();


    m_framebuffer.Bind(m_currentFrameBuffer);


    m_framebuffer.SetAttachment(m_currentFrameBuffer, 1, m_motionVectorUVMap);


    m_perPixelMesh.Draw(m_state, m_perFrameContext, m_perPixelContext);


    m_framebuffer.RemoveColorAttachment(m_currentFrameBuffer, 1);


    {
        const auto warpedImage = m_framebuffer.GetColorAttachmentTexture(m_currentFrameBuffer, 0);
        assert(warpedImage.get());
        m_state.blurTexture.Update(*warpedImage, m_perFrameContext);
    }


    for (auto& shape : m_customShapes)
    {
        shape->Draw();
    }
    for (auto& wave : m_customWaveforms)
    {
        wave->Draw(m_perFrameContext);
    }
    m_waveform.Draw(m_perFrameContext);


    if (*m_perFrameContext.darken_center > 0)
    {
        m_darkenCenter.Draw();
    }
    m_border.Draw(m_perFrameContext);


    m_flipTexture.Draw(m_framebuffer.GetColorAttachmentTexture(m_currentFrameBuffer, 0), nullptr, true, false);
    m_state.mainTexture = m_flipTexture.Texture();


    m_framebuffer.BindRead(m_currentFrameBuffer);
    m_framebuffer.BindDraw(m_previousFrameBuffer);

    m_finalComposite.Draw(m_state, m_perFrameContext);

    if (!m_finalComposite.HasCompositeShader())
    {

        m_flipTexture.Draw(m_framebuffer.GetColorAttachmentTexture(m_previousFrameBuffer, 0), m_framebuffer, m_previousFrameBuffer, true, false);
    }


    std::swap(m_currentFrameBuffer, m_previousFrameBuffer);

    m_isFirstFrame = false;
}

auto MilkdropPreset::OutputTexture() const -> std::shared_ptr<Renderer::Texture>
{

    return m_framebuffer.GetColorAttachmentTexture(m_currentFrameBuffer, 0);
}

void MilkdropPreset::DrawInitialImage(const std::shared_ptr<Renderer::Texture>& image, const Renderer::RenderContext& renderContext)
{
    m_framebuffer.SetSize(renderContext.viewportSizeX, renderContext.viewportSizeY);


    m_flipTexture.Draw(image, m_framebuffer, m_previousFrameBuffer);
}

void MilkdropPreset::BindFramebuffer()
{
    if (m_framebuffer.Width() > 0 && m_framebuffer.Height() > 0)
    {
        m_framebuffer.BindDraw(m_previousFrameBuffer);
    }
}

void MilkdropPreset::PerFrameUpdate()
{
    m_perFrameContext.LoadStateVariables(m_state);
    m_perPixelContext.LoadStateReadOnlyVariables(m_state, m_perFrameContext);

    m_perFrameContext.ExecutePerFrameCode();

    m_perPixelContext.LoadPerFrameQVariables(m_state, m_perFrameContext);


    *m_perFrameContext.gamma = std::max(0.0, std::min(8.0, *m_perFrameContext.gamma));
    *m_perFrameContext.echo_zoom = std::max(0.001, std::min(1000.0, *m_perFrameContext.echo_zoom));
}

void MilkdropPreset::Load(const std::string& pathname)
{
#ifdef MILKDROP_PRESET_DEBUG
    std::cerr << "[Preset] Loading preset from file \"" << pathname << "\"." << std::endl;
#endif

    SetFilename(ParseFilename(pathname));

    ::libprojectM::PresetFileParser parser;

    if (!parser.Read(pathname))
    {
#ifdef MILKDROP_PRESET_DEBUG
        std::cerr << "[Preset] Could not parse preset file." << std::endl;
#endif
        throw MilkdropPresetLoadException("Could not parse preset file \"" + pathname + "\"");
    }

    InitializePreset(parser);
}

void MilkdropPreset::Load(std::istream& stream)
{
#ifdef MILKDROP_PRESET_DEBUG
    std::cerr << "[Preset] Loading preset from stream." << std::endl;
#endif

    ::libprojectM::PresetFileParser parser;

    if (!parser.Read(stream))
    {
#ifdef MILKDROP_PRESET_DEBUG
        std::cerr << "[Preset] Could not parse preset data." << std::endl;
#endif
        throw MilkdropPresetLoadException("Could not parse preset data.");
    }

    InitializePreset(parser);
}

void MilkdropPreset::InitializePreset(::libprojectM::PresetFileParser& parsedFile)
{

    m_motionVectorUVMap = std::make_shared<Renderer::TextureAttachment>(GL_RG16F, GL_RG, GL_FLOAT, 0, 0);
    m_framebuffer.CreateColorAttachment(0, 0);
    m_framebuffer.CreateColorAttachment(1, 0);

    Renderer::Framebuffer::Unbind();


    m_state.Initialize(parsedFile);


    m_perFrameContext.RegisterBuiltinVariables();
    m_perPixelContext.RegisterBuiltinVariables();


    for (int i = 0; i < CustomWaveformCount; i++)
    {
        auto wave = std::make_unique<CustomWaveform>(m_state);
        wave->Initialize(parsedFile, i);
        m_customWaveforms[i] = std::move(wave);
    }


    for (int i = 0; i < CustomShapeCount; i++)
    {
        auto shape = std::make_unique<CustomShape>(m_state);
        shape->Initialize(parsedFile, i);
        m_customShapes[i] = std::move(shape);
    }


    LoadShaderCode();
}

void MilkdropPreset::CompileCodeAndRunInitExpressions()
{

    m_perFrameContext.LoadStateVariables(m_state);
    m_perFrameContext.EvaluateInitCode(m_state);
    m_perFrameContext.CompilePerFrameCode(m_state.perFrameCode);


    m_perPixelContext.CompilePerPixelCode(m_state.perPixelCode);

    for (int i = 0; i < CustomWaveformCount; i++)
    {
        auto& wave = m_customWaveforms[i];
        wave->CompileCodeAndRunInitExpressions(m_perFrameContext);
    }

    for (int i = 0; i < CustomShapeCount; i++)
    {
        auto& shape = m_customShapes[i];
        shape->CompileCodeAndRunInitExpressions();
    }
}

void MilkdropPreset::LoadShaderCode()
{
    m_perPixelMesh.LoadWarpShader(m_state);
    m_finalComposite.LoadCompositeShader(m_state);
}

auto MilkdropPreset::ParseFilename(const std::string& filename) -> std::string
{
    const std::size_t start = filename.find_last_of('/');

    if (start == std::string::npos || start >= (filename.length() - 1))
    {
        return "";
    }

    return filename.substr(start + 1, filename.length());
}


}
}

/**
 * projectM -- Milkdrop-esque visualisation SDK
 * Copyright (C)2003-2004 projectM Team
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

#include "MilkdropPreset.hpp"

#include "MilkdropPresetExceptions.hpp"
#include "MilkdropPresetFactory.hpp"
#include "PresetFileParser.hpp"

#ifdef MILKDROP_PRESET_DEBUG
#include <iostream>
#endif

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

void MilkdropPreset::Initialize(const RenderContext& renderContext)
{
    assert(renderContext.textureManager);
    m_state.renderContext = renderContext;

    // Initialize variables and code now we have a proper render state.
    CompileCodeAndRunInitExpressions();

    // Update framebuffer and texture sizes if needed
    m_framebuffer.SetSize(renderContext.viewportSizeX, renderContext.viewportSizeY);
    m_motionVectorUVMap->SetSize(renderContext.viewportSizeX, renderContext.viewportSizeY);
    if (m_state.mainTexture.expired())
    {
        m_state.mainTexture = m_framebuffer.GetColorAttachmentTexture(1, 0);
    }

    m_perPixelMesh.CompileWarpShader(m_state);
    m_finalComposite.CompileCompositeShader(m_state);
}

void MilkdropPreset::RenderFrame(const libprojectM::Audio::FrameAudioData& audioData, const RenderContext& renderContext)
{
    m_state.audioData = audioData;
    m_state.renderContext = renderContext;

    // Update framebuffer and u/v texture size if needed
    if (m_framebuffer.SetSize(renderContext.viewportSizeX, renderContext.viewportSizeY))
    {
        m_motionVectorUVMap->SetSize(renderContext.viewportSizeX, renderContext.viewportSizeY);
        m_isFirstFrame = true;
    }

    m_state.mainTexture = m_framebuffer.GetColorAttachmentTexture(m_previousFrameBuffer, 0);

    // First evaluate per-frame code
    PerFrameUpdate();

    glViewport(0, 0, renderContext.viewportSizeX, renderContext.viewportSizeY);

    m_framebuffer.Bind(m_previousFrameBuffer);
    // Motion vector field. Drawn to the previous frame texture before warping it.
    // Only do it after drawing one frame after init or resize.
    if (!m_isFirstFrame)
    {
        m_motionVectors.Draw(m_perFrameContext, m_motionVectorUVMap->Texture());
    }

    // y-flip the previous frame and assign the flipped texture as "main"
    m_flipTexture.Draw(m_framebuffer.GetColorAttachmentTexture(m_previousFrameBuffer, 0));
    m_state.mainTexture = m_flipTexture.FlippedTexture();

    // We now draw to the current framebuffer.
    m_framebuffer.Bind(m_currentFrameBuffer);

    // Add motion vector u/v texture for the warp mesh draw and clean both buffers.
    m_framebuffer.SetAttachment(m_currentFrameBuffer, 1, m_motionVectorUVMap);

    // Draw previous frame image warped via per-pixel mesh and warp shader
    m_perPixelMesh.Draw(m_state, m_perFrameContext, m_perPixelContext);

    // Remove the u/v texture from the framebuffer.
    m_framebuffer.RemoveColorAttachment(m_currentFrameBuffer, 1);

    // Update blur textures
    m_state.blurTexture.Update(m_state, m_perFrameContext);

    // Draw audio-data-related stuff
    for (auto& shape : m_customShapes)
    {
        shape->Draw();
    }
    for (auto& wave : m_customWaveforms)
    {
        wave->Draw(m_perFrameContext);
    }
    m_waveform.Draw(m_perFrameContext);

    // Done in DrawSprites() in Milkdrop
    if (*m_perFrameContext.darken_center > 0)
    {
        m_darkenCenter.Draw();
    }
    m_border.Draw(m_perFrameContext);

    // Todo: Song title anim would go here

    // y-flip the image for final compositing again
    m_flipTexture.Draw(m_framebuffer.GetColorAttachmentTexture(m_currentFrameBuffer, 0));
    m_state.mainTexture = m_flipTexture.FlippedTexture();

    // We no longer need the previous frame image, use it to render the final composite.
    m_framebuffer.BindRead(m_currentFrameBuffer);
    m_framebuffer.BindDraw(m_previousFrameBuffer);

    m_finalComposite.Draw(m_state, m_perFrameContext);

    // ToDo: Draw user sprites (can have evaluated code)

    if (!m_finalComposite.HasCompositeShader())
    {
        // Flip texture again in "previous" framebuffer as old-school effects are still upside down.
        m_flipTexture.Draw(m_framebuffer.GetColorAttachmentTexture(m_previousFrameBuffer, 0), m_framebuffer, m_previousFrameBuffer);
    }

    // TEST: Copy result to default framebuffer
    m_framebuffer.BindRead(m_previousFrameBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glReadBuffer(GL_COLOR_ATTACHMENT0);
#if USE_GLES
    {
        GLenum drawBuffers[] = {GL_BACK};
        glDrawBuffers(1, drawBuffers);
    }
#else
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
#endif
    glBlitFramebuffer(0, 0, renderContext.viewportSizeX, renderContext.viewportSizeY,
                      0, 0, renderContext.viewportSizeX, renderContext.viewportSizeY,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Swap framebuffers for the next frame.
    std::swap(m_currentFrameBuffer, m_previousFrameBuffer);

    m_isFirstFrame = false;
}


void MilkdropPreset::PerFrameUpdate()
{
    m_perFrameContext.LoadStateVariables(m_state);
    m_perPixelContext.LoadStateReadOnlyVariables(m_state, m_perFrameContext);

    m_perFrameContext.ExecutePerFrameCode();

    m_perPixelContext.LoadPerFrameQVariables(m_state, m_perFrameContext);

    // Clamp gamma and echo zoom values
    *m_perFrameContext.gamma = std::max(0.0, std::min(8.0, *m_perFrameContext.gamma));
    *m_perFrameContext.echo_zoom = std::max(0.001, std::min(1000.0, *m_perFrameContext.echo_zoom));
}

void MilkdropPreset::Load(const std::string& pathname)
{
#ifdef MILKDROP_PRESET_DEBUG
    std::cerr << "[Preset] Loading preset from file \"" << pathname << "\"." << std::endl;
#endif

    SetFilename(ParseFilename(pathname));

    PresetFileParser parser;

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

    PresetFileParser parser;

    if (!parser.Read(stream))
    {
#ifdef MILKDROP_PRESET_DEBUG
        std::cerr << "[Preset] Could not parse preset data." << std::endl;
#endif
        throw MilkdropPresetLoadException("Could not parse preset data.");
    }

    InitializePreset(parser);
}

void MilkdropPreset::InitializePreset(PresetFileParser& parsedFile)
{
    // Create the offscreen rendering surfaces.
    m_motionVectorUVMap = std::make_shared<TextureAttachment>(GL_RG16F, GL_RG, GL_FLOAT, 0, 0);
    m_framebuffer.CreateColorAttachment(0, 0); // Main image 1
    m_framebuffer.CreateColorAttachment(1, 0); // Main image 2

    Framebuffer::Unbind();

    // Load global init variables into the state
    m_state.Initialize(parsedFile);

    // Register code context variables
    m_perFrameContext.RegisterBuiltinVariables();
    m_perPixelContext.RegisterBuiltinVariables();

    // Custom waveforms:
    for (int i = 0; i < CustomWaveformCount; i++)
    {
        auto wave = std::make_unique<CustomWaveform>(m_state);
        wave->Initialize(parsedFile, i);
        m_customWaveforms[i] = std::move(wave);
    }

    // Custom shapes:
    for (int i = 0; i < CustomShapeCount; i++)
    {
        auto shape = std::make_unique<CustomShape>(m_state);
        shape->Initialize(parsedFile, i);
        m_customShapes[i] = std::move(shape);
    }

    // Preload shaders
    LoadShaderCode();
}

void MilkdropPreset::CompileCodeAndRunInitExpressions()
{
    // Per-frame init and code
    m_perFrameContext.LoadStateVariables(m_state);
    m_perFrameContext.EvaluateInitCode(m_state);
    m_perFrameContext.CompilePerFrameCode(m_state.perFrameCode);

    // Per-vertex code
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

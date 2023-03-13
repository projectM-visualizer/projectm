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


MilkdropPreset::MilkdropPreset(std::istream& presetData)
    : m_perFrameContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_perPixelContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_motionVectors(m_state)
    , m_waveform(m_state)
    , m_darkenCenter(m_state)
    , m_border(m_state)
    , m_filters(m_state)
{
    Load(presetData);
}


MilkdropPreset::MilkdropPreset(const std::string& absoluteFilePath)
    : m_absoluteFilePath(absoluteFilePath)
    , m_perFrameContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_perPixelContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_motionVectors(m_state)
    , m_waveform(m_state)
    , m_darkenCenter(m_state)
    , m_border(m_state)
    , m_filters(m_state)
{
    Load(absoluteFilePath);
}

void MilkdropPreset::RenderFrame(const libprojectM::Audio::FrameAudioData& audioData, const RenderContext& renderContext)
{
    m_state.audioData = audioData;
    m_state.renderContext = renderContext;
    m_state.mainTextureId = m_framebuffer.GetColorAttachmentTexture(1, 0);

    // Update framebuffer size if needed
    m_framebuffer.SetSize(renderContext.viewportSizeX, renderContext.viewportSizeY);

    // First evaluate per-frame code
    PerFrameUpdate();

    // Motion vector field. Drawn to the previous frame texture before warping it.
    m_framebuffer.Bind(1);
    m_motionVectors.Draw(m_perFrameContext);

    // We now draw to the first framebuffer, but read from the second one for warping and textured shapes.
    m_framebuffer.BindRead(1);
    m_framebuffer.BindDraw(0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // TEST: Copy for now, no warp
    //glBlitFramebuffer(0, 0, renderContext.viewportSizeX, renderContext.viewportSizeY,
    //                  0, 0, renderContext.viewportSizeX, renderContext.viewportSizeY,
    //                  GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // Draw previous frame image warped via per-pixel mesh and warp shader
    m_perPixelMesh.Draw(m_state, m_perFrameContext, m_perPixelContext);

    // Draw audio-data-related stuff
    for (auto& shape : m_customShapes)
    {
        shape->Draw(m_perFrameContext);
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

    // Copy pixels from framebuffer index 0 to 1
    m_framebuffer.BindRead(0);
    m_framebuffer.BindDraw(1);
    glBlitFramebuffer(0, 0, renderContext.viewportSizeX, renderContext.viewportSizeY,
                      0, 0, renderContext.viewportSizeX, renderContext.viewportSizeY,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // ToDo: Apply composite shader
    //m_framebuffer.Bind(0);

    // ToDo: Draw user sprites (can have evaluated code)

    // TEST: Copy result to default framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, renderContext.viewportSizeX, renderContext.viewportSizeY,
                      0, 0, renderContext.viewportSizeX, renderContext.viewportSizeY,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
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
        throw MilkdropPresetLoadException("Could not parse preset data.");
    }

    InitializePreset(parser);
}

void MilkdropPreset::InitializePreset(PresetFileParser& parsedFile)
{
    // Create the offscreen rendering surfaces.
    m_framebuffer.CreateColorAttachment(0, 0);
    m_framebuffer.CreateColorAttachment(1, 0);
    Framebuffer::Unbind();

    // Load global init variables into the state
    m_state.Initialize(parsedFile);

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

    CompileCodeAndRunInitExpressions();
    CompileShaders();
}

void MilkdropPreset::CompileCodeAndRunInitExpressions()
{
    // Per-frame init and code
    m_perFrameContext.RegisterBuiltinVariables();
    m_perFrameContext.LoadStateVariables(m_state);
    m_perFrameContext.EvaluateInitCode(m_state);
    m_perFrameContext.CompilePerFrameCode(m_state.perFrameCode);

    // Per-vertex code
    m_perPixelContext.RegisterBuiltinVariables();
    m_perPixelContext.CompilePerPixelCode(m_state.perPixelCode);

    for (int i = 0; i < CustomWaveformCount; i++)
    {
        auto& wave = m_customWaveforms[i];
        wave->CompileCodeAndRunInitExpressions(m_perFrameContext);
    }

    for (int i = 0; i < CustomShapeCount; i++)
    {
        auto& shape = m_customShapes[i];
        shape->CompileCodeAndRunInitExpressions(m_perFrameContext);
    }
}

void MilkdropPreset::CompileShaders()
{
    m_perPixelMesh.CompileWarpShader(m_state);
    return;
    // Composite shader
    if (m_state.compositeShaderVersion > 0)
    {
        std::string const defaultCompositeShader = R"(shader_body
            {
                ret = tex2D(sampler_main, uv).xyz;
            }
            )";

        m_compositeShader = std::make_unique<MilkdropShader>(MilkdropShader::ShaderType::CompositeShader);
        if (!m_state.warpShader.empty())
        {
            try
            {
                m_compositeShader->LoadCode(m_state.compositeShader);
            }
            catch (ShaderException& ex)
            {
                // Fall back to default shader
                m_compositeShader = std::make_unique<MilkdropShader>(MilkdropShader::ShaderType::CompositeShader);
                m_compositeShader->LoadCode(defaultCompositeShader);
            }
        }
        else
        {
            m_compositeShader->LoadCode(defaultCompositeShader);
        }
    }
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

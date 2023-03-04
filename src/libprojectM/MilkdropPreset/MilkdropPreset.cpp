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
#include "PresetFrameIO.hpp"

#ifdef _WIN32
#include "dirent.h"
#else
#include <dirent.h>
#endif /** _WIN32 */

#ifdef MILKDROP_PRESET_DEBUG
#include <iostream>
#endif


MilkdropPreset::MilkdropPreset(std::istream& presetData)
    : m_perFrameContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_perPixelContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_waveform(m_state)
{
    Load(presetData);
}


MilkdropPreset::MilkdropPreset(const std::string& absoluteFilePath)
    : m_absoluteFilePath(absoluteFilePath)
    , m_perFrameContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_perPixelContext(m_state.globalMemory, &m_state.globalRegisters)
    , m_waveform(m_state)
{
    Load(absoluteFilePath);
}

void MilkdropPreset::InitializePreset(PresetFileParser& parsedFile)
{
    // Load global init variables into the state
    m_state.Initialize(parsedFile);

    // Custom waveforms:
    for (int i = 0; i < CustomWaveformCount; i++)
    {
        auto wave = std::make_unique<CustomWaveform>(m_state);
        wave->Initialize(parsedFile, i);
        m_customWaveforms[0] = std::move(wave);
    }

    // Custom shapes:
    for (int i = 0; i < CustomShapeCount; i++)
    {
        auto shape = std::make_unique<CustomShape>(m_state);
        shape->Initialize(parsedFile, i);
        m_customShapes[0] = std::move(shape);
    }

    CompileCodeAndRunInitExpressions();
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
        shape->CompileCodeAndRunInitExpressions(m_perFrameContext);
    }
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

void MilkdropPreset::RenderFrame(const libprojectM::Audio::FrameAudioData& audioData, const RenderContext& renderContext)
{
    m_state.audioData = audioData;
    m_state.renderContext = renderContext;

    // First evaluate per-frame code
    PerFrameUpdate();

    // Motion vector field
    // ToDo: Draw motion vectors to separate texture

    // Draw previous frame image warped via per-pixel mesh and warp shader
    // ToDo: ComputeGridAlphaValues
    // ToDo: Per-Pixel and warp stuff

    // Draw audio-data-related stuff
    for (auto& shape : m_customShapes)
    {
        shape->Draw(m_perFrameContext);
    }
    for (auto& wave : m_customWaveforms)
    {
        wave->Draw(m_perFrameContext);
    }
    m_waveform.Draw();
    // ToDo: Sprite drawing would go here.
    // Todo: Song title anim would go here

    // ToDo: Store main texture for next frame

    // ToDo: Apply composite shader

    // ToDo: Draw user sprites (can have evaluated code)
}


void MilkdropPreset::PerFrameUpdate()
{
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

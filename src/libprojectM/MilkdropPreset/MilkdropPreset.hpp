/**
 * @file MilkdropPreset.hpp
 * @brief Base class that represents a single Milkdrop preset.
 *
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

#include "CustomShape.hpp"
#include "CustomWaveform.hpp"
#include "PerFrameContext.hpp"
#include "PerPixelContext.hpp"
#include "Preset.hpp"
#include "PresetFrameIO.hpp"
#include "Waveform.hpp"

#include <cassert>
#include <map>
#include <memory>
#include <string>

class MilkdropPresetFactory;

class PresetFileParser;

class MilkdropPreset : public Preset
{

public:
    /**
     * @brief LoadCode a MilkdropPreset by filename with input and output buffers specified.
     * @param factory The factory class that created this preset instance.
     * @param absoluteFilePath the absolute file path of a MilkdropPreset to load from the file system
     */
    MilkdropPreset(const std::string& absoluteFilePath);

    /**
     * @brief LoadCode a MilkdropPreset from an input stream with input and output buffers specified.
     * @param presetData an already initialized input stream to read the MilkdropPreset file from
     * @param presetOutputs initialized and filled with data parsed from a MilkdropPreset
     */
    MilkdropPreset(std::istream& presetData);

    /**
     * @brief Returns the absolute file path of the loaded MilkdropPreset
     * @return The path from which the preset was loaded.
     */
    auto AbsoluteFilePath() const -> const std::string&
    {
        return m_absoluteFilePath;
    }

    /**
     * @brief Renders the preset.
     * @param audioData The frame audio data.
     * @param renderContext The current rendering context/information.
     */
    void RenderFrame(const libprojectM::Audio::FrameAudioData& audioData,
                     const RenderContext& renderContext) override;

private:
    void PerFrameUpdate();

    void Load(const std::string& pathname);

    void Load(std::istream& stream);

    void InitializePreset(PresetFileParser& parsedFile);

    void CompileCodeAndRunInitExpressions();

    auto ParseFilename(const std::string& filename) -> std::string;

    std::string m_absoluteFilePath; //!< The absolute file path of the MilkdropPreset
    std::string m_absolutePath;     //!< The absolute path of the MilkdropPreset

    PresetState m_state; //!< Preset state container.
    PerFrameContext m_perFrameContext; //!< Preset per-frame evaluation code context.
    PerPixelContext m_perPixelContext; //!< Preset per-pixel/per-vertex evaluation code context.

    Waveform m_waveform; //!< Preset default waveform.
    std::array<std::unique_ptr<CustomWaveform>, CustomWaveformCount> m_customWaveforms; //!< Custom waveforms in this preset.
    std::array<std::unique_ptr<CustomShape>, CustomShapeCount> m_customShapes; //!< Custom shapes in this preset.

    friend class MilkdropPresetFactory;
};

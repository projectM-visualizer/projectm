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

#include "Border.hpp"
#include "CustomShape.hpp"
#include "CustomWaveform.hpp"
#include "DarkenCenter.hpp"
#include "Filters.hpp"
#include "FinalComposite.hpp"
#include "FlipTexture.hpp"
#include "MotionVectors.hpp"
#include "PerFrameContext.hpp"
#include "PerPixelContext.hpp"
#include "PerPixelMesh.hpp"
#include "Preset.hpp"
#include "Waveform.hpp"

#include <Renderer/Framebuffer.hpp>

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
     * @brief Initializes the preset with rendering-related data.
     * @param renderContext The initial render context.
     */
    void Initialize(const RenderContext& renderContext) override;

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

    /**
     * @brief Compiles the warp and composite shaders.
     */
    void LoadShaderCode();

    auto ParseFilename(const std::string& filename) -> std::string;

    std::string m_absoluteFilePath; //!< The absolute file path of the MilkdropPreset
    std::string m_absolutePath;     //!< The absolute path of the MilkdropPreset

    Framebuffer m_framebuffer{2};                           //!< Preset rendering framebuffer with two surfaces (last frame and current frame).
    int m_currentFrameBuffer{0};                            //!< Framebuffer ID of the current frame.
    int m_previousFrameBuffer{1};                           //!< Framebuffer ID of the previous frame.
    std::shared_ptr<TextureAttachment> m_motionVectorUVMap; //!< The UV map of the previous frame's warp mesh, used for motion vector reverse propagation.

    PresetState m_state;               //!< Preset state container.
    PerFrameContext m_perFrameContext; //!< Preset per-frame evaluation code context.
    PerPixelContext m_perPixelContext; //!< Preset per-pixel/per-vertex evaluation code context.

    PerPixelMesh m_perPixelMesh; //!< The per-pixel/per-vertex mesh, responsible for most of the movement/warp effects in Milkdrop presets.

    MotionVectors m_motionVectors;                                                      //!< Motion vector grid.
    Waveform m_waveform;                                                                //!< Preset default waveform.
    std::array<std::unique_ptr<CustomWaveform>, CustomWaveformCount> m_customWaveforms; //!< Custom waveforms in this preset.
    std::array<std::unique_ptr<CustomShape>, CustomShapeCount> m_customShapes;          //!< Custom shapes in this preset.
    DarkenCenter m_darkenCenter;                                                        //!< Center darkening effect.
    Border m_border;                                                                    //!< Inner/outer borders.
    FlipTexture m_flipTexture{ m_state };                                               //!< Texture flip filter

    FinalComposite m_finalComposite; //!< Final composite shader or filters.

    bool m_isFirstFrame{true}; //!< Controls drawing the motion vectors starting with the second frame.
};

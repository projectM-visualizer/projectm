#pragma once

#include "LineGeometry.hpp"
#include "WaveformMode.hpp"

#include "Waveforms/WaveformMath.hpp"

#include <Renderer/Mesh.hpp>

#include <glm/vec4.hpp>

#include <array>
#include <memory>
#include <vector>

namespace libprojectM {
namespace MilkdropPreset {

class PresetState;
class PerFrameContext;

class Waveform
{
public:
    explicit Waveform(PresetState& presetState);

    void Draw(const PerFrameContext& presetPerFrameContext);

private:
    /**
     * @brief Draws the waveform lines as scaled quad lines (line reference size set, not dots).
     */
    void DrawQuadLines(const std::array<Waveforms::WaveformMath::VertexList, 2>& waves,
                       const PerFrameContext& presetPerFrameContext, float lineScale);

    /**
     * @brief Calculates the wave color, with the alpha faded by mode, volume and render size.
     * @return The wave color and alpha.
     */
    auto MaximizeColors(const PerFrameContext& presetPerFrameContext) -> glm::vec4;
    void ModulateOpacityByVolume(const PerFrameContext& presetPerFrameContext);

    PresetState& m_presetState; //!< The preset state.

    Renderer::Mesh m_waveMesh;

    WaveformMode m_mode{WaveformMode::Circle}; //!< Line drawing mode.

    std::unique_ptr<Waveforms::WaveformMath> m_waveformMath; //!< The waveform vertex math implementation.

    float m_tempAlpha{0.0f}; //!< Calculated alpha value.
    int m_samples{};         //!< Number of samples in the current waveform. Depends on the mode.

    LineBatch m_lineBatch;                   //!< This frame's waveform strips, for the quad-line renderer.
    std::vector<LineBatch::Strip> m_strips;  //!< The strips in m_lineBatch.
    std::vector<ColoredPoint> m_coloredWave; //!< One wave with its color, for the batch.
};

} // namespace MilkdropPreset
} // namespace libprojectM

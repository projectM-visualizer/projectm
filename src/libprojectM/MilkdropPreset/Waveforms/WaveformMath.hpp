#pragma once

#include "Constants.hpp"
#include "WaveformMode.hpp"

#include "Renderer/RenderItem.hpp"

#include <array>
#include <vector>

namespace libprojectM {
namespace MilkdropPreset {

class PresetState;
class PerFrameContext;

namespace Waveforms {

class WaveformMath
{
public:
    using VertexList = std::vector<Renderer::RenderItem::Point>;

    virtual ~WaveformMath() = default;

    /**
     * @brief Calculates and smoothes the samples and outputs vertices ready for drawing.
     * Depending on the waveform type, only the first set of vertices might be present.
     * @param presetState The preset state older, including the render context.
     * @param presetPerFrameContext The preset per-frame context.
     * @return An array with either one or two sets of waveform vertices.
     */
    auto GetVertices(const PresetState& presetState,
                     const PerFrameContext& presetPerFrameContext)
        -> std::array<VertexList, 2>;

    /**
     * @brief Indicates whether the waveform should be drawn as a closed line loop instead of a strip.
     * @return true if the waveform should be a closed loop, false if it should be drawn as a strip.
     */
    virtual auto IsLoop() -> bool;

protected:
    /**
     * @brief Indicates whether the waveform uses spectrum or normal oscilloscope data.
     * @return true if the waveform needs spectrum data, false if it only needs oscilloscope data.
     */
    virtual auto IsSpectrumWave() -> bool;

    /**
     * @brief Indicates whether the waveform wants a normalized mystery param, or uses the raw configured value.
     * @return true if the value should be normalized to [-1, 1], false to keep it as configured.
     */
    virtual auto UsesNormalizedMysteryParam() -> bool;

    /**
     * @brief The actual waveform-specific math. Overridden by each implementation.
     * The vertices produces must not be smoothed. This is always performed afterwards.
     * @param presetState The preset state older, including the render context.
     * @param presetPerFrameContext The preset per-frame context.
     */
    virtual void GenerateVertices(const PresetState& presetState,
                                  const PerFrameContext& presetPerFrameContext) = 0;

    /**
     * @brief Does a better-than-linear smooth on a wave.
     *
     * Roughly doubles the number of points.
     *
     * @param inputVertices Vector of vertices to be smoothed.
     * @param outputVertices Reference to a buffer that will receive the smoothed data. Will be resized to 2 * vertexCount vertices.
     */
    void SmoothWave(const VertexList& inputVertices, VertexList& outputVertices);

    WaveformMode m_mode{WaveformMode::Line};

    int m_samples{};
    std::array<float, WaveformMaxPoints> m_pcmDataL{0.0f};
    std::array<float, WaveformMaxPoints> m_pcmDataR{0.0f};

    float m_aspectX{1.0f};
    float m_aspectY{1.0f};

    float m_waveX{0.5f};
    float m_waveY{0.5f};

    float m_mysteryWaveParam{1.0f};

    VertexList m_wave1Vertices;
    VertexList m_wave2Vertices;
};

} // namespace Waveforms
} // namespace MilkdropPreset
} // namespace libprojectM

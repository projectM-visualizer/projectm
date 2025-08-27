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


    auto GetVertices(const PresetState& presetState,
                     const PerFrameContext& presetPerFrameContext)
        -> std::array<VertexList, 2>;


    virtual auto IsLoop() -> bool;

protected:

    virtual auto IsSpectrumWave() -> bool;


    virtual auto UsesNormalizedMysteryParam() -> bool;


    virtual void GenerateVertices(const PresetState& presetState,
                                  const PerFrameContext& presetPerFrameContext) = 0;


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

}
}
}

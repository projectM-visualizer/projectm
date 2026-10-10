#pragma once

#include "WaveformMode.hpp"

#include "Waveforms/WaveformMath.hpp"

#include <Renderer/Backend/OpenGL/OpenGLRenderItem.hpp>

#include <memory>

namespace libprojectM {
namespace MilkdropPreset {

class PresetState;
class PerFrameContext;

class Waveform : public libprojectM::Renderer::Backend::OpenGL::OpenGLRenderItem
{
public:
    explicit Waveform(PresetState& presetState);

    void Draw(const PerFrameContext& presetPerFrameContext);

private:
    void MaximizeColors(const PerFrameContext& presetPerFrameContext);
    void ModulateOpacityByVolume(const PerFrameContext& presetPerFrameContext);

    PresetState& m_presetState; //!< The preset state.

    Renderer::Mesh m_waveMesh;

    WaveformMode m_mode{WaveformMode::Circle}; //!< Line drawing mode.

    std::unique_ptr<Waveforms::WaveformMath> m_waveformMath; //!< The waveform vertex math implementation.

    float m_tempAlpha{0.0f}; //!< Calculated alpha value.
    int m_samples{};         //!< Number of samples in the current waveform. Depends on the mode.
};

} // namespace MilkdropPreset
} // namespace libprojectM

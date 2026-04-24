#include "Renderer/PresetTransition.hpp"

#include "Renderer/TextureManager.hpp"

#include <algorithm>
#include <cmath>

namespace libprojectM {
namespace Renderer {

constexpr double PI = 3.14159265358979323846;

PresetTransition::PresetTransition(const std::shared_ptr<Shader>& transitionShader, double durationSeconds, double transitionStartTime)
    : m_mesh(VertexBufferUsage::StaticDraw)
    , m_transitionShader(transitionShader)
    , m_durationSeconds(durationSeconds)
    , m_transitionStartTime(transitionStartTime)
{
    m_mesh.SetRenderPrimitiveType(Mesh::PrimitiveType::TriangleStrip);

    m_mesh.Vertices().Set({{-1.0f, 1.0f},
                           {1.0f, 1.0f},
                           {-1.0f, -1.0f},
                           {1.0f, -1.0f}});

    m_mesh.Indices().Set({0, 1, 2, 3});

    m_mesh.Update();

    std::mt19937 rand32(m_randomDevice());
    m_staticRandomValues = {rand32(), rand32(), rand32(), rand32()};
}

auto PresetTransition::IsDone(double currentFrameTime) const -> bool
{
    const auto secondsSinceStart = currentFrameTime - m_transitionStartTime;
    return m_durationSeconds <= 0.0 || secondsSinceStart >= m_durationSeconds;
}

auto PresetTransition::Progress(double currentFrameTime) const -> double
{
    return std::min(std::max((currentFrameTime - m_transitionStartTime) / m_durationSeconds, 0.0), 1.0);
}

void PresetTransition::Draw(const Preset& oldPreset,
                            const Preset& newPreset,
                            const RenderContext& context,
                            const libprojectM::Audio::FrameAudioData& audioData,
                            double currentFrameTime)
{
    // This method is now a stub; OpenGL-specific rendering is in the backend implementation.
}

} // namespace Renderer
} // namespace libprojectM

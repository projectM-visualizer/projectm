#pragma once

#include "Filters.hpp"
#include "MilkdropShader.hpp"
#include "VideoEcho.hpp"

#include <Renderer/RenderItem.hpp>

#include <array>
#include <memory>

namespace libprojectM {
namespace MilkdropPreset {


class FinalComposite : public Renderer::RenderItem
{
public:
    FinalComposite();

    void InitVertexAttrib() override;


    void LoadCompositeShader(const PresetState& presetState);


    void CompileCompositeShader(PresetState& presetState);


    void Draw(const PresetState& presetState,
              const PerFrameContext& perFrameContext);


    auto HasCompositeShader() const -> bool;

private:

    struct MeshVertex {
        float x{};
        float y{};
        float r{};
        float g{};
        float b{};
        float a{};
        float u{};
        float v{};
        float radius{};
        float angle{};
    };


    void InitializeMesh(const PresetState& presetState);

    static float SquishToCenter(float x, float exponent);

    static void UvToMathSpace(float aspectX, float aspectY,
                              float u, float v, float& rad, float& ang);


    void ApplyHueShaderColors(const PresetState& presetState);

    static constexpr int compositeGridWidth{32};
    static constexpr int compositeGridHeight{24};
    static constexpr int vertexCount{compositeGridWidth * compositeGridHeight};
    static constexpr int indexCount{(compositeGridWidth - 2) * (compositeGridHeight - 2) * 6};

    GLuint m_elementBuffer{};
    std::array<MeshVertex, vertexCount> m_vertices{};
    std::array<int, indexCount> m_indices{};

    int m_viewportWidth{};
    int m_viewportHeight{};

    std::unique_ptr<MilkdropShader> m_compositeShader;
    std::unique_ptr<VideoEcho> m_videoEcho;
    std::unique_ptr<Filters> m_filters;
};

}
}

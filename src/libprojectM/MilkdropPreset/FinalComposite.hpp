#pragma once

#include "Filters.hpp"
#include "MilkdropShader.hpp"
#include "VideoEcho.hpp"

#include <Renderer/Backend/OpenGL/OpenGLRenderItem.hpp>

#include <memory>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Draws the final composite effect, either a shader or Milkdrop 1 effects.
 */
class FinalComposite : public libprojectM::Renderer::Backend::OpenGL::OpenGLRenderItem
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
        float x{}; //!< Vertex X coordinate.
        float y{}; //!< Vertex Y coordinate.
        float r{}; //!< Vertex red color value.
        float g{}; //!< Vertex green color value.
        float b{}; //!< Vertex blue color value.
        float a{}; //!< Vertex alpha value.
        float u{}; //!< Texture X coordinate.
        float v{}; //!< Texture Y coordinate.
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

    Renderer::Mesh m_compositeMesh;                                                                 //!< The composite shader mesh.
    Renderer::VertexBuffer<Renderer::Point> m_radiusAngle{Renderer::VertexBufferUsage::StreamDraw}; //!< Additional vertex attribute array for radius and angle.

    int m_viewportWidth{};  //!< Last known viewport width.
    int m_viewportHeight{}; //!< Last known viewport height.

    std::unique_ptr<MilkdropShader> m_compositeShader; //!< The composite shader. Either preset-defined or empty.
    std::unique_ptr<VideoEcho> m_videoEcho;            //!< Video echo effect. Used if no composite shader is loaded and video echo is enabled.
    std::unique_ptr<Filters> m_filters;                //!< Color post-processing filters. Used if no composite shader is loaded.
};

} // namespace MilkdropPreset
} // namespace libprojectM

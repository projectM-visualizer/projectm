#pragma once

#include "Filters.hpp"
#include "MilkdropShader.hpp"
#include "VideoEcho.hpp"

#include <Renderer/RenderItem.hpp>

#include <array>
#include <memory>

/**
 * @brief Draws the final composite effect, either a shader or Milkdrop 1 effects.
 */
class FinalComposite : public RenderItem
{
public:
    FinalComposite();

    void InitVertexAttrib() override;

    /**
     * @brief Loads the composite shader, if the preset uses one.
     * @param presetState The preset state to retrieve the shader from.
     */
    void LoadCompositeShader(const PresetState& presetState);

    /**
     * @brief Loads the required textures and compiles the composite shader.
     * @param presetState The preset state to retrieve the configuration values from.
     */
    void CompileCompositeShader(PresetState& presetState);

    /**
     * @brief Renders the composite quad with the appropriate effects or shaders.
     * @param presetState The preset state to retrieve the configuration values from.
     * @param presetPerFrameContext The per-frame context to retrieve the initial vars from.
     */
    void Draw(const PresetState& presetState,
              const PerFrameContext& perFrameContext);

    /**
     * @brief Returns if the final composite is using a shader or classic filters.
     * @return true if the final composite is done via a shader, false if not.
     */
    auto HasCompositeShader() const -> bool;

private:
    /**
     * Composite mesh vertex with all required attributes.
     */
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

    /**
     * @brief Initializes the vertex array and fills in static data if needed.
     *
     * The vertices will only be reinitialized if the viewport size changed.
     *
     * @param presetState The preset state to retrieve the configuration values from.
     */
    void InitializeMesh(const PresetState& presetState);

    static float SquishToCenter(float x, float exponent);

    static void UvToMathSpace(float aspectX, float aspectY,
                              float u, float v, float& rad, float& ang);

    /**
     * @brief Calculates the randomized, slowly changing diffuse colors.
     * @param presetState The preset state to retrieve the configuration values from.
     */
    void ApplyHueShaderColors(const PresetState& presetState);

    static constexpr int compositeGridWidth{32};
    static constexpr int compositeGridHeight{24};
    static constexpr int vertexCount{compositeGridWidth * compositeGridHeight};
    static constexpr int indexCount{(compositeGridWidth - 2) * (compositeGridHeight - 2) * 6};

    GLuint m_elementBuffer{}; //!< Element buffer holding the draw indices.
    std::array<MeshVertex, vertexCount> m_vertices{}; //!< Composite grid vertices
    std::array<int, indexCount> m_indices{}; //!< Composite grid draw indices

    int m_viewportWidth{};  //!< Last known viewport width.
    int m_viewportHeight{}; //!< Last known viewport height.

    std::unique_ptr<MilkdropShader> m_compositeShader; //!< The composite shader. Either preset-defined or empty.
    std::unique_ptr<VideoEcho> m_videoEcho; //!< Video echo effect. Used if no composite shader is loaded and video echo is enabled.
    std::unique_ptr<Filters> m_filters; //!< Color post-processing filters. Used if no composite shader is loaded.
};

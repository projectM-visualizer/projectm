#pragma once

#include <Renderer/RenderItem.hpp>
#include <Renderer/Shader.hpp>

#include <cstdint>
#include <vector>

namespace libprojectM {
namespace MilkdropPreset {

class PresetState;
class PerFrameContext;
class PerPixelContext;
class MilkdropShader;


class PerPixelMesh : public Renderer::RenderItem
{
public:
    PerPixelMesh();

    void InitVertexAttrib() override;


    void LoadWarpShader(const PresetState& presetState);


    void CompileWarpShader(PresetState& presetState);


    void Draw(const PresetState& presetState,
              const PerFrameContext& perFrameContext,
              PerPixelContext& perPixelContext);


private:

    struct MeshVertex {
        float x{};
        float y{};
        float radius{};
        float angle{};

        float zoom{};
        float zoomExp{};
        float rot{};
        float warp{};

        float centerX{};
        float centerY{};

        float distanceX{};
        float distanceY{};

        float stretchX{};
        float stretchY{};
    };

    using VertexList = std::vector<MeshVertex>;


    void InitializeMesh(const PresetState& presetState);


    void CalculateMesh(const PresetState& presetState,
                       const PerFrameContext& perFrameContext,
                       PerPixelContext& perPixelContext);


    void WarpedBlit(const PresetState& presetState, const PerFrameContext& perFrameContext);


    auto GetDefaultWarpShader(const PresetState& presetState) -> std::shared_ptr<Renderer::Shader>;

    int m_gridSizeX{};
    int m_gridSizeY{};

    int m_viewportWidth{};
    int m_viewportHeight{};

    VertexList m_vertices;

    std::vector<int> m_listIndices;
    VertexList m_drawVertices;

    std::weak_ptr<Renderer::Shader> m_perPixelMeshShader;
    std::unique_ptr<MilkdropShader> m_warpShader;
    Renderer::Sampler m_perPixelSampler{GL_CLAMP_TO_EDGE, GL_LINEAR};
};

}
}

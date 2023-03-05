#pragma once

#include <Renderer/RenderItem.hpp>

#include <cstdint>
#include <vector>

class PresetState;

class PerPixelContext;

/**
 * @brief The "per-pixel" transformation mesh.
 *
 * This mesh is responsible for most of the motion types in presets. Each mesh vertex
 * is transposed (also scaled, from the center) or rotated to create a frame-by-frame motion.
 * Fragment shader interpolation is then used to create smooth transitions in the space
 * between the grid points.
 *
 * A higher resolution grid means better quality, especially for rotations, but also quickly
 * increases the CPU usage as the per-pixel expression needs to be run for every grid point.
 *
 * The mesh size can be changed between frames, the class will reallocate the buffers if needed.
 */
class PerPixelMesh : public RenderItem
{
public:
    PerPixelMesh();

    void InitVertexAttrib() override;

    /**
     * @brief Renders the transformation mesh.
     * @param presetState The preset state to retrieve the configuration values from.
     * @param perPixelContext The per-pixel code context to use.
     */
    void Draw(const PresetState& presetState, PerPixelContext& perPixelContext);

private:
    /**
     * @brief Initializes the vertex array and fills in static data if needed.
     *
     * The vertices will be reallocated if the grid size has changed. If either this happened,
     * or the viewport size changed, the static values will be recalculated.
     *
     * @param presetState The preset state to retrieve the configuration values from.
     */
    void InitializeMesh(const PresetState& presetState);

    struct MeshVertex {
        float x{};
        float y{};

        float r{};
        float g{};
        float b{};
        float a{1.0f};

        float u{};
        float v{};

        float uOrigin{};
        float vOrigin{};
        float radius{};
        float angle{};
    };

    int m_gridSizeX{};
    int m_gridSizeY{};

    int m_viewportWidth{};
    int m_viewportHeight{};

    std::vector<MeshVertex> m_vertices;

    std::vector<int> m_stripIndices;
    std::vector<int> m_listIndices;
};

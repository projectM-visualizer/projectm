#pragma once

#include "PresetState.hpp"

#include <Renderer/Mesh.hpp>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief Classic Milkdrop 1 postprocessing effects.
 */
class Filters
{
public:
    Filters() = delete;
    explicit Filters(const PresetState& presetState);

    /**
     * @brief Applies the configured filters to the current output.
     */
    void Draw();

private:
    void UpdateMesh();

    /**
     * @brief Brightens the image.
     */
    void Brighten();

    /**
     * @brief Darkens the image.
     */
    void Darken();

    /**
     * @brief Applies a solarize effect.
     */
    void Solarize();

    /**
     * @brief Inverts the image colors.
     */
    void Invert();

    const PresetState& m_presetState; //!< The global preset state.

    Renderer::Mesh m_filterMesh;

    int m_viewportWidth{}; //!< Last known viewport width
    int m_viewportHeight{}; //!< Last known viewport height
};

} // namespace MilkdropPreset
} // namespace libprojectM

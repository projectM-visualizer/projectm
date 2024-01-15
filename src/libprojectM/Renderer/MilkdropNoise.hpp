#pragma once

#include <Renderer/Texture.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Implementation of Milkdrop's noise texture generator.
 *
 * <p>Uses the original Milkdrop noise code, generating a 8-bit RGBA 2D or 3D texture in different
 * resolutions.</p>
 *
 * <p>The preset authoring guide notes different resolutions (e.g. 32x32 for noise_hq) for the textures. These
 * resolutions actually state the number of random data points inside the texture, which are interpolated, and not
 * the actual texture size, which is always the size stated in the authoring guide time the zoom level used during
 * generation.</p>.
 *
 * <p>projectM versions up to 3.x used Perlin noise, which looks quite similar, but the same noise value was used
 * on all color channels. In addition to that, only the GLES version generated RGBA color channels, while the desktop
 * version only used RGB channels and left alpha empty.</p>
 */
class MilkdropNoise
{
public:
    MilkdropNoise() = delete;

    /**
     * Low-quality (high frequency) 2D noise texture, 256x256 with zoom level 1
     * @return A new noise texture ready for use in rendering.
     */
    static auto LowQuality() -> std::shared_ptr<Texture>;

    /**
     * Low-quality (high frequency) 2D noise texture, 32x32 with zoom level 1
     * @return A new noise texture ready for use in rendering.
     */
    static auto LowQualityLite() -> std::shared_ptr<Texture>;

    /**
     * Medium-quality (medium frequency) 2D noise texture, 256x256 with zoom level 4
     * @return A new noise texture ready for use in rendering.
     */
    static auto MediumQuality() -> std::shared_ptr<Texture>;

    /**
     * High-quality (low frequency) 2D noise texture, 256x256 with zoom level 8
     * @return A new noise texture ready for use in rendering.
     */
    static auto HighQuality() -> std::shared_ptr<Texture>;

    /**
     * Low-quality (high frequency) 3D noise texture, 32x32 with zoom level 1
     * @return A new noise texture ready for use in rendering.
     */
    static auto LowQualityVolume() -> std::shared_ptr<Texture>;

    /**
     * High-quality (low frequency) 3D noise texture, 32x32 with zoom level 4
     * @return A new noise texture ready for use in rendering.
     */
    static auto HighQualityVolume() -> std::shared_ptr<Texture>;

protected:

    static auto GetPreferredInternalFormat() -> int;

    /**
     * @brief Milkdrop 2D noise algorithm
     *
     * Creates a different, smoothed noise texture in each of the four color channels.
     *
     * @param size Texture size in pixels.
     * @param zoomFactor Zoom factor. Higher values give a more smoothed/interpolated look.
     * @return A vector with the texture data. Contains size² elements.
     */
    static auto generate2D(int size, int zoomFactor) -> std::vector<uint32_t>;

    /**
     * @brief Milkdrop §D noise algorithm
     *
     * Creates a different, smoothed noise texture in each of the four color channels.
     *
     * @param size Texture size in pixels.
     * @param zoomFactor Zoom factor. Higher values give a more smoothed/interpolated look.
     * @return A vector with the texture data. Contains size³ elements.
     */
    static auto generate3D(int size, int zoomFactor) -> std::vector<uint32_t>;

    static float fCubicInterpolate(float y0, float y1, float y2, float y3, float t);

    static uint32_t dwCubicInterpolate(uint32_t y0, uint32_t y1, uint32_t y2, uint32_t y3, float t);
};

} // namespace Renderer
} // namespace libprojectM

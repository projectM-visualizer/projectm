#pragma once

#include <cstdint>
#include <vector>

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
    /**
     * Constructor. Generates all noise textures.
     */
    MilkdropNoise();

    /**
     * Destructor. Deletes the allocated noise textures.
     */
    ~MilkdropNoise();

    uint32_t* noise_lq{ nullptr }; //!< Low-quality (high frequency) 2D noise texture, 256x256 with zoom level 1
    uint32_t* noise_lq_lite{ nullptr }; //!< Low-quality (high frequency) 2D noise texture, 32x32 with zoom level 1
    uint32_t* noise_mq{ nullptr }; //!< Medium-quality (medium frequency) 2D noise texture, 256x256 with zoom level 4
    uint32_t* noise_hq{ nullptr }; //!< High-quality (low frequency) 2D noise texture, 256x256 with zoom level 8

    uint32_t* noise_lq_vol{ nullptr }; //!< Low-quality (high frequency) 3D noise texture, 32x32 with zoom level 1
    uint32_t* noise_hq_vol{ nullptr }; //!< High-quality (low frequency) 3D noise texture, 32x32 with zoom level 4

protected:

    /**
     * @brief Milkdrop 2D noise algorithm
     *
     * Creates a different, smoothed noise texture in each of the four color channels.
     *
     * @param size Texture size in pixels.
     * @param zoomFactor Zoom factor. Higher values give a more smoothed/interpolated look.
     * @param textureData A pointer to the data structure that will receive the texture data. Must have size² elements.
     */
    static void generate2D(int size, int zoomFactor, uint32_t** textureData);

    /**
     * @brief Milkdrop §D noise algorithm
     *
     * Creates a different, smoothed noise texture in each of the four color channels.
     *
     * @param size Texture size in pixels.
     * @param zoomFactor Zoom factor. Higher values give a more smoothed/interpolated look.
     * @param textureData A pointer to the data structure that will receive the texture data. Must have size³ elements.
     */
    static void generate3D(int size, int zoomFactor, uint32_t** textureData);

    static float fCubicInterpolate(float y0, float y1, float y2, float y3, float t);

    static uint32_t dwCubicInterpolate(uint32_t y0, uint32_t y1, uint32_t y2, uint32_t y3, float t);
};



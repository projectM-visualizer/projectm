#include "MilkdropNoise.hpp"

#include "projectM-opengl.h"

#include <chrono>
#include <random>

namespace libprojectM {
namespace Renderer {

auto MilkdropNoise::LowQuality() -> std::shared_ptr<Texture>
{
    GLuint texture{};
    {
        auto textureData = generate2D(256, 1);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GetPreferredInternalFormat(), GL_UNSIGNED_BYTE, textureData.data());
    }
    return std::make_shared<Texture>("noise_lq", texture, GL_TEXTURE_2D, 256, 256, false);
}

auto MilkdropNoise::LowQualityLite() -> std::shared_ptr<Texture>
{
    GLuint texture{};

    {
        auto textureData = generate2D(32, 1);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 32, 32, 0, GetPreferredInternalFormat(), GL_UNSIGNED_BYTE, textureData.data());
    }

    return std::make_shared<Texture>("noise_lq_lite", texture, GL_TEXTURE_2D, 32, 32, false);
}

auto MilkdropNoise::MediumQuality() -> std::shared_ptr<Texture>
{
    GLuint texture{};

    {
        auto textureData = generate2D(256, 4);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GetPreferredInternalFormat(), GL_UNSIGNED_BYTE, textureData.data());
    }
    return std::make_shared<Texture>("noise_mq", texture, GL_TEXTURE_2D, 256, 256, false);
}

auto MilkdropNoise::HighQuality() -> std::shared_ptr<Texture>
{
    GLuint texture{};

    {
        auto textureData = generate2D(256, 8);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GetPreferredInternalFormat(), GL_UNSIGNED_BYTE, textureData.data());
    }

    return std::make_shared<Texture>("noise_hq", texture, GL_TEXTURE_2D, 256, 256, false);
}

auto MilkdropNoise::LowQualityVolume() -> std::shared_ptr<Texture>
{
    GLuint texture{};

    {
        auto textureData = generate3D(32, 1);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_3D, texture);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, 32, 32, 32, 0, GetPreferredInternalFormat(), GL_UNSIGNED_BYTE, textureData.data());
    }

    return std::make_shared<Texture>("noisevol_lq", texture, GL_TEXTURE_3D, 32, 32, false);
}

auto MilkdropNoise::HighQualityVolume() -> std::shared_ptr<Texture>
{
    GLuint texture{};

    {
        auto textureData = generate3D(32, 4);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_3D, texture);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, 32, 32, 32, 0, GetPreferredInternalFormat(), GL_UNSIGNED_BYTE, textureData.data());
    }

    return std::make_shared<Texture>("noisevol_hq", texture, GL_TEXTURE_3D, 32, 32, false);
}

auto MilkdropNoise::GetPreferredInternalFormat() -> int
{
#ifndef USE_GLES
    // We use GL_BGRA, as this is the best general-use format according to Khronos.
    return GL_BGRA;
#else
    // GLES only supports GL_RGB and GL_RGBA, so we always use the latter.
    return GL_RGBA;
#endif
}

auto MilkdropNoise::generate2D(int size, int zoomFactor) -> std::vector<uint32_t>
{
    uint32_t randomSeed = static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine randomGenerator(randomSeed);
    std::uniform_int_distribution<int> randomDistribution(0, INT32_MAX);

    std::vector<uint32_t> textureData;
    textureData.resize(size * size);

    // write to the bits...
    auto dst = textureData.data();
    auto RANGE = (zoomFactor > 1) ? 216 : 256;
    for (auto y = 0; y < size; y++)
    {
        for (auto x = 0; x < size; x++)
        {
            dst[x] = (static_cast<uint32_t>((randomDistribution(randomGenerator) % RANGE) + RANGE / 2) << 24) |
                     (static_cast<uint32_t>((randomDistribution(randomGenerator) % RANGE) + RANGE / 2) << 16) |
                     (static_cast<uint32_t>((randomDistribution(randomGenerator) % RANGE) + RANGE / 2) << 8) |
                     (static_cast<uint32_t>((randomDistribution(randomGenerator) % RANGE) + RANGE / 2));
        }
        // swap some pixels randomly, to improve 'randomness'
        for (auto x = 0; x < size; x++)
        {
            auto x1 = randomDistribution(randomGenerator) % size;
            auto x2 = randomDistribution(randomGenerator) % size;
            auto temp = dst[x2];
            dst[x2] = dst[x1];
            dst[x1] = temp;
        }
        dst += size;
    }

    // smoothing
    if (zoomFactor > 1)
    {
        dst = textureData.data();

        // first go ACROSS, blending cubically on X, but only on the main lines.
        for (auto y = 0; y < size; y += zoomFactor)
        {
            for (auto x = 0; x < size; x++)
            {
                if (x % zoomFactor)
                {
                    auto base_x = (x / zoomFactor) * zoomFactor + size;
                    auto base_y = y * size;
                    auto y0 = dst[base_y + ((base_x - zoomFactor) % size)];
                    auto y1 = dst[base_y + ((base_x) % size)];
                    auto y2 = dst[base_y + ((base_x + zoomFactor) % size)];
                    auto y3 = dst[base_y + ((base_x + zoomFactor * 2) % size)];

                    auto t = static_cast<float>(x % zoomFactor) / static_cast<float>(zoomFactor);

                    auto result = dwCubicInterpolate(y0, y1, y2, y3, t);

                    dst[y * size + x] = result;
                }
            }
        }

        // next go down, doing cubic interp along Y, on every line.
        for (auto x = 0; x < size; x++)
        {
            for (auto y = 0; y < size; y++)
            {
                if (y % zoomFactor)
                {
                    auto base_y = (y / zoomFactor) * zoomFactor + size;
                    auto y0 = dst[((base_y - zoomFactor) % size) * size + x];
                    auto y1 = dst[((base_y) % size) * size + x];
                    auto y2 = dst[((base_y + zoomFactor) % size) * size + x];
                    auto y3 = dst[((base_y + zoomFactor * 2) % size) * size + x];

                    auto t = static_cast<float>(y % zoomFactor) / static_cast<float>(zoomFactor);

                    auto result = dwCubicInterpolate(y0, y1, y2, y3, t);

                    dst[y * size + x] = result;
                }
            }
        }
    }

    return textureData;
}

auto MilkdropNoise::generate3D(int size, int zoomFactor) -> std::vector<uint32_t>
{
    uint32_t randomSeed = static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count());
    std::default_random_engine randomGenerator(randomSeed);
    std::uniform_int_distribution<int> randomDistribution(0, INT32_MAX);


    std::vector<uint32_t> textureData;
    textureData.resize(size * size * size);

    // write to the bits...
    int RANGE = (zoomFactor > 1) ? 216 : 256;
    for (auto z = 0; z < size; z++)
    {
        auto dst = (textureData.data()) + z * size * size;
        for (auto y = 0; y < size; y++)
        {
            for (auto x = 0; x < size; x++)
            {
                dst[x] = ((static_cast<uint32_t>(randomDistribution(randomGenerator) % RANGE) + RANGE / 2) << 24) |
                         ((static_cast<uint32_t>(randomDistribution(randomGenerator) % RANGE) + RANGE / 2) << 16) |
                         ((static_cast<uint32_t>(randomDistribution(randomGenerator) % RANGE) + RANGE / 2) << 8) |
                         ((static_cast<uint32_t>(randomDistribution(randomGenerator) % RANGE) + RANGE / 2));
            }
            // swap some pixels randomly, to improve 'randomness'
            for (auto x = 0; x < size; x++)
            {
                auto x1 = randomDistribution(randomGenerator) % size;
                auto x2 = randomDistribution(randomGenerator) % size;
                auto temp = dst[x2];
                dst[x2] = dst[x1];
                dst[x1] = temp;
            }
            dst += size;
        }
    }

    // smoothing
    if (zoomFactor > 1)
    {
        // first go ACROSS, blending cubically on X, but only on the main lines.
        auto dst = textureData.data();
        for (auto z = 0; z < size; z += zoomFactor)
        {
            for (auto y = 0; y < size; y += zoomFactor)
            {
                for (auto x = 0; x < size; x++)
                {
                    if (x % zoomFactor)
                    {
                        auto base_x = (x / zoomFactor) * zoomFactor + size;
                        auto base_y = z * size + y * size;
                        auto y0 = dst[base_y + ((base_x - zoomFactor) % size)];
                        auto y1 = dst[base_y + ((base_x) % size)];
                        auto y2 = dst[base_y + ((base_x + zoomFactor) % size)];
                        auto y3 = dst[base_y + ((base_x + zoomFactor * 2) % size)];

                        auto t = static_cast<float>(x % zoomFactor) / static_cast<float>(zoomFactor);

                        auto result = dwCubicInterpolate(y0, y1, y2, y3, t);

                        dst[z * size + y * size + x] = result;
                    }
                }
            }
        }

        // next go down, doing cubic interp along Y, on the main slices.
        for (auto z = 0; z < size; z += zoomFactor)
        {
            for (auto x = 0; x < size; x++)
            {
                for (auto y = 0; y < size; y++)
                {
                    if (y % zoomFactor)
                    {
                        auto base_y = (y / zoomFactor) * zoomFactor + size;
                        auto base_z = z * size;
                        auto y0 = dst[((base_y - zoomFactor) % size) * size + base_z + x];
                        auto y1 = dst[((base_y) % size) * size + base_z + x];
                        auto y2 = dst[((base_y + zoomFactor) % size) * size + base_z + x];
                        auto y3 = dst[((base_y + zoomFactor * 2) % size) * size + base_z + x];

                        auto t = static_cast<float>(y % zoomFactor) / static_cast<float>(zoomFactor);

                        auto result = dwCubicInterpolate(y0, y1, y2, y3, t);

                        dst[y * size + base_z + x] = result;
                    }
                }
            }
        }

        // next go through, doing cubic interp along Z, everywhere.
        for (auto x = 0; x < size; x++)
        {
            for (auto y = 0; y < size; y++)
            {
                for (auto z = 0; z < size; z++)
                {
                    if (z % zoomFactor)
                    {
                        auto base_y = y * size;
                        auto base_z = (z / zoomFactor) * zoomFactor + size;
                        auto y0 = dst[((base_z - zoomFactor) % size) * size + base_y + x];
                        auto y1 = dst[((base_z) % size) * size + base_y + x];
                        auto y2 = dst[((base_z + zoomFactor) % size) * size + base_y + x];
                        auto y3 = dst[((base_z + zoomFactor * 2) % size) * size + base_y + x];

                        auto t = static_cast<float>(z % zoomFactor) / static_cast<float>(zoomFactor);

                        auto result = dwCubicInterpolate(y0, y1, y2, y3, t);

                        dst[z * size + base_y + x] = result;
                    }
                }
            }
        }
    }

    return textureData;
}

float MilkdropNoise::fCubicInterpolate(float y0, float y1, float y2, float y3, float t)
{
    auto t2 = t * t;
    auto a0 = y3 - y2 - y0 + y1;
    auto a1 = y0 - y1 - a0;
    auto a2 = y2 - y0;
    auto a3 = y1;

    return (a0 * t * t2 + a1 * t2 + a2 * t + a3);
}

uint32_t MilkdropNoise::dwCubicInterpolate(uint32_t y0, uint32_t y1, uint32_t y2, uint32_t y3, float t)
{
    uint32_t ret = 0;
    uint32_t shift = 0;
    for (auto i = 0; i < 4; i++)
    {
        auto f = fCubicInterpolate(
            static_cast<float>((y0 >> shift) & 0xFF) / 255.0f,
            static_cast<float>((y1 >> shift) & 0xFF) / 255.0f,
            static_cast<float>((y2 >> shift) & 0xFF) / 255.0f,
            static_cast<float>((y3 >> shift) & 0xFF) / 255.0f,
            t);
        if (f < 0)
        {
            f = 0;
        }
        if (f > 1)
        {
            f = 1;
        }
        ret |= ((uint32_t) (f * 255)) << shift;
        shift += 8;
    }
    return ret;
}

} // namespace Renderer
} // namespace libprojectM

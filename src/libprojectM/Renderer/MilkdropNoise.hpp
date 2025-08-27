#pragma once

#include <Renderer/Texture.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace libprojectM {
namespace Renderer {


class MilkdropNoise
{
public:
    MilkdropNoise() = delete;


    static auto LowQuality() -> std::shared_ptr<Texture>;


    static auto LowQualityLite() -> std::shared_ptr<Texture>;


    static auto MediumQuality() -> std::shared_ptr<Texture>;


    static auto HighQuality() -> std::shared_ptr<Texture>;


    static auto LowQualityVolume() -> std::shared_ptr<Texture>;


    static auto HighQualityVolume() -> std::shared_ptr<Texture>;

protected:
    static auto GetPreferredInternalFormat() -> int;


    static auto generate2D(int size, int zoomFactor) -> std::vector<uint32_t>;


    static auto generate3D(int size, int zoomFactor) -> std::vector<uint32_t>;

    static float fCubicInterpolate(float y0, float y1, float y2, float y3, float t);

    static uint32_t dwCubicInterpolate(uint32_t y0, uint32_t y1, uint32_t y2, uint32_t y3, float t);
};

}
}

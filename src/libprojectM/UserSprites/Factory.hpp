#pragma once

#include "UserSprites/Sprite.hpp"

#include <memory>
#include <string>

namespace libprojectM {
namespace UserSprites {

class Factory
{
public:
    Factory() = delete;

    static auto CreateSprite(const std::string& type) -> Sprite::Ptr;
};

} // namespace UserSprites
} // namespace libprojectM

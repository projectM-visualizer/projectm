#include "UserSprites/Factory.hpp"

#include "UserSprites/MilkdropSprite.hpp"

#include <Utils.hpp>

namespace libprojectM {
namespace UserSprites {

auto Factory::CreateSprite(const std::string& type) -> Sprite::Ptr
{
    const auto lowerCaseType = Utils::ToLower(type);

    if (lowerCaseType == "milkdrop")
    {
        return std::make_unique<MilkdropSprite>();
    }

    return {};
}


} // namespace UserSprites
} // namespace libprojectM

#pragma once

#include "UserSprites/Sprite.hpp"

#include <Renderer/RenderContext.hpp>

#include <cstdint>
#include <list>
#include <set>
#include <utility>
#include <vector>

namespace libprojectM {
namespace UserSprites {

class SpriteManager
{
public:
    using SpriteIdentifier = uint32_t;


    auto Spawn(const std::string& type,
               const std::string& spriteData,
               const Renderer::RenderContext& renderContext) -> SpriteIdentifier;


    void Draw(const Audio::FrameAudioData& audioData,
              const Renderer::RenderContext& renderContext,
              uint32_t outputFramebufferObject,
              Sprite::PresetList presets);


    void Destroy(SpriteIdentifier spriteIdentifier);


    void DestroyAll();


    auto ActiveSpriteCount() const -> uint32_t;


    auto ActiveSpriteIdentifiers() const -> std::vector<SpriteIdentifier>;


    void SpriteSlots(uint32_t slots);


    auto SpriteSlots() const -> uint32_t;

private:
    using SpriteIdPair = std::pair<SpriteIdentifier, Sprite::Ptr>;


    SpriteIdentifier GetLowestFreeIdentifier();

    uint32_t m_spriteSlots{16};
    std::set<SpriteIdentifier> m_spriteIdentifiers;
    std::list<SpriteIdPair> m_sprites;
};

}
}

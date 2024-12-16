#include "UserSprites/SpriteManager.hpp"

#include "UserSprites/Factory.hpp"

#include <Renderer/Shader.hpp>

#include <algorithm>

namespace libprojectM {
namespace UserSprites {

auto SpriteManager::Spawn(const std::string& type,
                          const std::string& spriteData,
                          const Renderer::RenderContext& renderContext) -> uint32_t
{
    // If user set the limit to zero, don't bother.
    if (m_spriteSlots == 0)
    {
        return 0;
    }

    auto sprite = Factory::CreateSprite(type);

    if (!sprite)
    {
        return 0;
    }

    try
    {
        sprite->Init(spriteData, renderContext);
    }
    catch (SpriteException& ex)
    {
        return 0;
    }
    catch (Renderer::ShaderException& ex)
    {
        return 0;
    }
    catch (...)
    {
        return 0;
    }

    auto spriteIdentifier = GetLowestFreeIdentifier();

    // Already at max sprites, destroy the oldest sprite to make room.
    if (m_sprites.size() == m_spriteSlots)
    {
        Destroy(m_sprites.front().first);
    }

    m_sprites.emplace_back(spriteIdentifier, std::move(sprite));
    m_spriteIdentifiers.insert(spriteIdentifier);

    return spriteIdentifier;
}

void SpriteManager::Draw(const Audio::FrameAudioData& audioData,
                         const Renderer::RenderContext& renderContext,
                         uint32_t outputFramebufferObject,
                         Sprite::PresetList presets)
{
    for (auto& idAndSprite : m_sprites) {
        idAndSprite.second->Draw(audioData, renderContext, outputFramebufferObject, presets);

        if (idAndSprite.second->Done())
        {
            Destroy(idAndSprite.first);
        }
    }
}

void SpriteManager::Destroy(SpriteIdentifier spriteIdentifier)
{
    if (m_spriteIdentifiers.find(spriteIdentifier) == m_spriteIdentifiers.end())
    {
        return;
    }

    m_spriteIdentifiers.erase(spriteIdentifier);
    m_sprites.remove_if([spriteIdentifier](const auto& idAndSprite) {
        return idAndSprite.first == spriteIdentifier;
    });
}

void SpriteManager::DestroyAll()
{
    m_spriteIdentifiers.clear();
    m_sprites.clear();
}

auto SpriteManager::ActiveSpriteCount() const -> uint32_t
{
    return m_sprites.size();
}

auto SpriteManager::ActiveSpriteIdentifiers() const -> std::vector<SpriteIdentifier>
{
    std::vector<SpriteIdentifier> identifierList;
    for (auto& idAndSprite : m_sprites) {
        identifierList.emplace_back(idAndSprite.first);
    }

    return identifierList;
}

void SpriteManager::SpriteSlots(uint32_t slots)
{
    m_spriteSlots = slots;

    // Remove excess sprites if limit was lowered
    while (m_sprites.size() > slots)
    {
        m_spriteIdentifiers.erase(m_sprites.front().first);
        m_sprites.pop_front();
    }
}

auto SpriteManager::SpriteSlots() const -> uint32_t
{
    return m_spriteSlots;
}

auto SpriteManager::GetLowestFreeIdentifier() -> SpriteIdentifier
{
    SpriteIdentifier lowestId = 0;

    for (const auto& spriteId : m_spriteIdentifiers)
    {
        if (spriteId > lowestId + 1)
        {
            return lowestId + 1;
        }

        lowestId = spriteId;
    }

    return lowestId + 1;
}

} // namespace UserSprites
} // namespace libprojectM

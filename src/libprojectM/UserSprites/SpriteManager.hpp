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

    /**
     * @brief Spawns a new sprite.
     * @param type The type name of the sprite.
     * @param spriteData The sprite code/data, depending on the type.
     * @return A unique, non-zero identifier if the sprite was successfully spawned, or zero if an error occurred.
     */
    auto Spawn(const std::string& type,
               const std::string& spriteData,
               const Renderer::RenderContext& renderContext) -> SpriteIdentifier;

    /**
     * @brief Draws all active sprites.
     * @param audioData The frame audio data structure.
     * @param renderContext The current frame's rendering context.
     * @param outputFramebufferObject Framebuffer object the sprite will be rendered to.
     * @param presets The active preset, plus eventually the transitioning one. Used for the burn-in effect.
     */
    void Draw(const Audio::FrameAudioData& audioData,
              const Renderer::RenderContext& renderContext,
              uint32_t outputFramebufferObject,
              Sprite::PresetList presets);

    /**
     * @brief Destroys a single active sprite.
     *
     * If spriteIdentifier is invalid, no action will be taken.
     *
     * @param spriteIdentifier The identifier of the sprite to destroy.
     */
    void Destroy(SpriteIdentifier spriteIdentifier);

    /**
     * @brief Destroys all active sprites.
     *
     * Sprites will be removed when drawing the next frame.
     */
    void DestroyAll();

    /**
     * @brief Returns the current number of active sprites.
     * @return The current number of active sprites.
     */
    auto ActiveSpriteCount() const -> uint32_t;

    /**
     * @brief Returns a set of identifiers for all active sprites.
     * @return A vector with the identifiers of all active sprites.
     */
    auto ActiveSpriteIdentifiers() const -> std::vector<SpriteIdentifier>;

    /**
     * @brief Sets the number of available sprite slots, e.g. the number of concurrently active sprites.
     * If there are more active sprites than the newly set limit, the oldest sprites will be destroyed
     * in order until the new limit is matched.
     * @param slots The maximum number of active sprites. 0 disables user sprites. Default is 16.
     */
    void SpriteSlots(uint32_t slots);

    /**
     * @brief Returns the currently set maximum number of active sprites.
     * @return The maximum number of active sprites.
     */
    auto SpriteSlots() const -> uint32_t;

private:
    using SpriteIdPair = std::pair<SpriteIdentifier, Sprite::Ptr>;

    /**
     * Returns the lowest free ID, starting at 1.
     * @return The lowest available/unused sprite ID.
     */
    SpriteIdentifier GetLowestFreeIdentifier();

    uint32_t m_spriteSlots{16};                     //!< Max number of active sprites.
    std::set<SpriteIdentifier> m_spriteIdentifiers; //!< Set to keep track of ordered sprite IDs.
    std::list<SpriteIdPair> m_sprites;              //!< Active sprites with their identifier.
};

} // namespace UserSprites
} // namespace libprojectM

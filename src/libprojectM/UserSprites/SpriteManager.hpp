#pragma once

#include "Sprite.hpp"

#include <cstdint>
#include <list>

namespace libprojectM {
namespace UserSprites {

class SpriteManager
{
public:
    SpriteManager();

    virtual ~SpriteManager();

    /**
     * @brief Spawns a new sprite.
     */
    void Spawn();

    /**
     * @brief Draws all active sprites.
     */
    void Draw();

    /**
     * @brief Queues a single active sprite for destruction.
     *
     * The index is the 0-based index of the sprite in the list of currently active sprites, with 0 being the
     * oldest sprite being displayed. Sprites marked for destruction will be removed when drawing the next frame.
     *
     * If spriteIndex is larger than the maximum index available, no action is taken.
     *
     * @param spriteIndex The slot index of the sprite to destroy.
     */
    void Destroy(uint32_t spriteIndex);

    /**
     * @brief Marks all active sprites for destruction.
     *
     * Sprites will be removed when drawing the next frame.
     */
    void DestroyAll();

    /**
     * @brief Returns the current number of active sprites.
     * @return The current number of active sprites.
     */
    auto ActiveSprites() const -> uint32_t;

    /**
     * @brief Sets the numer of available sprite slots, e.g. the number of concurrently active sprites.
     * If there are more active sprites than the newly set limit, the oldest sprites will be destroyed
     * when drawing the next frame until the new limit it met.
     * @param slots The maximum number of active sprites. 0 disables user sprites. Default is 16.
     */
    void SpriteSlots(uint32_t slots);

    /**
     * @brief Returns the currently set maximum number of active sprites.
     * @return The maximum number of active sprites.
     */
    auto SpriteSlots() const -> uint32_t;

private:
    uint32_t m_spriteSlots{16};       //!< Max number of active sprites.
    std::list<Sprite::Ptr> m_sprites; //!< Active sprites.
};

} // namespace UserSprites
} // namespace libprojectM

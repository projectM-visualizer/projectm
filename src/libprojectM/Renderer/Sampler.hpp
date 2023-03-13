/**
* @file Sampler.hpp
* @brief Defines a class to hold a texture sampling descriptor.
*/
#pragma once

#include "projectM-opengl.h"

#include <memory>

/**
 * @brief Stores a single texture sampler.
 *
 * A sampler is used to provide information about how to wrap and filter texels
 * when sampling them from shaders. One texture can have several samplers with
 * different modes.
 *
 * Currently, each texture has its own sampler instances. It also uses the same filter mode
 * for min and max. As projectM doesn't use mipmaps yet it makes no real difference.
 */
class Sampler
{
public:
    using Ptr = std::shared_ptr<Sampler>;

    Sampler() = delete;
    Sampler(const Sampler&) = delete;
    auto operator=(const Sampler&) -> Sampler& = delete;

    /**
     * @brief Constructor. Creates a new sampler with the given modes.
     * @param wrapMode
     * @param filterMode
     */
    Sampler(GLint wrapMode, GLint filterMode);

    Sampler(Sampler&& other) = default;
    auto operator=(Sampler&& other) -> Sampler& = default;

    ~Sampler();

    /**
     * @brief Binds the sampler to the given texture unit.
     * @param unit The texture unit to bind the current sampler to.
     */
    void Bind(GLuint unit) const;

    /**
     * @brief Unbinds a previously bound sampler from the given texture unit.
     * @param unit The texture unit to unbind the sampler from.
     */
    static void Unbind(GLuint unit);

    /**
     * @brief Returns the sampler wrap mode.
     * @return The sampler texture wrap mode.
     */
    auto WrapMode() const -> GLint;

    /**
     * @brief Sets a new wrap mode for this sampler.
     * @param wrapMode the new wrap mode.
     */
    void WrapMode(GLint wrapMode);

    /**
     * @brief Returns the sampler filter mode.
     * @return The sampler filter mode.
     */
    auto FilterMode() const -> GLint;

    /**
     * @brief Sets a new filter mode for this sampler.
     * @param filterMode the new filter mode.
     */
    void FilterMode(GLint filterMode);

private:
    GLuint m_samplerId{0}; //!< the OpenGL Sampler name/ID.
    GLint m_wrapMode{0}; //!< The texture wrap mode.
    GLint m_filterMode{0}; //!< The texture filtering mode.
};
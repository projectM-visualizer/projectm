#pragma once

#include "TextureAttachment.hpp"

#include "projectM-opengl.h"

#include <map>
#include <vector>

/**
 * @brief A framebuffer class holding one or more framebuffer objects.
 *
 * Framebuffers act as both render targets and sampling sources. This class holds one or more of those
 * objects, either used to store current and previous frame images or sub images for multi-stage
 * rendering.
 *
 * Each framebuffer can have multiple color attachments (at least up to 8), one depth buffer,
 * one stencil buffer and one depth stencil buffer.
 *
 * All framebuffers and their attachments will share the same size.
 */
class Framebuffer
{
public:
    /**
     * @brief Creates a new framebuffer object with one framebuffer.
     */
    Framebuffer();

    /**
     * @brief Creates a new framebuffer object with a given number of framebuffers.
     * @param framebufferCount The number of framebuffers to create. Must be at least 1.
     */
    explicit Framebuffer(int framebufferCount);

    /**
     * @brief Destroys the framebuffer object and all attachments.
     */
    ~Framebuffer();

    /**
     * @brief Returns the number of framebuffers in this object.
     * @return The number of framebuffers in this object.
     */
    int Count() const;

    /**
     * @brief Binds the given index as the current read/write framebuffer.
     * @param framebufferIndex The framebuffer index.
     */
    void Bind(int framebufferIndex);

    /**
     * @brief Binds the given index as the current read framebuffer.
     * @param framebufferIndex The framebuffer index.
     */
    void BindRead(int framebufferIndex);

    /**
     * @brief Binds the given index as the current write/draw framebuffer.
     * @param framebufferIndex The framebuffer index.
     */
    void BindDraw(int framebufferIndex);

    /**
     * @brief Sets the framebuffer texture size.
     *
     * This will bind the framebuffers and reallocate all attachment textures, creating new
     * textures with the given size. The default framebuffer is bound after the call is finished.
     * If either width or height is zero or both equal the the current size, the framebuffer contents
     * won't be changed.
     * @param width The width of the framebuffer.
     * @param height The height of the framebuffer.
     */
    void SetSize(int width, int height);

    /**
     * @brief Adds a new color attachment to the framebuffer.
     * The texture is always created in RGBA format.
     * @param index The index of the attachment, at least indices 0-7 are guaranteed to be available.
     */
    void CreateColorAttachment(int framebufferIndex, int attachmentIndex);

    /**
     * @brief Adds a depth attachment to the framebuffer.
     */
    void CreateDepthAttachment(int framebufferIndex);

    /**
     * @brief Adds a stencil buffer attachment to the framebuffer.
     */
    void CreateStencilAttachment(int framebufferIndex);

    /**
     * @brief Adds a depth stencil buffer attachment to the framebuffer.
     */
    void CreateDepthStencilAttachment(int framebufferIndex);

private:
    std::vector<unsigned int> m_framebufferIds{}; //!< The framebuffer IDs returned by OpenGL
    std::map<int, std::map<GLenum, TextureAttachment>> m_attachments; //!< Framebuffer texture attachments.

    int m_width{}; //!< Framebuffers texture width
    int m_height{}; //!< Framebuffers texture height.
};

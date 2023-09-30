/**
* @file Framebuffer.hpp
* @brief Defines a class to hold one or more render framebuffers.
*/
#pragma once

#include "TextureAttachment.hpp"

#include "projectM-opengl.h"

#include <map>
#include <vector>

/**
 * @brief A framebuffer class holding one or more framebuffer objects.
 *
 * <p>Framebuffers act as both render targets and sampling sources. This class holds one or more of those
 * objects, either used to store current and previous frame images or sub images for multi-stage
 * rendering.</p>
 *
 * <p>Each framebuffer can have multiple color attachments (at least up to 8), one depth buffer,
 * one stencil buffer and one depth stencil buffer.</p>
 *
 * <p>All framebuffers and their attachments will share the same size.</p>
 *
 * <p>Draw buffers will be configured in this order for each framebuffer:</p>
 *
 * <ol>
 * <li>Color attachments in ascending order</li>
 * <li>Depth</li>
 * <li>Stencil</li>
 * <li>Depth Stencil</li>
 * </ol>
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
     * @brief Binds the default framebuffer for both reading and writing.
     */
    static void Unbind();

    /**
     * @brief Sets the framebuffer texture size.
     *
     * This will bind the framebuffers and reallocate all attachment textures, creating new
     * textures with the given size. The default framebuffer is bound after the call is finished.
     * If either width or height is zero or both equal the the current size, the framebuffer contents
     * won't be changed.
     * @param width The width of the framebuffer.
     * @param height The height of the framebuffer.
     * @return true if the framebuffer was resized, false if it's contents remain unchanged.
     */
    bool SetSize(int width, int height);

    /**
     * @brief Returns a texture attachment object.
     * @param framebufferIndex The framebuffer index.
     * @param type The attachment type to retrieve.
     * @param attachmentIndex The index of the color attachment, at least indices 0-7 are guaranteed
     *                        to be available. Ignored for non-color attachments.
     * @return The requested attachment or nullptr if there is no attachment in the requested slot.
     */
    auto GetAttachment(int framebufferIndex, TextureAttachment::AttachmentType type, int attachmentIndex = 0) const -> std::shared_ptr<TextureAttachment>;

    /**
     * @brief Sets a texture attachment slot to the given object.
     * Sets the read/write FBOs to the previously used ones in this instance. If a different
     * Framebuffer instance was used to read or draw, it must be bound again explicitly after this call.
     * @param framebufferIndex The framebuffer index.
     * @param attachmentIndex The index of the color attachment, at least indices 0-7 are guaranteed
     *                        to be available. Ignored for non-color attachments.
     * @param attachment The attachment to add to the framebuffer.
     */
    void SetAttachment(int framebufferIndex, int attachmentIndex, const std::shared_ptr<TextureAttachment>& attachment);

    /**
     * @brief Adds a new color attachment to the framebuffer.
     * The texture is always created in RGBA format.
     * @param framebufferIndex The framebuffer index.
     * @param attachmentIndex The index of the attachment, at least indices 0-7 are guaranteed to be available.
     */
    void CreateColorAttachment(int framebufferIndex, int attachmentIndex);

    /**
     * @brief Adds a new color attachment to the framebuffer with the specified format.
     * @param framebufferIndex The framebuffer index.
     * @param index The index of the attachment, at least indices 0-7 are guaranteed to be available.
     * @param internalFormat OpenGL internal format, e.g. GL_RGBA8
     * @param format OpenGL color format, e.g. GL_RGBA
     * @param type OpenGL component storage type, e.g. GL_UNSIGNED _BYTE
     */
    void CreateColorAttachment(int framebufferIndex, int attachmentIndex,
                               GLint internalFormat, GLenum format, GLenum type);

    /**
     * Removes the color attachment from the given slot, if there is any assigned.
     * Sets the read/write FBOs to the previously used ones in this instance. If a different
     * Framebuffer instance was used to read or draw, it must be bound again explicitly after this call.
     * @param framebufferIndex The framebuffer index.
     * @param attachmentIndex The index of the attachment to remove, at least indices 0-7 are guaranteed to be available.
     */
    void RemoveColorAttachment(int framebufferIndex, int attachmentIndex);

    /**
     * @brief Returns the texture ID of the given framebuffer and color attachment.
     * @param framebufferIndex The framebuffer index.
     * @param attachmentIndex The index of the attachment to return the texture for.
     * @return A shared pointer to the texture or nullptr if no texture is assigned.
     */
    auto GetColorAttachmentTexture(int framebufferIndex, int attachmentIndex) const -> std::shared_ptr<class Texture>;

    /**
     * @brief Adds a depth attachment to the framebuffer.
     * @param framebufferIndex The framebuffer index.
     */
    void CreateDepthAttachment(int framebufferIndex);

    /**
     * @brief Removes the depth attachment from the given framebuffer, if there is any assigned.
     * Sets the read/write FBOs to the previously used ones in this instance. If a different
     * Framebuffer instance was used to read or draw, it must be bound again explicitly after this call.
     * @param framebufferIndex The framebuffer index.
     */
    void RemoveDepthAttachment(int framebufferIndex);

    /**
     * @brief Adds a stencil buffer attachment to the framebuffer.
     * @param framebufferIndex The framebuffer index.
     */
    void CreateStencilAttachment(int framebufferIndex);

    /**
     * @brief Removes the stencil attachment from the given framebuffer, if there is any assigned.
     * Sets the read/write FBOs to the previously used ones in this instance. If a different
     * Framebuffer instance was used to read or draw, it must be bound again explicitly after this call.
     * @param framebufferIndex The framebuffer index.
     */
    void RemoveStencilAttachment(int framebufferIndex);

    /**
     * @brief Adds a depth stencil buffer attachment to the framebuffer.
     * @param framebufferIndex The framebuffer index.
     */
    void CreateDepthStencilAttachment(int framebufferIndex);

    /**
     * @brief Removes the depth stencil attachment from the given framebuffer, if there is any assigned.
     * Sets the read/write FBOs to the previously used ones in this instance. If a different
     * Framebuffer instance was used to read or draw, it must be bound again explicitly after this call.
     * @param framebufferIndex The framebuffer index.
     */
    void RemoveDepthStencilAttachment(int framebufferIndex);

    /**
     * @brief Sets the masked flag for a specific draw buffer.
     * This can be used to enable or disable rendering to specific color attachments.
     * With GLES 3.1 and lower, this will always mask all buffers.
     * @param bufferIndex The index of the buffer to set the mask flag on.
     * @param masked true if the attachment should be masked, false if not.
     */
    void MaskDrawBuffer(int bufferIndex, bool masked);

private:
    /**
     * @brief Updates the draw buffer list for the fragment shader outputs of the given framebuffer.
     * Note that when calling this function, the framebuffer must already be bound.
     * @param framebufferIndex The framebuffer index.
     */
    void UpdateDrawBuffers(int framebufferIndex);

    /**
     * @brief Removes the given attachment type from the framebuffer.
     * @param framebufferIndex The framebuffer index.
     * @param attachmentType The attachment type to remove.
     */
    void RemoveAttachment(int framebufferIndex, GLenum attachmentType);

    using AttachmentsPerSlot = std::map<GLenum, std::shared_ptr<TextureAttachment>>;
    std::vector<unsigned int> m_framebufferIds{}; //!< The framebuffer IDs returned by OpenGL
    std::map<int, AttachmentsPerSlot> m_attachments; //!< Framebuffer texture attachments.

    int m_width{}; //!< Framebuffers texture width
    int m_height{}; //!< Framebuffers texture height.

    int m_readFramebuffer{}; //!< Index of the framebuffer currently being read.
    int m_drawFramebuffer{}; //!< Index of the framebuffer currently being drawn to.
};

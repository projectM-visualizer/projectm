/**
 * @file TextureAttachment.hpp
 * @brief Defines a class to hold a framebuffer texture attachment.
 */
#pragma once

#include "Texture.hpp"

#include "projectM-opengl.h"

#include <memory>

/**
 * @brief Framebuffer texture attachment. Stores the texture and attachment type.
 */
class TextureAttachment
{
public:
    /**
     * Framebuffer attachment type this attachment is used for.
     */
    enum class AttachmentType
    {
        Color,       //!< Color texture attachment. Uses GL_RGBA format.
        Depth,       //!< Depth buffer attachment. Uses GL_DEPTH_COMPONENT with GL_FLOAT pixel format.
        Stencil,     //!< Stencil buffer attachment. Uses GL_STENCIL_INDEX with GL_UNSIGNED_BYTE pixel format.
        DepthStencil //!< Depth stencil buffer attachment. Uses GL_DEPTH_STENCIL with GL_UNSIGNED_INT_24_8 pixel format.
    };

    TextureAttachment() = delete;
    TextureAttachment(const TextureAttachment&) = delete;
    auto operator=(const TextureAttachment&) -> TextureAttachment& = delete;

    /**
     * @brief Creates a new texture attachment with the given type and size.
     * @param attachmentType The attachment type.
     * @param width The width of the texture in pixels.
     * @param height The height of the texture in pixels.
     */
    explicit TextureAttachment(AttachmentType attachmentType, int width, int height);

    /**
     * @brief Creates a new 2D color texture attachment with the given format and size.
     * Must have the same size as the framebuffer.
     * @param internalFormat OpenGL internal format, e.g. GL_RGBA8
     * @param format OpenGL color format, e.g. GL_RGBA
     * @param type OpenGL component storage type, e.g. GL_UNSIGNED _BYTE
     * @param width The width of the texture in pixels.
     * @param height The height of the texture in pixels.
     */
    explicit TextureAttachment(GLint internalFormat, GLenum format, GLenum type, int width, int height);

    TextureAttachment(TextureAttachment&& other) = default;
    auto operator=(TextureAttachment&& other) -> TextureAttachment& = default;

    ~TextureAttachment() = default;

    /**
     * @brief Returns the attachment type.
     * @return The attachment type value.
     */
    auto Type() const -> AttachmentType;

    /**
     * @brief Returns the internal texture of this attachment.
     * @return The texture instance of this attachment.
     */
    auto Texture() const -> std::shared_ptr<class Texture>;

    /**
     * @brief Replaces the current internal texture with the given one.
     * Texture size is not changed.
     * @param texture
     */
    void Texture(const std::shared_ptr<class Texture>& texture);

    /**
     * @brief Sets a new texture size.
     * Effectively reallocates the texture.
     * @param width The new width.
     * @param height The new height.
     */
    void SetSize(int width, int height);

private:
    /**
     * @brief Replaces the current texture with a new one, e.g. if the framebuffer was resized.
     * @param width The new texture width.
     * @param height The new texture height.
     */
    void ReplaceTexture(int width, int height);

    AttachmentType m_attachmentType{AttachmentType::Color};                      //!< Attachment type of this texture.
    std::shared_ptr<class Texture> m_texture{std::make_shared<class Texture>()}; //!< The texture.

    GLint m_internalFormat{}; //!< OpenGL internal format, e.g. GL_RGBA8
    GLenum m_format{};        //!< OpenGL color format, e.g. GL_RGBA
    GLenum m_type{};          //!< OpenGL component storage type, e.g. GL_UNSIGNED _BYTE
};

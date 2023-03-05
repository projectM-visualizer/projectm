#pragma once

#include "projectM-opengl.h"

#include <memory>

/**
 * @brief Framebuffer texture attachment. Stores the texture and attachment type.
 */
class TextureAttachment
{
public:
    enum class AttachmentType
    {
        Color,
        Depth,
        Stencil,
        DepthStencil
    };

    TextureAttachment() = delete;

    explicit TextureAttachment(AttachmentType attachmentType, int width, int height);

    ~TextureAttachment();

    auto Type() const -> AttachmentType;

    auto TextureId() const -> GLuint;

    /**
     * @brief Sets a new texture size.
     * Effectively reallocates the texture.
     * @param width The new width.
     * @param height The new height.
     */
    void SetSize(int width, int height);

private:
    void ReplaceTexture(int width, int height);

    AttachmentType m_attachmentType{AttachmentType::Color}; //!< Attachment type of this texture.
    GLuint m_textureId{}; //!< The Texture ID.
};

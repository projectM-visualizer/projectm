#include "TextureAttachment.hpp"

// OpenGL ES might not define this constant in its headers, e.g. in the iOS and Emscripten SDKs.
#ifndef GL_STENCIL_INDEX
#define GL_STENCIL_INDEX 0x1901
#endif

TextureAttachment::TextureAttachment(AttachmentType attachment, int width, int height)
    : m_attachmentType(attachment)
{
    if (width > 0 && height > 0)
    {
        ReplaceTexture(width, height);
    }
}

TextureAttachment::TextureAttachment(GLint internalFormat, GLenum format, GLenum type, int width, int height)
    : m_attachmentType(AttachmentType::Color)
    , m_internalFormat(internalFormat)
    , m_format(format)
    , m_type(type)
{
    if (width > 0 && height > 0)
    {
        ReplaceTexture(width, height);
    }
}

auto TextureAttachment::Type() const -> TextureAttachment::AttachmentType
{
    return m_attachmentType;
}

auto TextureAttachment::Texture() const -> std::shared_ptr<class Texture>
{
    return m_texture;
}

void TextureAttachment::Texture(const std::shared_ptr<struct Texture>& texture)
{
    m_texture = texture;
}

void TextureAttachment::SetSize(int width, int height)
{
    if (width > 0 && height > 0)
    {
        ReplaceTexture(width, height);
    }
    else
    {
        m_texture = std::make_unique<class Texture>();
    }
}

void TextureAttachment::ReplaceTexture(int width, int height)
{
    GLint internalFormat;
    GLint textureFormat;
    GLenum pixelFormat;

    // Don't replace if size hasn't changed
    if (m_texture->Width() == width && m_texture->Height() == height)
    {
        return;
    }

    switch(m_attachmentType)
    {
        case AttachmentType::Color:
            if (m_internalFormat == 0)
            {
                internalFormat = GL_RGBA;
                textureFormat = GL_RGBA;
                pixelFormat = GL_UNSIGNED_BYTE;
            }
            else
            {
                internalFormat = m_internalFormat;
                textureFormat = m_format;
                pixelFormat = m_type;
            }
            break;
        case AttachmentType::Depth:
            internalFormat = GL_DEPTH_COMPONENT16;
            textureFormat = GL_DEPTH_COMPONENT;
            pixelFormat = GL_FLOAT;
            break;
        case AttachmentType::Stencil:
            internalFormat = GL_STENCIL_INDEX8;
            textureFormat = GL_STENCIL_INDEX;
            pixelFormat = GL_UNSIGNED_BYTE;
            break;
        case AttachmentType::DepthStencil:
            internalFormat = GL_DEPTH24_STENCIL8;
            textureFormat = GL_DEPTH_STENCIL;
            pixelFormat = GL_UNSIGNED_INT_24_8;
            break;

        default:
            return;
    }

    m_texture.reset();

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, textureFormat, pixelFormat, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_texture = std::make_shared<class Texture>("", textureId, GL_TEXTURE_2D, width, height, false);
}

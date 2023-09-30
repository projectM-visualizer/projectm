#include "Framebuffer.hpp"

#include <cassert>

Framebuffer::Framebuffer()
{
    m_framebufferIds.resize(1);
    glGenFramebuffers(1, m_framebufferIds.data());
    m_attachments.emplace(0, AttachmentsPerSlot());
}

Framebuffer::Framebuffer(int framebufferCount)
{
    m_framebufferIds.resize(framebufferCount);
    glGenFramebuffers(framebufferCount, m_framebufferIds.data());
    for (int index = 0; index < framebufferCount; index++)
    {
        m_attachments.emplace(index, AttachmentsPerSlot());
    }
}

Framebuffer::~Framebuffer()
{
    if (!m_framebufferIds.empty())
    {
        // Delete attached textures first
        m_attachments.clear();

        glDeleteFramebuffers(static_cast<int>(m_framebufferIds.size()), m_framebufferIds.data());
        m_framebufferIds.clear();
    }
}

auto Framebuffer::Count() const -> int
{
    return static_cast<int>(m_framebufferIds.size());
}

void Framebuffer::Bind(int framebufferIndex)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIds.at(framebufferIndex));

    m_readFramebuffer = m_drawFramebuffer = framebufferIndex;
}

void Framebuffer::BindRead(int framebufferIndex)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebufferIds.at(framebufferIndex));

    m_readFramebuffer = framebufferIndex;
}

void Framebuffer::BindDraw(int framebufferIndex)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebufferIds.at(framebufferIndex));

    m_drawFramebuffer = framebufferIndex;
}

void Framebuffer::Unbind()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

bool Framebuffer::SetSize(int width, int height)
{
    if (width == 0 || height == 0 ||
        (width == m_width && height == m_height))
    {
        return false;
    }

    m_width = width;
    m_height = height;

    for (auto& attachments : m_attachments)
    {
        Bind(attachments.first);
        for (auto& texture : attachments.second)
        {
            texture.second->SetSize(width, height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, texture.first, GL_TEXTURE_2D, texture.second->Texture()->TextureID(), 0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

auto Framebuffer::GetAttachment(int framebufferIndex, TextureAttachment::AttachmentType type, int attachmentIndex) const -> std::shared_ptr<TextureAttachment>
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return {};
    }

    const auto& framebufferAttachments = m_attachments.at(framebufferIndex);
    GLenum textureType;

    switch (type)
    {
        case TextureAttachment::AttachmentType::Color:
            textureType = GL_COLOR_ATTACHMENT0 + attachmentIndex;
            break;
        case TextureAttachment::AttachmentType::Depth:
            textureType = GL_DEPTH_ATTACHMENT;
            break;
        case TextureAttachment::AttachmentType::Stencil:
            textureType = GL_STENCIL_ATTACHMENT;
            break;
        case TextureAttachment::AttachmentType::DepthStencil:
            textureType = GL_DEPTH_STENCIL_ATTACHMENT;
            break;
    }

    if (framebufferAttachments.find(textureType) == framebufferAttachments.end()) {
        return {};
    }

    return framebufferAttachments.at(textureType);
}

void Framebuffer::SetAttachment(int framebufferIndex, int attachmentIndex, const std::shared_ptr<TextureAttachment>& attachment)
{
    if (!attachment)
    {
        return;
    }

    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    GLenum textureType;

    switch (attachment->Type())
    {
        case TextureAttachment::AttachmentType::Color:
            textureType = GL_COLOR_ATTACHMENT0 + attachmentIndex;
            break;
        case TextureAttachment::AttachmentType::Depth:
            textureType = GL_DEPTH_ATTACHMENT;
            break;
        case TextureAttachment::AttachmentType::Stencil:
            textureType = GL_STENCIL_ATTACHMENT;
            break;
        case TextureAttachment::AttachmentType::DepthStencil:
            textureType = GL_DEPTH_STENCIL_ATTACHMENT;
            break;
    }
    m_attachments.at(framebufferIndex).insert({textureType, attachment});

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIds.at(framebufferIndex));

    if (m_width > 0 && m_height > 0)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, textureType, GL_TEXTURE_2D, attachment->Texture()->TextureID(), 0);
    }
    UpdateDrawBuffers(framebufferIndex);

    // Reset to previous read/draw buffers
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebufferIds.at(m_readFramebuffer));
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebufferIds.at(m_drawFramebuffer));
}

void Framebuffer::CreateColorAttachment(int framebufferIndex, int attachmentIndex)
{
    CreateColorAttachment(framebufferIndex, attachmentIndex, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
}

void Framebuffer::CreateColorAttachment(int framebufferIndex, int attachmentIndex, GLint internalFormat, GLenum format, GLenum type)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    auto textureAttachment = std::make_shared<TextureAttachment>(internalFormat, format, type, m_width, m_height);
    const auto texture = textureAttachment->Texture();
    m_attachments.at(framebufferIndex).insert({GL_COLOR_ATTACHMENT0 + attachmentIndex, std::move(textureAttachment)});

    Bind(framebufferIndex);
    if (m_width > 0 && m_height > 0)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_2D, texture->TextureID(), 0);
    }
    UpdateDrawBuffers(framebufferIndex);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::RemoveColorAttachment(int framebufferIndex, int attachmentIndex)
{
    RemoveAttachment(framebufferIndex,  GL_COLOR_ATTACHMENT0 + attachmentIndex);
}

auto Framebuffer::GetColorAttachmentTexture(int framebufferIndex, int attachmentIndex) const -> std::shared_ptr<class Texture>
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return {};
    }

    const auto& attachment = m_attachments.at(framebufferIndex);
    if (attachment.find(GL_COLOR_ATTACHMENT0 + attachmentIndex) == attachment.end())
    {
        return {};
    }

    return attachment.at(GL_COLOR_ATTACHMENT0 + attachmentIndex)->Texture();
}

void Framebuffer::CreateDepthAttachment(int framebufferIndex)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    auto textureAttachment = std::make_shared<TextureAttachment>(TextureAttachment::AttachmentType::Depth, m_width, m_height);
    const auto texture = textureAttachment->Texture();
    m_attachments.at(framebufferIndex).insert({GL_DEPTH_ATTACHMENT, std::move(textureAttachment)});

    Bind(framebufferIndex);
    if (m_width > 0 && m_height > 0)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->TextureID(), 0);
    }
    UpdateDrawBuffers(framebufferIndex);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::RemoveDepthAttachment(int framebufferIndex)
{
    RemoveAttachment(framebufferIndex, GL_DEPTH_ATTACHMENT);
}

void Framebuffer::CreateStencilAttachment(int framebufferIndex)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    auto textureAttachment = std::make_shared<TextureAttachment>(TextureAttachment::AttachmentType::Stencil, m_width, m_height);
    const auto texture = textureAttachment->Texture();
    m_attachments.at(framebufferIndex).insert({GL_STENCIL_ATTACHMENT, std::move(textureAttachment)});

    Bind(framebufferIndex);
    if (m_width > 0 && m_height > 0)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->TextureID(), 0);
    }
    UpdateDrawBuffers(framebufferIndex);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::RemoveStencilAttachment(int framebufferIndex)
{
    RemoveAttachment(framebufferIndex, GL_STENCIL_ATTACHMENT);
}

void Framebuffer::CreateDepthStencilAttachment(int framebufferIndex)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    auto textureAttachment = std::make_shared<TextureAttachment>(TextureAttachment::AttachmentType::DepthStencil, m_width, m_height);
    const auto texture = textureAttachment->Texture();
    m_attachments.at(framebufferIndex).insert({GL_DEPTH_STENCIL_ATTACHMENT, std::move(textureAttachment)});

    Bind(framebufferIndex);
    if (m_width > 0 && m_height > 0)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->TextureID(), 0);
    }
    UpdateDrawBuffers(framebufferIndex);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::RemoveDepthStencilAttachment(int framebufferIndex)
{
    RemoveAttachment(framebufferIndex, GL_DEPTH_STENCIL_ATTACHMENT);
}

void Framebuffer::MaskDrawBuffer(int bufferIndex, bool masked)
{
    // Invert the flag, as "true" means the color channel *will* be written.
    auto glMasked = static_cast<GLboolean>(!masked);
#if USE_GLES
    glColorMask(glMasked, glMasked, glMasked, glMasked);
#else
    glColorMaski(bufferIndex, glMasked, glMasked, glMasked, glMasked);
#endif
}

void Framebuffer::UpdateDrawBuffers(int framebufferIndex)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    const auto& attachments = m_attachments.at(framebufferIndex);

    std::vector<GLenum> buffers;

    bool hasDepthAttachment = false;
    bool hasStencilAttachment = false;
    bool hasDepthStencilAttachment = false;

    for (const auto& attachment : attachments)
    {
        if (attachment.first != GL_DEPTH_ATTACHMENT &&
            attachment.first != GL_STENCIL_ATTACHMENT &&
            attachment.first != GL_DEPTH_STENCIL_ATTACHMENT)
        {
            buffers.push_back(attachment.first);
        }
        else
        {
            switch (attachment.first)
            {
                case GL_DEPTH_ATTACHMENT:
                    hasDepthAttachment = true;
                    break;
                case GL_STENCIL_ATTACHMENT:
                    hasStencilAttachment = true;
                    break;
                case GL_DEPTH_STENCIL_ATTACHMENT:
                    hasDepthStencilAttachment = true;
                    break;
                default:
                    break;
            }
        }
    }

    if (hasDepthAttachment)
    {
        buffers.push_back(GL_DEPTH_ATTACHMENT);
    }
    if (hasStencilAttachment)
    {
        buffers.push_back(GL_STENCIL_ATTACHMENT);
    }
    if (hasDepthStencilAttachment)
    {
        buffers.push_back(GL_DEPTH_STENCIL_ATTACHMENT);
    }

    glDrawBuffers(static_cast<GLsizei>(buffers.size()), buffers.data());
}

void Framebuffer::RemoveAttachment(int framebufferIndex, GLenum attachmentType)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferIds.at(framebufferIndex));

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, 0, 0);
    UpdateDrawBuffers(framebufferIndex);

    m_attachments.at(framebufferIndex).erase(attachmentType);

    // Reset to previous read/draw buffers
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebufferIds.at(m_readFramebuffer));
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebufferIds.at(m_drawFramebuffer));
}

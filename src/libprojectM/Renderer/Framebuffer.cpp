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
}

void Framebuffer::BindRead(int framebufferIndex)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebufferIds.at(framebufferIndex));
}

void Framebuffer::BindDraw(int framebufferIndex)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebufferIds.at(framebufferIndex));
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
            texture.second.SetSize(width, height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, texture.first, GL_TEXTURE_2D, texture.second.Texture()->TextureID(), 0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
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

    TextureAttachment textureAttachment{internalFormat, format, type, m_width, m_height };
    const auto texture = textureAttachment.Texture();
    m_attachments.at(framebufferIndex).insert({GL_COLOR_ATTACHMENT0 + attachmentIndex, std::move(textureAttachment)});

    Bind(framebufferIndex);
    if (m_width > 0 && m_height > 0)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_TEXTURE_2D, texture->TextureID(), 0);
    }
    UpdateDrawBuffers(framebufferIndex);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    return attachment.at(GL_COLOR_ATTACHMENT0 + attachmentIndex).Texture();
}

void Framebuffer::CreateDepthAttachment(int framebufferIndex)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    TextureAttachment textureAttachment{TextureAttachment::AttachmentType::Depth, m_width, m_height};
    const auto texture = textureAttachment.Texture();
    m_attachments.at(framebufferIndex).insert({GL_DEPTH_ATTACHMENT, std::move(textureAttachment)});

    Bind(framebufferIndex);
    if (m_width > 0 && m_height > 0)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->TextureID(), 0);
    }
    UpdateDrawBuffers(framebufferIndex);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::CreateStencilAttachment(int framebufferIndex)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    TextureAttachment textureAttachment{TextureAttachment::AttachmentType::Stencil, m_width, m_height};
    const auto texture = textureAttachment.Texture();
    m_attachments.at(framebufferIndex).insert({GL_STENCIL_ATTACHMENT, std::move(textureAttachment)});

    Bind(framebufferIndex);
    if (m_width > 0 && m_height > 0)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->TextureID(), 0);
    }
    UpdateDrawBuffers(framebufferIndex);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::CreateDepthStencilAttachment(int framebufferIndex)
{
    if (framebufferIndex < 0 || framebufferIndex >= static_cast<int>(m_framebufferIds.size()))
    {
        return;
    }

    TextureAttachment textureAttachment{TextureAttachment::AttachmentType::DepthStencil, m_width, m_height};
    const auto texture = textureAttachment.Texture();
    m_attachments.at(framebufferIndex).insert({GL_DEPTH_STENCIL_ATTACHMENT, std::move(textureAttachment)});

    Bind(framebufferIndex);
    if (m_width > 0 && m_height > 0)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->TextureID(), 0);
    }
    UpdateDrawBuffers(framebufferIndex);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::MaskDrawBuffer(int bufferIndex, bool masked)
{
    // Invert the flag, as "true" means the color channel *will* be written.
    auto glMasked = static_cast<GLboolean>(!masked);
    glColorMaski(bufferIndex, glMasked, glMasked, glMasked, glMasked);
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

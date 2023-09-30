#include "FlipTexture.hpp"

FlipTexture::FlipTexture(const PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
{
    RenderItem::Init();

    m_framebuffer.CreateColorAttachment(0, 0);
}

void FlipTexture::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, x))); // Position
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, u))); // Texture coordinate

    std::array<RenderItem::TexturedPoint, 4> points;

    points[0].x = -1.0;
    points[0].y = 1.0;
    points[1].x = 1.0;
    points[1].y = 1.0;
    points[2].x = -1.0;
    points[2].y = -1.0;
    points[3].x = 1.0;
    points[3].y = -1.0;

    points[0].u = 0.0;
    points[0].v = 1.0;
    points[1].u = 1.0;
    points[1].v = 1.0;
    points[2].u = 0.0;
    points[2].v = 0.0;
    points[3].u = 1.0;
    points[3].v = 0.0;

    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points.data(), GL_STATIC_DRAW);
}

void FlipTexture::Draw(const std::shared_ptr<Texture>& originalTexture, const std::shared_ptr<Texture>& targetTexture)
{
    if (originalTexture == nullptr || originalTexture == targetTexture)
    {
        return;
    }

    UpdateTextureSize();

    if (m_viewportWidth == 0 || m_viewportHeight == 0)
    {
        return;
    }

    std::shared_ptr<Texture> internalTexture;

    m_framebuffer.Bind(0);

    // Draw from unflipped texture
    originalTexture->Bind(0);

    if (targetTexture)
    {
        internalTexture = m_framebuffer.GetColorAttachmentTexture(0, 0);
        m_framebuffer.GetAttachment(0, TextureAttachment::AttachmentType::Color, 0)->Texture(targetTexture);
    }

    Flip();

    // Rebind our internal texture.
    if (targetTexture)
    {
        m_framebuffer.GetAttachment(0, TextureAttachment::AttachmentType::Color, 0)->Texture(internalTexture);
    }

    Framebuffer::Unbind();
}

void FlipTexture::Draw(const std::shared_ptr<Texture>& originalTexture, Framebuffer& framebuffer, int framebufferIndex)
{
    if (originalTexture == nullptr || framebuffer.GetColorAttachmentTexture(framebufferIndex, 0) == nullptr)
    {
        return;
    }

    UpdateTextureSize();

    if (m_viewportWidth == 0 || m_viewportHeight == 0)
    {
        return;
    }

    m_framebuffer.Bind(0);

    // Draw from unflipped texture
    originalTexture->Bind(0);

    Flip();

    // Swap texture attachments
    auto tempAttachment = framebuffer.GetAttachment(framebufferIndex, TextureAttachment::AttachmentType::Color, 0);
    framebuffer.RemoveColorAttachment(framebufferIndex, 0);
    framebuffer.SetAttachment(framebufferIndex, 0, m_framebuffer.GetAttachment(0, TextureAttachment::AttachmentType::Color, 0));
    m_framebuffer.RemoveColorAttachment(0, 0);
    m_framebuffer.SetAttachment(0, 0, tempAttachment);

    Framebuffer::Unbind();
}

auto FlipTexture::FlippedTexture() -> std::shared_ptr<Texture>
{
    return m_framebuffer.GetColorAttachmentTexture(0, 0);
}

void FlipTexture::UpdateTextureSize()
{
    if (m_viewportWidth == m_presetState.renderContext.viewportSizeX &&
        m_viewportHeight == m_presetState.renderContext.viewportSizeY)
    {
        return;
    }

    m_viewportWidth = m_presetState.renderContext.viewportSizeX;
    m_viewportHeight = m_presetState.renderContext.viewportSizeY;

    m_framebuffer.SetSize(m_viewportWidth, m_viewportHeight);
}

void FlipTexture::Flip() const
{
    m_presetState.texturedShader.Bind();
    m_presetState.texturedShader.SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
    m_presetState.texturedShader.SetUniformInt("texture_sampler", 0);

    m_sampler.Bind(0);

    glBindVertexArray(m_vaoID);
    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0); // Color
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    Sampler::Unbind(0);
    Shader::Unbind();
}
#include "CopyTexture.hpp"

#include <array>
#include <iostream>

namespace libprojectM {
namespace Renderer {

#ifdef USE_GLES
static constexpr char ShaderVersion[] = "#version 300 es\n\n";
#else
static constexpr char ShaderVersion[] = "#version 330\n\n";
#endif

static constexpr char CopyTextureVertexShader[] = R"(
precision mediump float;

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coord;

out vec2 fragment_tex_coord;

uniform ivec2 flip;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    fragment_tex_coord = tex_coord;
    if (flip.x > 0)
    {
        fragment_tex_coord.s = 1.0 - fragment_tex_coord.s;
    }
    if (flip.y > 0)
    {
        fragment_tex_coord.t = 1.0 - fragment_tex_coord.t;
    }
}
)";

static constexpr char CopyTextureFragmentShader[] = R"(
precision mediump float;

in vec2 fragment_tex_coord;

uniform sampler2D texture_sampler;

out vec4 color;

void main(){
    color = texture(texture_sampler, fragment_tex_coord);
}

)";

CopyTexture::CopyTexture()
{
    RenderItem::Init();

    m_framebuffer.CreateColorAttachment(0, 0);

    std::string vertexShader(static_cast<const char*>(ShaderVersion));
    std::string fragmentShader(static_cast<const char*>(ShaderVersion));
    vertexShader.append(static_cast<const char*>(CopyTextureVertexShader));
    fragmentShader.append(static_cast<const char*>(CopyTextureFragmentShader));

    m_shader.CompileProgram(vertexShader, fragmentShader);
}

void CopyTexture::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, x))); // Position
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, u))); // Texture coordinate

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

void CopyTexture::Draw(const std::shared_ptr<class Texture>& originalTexture, bool flipVertical, bool flipHorizontal)
{
    if (originalTexture == nullptr)
    {
        return;
    }

    // Just bind the texture and draw it to the currently bound buffer.
    originalTexture->Bind(0);
    Copy(flipVertical, flipHorizontal);
}

void CopyTexture::Draw(const std::shared_ptr<class Texture>& originalTexture, const std::shared_ptr<class Texture>& targetTexture,
                       bool flipVertical, bool flipHorizontal)
{
    if (originalTexture == nullptr ||
        originalTexture->Empty() ||
        (targetTexture != nullptr && targetTexture->Empty()) ||
        originalTexture == targetTexture)
    {
        return;
    }

    if (targetTexture == nullptr)
    {
        UpdateTextureSize(originalTexture->Width(), originalTexture->Height());
    }
    else
    {
        UpdateTextureSize(targetTexture->Width(), targetTexture->Height());
    }

    if (m_width == 0 || m_height == 0)
    {
        return;
    }

    std::shared_ptr<class Texture> internalTexture;

    m_framebuffer.Bind(0);

    // Draw from unflipped texture
    originalTexture->Bind(0);

    if (targetTexture)
    {
        internalTexture = m_framebuffer.GetColorAttachmentTexture(0, 0);
        m_framebuffer.GetAttachment(0, TextureAttachment::AttachmentType::Color, 0)->Texture(targetTexture);
    }

    Copy(flipVertical, flipHorizontal);

    // Rebind our internal texture.
    if (targetTexture)
    {
        m_framebuffer.GetAttachment(0, TextureAttachment::AttachmentType::Color, 0)->Texture(internalTexture);
    }

    Framebuffer::Unbind();
}

void CopyTexture::Draw(const std::shared_ptr<class Texture>& originalTexture, Framebuffer& framebuffer, int framebufferIndex,
                       bool flipVertical, bool flipHorizontal)
{
    if (originalTexture == nullptr
        || originalTexture->Empty()
        || framebuffer.GetColorAttachmentTexture(framebufferIndex, 0) == nullptr
        || framebuffer.GetColorAttachmentTexture(framebufferIndex, 0)->Empty())
    {
        return;
    }

    UpdateTextureSize(framebuffer.Width(), framebuffer.Height());

    if (m_width == 0 || m_height == 0)
    {
        return;
    }

    m_framebuffer.Bind(0);

    // Draw from unflipped texture
    originalTexture->Bind(0);

    Copy(flipVertical, flipHorizontal);

    // Swap texture attachments
    auto tempAttachment = framebuffer.GetAttachment(framebufferIndex, TextureAttachment::AttachmentType::Color, 0);
    framebuffer.RemoveColorAttachment(framebufferIndex, 0);
    framebuffer.SetAttachment(framebufferIndex, 0, m_framebuffer.GetAttachment(0, TextureAttachment::AttachmentType::Color, 0));
    m_framebuffer.RemoveColorAttachment(0, 0);
    m_framebuffer.SetAttachment(0, 0, tempAttachment);

    Framebuffer::Unbind();
}

auto CopyTexture::Texture() -> std::shared_ptr<class Texture>
{
    return m_framebuffer.GetColorAttachmentTexture(0, 0);
}

void CopyTexture::UpdateTextureSize(int width, int height)
{
    if (m_width == width &&
        m_height == height)
    {
        return;
    }

    m_width = width;
    m_height = height;

    m_framebuffer.SetSize(m_width, m_height);
}

void CopyTexture::Copy(bool flipVertical, bool flipHorizontal) const
{
    m_shader.Bind();
    m_shader.SetUniformInt("texture_sampler", 0);
    m_shader.SetUniformInt2("flip", {flipHorizontal ? 1 : 0, flipVertical ? 1 : 0});

    m_sampler.Bind(0);

    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
    Sampler::Unbind(0);
    Shader::Unbind();
}

} // namespace Renderer
} // namespace libprojectM

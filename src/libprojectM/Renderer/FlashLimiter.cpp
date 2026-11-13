#include "Renderer/FlashLimiter.hpp"

#include <algorithm>

namespace libprojectM {
namespace Renderer {

#ifdef USE_GLES
static constexpr char ShaderVersion[] = "#version 300 es\n\n";
#else
static constexpr char ShaderVersion[] = "#version 330\n\n";
#endif

static constexpr char FlashLimiterVertexShader[] = R"(
precision highp float;

layout(location = 0) in vec2 position;
layout(location = 2) in vec2 tex_coord;

out vec2 fragment_tex_coord;

void main() {
    gl_Position = vec4(position, 0.0, 1.0);
    fragment_tex_coord = tex_coord;
}
)";

static constexpr char FlashLimiterFragmentShader[] = R"(
precision highp float;

in vec2 fragment_tex_coord;

uniform sampler2D current_frame;
uniform sampler2D previous_frame;
uniform float max_change;

out vec4 color;

void main() {
    vec3 current = texture(current_frame, fragment_tex_coord).rgb;
    vec3 previous = texture(previous_frame, fragment_tex_coord).rgb;
    color = vec4(previous + clamp(current - previous, -max_change, max_change), 1.0);
}
)";

FlashLimiter::FlashLimiter()
    : m_mesh(VertexBufferUsage::StaticDraw, false, true)
{
    // 10 bits per channel, so small changes per frame aren't rounded away.
    for (int framebufferIndex = 0; framebufferIndex < 3; framebufferIndex++)
    {
        m_framebuffer.CreateColorAttachment(framebufferIndex, 0, GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV);
    }

    m_mesh.SetRenderPrimitiveType(Mesh::PrimitiveType::TriangleStrip);
    m_mesh.SetVertexCount(4);
    m_mesh.Vertices().Set({{-1.0, -1.0},
                           {1.0, -1.0},
                           {-1.0, 1.0},
                           {1.0, 1.0}});
    m_mesh.UVs().Set({{0.0, 0.0},
                      {1.0, 0.0},
                      {0.0, 1.0},
                      {1.0, 1.0}});
    m_mesh.Indices().Set({0, 1, 2, 3});
    m_mesh.Update();
}

void FlashLimiter::BindInput(int width, int height)
{
    if (m_framebuffer.SetSize(width, height))
    {
        m_reset = true;
    }

    m_framebuffer.Bind(InputBuffer);
}

void FlashLimiter::Reset()
{
    m_reset = true;
}

void FlashLimiter::Draw(ShaderCache& shaderCache, float maxChange, uint32_t targetFramebufferObject)
{
    if (m_framebuffer.Width() == 0 || m_framebuffer.Height() == 0)
    {
        return;
    }

    if (m_reset)
    {
        m_framebuffer.Bind(m_previousBuffer);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        m_reset = false;
    }

    const int currentBuffer = m_previousBuffer == 1 ? 2 : 1;
    const float clampedChange = std::max(0.0f, std::min(1.0f, maxChange));

    auto shader = BindShader(shaderCache);

    m_framebuffer.GetColorAttachmentTexture(InputBuffer, 0)->Bind(0);
    m_sampler.Bind(0);
    m_framebuffer.GetColorAttachmentTexture(m_previousBuffer, 0)->Bind(1);
    m_sampler.Bind(1);

    glViewport(0, 0, m_framebuffer.Width(), m_framebuffer.Height());

    // Keep the limited frame for the next one...
    m_framebuffer.Bind(currentBuffer);
    DrawLimited(*shader, clampedChange);

    // ...and draw the same frame to the target.
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, static_cast<GLuint>(targetFramebufferObject));
    DrawLimited(*shader, clampedChange);

    m_previousBuffer = currentBuffer;

    Sampler::Unbind(0);
    Sampler::Unbind(1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    Mesh::Unbind();
    Shader::Unbind();
}

void FlashLimiter::DrawLimited(Shader& shader, float maxChange)
{
    shader.SetUniformInt("current_frame", 0);
    shader.SetUniformInt("previous_frame", 1);
    shader.SetUniformFloat("max_change", maxChange);
    m_mesh.Draw();
}

auto FlashLimiter::BindShader(ShaderCache& shaderCache) -> std::shared_ptr<Shader>
{
    auto shader = m_shader.lock();

    if (!shader)
    {
        shader = shaderCache.Get("flash_limiter");
    }

    if (!shader)
    {
        std::string vertexShader(ShaderVersion);
        std::string fragmentShader(ShaderVersion);
        vertexShader.append(FlashLimiterVertexShader);
        fragmentShader.append(FlashLimiterFragmentShader);

        shader = std::make_shared<Shader>();
        shader->CompileProgram(vertexShader, fragmentShader);

        shaderCache.Insert("flash_limiter", shader);
    }

    m_shader = shader;
    shader->Bind();

    return shader;
}

} // namespace Renderer
} // namespace libprojectM

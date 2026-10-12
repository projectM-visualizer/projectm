#include "LineRenderer.hpp"

#include "MilkdropStaticShaders.hpp"

#include <Renderer/ShaderCache.hpp>

#include <cstddef>
#include <cstdint>

namespace libprojectM {
namespace MilkdropPreset {

namespace {
constexpr const char* LineShaderName = "milkdrop_quad_lines";
} // namespace

auto LineTieBias() -> glm::vec2
{
#ifdef USE_GLES
    // GL line tie-breaking is implementation-defined; this matches Mali-G52's GL lines. Desktop GL
    // needs none (its GL lines and the quads already agree).
    return {0.0f, 1.0f / 64.0f};
#else
    return {0.0f, 0.0f};
#endif
}

LineRenderer::~LineRenderer()
{
    if (m_vboId != 0)
    {
        glDeleteBuffers(1, &m_vboId);
    }
    if (m_vaoId != 0)
    {
        glDeleteVertexArrays(1, &m_vaoId);
    }
}

auto LineRenderer::Prepare(const Renderer::RenderContext& context) -> bool
{
    if (m_shader)
    {
        return true;
    }
    if (m_failed || context.shaderCache == nullptr)
    {
        return false;
    }

    m_shader = context.shaderCache->Get(LineShaderName);
    if (m_shader)
    {
        return true;
    }

    auto staticShaders = MilkdropStaticShaders::Get();
    auto shader = std::make_shared<Renderer::Shader>();
    try
    {
        shader->CompileProgram(staticShaders->GetLineVertexShader(), staticShaders->GetLineFragmentShader());
    }
    catch (const Renderer::ShaderException&)
    {
        // A driver that rejects the program: the preset draws its lines as GL lines.
        m_failed = true;
        return false;
    }
    context.shaderCache->Insert(LineShaderName, shader);
    m_shader = shader;
    return true;
}

void LineRenderer::Upload(const LineBatch& batch)
{
    const auto& points = batch.Points();
    if (!m_shader || points.empty())
    {
        return;
    }

    if (m_vaoId == 0)
    {
        glGenVertexArrays(1, &m_vaoId);
        glGenBuffers(1, &m_vboId);
        glBindVertexArray(m_vaoId);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
        for (GLuint location = 0; location < 6; location++)
        {
            glEnableVertexAttribArray(location);
            glVertexAttribDivisor(location, 1);
        }
        glBindVertexArray(0);
    }

    // Orphan the buffer so the GPU can keep reading the previous upload.
    glBindBuffer(GL_ARRAY_BUFFER, m_vboId);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(ColoredPoint) * points.size()), points.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LineRenderer::Begin(const glm::mat4& transformation, const Renderer::RenderContext& context)
{
    m_ready = m_shader && m_vaoId != 0 && context.viewportSizeX > 0 && context.viewportSizeY > 0;
    if (!m_ready)
    {
        return;
    }
    m_shader->Bind();
    m_shader->SetUniformMat4x4("vertex_transformation", transformation);
    m_shader->SetUniformFloat2("viewport_size", glm::vec2(static_cast<float>(context.viewportSizeX),
                                                          static_cast<float>(context.viewportSizeY)));
    m_shader->SetUniformFloat("antialias", context.lineAntialiasing ? 1.0f : 0.0f);
    m_shader->SetUniformFloat2("tie_bias", LineTieBias());

    m_halfWidthSet = false;
    m_passOffsetSet = false;
}

void LineRenderer::Draw(const LineBatch::Strip& strip, const LineStyle& style)
{
    if (!m_ready || strip.segments <= 0 || style.passes <= 0)
    {
        return;
    }

    m_shader->Bind();
    glBindVertexArray(m_vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboId);

    // Instance i reads the strip's points i (previous), i + 1 (A), i + 2 (B) and i + 3 (next).
    const auto stride = static_cast<GLsizei>(sizeof(ColoredPoint));
    const auto pointer = [&](int point, size_t member) {
        return reinterpret_cast<const void*>(static_cast<uintptr_t>(strip.firstPoint + point) * sizeof(ColoredPoint) + member);
    };
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, pointer(0, offsetof(ColoredPoint, x)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, pointer(1, offsetof(ColoredPoint, x)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, pointer(2, offsetof(ColoredPoint, x)));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, pointer(3, offsetof(ColoredPoint, x)));
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, pointer(1, offsetof(ColoredPoint, r)));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, pointer(2, offsetof(ColoredPoint, r)));

    if (!m_halfWidthSet || style.halfWidth != m_lastHalfWidth)
    {
        m_shader->SetUniformFloat("half_width", style.halfWidth);
        m_lastHalfWidth = style.halfWidth;
        m_halfWidthSet = true;
    }

    for (int pass = 0; pass < style.passes; pass++)
    {
        // Every pass states its offset, so nothing leaks into the next style's draws; the value only
        // reaches GL when it changes.
        const auto offset = PassOffsetFor(style, pass);
        const glm::vec2 passOffset(offset.x, offset.y);
        if (!m_passOffsetSet || passOffset != m_lastPassOffset)
        {
            m_shader->SetUniformFloat2("pass_offset", passOffset);
            m_lastPassOffset = passOffset;
            m_passOffsetSet = true;
        }
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, strip.segments);
    }
}

void LineRenderer::End()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    Renderer::Shader::Unbind();
    m_ready = false;
}

ScopedProgramPointSize::ScopedProgramPointSize(bool enable)
{
#ifndef USE_GLES
    if (enable && glIsEnabled(GL_PROGRAM_POINT_SIZE) == GL_FALSE)
    {
        glEnable(GL_PROGRAM_POINT_SIZE);
        m_enabled = true;
    }
#else
    static_cast<void>(enable);
#endif
}

ScopedProgramPointSize::~ScopedProgramPointSize()
{
#ifndef USE_GLES
    if (m_enabled)
    {
        glDisable(GL_PROGRAM_POINT_SIZE);
    }
#endif
}

} // namespace MilkdropPreset
} // namespace libprojectM

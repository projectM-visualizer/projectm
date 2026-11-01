#include "LineRenderer.hpp"

#include "LineShaderResources.hpp"

#include <Renderer/ShaderCache.hpp>

#include <cstddef>
#include <cstdint>

namespace libprojectM {
namespace Renderer {

namespace {
constexpr const char* LineShaderName = "renderer_quad_lines";
#ifdef USE_GLES
constexpr const char* VersionHeader = "#version 300 es\n";
#else
constexpr const char* VersionHeader = "#version 330\n";
#endif
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

auto LineRenderer::Prepare(const RenderContext& context) -> bool
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

    auto shader = std::make_shared<Shader>();
    try
    {
        shader->CompileProgram(VersionHeader + kLineVertexShaderGlsl330, FragmentShaderSource());
    }
    catch (const ShaderException&)
    {
        // A driver that rejects the program: the preset draws its lines as GL lines.
        m_failed = true;
        return false;
    }
    context.shaderCache->Insert(LineShaderName, shader);
    m_shader = shader;
    return true;
}

auto LineRenderer::FragmentShaderSource() -> std::string
{
    return VersionHeader + kLineFragmentShaderGlsl330;
}

void LineRenderer::Upload(const LineBatch& batch)
{
    const auto& points = batch.Points();
    if (!m_shader || points.empty())
    {
        return;
    }

    if (!m_mesh)
    {
        m_mesh = std::make_unique<Mesh>(VertexBufferUsage::StreamDraw);
        m_mesh->SetRenderPrimitiveType(Mesh::PrimitiveType::TriangleStrip);
    }
    m_mesh->UpdateInstanceData(points.data(), sizeof(ColoredPoint) * points.size());
}

void LineRenderer::Begin(const glm::mat4& transformation, const RenderContext& context)
{
    m_ready = m_shader && m_mesh != nullptr && context.viewportSizeX > 0 && context.viewportSizeY > 0;
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

    m_firstPoint = -1;
    m_halfWidthSet = false;
    m_passOffsetSet = false;
}

void LineRenderer::Draw(const LineBatch::Strip& strip, float halfWidth, const glm::vec2& passOffset)
{
    if (!m_ready || strip.segments <= 0)
    {
        return;
    }

    m_shader->Bind();
    if (m_firstPoint != strip.firstPoint)
    {
        // Instance i reads previous, A, B and next points from the same padded stream.
        const auto stride = static_cast<GLsizei>(sizeof(ColoredPoint));
        const size_t pointSize = sizeof(ColoredPoint);
        m_mesh->SetInstanceAttributes({{0, 2, GL_FLOAT, GL_FALSE, stride, offsetof(ColoredPoint, x), 1},
                                       {1, 2, GL_FLOAT, GL_FALSE, stride, pointSize + offsetof(ColoredPoint, x), 1},
                                       {2, 2, GL_FLOAT, GL_FALSE, stride, 2 * pointSize + offsetof(ColoredPoint, x), 1},
                                       {3, 2, GL_FLOAT, GL_FALSE, stride, 3 * pointSize + offsetof(ColoredPoint, x), 1},
                                       {4, 4, GL_FLOAT, GL_FALSE, stride, pointSize + offsetof(ColoredPoint, r), 1},
                                       {5, 4, GL_FLOAT, GL_FALSE, stride, 2 * pointSize + offsetof(ColoredPoint, r), 1}},
                                      static_cast<size_t>(strip.firstPoint) * pointSize);
        m_firstPoint = strip.firstPoint;
    }

    if (!m_halfWidthSet || halfWidth != m_lastHalfWidth)
    {
        m_shader->SetUniformFloat("half_width", halfWidth);
        m_lastHalfWidth = halfWidth;
        m_halfWidthSet = true;
    }

    if (!m_passOffsetSet || passOffset != m_lastPassOffset)
    {
        m_shader->SetUniformFloat2("pass_offset", passOffset);
        m_lastPassOffset = passOffset;
        m_passOffsetSet = true;
    }
    m_mesh->DrawInstanced(4, static_cast<uint32_t>(strip.segments));
}

void LineRenderer::End()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    Shader::Unbind();
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

} // namespace Renderer
} // namespace libprojectM

#include "CustomShape.hpp"

#include "PresetFileParser.hpp"

#include <Renderer/BlendMode.hpp>
#include <Renderer/TextureManager.hpp>

#include <vector>

namespace libprojectM {
namespace MilkdropPreset {

CustomShape::CustomShape(PresetState& presetState)
    : m_outlineMesh(Renderer::VertexBufferUsage::StreamDraw)
    , m_fillMesh(Renderer::VertexBufferUsage::StreamDraw, true, false)
    , m_presetState(presetState)
    , m_perFrameContext(presetState.globalMemory, &presetState.globalRegisters)
{
    m_outlineMesh.SetVertexCount(100);
    m_outlineMesh.SetRenderPrimitiveType(Renderer::Mesh::PrimitiveType::LineLoop);

    glGenVertexArrays(1, &m_vaoIdTextured);
    glGenBuffers(1, &m_vboIdTextured);

    glGenVertexArrays(1, &m_vaoIdUntextured);
    glGenBuffers(1, &m_vboIdUntextured);

    glBindVertexArray(m_vaoIdTextured);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIdTextured);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, x))); // Position
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, r))); // Color
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, u))); // Texture coordinate

    glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedPoint) * vertexData.size(), vertexData.data(), GL_STREAM_DRAW);

    glBindVertexArray(m_vaoIdUntextured);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIdUntextured);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, x))); // Position
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, r))); // Color

    glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedPoint) * vertexData.size(), vertexData.data(), GL_STREAM_DRAW);

    Init();

    m_perFrameContext.RegisterBuiltinVariables();
}

void CustomShape::Initialize(PresetFileParser& parsedFile, int index)
{
    std::string const shapecodePrefix = "shapecode_" + std::to_string(index) + "_";

    m_index = index;
    m_enabled = parsedFile.GetBool(shapecodePrefix + "enabled", m_enabled);
    m_sides = parsedFile.GetInt(shapecodePrefix + "sides", m_sides);
    m_additive = parsedFile.GetBool(shapecodePrefix + "additive", m_additive);
    m_thickOutline = parsedFile.GetBool(shapecodePrefix + "thickOutline", m_thickOutline);
    m_textured = parsedFile.GetBool(shapecodePrefix + "textured", m_textured);
    m_instances = parsedFile.GetInt(shapecodePrefix + "num_inst", m_instances);
    m_x = parsedFile.GetFloat(shapecodePrefix + "x", m_x);
    m_y = parsedFile.GetFloat(shapecodePrefix + "y", m_y);
    m_radius = parsedFile.GetFloat(shapecodePrefix + "rad", m_radius);
    m_angle = parsedFile.GetFloat(shapecodePrefix + "ang", m_angle);
    m_tex_ang = parsedFile.GetFloat(shapecodePrefix + "tex_ang", m_tex_ang);
    m_tex_zoom = parsedFile.GetFloat(shapecodePrefix + "tex_zoom", m_tex_zoom);
    m_r = parsedFile.GetFloat(shapecodePrefix + "r", m_r);
    m_g = parsedFile.GetFloat(shapecodePrefix + "g", m_g);
    m_b = parsedFile.GetFloat(shapecodePrefix + "b", m_b);
    m_a = parsedFile.GetFloat(shapecodePrefix + "a", m_a);
    m_r2 = parsedFile.GetFloat(shapecodePrefix + "r2", m_r2);
    m_g2 = parsedFile.GetFloat(shapecodePrefix + "g2", m_g2);
    m_b2 = parsedFile.GetFloat(shapecodePrefix + "b2", m_b2);
    m_a2 = parsedFile.GetFloat(shapecodePrefix + "a2", m_a2);
    m_border_r = parsedFile.GetFloat(shapecodePrefix + "border_r", m_border_r);
    m_border_g = parsedFile.GetFloat(shapecodePrefix + "border_g", m_border_g);
    m_border_b = parsedFile.GetFloat(shapecodePrefix + "border_b", m_border_b);
    m_border_a = parsedFile.GetFloat(shapecodePrefix + "border_a", m_border_a);

    // projectM addition: texture name to use for rendering the shape
    m_image = parsedFile.GetString(shapecodePrefix + "image", "");
}

void CustomShape::CompileCodeAndRunInitExpressions()
{
    m_perFrameContext.LoadStateVariables(m_presetState, *this, 0);
    m_perFrameContext.EvaluateInitCode(m_presetState.customShapeInitCode[m_index], *this);

    for (int t = 0; t < TVarCount; t++)
    {
        m_tValuesAfterInitCode[t] = *m_perFrameContext.t_vars[t];
    }

    m_perFrameContext.CompilePerFrameCode(m_presetState.customShapePerFrameCode[m_index], *this);
}

void CustomShape::Draw()
{
    static constexpr float pi = 3.141592653589793f;

    if (!m_enabled)
    {
        return;
    }

    Renderer::BlendMode::SetBlendActive(true);

    for (int instance = 0; instance < m_instances; instance++)
    {
        m_perFrameContext.LoadStateVariables(m_presetState, *this, instance);
        m_perFrameContext.ExecutePerFrameCode();

        int sides = static_cast<int>(*m_perFrameContext.sides);
        if (sides < 3)
        {
            sides = 3;
        }
        if (sides > 100)
        {
            sides = 100;
        }

        // Additive Drawing or Overwrite
        Renderer::BlendMode::SetBlendFunction(Renderer::BlendMode::Function::SourceAlpha,
                                              static_cast<int>(*m_perFrameContext.additive) != 0
                                                  ? Renderer::BlendMode::Function::One
                                                  : Renderer::BlendMode::Function::OneMinusSourceAlpha);

        auto& vertexData = m_fillMesh.Vertices();
        auto& colorData = m_fillMesh.Colors();

        vertexData[0] = Renderer::Point(static_cast<float>(*m_perFrameContext.x * 2.0 - 1.0),
                                        static_cast<float>(*m_perFrameContext.y * -2.0 + 1.0));

        // x = f*255.0 & 0xFF = (f*255.0) % 256
        // f' = x/255.0 = f % (256/255)
        // 1.0 -> 255 (0xFF)
        // 2.0 -> 254 (0xFE)
        // -1.0 -> 0x01

        colorData[0] = Renderer::Color::Modulo(Renderer::Color(static_cast<float>(*m_perFrameContext.r),
                                                               static_cast<float>(*m_perFrameContext.g),
                                                               static_cast<float>(*m_perFrameContext.b),
                                                               static_cast<float>(*m_perFrameContext.a)));

        colorData[1] = Renderer::Color::Modulo(Renderer::Color(static_cast<float>(*m_perFrameContext.r2),
                                                               static_cast<float>(*m_perFrameContext.g2),
                                                               static_cast<float>(*m_perFrameContext.b2),
                                                               static_cast<float>(*m_perFrameContext.a2)));

        for (int i = 1; i < sides + 1; i++)
        {
            const float cornerProgress = static_cast<float>(i - 1) / static_cast<float>(sides);
            const float angle = cornerProgress * pi * 2.0f + static_cast<float>(*m_perFrameContext.ang) + pi * 0.25f;

            // Todo: There's still some issue with aspect ratio here, as everything gets squashed horizontally if Y > x.
            vertexData[i] = Renderer::Point(vertexData[0].X() + static_cast<float>(*m_perFrameContext.rad) * cosf(angle) * m_presetState.renderContext.aspectY,
                                            vertexData[0].Y() + static_cast<float>(*m_perFrameContext.rad) * sinf(angle));

            colorData[i] = colorData[1];
        }

        // Duplicate last vertex.
        vertexData[sides + 1] = vertexData[1];
        colorData[sides + 1] = colorData[1];

        m_fillMesh.SetUseUV(static_cast<int>(*m_perFrameContext.textured) != 0);

        if (m_fillMesh.UseUV())
        {
            auto shader = m_presetState.texturedShader.lock();
            shader->Bind();
            shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
            shader->SetUniformInt("texture_sampler", 0);

            // Textured shape, either main texture or texture from "image" key
            auto textureAspectY = m_presetState.renderContext.aspectY;
            if (m_image.empty())
            {
                assert(!m_presetState.mainTexture.expired());
                m_presetState.mainTexture.lock()->Bind(0);
            }
            else
            {
                auto desc = m_presetState.renderContext.textureManager->GetTexture(m_image);
                if (!desc.Empty())
                {
                    desc.Bind(0, *shader);
                    textureAspectY = 1.0f;
                }
                else
                {
                    // No texture found, fall back to main texture.
                    assert(!m_presetState.mainTexture.expired());
                    m_presetState.mainTexture.lock()->Bind(0);
                }
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            auto& uvs = m_fillMesh.UVs();

            uvs[0] = Renderer::TextureUV(0.5f, 0.5f);

            for (int i = 1; i < sides + 1; i++)
            {
                const float cornerProgress = static_cast<float>(i - 1) / static_cast<float>(sides);
                const float angle = cornerProgress * pi * 2.0f + static_cast<float>(*m_perFrameContext.tex_ang) + pi * 0.25f;

                uvs[i] = Renderer::TextureUV(0.5f + 0.5f * cosf(angle) / static_cast<float>(*m_perFrameContext.tex_zoom) * textureAspectY,
                                             1.0f - (0.5f - 0.5f * sinf(angle) / static_cast<float>(*m_perFrameContext.tex_zoom))); // Vertical flip required!
            }

            uvs[sides + 1] = uvs[1];
        }
        else
        {
            // Untextured (creates a color gradient: center=r/g/b/a to border=r2/b2/g2/a2)
            auto shader = m_presetState.untexturedShader.lock();
            shader->Bind();
            shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
        }

        m_fillMesh.Indices().Resize(sides + 2);
        m_fillMesh.Indices().MakeContinuous();
        m_fillMesh.Update();
        m_fillMesh.Draw();

        glBindTexture(GL_TEXTURE_2D, 0);
        Renderer::Sampler::Unbind(0);

        if (*m_perFrameContext.border_a > 0.0001f)
        {
            m_outlineMesh.Indices().Resize(sides);
            m_outlineMesh.Indices().MakeContinuous();

            auto& points = m_outlineMesh.Vertices();

            for (int i = 0; i < sides; i++)
            {
                points[i] = m_fillMesh.Vertex(i + 1);
            }

            auto shader = m_presetState.untexturedShader.lock();
            shader->Bind();
            shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
            shader->SetUniformFloat("vertex_point_size", 1.0f);

            glVertexAttrib4f(1,
                             static_cast<float>(*m_perFrameContext.border_r),
                             static_cast<float>(*m_perFrameContext.border_g),
                             static_cast<float>(*m_perFrameContext.border_b),
                             static_cast<float>(*m_perFrameContext.border_a));
            glLineWidth(1);
#ifndef USE_GLES
            glEnable(GL_LINE_SMOOTH);
#endif

            const auto iterations = m_thickOutline ? 4 : 1;

            // Need to use +/- 1.0 here instead of 2.0 used in Milkdrop to achieve the same rendering result.
            const auto incrementX = 1.0f / static_cast<float>(m_presetState.renderContext.viewportSizeX);
            const auto incrementY = 1.0f / static_cast<float>(m_presetState.renderContext.viewportSizeY);

            // If thick outline is used, draw the shape four times with slight offsets
            // (top left, top right, bottom right, bottom left).
            for (auto iteration = 0; iteration < iterations; iteration++)
            {
                switch (iteration)
                {
                    case 0:
                        break;

                    case 1:
                        for (auto j = 0; j < sides; j++)
                        {
                            points[j].SetX(points[j].X() + incrementX);
                        }
                        break;

                    case 2:
                        for (auto j = 0; j < sides; j++)
                        {
                            points[j].SetY(points[j].Y() + incrementY);
                        }
                        break;

                    case 3:
                        for (auto j = 0; j < sides; j++)
                        {
                            points[j].SetX(points[j].X() - incrementX);
                        }
                        break;
                }

                m_outlineMesh.Update();
                m_outlineMesh.Draw();
            }
        }
    }

    Renderer::Mesh::Unbind();
    Renderer::Shader::Unbind();

#ifndef USE_GLES
    glDisable(GL_LINE_SMOOTH);
#endif
    Renderer::BlendMode::SetBlendActive(false);
}

} // namespace MilkdropPreset
} // namespace libprojectM

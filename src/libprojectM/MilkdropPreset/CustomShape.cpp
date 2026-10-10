#include "CustomShape.hpp"

#include "PresetFileParser.hpp"

#include <Renderer/BlendMode.hpp>
#include <Renderer/TextureManager.hpp>

#include <array>
#include <optional>
#include <vector>

namespace libprojectM {
namespace MilkdropPreset {

CustomShape::CustomShape(PresetState& presetState)
    : m_outlineMesh(Renderer::VertexBufferUsage::StreamDraw)
    , m_batchMesh(Renderer::VertexBufferUsage::StreamDraw, true, false)
    , m_outlineBatchMesh(Renderer::VertexBufferUsage::StreamDraw, true, false)
    , m_presetState(presetState)
    , m_perFrameContext(presetState.globalMemory, &presetState.globalRegisters)
{
    m_outlineMesh.SetVertexCount(100);
    m_outlineMesh.SetRenderPrimitiveType(Renderer::Mesh::PrimitiveType::LineLoop);

    m_batchMesh.SetRenderPrimitiveType(Renderer::Mesh::PrimitiveType::Triangles);
    m_outlineBatchMesh.SetRenderPrimitiveType(Renderer::Mesh::PrimitiveType::Lines);

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

    // Shape fills are batched: consecutive instances that share a blend mode and texturing
    // are collected in m_batchMesh as one triangle list and drawn with a single call. A draw
    // call per instance (a shape can have up to 1024) is very slow on drivers with a high
    // per-draw cost, e.g. the OpenGL-over-Metal layer on macOS. Borders are drawn per
    // instance right after flushing the batch, so the draw order stays exactly the same,
    // except for additive instances: additive blending is order-independent, so their borders
    // are collected in m_outlineBatchMesh as a line list and drawn right after the fills.
    auto& batchVertices = m_batchMesh.Vertices().Get();
    auto& batchColors = m_batchMesh.Colors().Get();
    auto& batchUVs = m_batchMesh.UVs().Get();
    auto& batchIndices = m_batchMesh.Indices().Get();
    auto& outlineVertices = m_outlineBatchMesh.Vertices().Get();
    auto& outlineColors = m_outlineBatchMesh.Colors().Get();
    bool batchAdditive{false};
    bool batchTextured{false};

    // Aspect used for the UVs of textured instances; depends only on the shape's image.
    std::optional<float> imageAspectY;

    // Need to use +/- 1.0 here instead of 2.0 used in Milkdrop to achieve the same rendering result.
    const auto incrementX = 1.0f / static_cast<float>(m_presetState.renderContext.viewportSizeX);
    const auto incrementY = 1.0f / static_cast<float>(m_presetState.renderContext.viewportSizeY);

    // If thick outline is used, the border is drawn four times with slight offsets
    // (top left, top right, bottom right, bottom left).
    const std::array<Renderer::Point, 4> outlineOffsets{
        Renderer::Point(0.0f, 0.0f), Renderer::Point(incrementX, 0.0f),
        Renderer::Point(incrementX, incrementY), Renderer::Point(0.0f, incrementY)};
    const auto iterations = m_thickOutline ? 4 : 1;

    const auto bindOutlineState = [&]() {
        auto shader = m_presetState.untexturedShader.lock();
        shader->Bind();
        shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
        shader->SetUniformFloat("vertex_point_size", 1.0f);
        glLineWidth(1);
#ifndef USE_GLES
        glEnable(GL_LINE_SMOOTH);
#endif
    };

    const auto flushBatch = [&]() {
        if (batchIndices.empty())
        {
            return;
        }

        // Additive Drawing or Overwrite
        Renderer::BlendMode::SetBlendFunction(Renderer::BlendMode::Function::SourceAlpha,
                                              batchAdditive
                                                  ? Renderer::BlendMode::Function::One
                                                  : Renderer::BlendMode::Function::OneMinusSourceAlpha);

        m_batchMesh.SetUseUV(batchTextured);

        if (batchTextured)
        {
            auto shader = m_presetState.texturedShader.lock();
            shader->Bind();
            shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
            shader->SetUniformInt("texture_sampler", 0);

            // Textured shape, either main texture or texture from "image" key
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
        }
        else
        {
            // Untextured (creates a color gradient: center=r/g/b/a to border=r2/b2/g2/a2)
            auto shader = m_presetState.untexturedShader.lock();
            shader->Bind();
            shader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
        }

        m_batchMesh.Update();
        m_batchMesh.Draw();

        glBindTexture(GL_TEXTURE_2D, 0);
        Renderer::Sampler::Unbind(0);

        batchVertices.clear();
        batchColors.clear();
        batchUVs.clear();
        batchIndices.clear();

        // Borders of the additive instances just drawn, with the same blend function.
        if (!outlineVertices.empty())
        {
            bindOutlineState();
            m_outlineBatchMesh.Update();
            m_outlineBatchMesh.Draw();

            outlineVertices.clear();
            outlineColors.clear();
        }
    };

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

        const bool additive = static_cast<int>(*m_perFrameContext.additive) != 0;
        const bool textured = static_cast<int>(*m_perFrameContext.textured) != 0;

        // The batch can only hold instances drawn with identical state.
        if (additive != batchAdditive || textured != batchTextured)
        {
            flushBatch();
            batchAdditive = additive;
            batchTextured = textured;
        }

        const auto center = Renderer::Point(static_cast<float>(*m_perFrameContext.x * 2.0 - 1.0),
                                            static_cast<float>(*m_perFrameContext.y * -2.0 + 1.0));

        // x = f*255.0 & 0xFF = (f*255.0) % 256
        // f' = x/255.0 = f % (256/255)
        // 1.0 -> 255 (0xFF)
        // 2.0 -> 254 (0xFE)
        // -1.0 -> 0x01

        const auto centerColor = Renderer::Color::Modulo(Renderer::Color(static_cast<float>(*m_perFrameContext.r),
                                                                         static_cast<float>(*m_perFrameContext.g),
                                                                         static_cast<float>(*m_perFrameContext.b),
                                                                         static_cast<float>(*m_perFrameContext.a)));

        const auto edgeColor = Renderer::Color::Modulo(Renderer::Color(static_cast<float>(*m_perFrameContext.r2),
                                                                       static_cast<float>(*m_perFrameContext.g2),
                                                                       static_cast<float>(*m_perFrameContext.b2),
                                                                       static_cast<float>(*m_perFrameContext.a2)));

        // Append this instance as a triangle fan around the center vertex (center + `sides`
        // edge vertices), written as indexed triangles.
        const auto baseIndex = static_cast<uint32_t>(batchVertices.size());

        batchVertices.push_back(center);
        batchColors.push_back(centerColor);

        std::array<Renderer::Point, 100> edgePoints;

        for (int i = 0; i < sides; i++)
        {
            const float cornerProgress = static_cast<float>(i) / static_cast<float>(sides);
            const float angle = cornerProgress * pi * 2.0f + static_cast<float>(*m_perFrameContext.ang) + pi * 0.25f;

            // Todo: There's still some issue with aspect ratio here, as everything gets squashed horizontally if Y > x.
            edgePoints[i] = Renderer::Point(center.X() + static_cast<float>(*m_perFrameContext.rad) * cosf(angle) * m_presetState.renderContext.aspectY,
                                            center.Y() + static_cast<float>(*m_perFrameContext.rad) * sinf(angle));

            batchVertices.push_back(edgePoints[i]);
            batchColors.push_back(edgeColor);
        }

        if (textured)
        {
            if (!imageAspectY.has_value())
            {
                imageAspectY = m_presetState.renderContext.aspectY;
                if (!m_image.empty() && !m_presetState.renderContext.textureManager->GetTexture(m_image).Empty())
                {
                    imageAspectY = 1.0f;
                }
            }

            batchUVs.push_back(Renderer::TextureUV(0.5f, 0.5f));

            for (int i = 0; i < sides; i++)
            {
                const float cornerProgress = static_cast<float>(i) / static_cast<float>(sides);
                const float angle = cornerProgress * pi * 2.0f + static_cast<float>(*m_perFrameContext.tex_ang) + pi * 0.25f;

                batchUVs.push_back(Renderer::TextureUV(0.5f + 0.5f * cosf(angle) / static_cast<float>(*m_perFrameContext.tex_zoom) * *imageAspectY,
                                                       1.0f - (0.5f - 0.5f * sinf(angle) / static_cast<float>(*m_perFrameContext.tex_zoom)))); // Vertical flip required!
            }
        }

        for (int i = 0; i < sides; i++)
        {
            batchIndices.push_back(baseIndex);
            batchIndices.push_back(baseIndex + 1 + i);
            batchIndices.push_back(baseIndex + 1 + (i + 1) % sides);
        }

        if (*m_perFrameContext.border_a > 0.0001f)
        {
            const Renderer::Color borderColor(static_cast<float>(*m_perFrameContext.border_r),
                                              static_cast<float>(*m_perFrameContext.border_g),
                                              static_cast<float>(*m_perFrameContext.border_b),
                                              static_cast<float>(*m_perFrameContext.border_a));

            if (additive)
            {
                // Each loop edge as its own line segment, once per thick-outline offset.
                for (auto iteration = 0; iteration < iterations; iteration++)
                {
                    const auto& offset = outlineOffsets[iteration];
                    for (int i = 0; i < sides; i++)
                    {
                        const auto& from = edgePoints[i];
                        const auto& to = edgePoints[(i + 1) % sides];
                        outlineVertices.emplace_back(from.X() + offset.X(), from.Y() + offset.Y());
                        outlineVertices.emplace_back(to.X() + offset.X(), to.Y() + offset.Y());
                        outlineColors.push_back(borderColor);
                        outlineColors.push_back(borderColor);
                    }
                }
                continue;
            }

            // Draw everything collected so far, this instance's fill included, before its border.
            flushBatch();

            m_outlineMesh.Indices().Resize(sides);
            m_outlineMesh.Indices().MakeContinuous();

            auto& points = m_outlineMesh.Vertices();

            bindOutlineState();
            glVertexAttrib4f(1, borderColor.R(), borderColor.G(), borderColor.B(), borderColor.A());

            for (auto iteration = 0; iteration < iterations; iteration++)
            {
                const auto& offset = outlineOffsets[iteration];
                for (int i = 0; i < sides; i++)
                {
                    points[i] = Renderer::Point(edgePoints[i].X() + offset.X(), edgePoints[i].Y() + offset.Y());
                }

                m_outlineMesh.Update();
                m_outlineMesh.Draw();
            }
        }
    }

    flushBatch();

    Renderer::Mesh::Unbind();
    Renderer::Shader::Unbind();

#ifndef USE_GLES
    glDisable(GL_LINE_SMOOTH);
#endif
    Renderer::BlendMode::SetBlendActive(false);
}

} // namespace MilkdropPreset
} // namespace libprojectM

#include "CustomShape.hpp"

#include "PresetFileParser.hpp"

#include "Audio/BeatDetect.hpp"
#include "Renderer/TextureManager.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <vector>

CustomShape::CustomShape(PresetState& presetState)
    : RenderItem()
    , m_presetState(presetState)
    , m_perFrameContext(presetState.globalMemory, &presetState.globalRegisters)
{
    glGenVertexArrays(1, &m_vaoID_texture);
    glGenBuffers(1, &m_vboID_texture);

    glGenVertexArrays(1, &m_vaoID_not_texture);
    glGenBuffers(1, &m_vboID_not_texture);

    glBindVertexArray(m_vaoID_texture);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID_texture);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          sizeof(ShapeVertexShaderData), nullptr); // Positions
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                          sizeof(ShapeVertexShaderData), reinterpret_cast<void*>(sizeof(float) * 2)); // Colors
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          sizeof(ShapeVertexShaderData), reinterpret_cast<void*>(sizeof(float) * 6)); // Textures

    glBindVertexArray(m_vaoID_not_texture);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID_not_texture);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
                          sizeof(ShapeVertexShaderData), nullptr); // Points
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
                          sizeof(ShapeVertexShaderData), reinterpret_cast<void*>(sizeof(float) * 2)); // Colors

    RenderItem::Init();
}

CustomShape::~CustomShape()
{
    glDeleteBuffers(1, &m_vboID_texture);
    glDeleteVertexArrays(1, &m_vaoID_texture);

    glDeleteBuffers(1, &m_vboID_not_texture);
    glDeleteVertexArrays(1, &m_vaoID_not_texture);
}

void CustomShape::InitVertexAttrib()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr); // points
    glDisableVertexAttribArray(1);
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
    m_image = parsedFile.GetString("image", "");

    m_perFrameContext.RegisterBuiltinVariables();
}

void CustomShape::CompileCodeAndRunInitExpressions(const PerFrameContext& presetPerFrameContext)
{
    m_perFrameContext.LoadStateVariables(m_presetState, presetPerFrameContext, *this, 0);
    m_perFrameContext.EvaluateInitCode(m_presetState.customShapeInitCode[m_index], *this);

    for (int t = 0; t < TVarCount; t++)
    {
        m_tValuesAfterInitCode[t] = *m_perFrameContext.t_vars[t];
    }

    m_perFrameContext.CompilePerFrameCode(m_presetState.customShapePerFrameCode[m_index], *this);
}

void CustomShape::Draw(const PerFrameContext& presetPerFrameContext)
{
    static constexpr float pi = 3.141592653589793f;

    if (!m_enabled)
    {
        return;
    }

    glEnable(GL_BLEND);

    for (int instance = 0; instance < m_instances; instance++)
    {
        m_perFrameContext.LoadStateVariables(m_presetState, presetPerFrameContext, *this, instance);
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
        glBlendFunc(GL_SRC_ALPHA, static_cast<int>(*m_perFrameContext.additive) != 0 ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA);

        std::vector<ShapeVertexShaderData> vertexData(sides + 2);

        vertexData[0].point_x = static_cast<float>(*m_perFrameContext.x * 2.0 - 1.0);
        vertexData[0].point_y = static_cast<float>(*m_perFrameContext.y * -2.0 + 1.0);

        vertexData[0].tex_x = 0.5f;
        vertexData[0].tex_y = 0.5f;

        vertexData[0].color_r = static_cast<float>(*m_perFrameContext.r);
        vertexData[0].color_g = static_cast<float>(*m_perFrameContext.g);
        vertexData[0].color_b = static_cast<float>(*m_perFrameContext.b);
        vertexData[0].color_a = static_cast<float>(*m_perFrameContext.a);

        vertexData[1].color_r = static_cast<float>(*m_perFrameContext.r2);
        vertexData[1].color_g = static_cast<float>(*m_perFrameContext.g2);
        vertexData[1].color_b = static_cast<float>(*m_perFrameContext.b2);
        vertexData[1].color_a = static_cast<float>(*m_perFrameContext.a2);

        for (int i = 1; i < sides + 1; i++)
        {
            const float cornerProgress = static_cast<float>(i - 1) / static_cast<float>(sides);
            const float angle = cornerProgress * pi * 2.0f + static_cast<float>(*m_perFrameContext.ang) + pi * 0.25f;

            // Todo: There's still some issue with aspect ratio here, as everything gets squashed horizontally if Y > x.
            vertexData[i].point_x = vertexData[0].point_x + static_cast<float>(*m_perFrameContext.rad) * cosf(angle) * m_presetState.renderContext.aspectY;
            vertexData[i].point_y = vertexData[0].point_y + static_cast<float>(*m_perFrameContext.rad) * sinf(angle);

            vertexData[i].color_r = vertexData[1].color_r;
            vertexData[i].color_g = vertexData[1].color_g;
            vertexData[i].color_b = vertexData[1].color_b;
            vertexData[i].color_a = vertexData[1].color_a;
        }

        // Duplicate last vertex.
        vertexData[sides + 1] = vertexData[1];

        if (static_cast<int>(*m_perFrameContext.textured) != 0)
        {
            m_presetState.texturedShader.Bind();
            m_presetState.texturedShader.SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
            m_presetState.texturedShader.SetUniformInt("texture_sampler", 0);

            // Textured shape, either main texture or texture from "image" key
            auto textureAspectY = m_presetState.renderContext.aspectY;
            if (m_image.empty())
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_presetState.mainTextureId);
            }
            else
            {
                auto desc = m_presetState.renderContext.textureManager->GetTexture(m_image);
                if (!desc.Empty())
                {
                    desc.Bind(0,  m_presetState.texturedShader);
                    textureAspectY = 1.0f;
                }
                else
                {
                    // No texture found, fall back to main texture.
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, m_presetState.mainTextureId);
                }
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            for (int i = 1; i < sides + 1; i++)
            {
                const float cornerProgress = static_cast<float>(i - 1) / static_cast<float>(sides);
                const float angle = cornerProgress * pi * 2.0f + static_cast<float>(*m_perFrameContext.tex_ang) + pi * 0.25f;

                vertexData[i].tex_x = 0.5f + 0.5f * cosf(angle) / static_cast<float>(*m_perFrameContext.tex_zoom) * textureAspectY;
                vertexData[i].tex_y = 0.5f + 0.5f * sinf(angle) / static_cast<float>(*m_perFrameContext.tex_zoom);
            }

            vertexData[sides + 1] = vertexData[1];

            glBindBuffer(GL_ARRAY_BUFFER, m_vboID_texture);

            glBufferData(GL_ARRAY_BUFFER, sizeof(ShapeVertexShaderData) * (sides + 2), vertexData.data(), GL_DYNAMIC_DRAW);


            glBindVertexArray(m_vaoID_texture);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sides + 2);
            glBindVertexArray(0);

            glBindTexture(GL_TEXTURE_2D, 0);
            Sampler::Unbind(0);
        }
        else
        {
            // Untextured (creates a color gradient: center=r/g/b/a to border=r2/b2/g2/a2)
            glBindBuffer(GL_ARRAY_BUFFER, m_vboID_not_texture);

            glBufferData(GL_ARRAY_BUFFER, sizeof(ShapeVertexShaderData) * (sides + 2), vertexData.data(), GL_DYNAMIC_DRAW);

            m_presetState.untexturedShader.Bind();
            m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);

            glBindVertexArray(m_vaoID_not_texture);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sides + 2);
            glBindVertexArray(0);
        }

        if (*m_perFrameContext.border_a > 0.0001f)
        {
            std::vector<ShapeVertex> points(sides + 1);

            for (int i = 0; i < sides + 1; i++)
            {
                points[i].x = vertexData[i + 1].point_x;
                points[i].y = vertexData[i + 1].point_y;
            }

            m_presetState.untexturedShader.Bind();
            m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);

            glVertexAttrib4f(1,
                             static_cast<float>(*m_perFrameContext.border_r),
                             static_cast<float>(*m_perFrameContext.border_g),
                             static_cast<float>(*m_perFrameContext.border_b),
                             static_cast<float>(*m_perFrameContext.border_a));
            glLineWidth(1);
#if USE_GLES == 0
            glEnable(GL_LINE_SMOOTH);
#endif

            glBindVertexArray(m_vaoID);
            glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

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
                        for (auto j = 0; j < sides + 1; j++)
                        {
                            points[j].x += incrementX;
                        }
                        break;

                    case 2:
                        for (auto j = 0; j < sides + 1; j++)
                        {
                            points[j].y += incrementY;
                        }
                        break;

                    case 3:
                        for (auto j = 0; j < sides + 1; j++)
                        {
                            points[j].x -= incrementX;
                        }
                        break;
                }

                glBufferData(GL_ARRAY_BUFFER, sizeof(floatPair) * (sides), points.data(), GL_DYNAMIC_DRAW);
                glDrawArrays(GL_LINE_LOOP, 0, sides);
            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

#if USE_GLES == 0
    glDisable(GL_LINE_SMOOTH);
#endif
    glDisable(GL_BLEND);

    Shader::Unbind();
}

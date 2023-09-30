#include "CustomShape.hpp"

#include "PresetFileParser.hpp"

#include "Audio/BeatDetect.hpp"
#include "Renderer/TextureManager.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <vector>

CustomShape::CustomShape(PresetState& presetState)
    : m_presetState(presetState)
    , m_perFrameContext(presetState.globalMemory, &presetState.globalRegisters)
{
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

    glBindVertexArray(m_vaoIdUntextured);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIdUntextured);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, x))); // Position
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(TexturedPoint), reinterpret_cast<void*>(offsetof(TexturedPoint, r))); // Color

    RenderItem::Init();

    m_perFrameContext.RegisterBuiltinVariables();
}

CustomShape::~CustomShape()
{
    glDeleteBuffers(1, &m_vboIdTextured);
    glDeleteVertexArrays(1, &m_vaoIdTextured);

    glDeleteBuffers(1, &m_vboIdUntextured);
    glDeleteVertexArrays(1, &m_vaoIdUntextured);
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

    glEnable(GL_BLEND);

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
        glBlendFunc(GL_SRC_ALPHA, static_cast<int>(*m_perFrameContext.additive) != 0 ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA);

        std::vector<TexturedPoint> vertexData(sides + 2);

        vertexData[0].x = static_cast<float>(*m_perFrameContext.x * 2.0 - 1.0);
        vertexData[0].y = static_cast<float>(*m_perFrameContext.y * -2.0 + 1.0);

        vertexData[0].u = 0.5f;
        vertexData[0].v = 0.5f;

        vertexData[0].r = static_cast<float>(*m_perFrameContext.r);
        vertexData[0].g = static_cast<float>(*m_perFrameContext.g);
        vertexData[0].b = static_cast<float>(*m_perFrameContext.b);
        vertexData[0].a = static_cast<float>(*m_perFrameContext.a);

        vertexData[1].r = static_cast<float>(*m_perFrameContext.r2);
        vertexData[1].g = static_cast<float>(*m_perFrameContext.g2);
        vertexData[1].b = static_cast<float>(*m_perFrameContext.b2);
        vertexData[1].a = static_cast<float>(*m_perFrameContext.a2);

        for (int i = 1; i < sides + 1; i++)
        {
            const float cornerProgress = static_cast<float>(i - 1) / static_cast<float>(sides);
            const float angle = cornerProgress * pi * 2.0f + static_cast<float>(*m_perFrameContext.ang) + pi * 0.25f;

            // Todo: There's still some issue with aspect ratio here, as everything gets squashed horizontally if Y > x.
            vertexData[i].x = vertexData[0].x + static_cast<float>(*m_perFrameContext.rad) * cosf(angle) * m_presetState.renderContext.aspectY;
            vertexData[i].y = vertexData[0].y + static_cast<float>(*m_perFrameContext.rad) * sinf(angle);

            vertexData[i].r = vertexData[1].r;
            vertexData[i].g = vertexData[1].g;
            vertexData[i].b = vertexData[1].b;
            vertexData[i].a = vertexData[1].a;
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
                assert(!m_presetState.mainTexture.expired());
                m_presetState.mainTexture.lock()->Bind(0);
            }
            else
            {
                auto desc = m_presetState.renderContext.textureManager->GetTexture(m_image);
                if (!desc.Empty())
                {
                    desc.Bind(0, m_presetState.texturedShader);
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

            for (int i = 1; i < sides + 1; i++)
            {
                const float cornerProgress = static_cast<float>(i - 1) / static_cast<float>(sides);
                const float angle = cornerProgress * pi * 2.0f + static_cast<float>(*m_perFrameContext.tex_ang) + pi * 0.25f;

                vertexData[i].u = 0.5f + 0.5f * cosf(angle) / static_cast<float>(*m_perFrameContext.tex_zoom) * textureAspectY;
                vertexData[i].v = 1.0f - (0.5f - 0.5f * sinf(angle) / static_cast<float>(*m_perFrameContext.tex_zoom)); // Vertical flip required!
            }

            vertexData[sides + 1] = vertexData[1];

            glBindBuffer(GL_ARRAY_BUFFER, m_vboIdTextured);

            glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedPoint) * (sides + 2), vertexData.data(), GL_DYNAMIC_DRAW);

            glBindVertexArray(m_vaoIdTextured);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sides + 2);
            glBindVertexArray(0);

            glBindTexture(GL_TEXTURE_2D, 0);
            Sampler::Unbind(0);
        }
        else
        {
            // Untextured (creates a color gradient: center=r/g/b/a to border=r2/b2/g2/a2)
            glBindBuffer(GL_ARRAY_BUFFER, m_vboIdUntextured);

            glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedPoint) * (sides + 2), vertexData.data(), GL_DYNAMIC_DRAW);

            m_presetState.untexturedShader.Bind();
            m_presetState.untexturedShader.SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);

            glBindVertexArray(m_vaoIdUntextured);
            glDrawArrays(GL_TRIANGLE_FAN, 0, sides + 2);
            glBindVertexArray(0);
        }

        if (*m_perFrameContext.border_a > 0.0001f)
        {
            std::vector<ShapeVertex> points(sides);

            for (int i = 0; i < sides; i++)
            {
                points[i].x = vertexData[i + 1].x;
                points[i].y = vertexData[i + 1].y;
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
                        for (auto j = 0; j < sides; j++)
                        {
                            points[j].x += incrementX;
                        }
                        break;

                    case 2:
                        for (auto j = 0; j < sides; j++)
                        {
                            points[j].y += incrementY;
                        }
                        break;

                    case 3:
                        for (auto j = 0; j < sides; j++)
                        {
                            points[j].x -= incrementX;
                        }
                        break;
                }

                glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(Point) * sides), points.data(), GL_DYNAMIC_DRAW);
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

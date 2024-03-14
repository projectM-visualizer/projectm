#include "FinalComposite.hpp"

#include "PresetState.hpp"

#include <cstddef>

#ifdef MILKDROP_PRESET_DEBUG
#include <iostream>
#endif

namespace libprojectM {
namespace MilkdropPreset {

static std::string const defaultCompositeShader = "shader_body\n{\nret = tex2D(sampler_main, uv).xyz;\n}";

FinalComposite::FinalComposite()
{
    RenderItem::Init();
}

void FinalComposite::InitVertexAttrib()
{
    glGenBuffers(1, &m_elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), nullptr);                                               // Positions
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, r)));      // Colors
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, u)));      // Textures
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, radius))); // Radius/Angle

    // Pre-allocate vertex and index buffers
    glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * vertexCount, m_vertices.data(), GL_STREAM_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * m_indices.size(), m_indices.data(), GL_STREAM_DRAW);
}

void FinalComposite::LoadCompositeShader(const PresetState& presetState)
{
    if (presetState.compositeShaderVersion > 0)
    {
        m_compositeShader = std::make_unique<MilkdropShader>(MilkdropShader::ShaderType::CompositeShader);
        if (!presetState.compositeShader.empty())
        {
            try
            {
                m_compositeShader->LoadCode(presetState.compositeShader);
#ifdef MILKDROP_PRESET_DEBUG
                std::cerr << "[Composite Shader] Loaded composite shader code." << std::endl;
#endif
            }
            catch (Renderer::ShaderException& ex)
            {
#ifdef MILKDROP_PRESET_DEBUG
                std::cerr << "[Composite Shader] Error loading composite warp shader code:" << ex.message() << std::endl;
                std::cerr << "[Composite Shader] Using fallback shader." << std::endl;
#else
                (void)ex; // silence unused parameter warning
#endif
                // Fall back to default shader
                m_compositeShader = std::make_unique<MilkdropShader>(MilkdropShader::ShaderType::CompositeShader);
                m_compositeShader->LoadCode(defaultCompositeShader);
            }
        }
        else
        {
            m_compositeShader->LoadCode(defaultCompositeShader);
#ifdef MILKDROP_PRESET_DEBUG
            std::cerr << "[Composite Shader] Loaded default composite shader code." << std::endl;
#endif
        }
    }
    else
    {
        // Video echo OR gamma adjustment with random hue.
        m_videoEcho = std::make_unique<VideoEcho>(presetState);
        if (presetState.brighten ||
            presetState.darken ||
            presetState.solarize ||
            presetState.invert)
        {
            m_filters = std::make_unique<Filters>(presetState);
        }
    }
}

void FinalComposite::CompileCompositeShader(PresetState& presetState)
{
    if (m_compositeShader)
    {
        try
        {
            m_compositeShader->LoadTexturesAndCompile(presetState);
#ifdef MILKDROP_PRESET_DEBUG
            std::cerr << "[Composite Shader] Successfully compiled composite shader code." << std::endl;
#endif
        }
        catch (Renderer::ShaderException& ex)
        {
#ifdef MILKDROP_PRESET_DEBUG
            std::cerr << "[Composite Shader] Error compiling composite warp shader code:" << ex.message() << std::endl;
            std::cerr << "[Composite Shader] Using fallback shader." << std::endl;
#else
                (void)ex; // silence unused parameter warning
#endif
            // Fall back to default shader
            m_compositeShader = std::make_unique<MilkdropShader>(MilkdropShader::ShaderType::CompositeShader);
            m_compositeShader->LoadCode(defaultCompositeShader);
            m_compositeShader->LoadTexturesAndCompile(presetState);
        }
    }
}

void FinalComposite::Draw(const PresetState& presetState, const PerFrameContext& perFrameContext)
{
    if (m_compositeShader)
    {
        InitializeMesh(presetState);
        ApplyHueShaderColors(presetState);

        // Render the grid
        glDisable(GL_BLEND);
        glBindVertexArray(m_vaoID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(MeshVertex) * vertexCount, m_vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_compositeShader->LoadVariables(presetState, perFrameContext);

        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        // Apply old-school filters
        m_videoEcho->Draw();
        if (m_filters)
        {
            m_filters->Draw();
        }
    }

    glBindVertexArray(0);
    Renderer::Shader::Unbind();
}

auto FinalComposite::HasCompositeShader() const -> bool
{
    return m_compositeShader != nullptr;
}

void FinalComposite::InitializeMesh(const PresetState& presetState)
{
    if (m_viewportWidth == presetState.renderContext.viewportSizeX &&
        m_viewportHeight == presetState.renderContext.viewportSizeY)
    {
        return;
    }

    float const halfTexelWidth = 0.5f / static_cast<float>(presetState.renderContext.viewportSizeX);
    float const halfTexelHeight = 0.5f / static_cast<float>(presetState.renderContext.viewportSizeY);

    float const dividedByX = 1.0f / static_cast<float>(compositeGridWidth - 2);
    float const dividedByY = 1.0f / static_cast<float>(compositeGridHeight - 2);

    constexpr float PI = 3.1415926535898f;

    for (int gridY = 0; gridY < compositeGridHeight; gridY++)
    {
        int const gridY2 = gridY - gridY / (compositeGridHeight / 2);
        float const v = SquishToCenter(gridY2 * dividedByY, 3.0f);
        float const sy = -(v * 2.0f - 1.0f);

        for (int gridX = 0; gridX < compositeGridWidth; gridX++)
        {
            int const gridX2 = gridX - gridX / (compositeGridWidth / 2);
            float const u = SquishToCenter(gridX2 * dividedByX, 3.0f);
            float const sx = u * 2.0f - 1.0f;

            auto& vertex = m_vertices.at(gridX + gridY * compositeGridWidth);

            vertex.x = sx;
            vertex.y = sy;

            float rad;
            float ang;
            UvToMathSpace(presetState.renderContext.aspectX,
                          presetState.renderContext.aspectY,
                          u, v, rad, ang);

            // fix-ups:
            if (gridX == compositeGridWidth / 2 - 1)
            {
                if (gridY < compositeGridHeight / 2 - 1)
                {
                    ang = PI * 1.5f;
                }
                else if (gridY == compositeGridHeight / 2 - 1)
                {
                    ang = PI * 1.25f;
                }
                else if (gridY == compositeGridHeight / 2)
                {
                    ang = PI * 0.75f;
                }
                else
                {
                    ang = PI * 0.5f;
                }
            }
            else if (gridX == compositeGridWidth / 2)
            {
                if (gridY < compositeGridHeight / 2 - 1)
                {
                    ang = PI * 1.5f;
                }
                else if (gridY == compositeGridHeight / 2 - 1)
                {
                    ang = PI * 1.75f;
                }
                else if (gridY == compositeGridHeight / 2)
                {
                    ang = PI * 0.25f;
                }
                else
                {
                    ang = PI * 0.5f;
                }
            }
            else if (gridY == compositeGridHeight / 2 - 1)
            {
                if (gridX < compositeGridWidth / 2 - 1)
                {
                    ang = PI * 1.0f;
                }
                else
                {
                    ang = PI * 2.0f;
                }
            }
            else if (gridY == compositeGridHeight / 2)
            {
                if (gridX < compositeGridWidth / 2 - 1)
                {
                    ang = PI * 1.0f;
                }
                else
                {
                    ang = PI * 0.0f;
                }
            }
            vertex.u = u + halfTexelWidth;
            vertex.v = v + halfTexelHeight;

            vertex.radius = rad;
            vertex.angle = ang;
        }
    }

    // build index list for final composite blit -
    // order should be friendly for interpolation of 'ang' value!
    int currentIndex = 0;
    for (int gridY = 0; gridY < compositeGridHeight - 1; gridY++)
    {
        if (gridY == compositeGridHeight / 2 - 1)
        {
            continue;
        }

        for (int gridX = 0; gridX < compositeGridWidth - 1; gridX++)
        {
            if (gridX == compositeGridWidth / 2 - 1)
            {
                continue;
            }

            bool const leftHalf = (gridX < compositeGridWidth / 2);
            bool const topHalf = (gridY < compositeGridHeight / 2);
            bool const center4 = ((gridX == compositeGridWidth / 2) &&
                                  (gridY == compositeGridHeight / 2));

            if ((static_cast<int>(leftHalf) + static_cast<int>(topHalf) + static_cast<int>(center4)) % 2 == 1)
            {
                m_indices[currentIndex + 0] = gridY * compositeGridWidth + gridX;
                m_indices[currentIndex + 1] = gridY * compositeGridWidth + gridX + 1;
                m_indices[currentIndex + 2] = (gridY + 1) * compositeGridWidth + gridX + 1;
                m_indices[currentIndex + 3] = (gridY + 1) * compositeGridWidth + gridX + 1;
                m_indices[currentIndex + 4] = (gridY + 1) * compositeGridWidth + gridX;
                m_indices[currentIndex + 5] = gridY * compositeGridWidth + gridX;
            }
            else
            {
                m_indices[currentIndex + 0] = (gridY + 1) * compositeGridWidth + (gridX);
                m_indices[currentIndex + 1] = (gridY) *compositeGridWidth + (gridX);
                m_indices[currentIndex + 2] = (gridY) *compositeGridWidth + (gridX + 1);
                m_indices[currentIndex + 3] = (gridY) *compositeGridWidth + (gridX + 1);
                m_indices[currentIndex + 4] = (gridY + 1) * compositeGridWidth + (gridX + 1);
                m_indices[currentIndex + 5] = (gridY + 1) * compositeGridWidth + (gridX);
            }

            currentIndex += 6;
        }
    }

    // Store indices.
    // ToDo: Probably don't need to store m_indices
    glBindVertexArray(m_vaoID);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(int) * m_indices.size(), m_indices.data());
    glBindVertexArray(0);
}

float FinalComposite::SquishToCenter(float x, float exponent)
{
    if (x > 0.5f)
    {
        return powf(x * 2.0f - 1.0f, exponent) * 0.5f + 0.5f;
    }

    return (1.0f - powf(1.0f - x * 2.0f, exponent)) * 0.5f;
}

void FinalComposite::UvToMathSpace(float aspectX, float aspectY,
                                   float u, float v, float& rad, float& ang)
{
    // (screen space = -1..1 on both axes; corresponds to UV space)
    // uv space = [0..1] on both axes
    // "math" space = what the preset authors are used to:
    //      upper left = [0,0]
    //      bottom right = [1,1]
    //      rad == 1 at corners of screen
    //      ang == 0 at three o'clock, and increases counter-clockwise (to 6.28).
    float const px = (u * 2.0f - 1.0f) * aspectX; // probably 1.0
    float const py = (v * 2.0f - 1.0f) * aspectY; // probably <1

    rad = sqrtf(px * px + py * py) / sqrtf(aspectX * aspectX + aspectY * aspectY);
    ang = atan2f(py, px);
    if (ang < 0)
    {
        ang += 6.2831853071796f;
    }
}

void FinalComposite::ApplyHueShaderColors(const PresetState& presetState)
{
    std::array<std::array<float, 3>, 4> shade = {{{1.0f, 1.0f, 1.0f},
                                                  {1.0f, 1.0f, 1.0f},
                                                  {1.0f, 1.0f, 1.0f},
                                                  {1.0f, 1.0f, 1.0f}}};

    for (int i = 0; i < 4; i++)
    {
        auto const indexFloat = static_cast<float>(i);
        shade[i][0] = 0.6f + 0.3f * sinf(presetState.renderContext.time * 30.0f * 0.0143f + 3 + indexFloat * 21 + presetState.hueRandomOffsets[3]);
        shade[i][1] = 0.6f + 0.3f * sinf(presetState.renderContext.time * 30.0f * 0.0107f + 1 + indexFloat * 13 + presetState.hueRandomOffsets[1]);
        shade[i][2] = 0.6f + 0.3f * sinf(presetState.renderContext.time * 30.0f * 0.0129f + 6 + indexFloat * 9 + presetState.hueRandomOffsets[2]);

        float const max = std::max(shade[i][0], std::max(shade[i][1], shade[i][2]));

        for (int k = 0; k < 3; k++)
        {
            shade[i][k] /= max;
            shade[i][k] = 0.5f + 0.5f * shade[i][k];
        }
    }

    // Interpolate and apply to all grid vertices.
    for (int gridY = 0; gridY < compositeGridHeight; gridY++)
    {
        for (int gridX = 0; gridX < compositeGridWidth; gridX++)
        {
            auto& vertex = m_vertices[gridX + gridY * compositeGridWidth];
            float x = vertex.x * 0.5f + 0.5f;
            float y = vertex.y * 0.5f + 0.5f;

            std::array<float, 3> color{{1.0f, 1.0f, 1.0f}};
            for (int col = 0; col < 3; col++)
            {
                color[col] = shade[0][col] * (x) * (y) +
                             shade[1][col] * (1 - x) * (y) +
                             shade[2][col] * (x) * (1 - y) +
                             shade[3][col] * (1 - x) * (1 - y);
            }

            vertex.r = color[0];
            vertex.g = color[1];
            vertex.b = color[2];
            vertex.a = 1.0f;
        }
    }
}

} // namespace MilkdropPreset
} // namespace libprojectM

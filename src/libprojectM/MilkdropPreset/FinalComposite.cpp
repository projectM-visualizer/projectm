#include "FinalComposite.hpp"

#include "PresetState.hpp"

#include <Logging.hpp>
#include <Renderer/BlendMode.hpp>

#include <cstddef>

namespace libprojectM {
namespace MilkdropPreset {

static std::string const defaultCompositeShader = "shader_body\n{\nret = tex2D(sampler_main, uv).xyz;\n}";

FinalComposite::FinalComposite()
    : m_compositeMesh(Renderer::VertexBufferUsage::StreamDraw, true, true)
{
    Init();
}

    // Add attribute array for radius and angle information to the mesh.
    m_compositeMesh.Bind();
    m_radiusAngle.Bind();
    m_radiusAngle.Resize(vertexCount);
    m_radiusAngle.InitializeAttributePointer(3);
    Renderer::VertexBuffer<Renderer::Point>::SetEnableAttributeArray(3, true);

    // Pre-allocate vertex and index buffers
    m_compositeMesh.SetVertexCount(vertexCount);
    m_compositeMesh.Indices().Resize(indexCount);
    m_compositeMesh.Update();

    Renderer::Mesh::Unbind();
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
                LOG_DEBUG("[FinalComposite] Successfully loaded composite shader code.");
            }
            catch (Renderer::ShaderException& ex)
            {
                LOG_WARN("[FinalComposite] Error loading composite warp shader code: " + ex.message() + " - Using fallback shader.");

                // Fall back to default shader
                m_compositeShader = std::make_unique<MilkdropShader>(MilkdropShader::ShaderType::CompositeShader);
                m_compositeShader->LoadCode(defaultCompositeShader);
            }
        }
        else
        {
            LOG_DEBUG("[FinalComposite] No composite shader code in preset, loading default.");
            m_compositeShader->LoadCode(defaultCompositeShader);
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
            LOG_DEBUG("[FinalComposite] Successfully compiled composite shader code.");
        }
        catch (Renderer::ShaderException& ex)
        {
            LOG_WARN("[FinalComposite] Error compiling composite warp shader code - Using fallback shader.");

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
        Renderer::BlendMode::SetBlendActive(false);
        m_compositeShader->LoadVariables(presetState, perFrameContext);

        m_compositeMesh.Draw();
        Renderer::Mesh::Unbind();
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

    auto& vertices = m_compositeMesh.Vertices();
    auto& uvs = m_compositeMesh.UVs();

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

            const size_t vertexIndex = gridX + gridY * compositeGridWidth;

            vertices[vertexIndex] = {sx, sy};

            float rad{};
            float ang{};
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
            uvs[vertexIndex] = {u + halfTexelWidth,
                                v + halfTexelHeight};

            m_radiusAngle[vertexIndex] = {rad, ang};
        }
    }

    // build index list for final composite blit -
    // order should be friendly for interpolation of 'ang' value!
    auto& indices = m_compositeMesh.Indices();
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
                indices[currentIndex + 0] = gridY * compositeGridWidth + gridX;
                indices[currentIndex + 1] = gridY * compositeGridWidth + gridX + 1;
                indices[currentIndex + 2] = (gridY + 1) * compositeGridWidth + gridX + 1;
                indices[currentIndex + 3] = (gridY + 1) * compositeGridWidth + gridX + 1;
                indices[currentIndex + 4] = (gridY + 1) * compositeGridWidth + gridX;
                indices[currentIndex + 5] = gridY * compositeGridWidth + gridX;
            }
            else
            {
                indices[currentIndex + 0] = (gridY + 1) * compositeGridWidth + (gridX);
                indices[currentIndex + 1] = (gridY) *compositeGridWidth + (gridX);
                indices[currentIndex + 2] = (gridY) *compositeGridWidth + (gridX + 1);
                indices[currentIndex + 3] = (gridY) *compositeGridWidth + (gridX + 1);
                indices[currentIndex + 4] = (gridY + 1) * compositeGridWidth + (gridX + 1);
                indices[currentIndex + 5] = (gridY + 1) * compositeGridWidth + (gridX);
            }

            currentIndex += 6;
        }
    }

    // Update mesh geometry and indices.
    m_compositeMesh.Update();
    m_radiusAngle.Update();
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
    auto& vertices = m_compositeMesh.Vertices();
    auto& colors = m_compositeMesh.Colors();

    for (int gridY = 0; gridY < compositeGridHeight; gridY++)
    {
        for (int gridX = 0; gridX < compositeGridWidth; gridX++)
        {
            auto vertexIndex = gridX + gridY * compositeGridWidth;
            float x = vertices[vertexIndex].X() * 0.5f + 0.5f;
            float y = vertices[vertexIndex].Y() * 0.5f + 0.5f;

            std::array<float, 3> color{{1.0f, 1.0f, 1.0f}};
            for (int col = 0; col < 3; col++)
            {
                color[col] = shade[0][col] * (x) * (y) +
                             shade[1][col] * (1 - x) * (y) +
                             shade[2][col] * (x) * (1 - y) +
                             shade[3][col] * (1 - x) * (1 - y);
            }

            colors[vertexIndex] = {color[0],
                                   color[1],
                                   color[2],
                                   1.0f};
        }
    }

    // Only update color buffer.
    m_compositeMesh.Bind();
    m_compositeMesh.Colors().Update();
}

} // namespace MilkdropPreset
} // namespace libprojectM

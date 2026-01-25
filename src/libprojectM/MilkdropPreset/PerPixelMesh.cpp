#include "PerPixelMesh.hpp"

#include "MilkdropShader.hpp"
#include "MilkdropStaticShaders.hpp"
#include "PerFrameContext.hpp"
#include "PerPixelContext.hpp"
#include "PresetState.hpp"

#include <Logging.hpp>
#include <Renderer/BlendMode.hpp>
#include <Renderer/ShaderCache.hpp>

#include <algorithm>
#include <cmath>

namespace libprojectM {
namespace MilkdropPreset {

PerPixelMesh::PerPixelMesh()
    : m_warpMesh(Renderer::VertexBufferUsage::StreamDraw)
{
    m_warpMesh.SetRenderPrimitiveType(Renderer::Mesh::PrimitiveType::Triangles);

    m_warpMesh.Bind();
    m_radiusAngleBuffer.Bind();
    m_zoomRotWarpBuffer.Bind();
    m_centerBuffer.Bind();
    m_distanceBuffer.Bind();
    m_stretchBuffer.Bind();

    m_radiusAngleBuffer.InitializeAttributePointer(3);
    m_zoomRotWarpBuffer.InitializeAttributePointer(4);
    m_centerBuffer.InitializeAttributePointer(5);
    m_distanceBuffer.InitializeAttributePointer(6);
    m_stretchBuffer.InitializeAttributePointer(7);

    Renderer::VertexBuffer<Renderer::Point>::SetEnableAttributeArray(3, true);
    Renderer::VertexBuffer<Renderer::Point>::SetEnableAttributeArray(4, true);
    Renderer::VertexBuffer<Renderer::Point>::SetEnableAttributeArray(5, true);
    Renderer::VertexBuffer<Renderer::Point>::SetEnableAttributeArray(6, true);
    Renderer::VertexBuffer<Renderer::Point>::SetEnableAttributeArray(7, true);

    Renderer::Mesh::Unbind();
}

void PerPixelMesh::LoadWarpShader(const PresetState& presetState)
{
    // Compile warp shader if preset specifies one.
    if (presetState.warpShaderVersion > 0)
    {
        if (!presetState.warpShader.empty())
        {
            try
            {
                m_warpShader = std::make_unique<MilkdropShader>(MilkdropShader::ShaderType::WarpShader);
                m_warpShader->LoadCode(presetState.warpShader);
                LOG_DEBUG("[PerPixelMesh] Successfully loaded preset warp shader code.");
            }
            catch (Renderer::ShaderException& ex)
            {
                LOG_ERROR("[PerPixelMesh] Error loading warp shader code:" + ex.message());
                LOG_DEBUG("[PerPixelMesh] Warp shader code:\n" + presetState.warpShader);
                m_warpShader.reset();
            }
        }
    }
}

void PerPixelMesh::CompileWarpShader(PresetState& presetState)
{
    if (m_warpShader)
    {
        try
        {
            m_warpShader->LoadTexturesAndCompile(presetState);
            LOG_DEBUG("[PerPixelMesh] Successfully compiled warp shader code.");
        }
        catch (Renderer::ShaderException&)
        {
            LOG_ERROR("[PerPixelMesh] Error compiling warp shader code.");
            m_warpShader.reset();
        }
    }
}

void PerPixelMesh::Draw(const PresetState& presetState,
                        const PerFrameContext& perFrameContext,
                        PerPixelContext& perPixelContext)
{
    if (presetState.renderContext.viewportSizeX == 0 ||
        presetState.renderContext.viewportSizeY == 0 ||
        presetState.renderContext.perPixelMeshX == 0 ||
        presetState.renderContext.perPixelMeshY == 0)
    {
        return;
    }

    // Initialize or recreate the mesh (if grid size changed)
    InitializeMesh(presetState);

    // Calculate the dynamic movement values
    CalculateMesh(presetState, perFrameContext, perPixelContext);

    // Render the resulting mesh.
    WarpedBlit(presetState, perFrameContext);
}

void PerPixelMesh::InitializeMesh(const PresetState& presetState)
{
    if (m_gridSizeX != presetState.renderContext.perPixelMeshX ||
        m_gridSizeY != presetState.renderContext.perPixelMeshY)
    {
        m_gridSizeX = presetState.renderContext.perPixelMeshX;
        m_gridSizeY = presetState.renderContext.perPixelMeshY;

        // Grid size has changed, resize buffers accordingly
        const size_t vertexCount = (m_gridSizeX + 1) * (m_gridSizeY + 1);

        m_warpMesh.SetVertexCount(vertexCount);
        m_radiusAngleBuffer.Resize(vertexCount);
        m_zoomRotWarpBuffer.Resize(vertexCount);
        m_centerBuffer.Resize(vertexCount);
        m_distanceBuffer.Resize(vertexCount);
        m_stretchBuffer.Resize(vertexCount);

        m_warpMesh.Indices().Resize(m_gridSizeX * m_gridSizeY * 6);
    }
    else if (m_viewportWidth == presetState.renderContext.viewportSizeX &&
             m_viewportHeight == presetState.renderContext.viewportSizeY)
    {
        // Nothing changed, just go on to the dynamic calculation.
        return;
    }

    const float aspectX = presetState.renderContext.aspectX;
    const float aspectY = presetState.renderContext.aspectY;

    // Either viewport size or mesh size changed, reinitialize the vertices.
    auto& vertices = m_warpMesh.Vertices();
    int vertexIndex{0};
    for (int gridY = 0; gridY <= m_gridSizeY; gridY++)
    {
        for (int gridX = 0; gridX <= m_gridSizeX; gridX++)
        {
            const float x = static_cast<float>(gridX) / static_cast<float>(m_gridSizeX) * 2.0f - 1.0f;
            const float y = static_cast<float>(gridY) / static_cast<float>(m_gridSizeY) * 2.0f - 1.0f;
            vertices[vertexIndex] = {x, y};

            // Milkdrop uses sqrtf, but hypotf is probably safer.
            m_radiusAngleBuffer[vertexIndex].radius = hypotf(x * aspectX, y * aspectY);
            if (gridY == m_gridSizeY / 2 && gridX == m_gridSizeX / 2)
            {
                m_radiusAngleBuffer[vertexIndex].angle = 0.0f;
            }
            else
            {
                m_radiusAngleBuffer[vertexIndex].angle = atan2f(y * aspectY, x * aspectX);
            }

            vertexIndex++;
        }
    }

    // Generate triangle lists for drawing the main warp mesh
    int vertexListIndex{0};
    for (int quadrant = 0; quadrant < 4; quadrant++)
    {
        for (int slice = 0; slice < m_gridSizeY / 2; slice++)
        {
            for (int gridX = 0; gridX < m_gridSizeX / 2; gridX++)
            {
                int xReference = gridX;
                int yReference = slice;

                if ((quadrant & 1) != 0)
                {
                    xReference = m_gridSizeX - 1 - xReference;
                }
                if ((quadrant & 2) != 0)
                {
                    yReference = m_gridSizeY - 1 - yReference;
                }

                int const vertex = xReference + yReference * (m_gridSizeX + 1);

                // 0 - 1      3
                //   /      /
                // 2      4 - 5
                m_warpMesh.Indices()[vertexListIndex++] = vertex;
                m_warpMesh.Indices()[vertexListIndex++] = vertex + 1;
                m_warpMesh.Indices()[vertexListIndex++] = vertex + m_gridSizeX + 1;
                m_warpMesh.Indices()[vertexListIndex++] = vertex + 1;
                m_warpMesh.Indices()[vertexListIndex++] = vertex + m_gridSizeX + 1;
                m_warpMesh.Indices()[vertexListIndex++] = vertex + m_gridSizeX + 2;
            }
        }
    }
}

void PerPixelMesh::CalculateMesh(const PresetState& presetState, const PerFrameContext& perFrameContext, PerPixelContext& perPixelContext)
{
    // Cache some per-frame values as floats
    float zoom = static_cast<float>(*perFrameContext.zoom);
    float zoomExp = static_cast<float>(*perFrameContext.zoomexp);
    float rot = static_cast<float>(*perFrameContext.rot);
    float warp = static_cast<float>(*perFrameContext.warp);
    float cx = static_cast<float>(*perFrameContext.cx);
    float cy = static_cast<float>(*perFrameContext.cy);
    float dx = static_cast<float>(*perFrameContext.dx);
    float dy = static_cast<float>(*perFrameContext.dy);
    float sx = static_cast<float>(*perFrameContext.sx);
    float sy = static_cast<float>(*perFrameContext.sy);

    int vertex = 0;

    // Can't make this multithreaded as per-pixel code may use gmegabuf or regXX vars.
    auto& vertices = m_warpMesh.Vertices();
    for (int y = 0; y <= m_gridSizeY; y++)
    {
        for (int x = 0; x <= m_gridSizeX; x++)
        {
            auto& curVertex = vertices[vertex];
            auto& curRadiusAngle = m_radiusAngleBuffer[vertex];
            auto& curZoomRotWarp = m_zoomRotWarpBuffer[vertex];
            auto& curCenter = m_centerBuffer[vertex];
            auto& curDistance = m_distanceBuffer[vertex];
            auto& curStretch = m_stretchBuffer[vertex];

            // Execute per-vertex/per-pixel code if the preset uses it.
            if (perPixelContext.perPixelCodeHandle)
            {
                *perPixelContext.x = static_cast<double>(curVertex.X() * 0.5f * presetState.renderContext.aspectX + 0.5f);
                *perPixelContext.y = static_cast<double>(curVertex.Y() * 0.5f * presetState.renderContext.aspectY + 0.5f);
                *perPixelContext.rad = static_cast<double>(curRadiusAngle.radius);
                *perPixelContext.ang = static_cast<double>(-curRadiusAngle.angle);
                *perPixelContext.zoom = static_cast<double>(*perFrameContext.zoom);
                *perPixelContext.zoomexp = static_cast<double>(*perFrameContext.zoomexp);
                *perPixelContext.rot = static_cast<double>(*perFrameContext.rot);
                *perPixelContext.warp = static_cast<double>(*perFrameContext.warp);
                *perPixelContext.cx = static_cast<double>(*perFrameContext.cx);
                *perPixelContext.cy = static_cast<double>(*perFrameContext.cy);
                *perPixelContext.dx = static_cast<double>(*perFrameContext.dx);
                *perPixelContext.dy = static_cast<double>(*perFrameContext.dy);
                *perPixelContext.sx = static_cast<double>(*perFrameContext.sx);
                *perPixelContext.sy = static_cast<double>(*perFrameContext.sy);

                perPixelContext.ExecutePerPixelCode();

                curZoomRotWarp.zoom = static_cast<float>(*perPixelContext.zoom);
                curZoomRotWarp.zoomExp = static_cast<float>(*perPixelContext.zoomexp);
                curZoomRotWarp.rot = static_cast<float>(*perPixelContext.rot);
                curZoomRotWarp.warp = static_cast<float>(*perPixelContext.warp);
                curCenter = {static_cast<float>(*perPixelContext.cx),
                             static_cast<float>(*perPixelContext.cy)};
                curDistance = {static_cast<float>(*perPixelContext.dx),
                               static_cast<float>(*perPixelContext.dy)};
                curStretch = {static_cast<float>(*perPixelContext.sx),
                              static_cast<float>(*perPixelContext.sy)};
            }
            else
            {
                curZoomRotWarp.zoom = zoom;
                curZoomRotWarp.zoomExp = zoomExp;
                curZoomRotWarp.rot = rot;
                curZoomRotWarp.warp = warp;
                curCenter = { cx, cy};
                curDistance = {dx, dy};
                curStretch = {sx, sy};
            }

            vertex++;
        }
    }

    m_warpMesh.Update();
    m_radiusAngleBuffer.Update();
    m_zoomRotWarpBuffer.Update();
    m_centerBuffer.Update();
    m_distanceBuffer.Update();
    m_stretchBuffer.Update();
}

void PerPixelMesh::WarpedBlit(const PresetState& presetState,
                              const PerFrameContext& perFrameContext)
{
    // Warp stuff
    float const warpTime = presetState.renderContext.time * presetState.warpAnimSpeed;
    float const warpScaleInverse = 1.0f / presetState.warpScale;
    glm::vec4 const warpFactors{
        11.68f + 4.0f * cosf(warpTime * 1.413f + 10),
        8.77f + 3.0f * cosf(warpTime * 1.113f + 7),
        10.54f + 3.0f * cosf(warpTime * 1.233f + 3),
        11.49f + 4.0f * cosf(warpTime * 0.933f + 5),
    };

    // Texel alignment
    glm::vec2 const texelOffsets{presetState.renderContext.texelOffsetX / static_cast<float>(presetState.renderContext.viewportSizeX),
                                 presetState.renderContext.texelOffsetY / static_cast<float>(presetState.renderContext.viewportSizeY)};

    // Decay
    float decay = std::min(static_cast<float>(*perFrameContext.decay), 1.0f);

    // No blending between presets here, so we make sure blending is disabled.
    Renderer::BlendMode::SetBlendActive(false);

    if (!m_warpShader)
    {
        auto perPixelMeshShader = GetDefaultWarpShader(presetState);
        perPixelMeshShader->Bind();
        perPixelMeshShader->SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
        perPixelMeshShader->SetUniformInt("texture_sampler", 0);
        perPixelMeshShader->SetUniformFloat4("aspect", {presetState.renderContext.aspectX,
                                                        presetState.renderContext.aspectY,
                                                        presetState.renderContext.invAspectX,
                                                        presetState.renderContext.invAspectY});
        perPixelMeshShader->SetUniformFloat("warpTime", warpTime);
        perPixelMeshShader->SetUniformFloat("warpScaleInverse", warpScaleInverse);
        perPixelMeshShader->SetUniformFloat4("warpFactors", warpFactors);
        perPixelMeshShader->SetUniformFloat2("texelOffset", texelOffsets);
        perPixelMeshShader->SetUniformFloat("decay", decay);
    }
    else
    {
        m_warpShader->LoadVariables(presetState, perFrameContext);
        auto& shader = m_warpShader->Shader();
        shader.SetUniformFloat4("aspect", {presetState.renderContext.aspectX,
                                           presetState.renderContext.aspectY,
                                           presetState.renderContext.invAspectX,
                                           presetState.renderContext.invAspectY});
        shader.SetUniformFloat("warpTime", warpTime);
        shader.SetUniformFloat("warpScaleInverse", warpScaleInverse);
        shader.SetUniformFloat4("warpFactors", warpFactors);
        shader.SetUniformFloat2("texelOffset", texelOffsets);
        shader.SetUniformFloat("decay", decay);
    }

    assert(!presetState.mainTexture.expired());
    presetState.mainTexture.lock()->Bind(0);

    // Set wrap mode and bind the sampler to get interpolation right.
    if (*perFrameContext.wrap > 0.0001f)
    {
        m_perPixelSampler.WrapMode(GL_REPEAT);
    }
    else
    {
        m_perPixelSampler.WrapMode(GL_CLAMP_TO_EDGE);
    }
    m_perPixelSampler.Bind(0);

    m_warpMesh.Draw();

    Renderer::Mesh::Unbind();
    Renderer::Sampler::Unbind(0);
    Renderer::Shader::Unbind();
}

auto PerPixelMesh::GetDefaultWarpShader(const PresetState& presetState) -> std::shared_ptr<Renderer::Shader>
{
    auto perPixelMeshShader = m_perPixelMeshShader.lock();
    if (perPixelMeshShader)
    {
        return perPixelMeshShader;
    }

    perPixelMeshShader = presetState.renderContext.shaderCache->Get("milkdrop_default_warp_shader");
    if (perPixelMeshShader)
    {
        return perPixelMeshShader;
    }

    auto staticShaders = libprojectM::MilkdropPreset::MilkdropStaticShaders::Get();

    perPixelMeshShader = std::make_shared<Renderer::Shader>();
    perPixelMeshShader->CompileProgram(staticShaders->GetPresetWarpVertexShader(),
                                       staticShaders->GetPresetWarpFragmentShader());

    presetState.renderContext.shaderCache->Insert("milkdrop_default_warp_shader", perPixelMeshShader);
    m_perPixelMeshShader = perPixelMeshShader;

    return perPixelMeshShader;
}

} // namespace MilkdropPreset
} // namespace libprojectM

#include "PerPixelMesh.hpp"

#include "MilkdropShader.hpp"
#include "PerFrameContext.hpp"
#include "PerPixelContext.hpp"
#include "PresetState.hpp"

#include <MilkdropStaticShaders.hpp>

#include <algorithm>
#include <cmath>

PerPixelMesh::PerPixelMesh()
    : RenderItem()
{
    RenderItem::Init();

    auto staticShaders = libprojectM::MilkdropPreset::MilkdropStaticShaders::Get();
    m_perPixelMeshShader.CompileProgram(staticShaders->GetPresetWarpVertexShader(),
                                        staticShaders->GetPresetWarpFragmentShader());
}

void PerPixelMesh::InitVertexAttrib()
{
    glGenVertexArrays(1, &m_vaoID);
    glGenBuffers(1, &m_vboID);

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);

    // Only position & texture coordinates are per-vertex, colors are equal all over the grid (used for decay).
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, x)));         // Position, radius & angle
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, radius)));    // Position, radius & angle
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, zoom)));      // zoom, zoom exponent, rotation & warp
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, centerX)));   // Center coord
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, distanceX))); // Distance
    glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, stretchX)));  // Stretch

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
#ifdef MILKDROP_PRESET_DEBUG
                std::cerr << "[Warp Shader] Loaded preset warp shader code." << std::endl;
#endif
            }
            catch (ShaderException& ex)
            {
#ifdef MILKDROP_PRESET_DEBUG
                std::cerr << "[Warp Shader] Error loading warp shader code:" << ex.message() << std::endl;
#endif
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
#ifdef MILKDROP_PRESET_DEBUG
            std::cerr << "[Warp Shader] Successfully compiled warp shader code." << std::endl;
#endif
        }
        catch (ShaderException& ex)
        {
#ifdef MILKDROP_PRESET_DEBUG
            std::cerr << "[Warp Shader] Error compiling warp shader code:" << ex.message() << std::endl;
#endif
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

        // Grid size has changed, reallocate vertex buffers
        m_vertices.resize((m_gridSizeX + 1) * (m_gridSizeY + 1));
        m_listIndices.resize(m_gridSizeX * m_gridSizeY * 6);
        m_drawVertices.resize(1024 * 3); // Fixed size, may scale it later depending on GPU caps.
    }
    else if (m_viewportWidth == presetState.renderContext.viewportSizeX &&
             m_viewportHeight == presetState.renderContext.viewportSizeY)
    {
        // Nothing changed, just go on to the dynamic calculation.
        return;
    }

    float aspectX = static_cast<float>(presetState.renderContext.aspectX);
    float aspectY = static_cast<float>(presetState.renderContext.aspectY);

    // Either viewport size or mesh size changed, reinitialize the vertices.
    int vertexIndex{0};
    for (int gridY = 0; gridY <= m_gridSizeY; gridY++)
    {
        for (int gridX = 0; gridX <= m_gridSizeX; gridX++)
        {
            auto& vertex = m_vertices.at(vertexIndex);

            vertex.x = static_cast<float>(gridX) / static_cast<float>(m_gridSizeX) * 2.0f - 1.0f;
            vertex.y = static_cast<float>(gridY) / static_cast<float>(m_gridSizeY) * 2.0f - 1.0f;

            // Milkdrop uses sqrtf, but hypotf is probably safer.
            vertex.radius = hypotf(vertex.x * aspectX, vertex.y * aspectY);
            if (gridY == m_gridSizeY / 2 && gridX == m_gridSizeX / 2)
            {
                vertex.angle = 0.0f;
            }
            else
            {
                vertex.angle = atan2f(vertex.y * aspectY, vertex.x * aspectX);
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
                m_listIndices.at(vertexListIndex++) = vertex;
                m_listIndices.at(vertexListIndex++) = vertex + 1;
                m_listIndices.at(vertexListIndex++) = vertex + m_gridSizeX + 1;
                m_listIndices.at(vertexListIndex++) = vertex + 1;
                m_listIndices.at(vertexListIndex++) = vertex + m_gridSizeX + 1;
                m_listIndices.at(vertexListIndex++) = vertex + m_gridSizeX + 2;
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
    for (int y = 0; y <= m_gridSizeY; y++)
    {
        for (int x = 0; x <= m_gridSizeX; x++)
        {
            auto& curVertex = m_vertices[vertex];

            // Execute per-vertex/per-pixel code if the preset uses it.
            if (perPixelContext.perPixelCodeHandle)
            {
                *perPixelContext.x = static_cast<double>(curVertex.x * 0.5f * presetState.renderContext.aspectX + 0.5f);
                *perPixelContext.y = static_cast<double>(curVertex.y * -0.5f * presetState.renderContext.aspectY + 0.5f);
                *perPixelContext.rad = static_cast<double>(curVertex.radius);
                *perPixelContext.ang = static_cast<double>(curVertex.angle);
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

                curVertex.zoom = static_cast<float>(*perPixelContext.zoom);
                curVertex.zoomExp = static_cast<float>(*perPixelContext.zoomexp);
                curVertex.rot = static_cast<float>(*perPixelContext.rot);
                curVertex.warp = static_cast<float>(*perPixelContext.warp);
                curVertex.centerX = static_cast<float>(*perPixelContext.cx);
                curVertex.centerY = static_cast<float>(*perPixelContext.cy);
                curVertex.distanceX = static_cast<float>(*perPixelContext.dx);
                curVertex.distanceY = static_cast<float>(*perPixelContext.dy);
                curVertex.stretchX = static_cast<float>(*perPixelContext.sx);
                curVertex.stretchY = static_cast<float>(*perPixelContext.sy);
            }
            else
            {
                curVertex.zoom = zoom;
                curVertex.zoomExp = zoomExp;
                curVertex.rot = rot;
                curVertex.warp = warp;
                curVertex.centerX = cx;
                curVertex.centerY = cy;
                curVertex.distanceX = dx;
                curVertex.distanceY = dy;
                curVertex.stretchX = sx;
                curVertex.stretchY = sy;
            }

            vertex++;
        }
    }
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
    glm::vec2 const texelOffsets{0.5f / static_cast<float>(presetState.renderContext.viewportSizeX),
                                 0.5f / static_cast<float>(presetState.renderContext.viewportSizeY)};

    // Decay
    float decay = static_cast<float>(*perFrameContext.decay);
    if (decay < 0.9999f)
    {
        // Milkdrop uses a GPU-specific value, either 0.0 or 2.0.
        // We'll simply assume 2, as it's the default value.
        decay = std::min(decay, (32.0f - 2.0f) / 32.0f);
    }

    // No blending between presets here, so we make sure blending is disabled.
    glDisable(GL_BLEND);

    if (!m_warpShader)
    {
        m_perPixelMeshShader.Bind();
        m_perPixelMeshShader.SetUniformMat4x4("vertex_transformation", PresetState::orthogonalProjection);
        m_perPixelMeshShader.SetUniformInt("texture_sampler", 0);
        m_perPixelMeshShader.SetUniformFloat4("aspect", {presetState.renderContext.aspectX,
                                                         presetState.renderContext.aspectY,
                                                         presetState.renderContext.invAspectX,
                                                         presetState.renderContext.invAspectY});
        m_perPixelMeshShader.SetUniformFloat("warpTime", warpTime);
        m_perPixelMeshShader.SetUniformFloat("warpScaleInverse", warpScaleInverse);
        m_perPixelMeshShader.SetUniformFloat4("warpFactors", warpFactors);
        m_perPixelMeshShader.SetUniformFloat2("texelOffset", texelOffsets);
        m_perPixelMeshShader.SetUniformFloat("decay", decay);
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

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    int trianglesPerBatch = m_drawVertices.size() / 3 - 4;
    int triangleCount = m_gridSizeX * m_gridSizeY * 2; // Two triangles per quad/grid cell.
    int sourceIndex = 0;

    while (sourceIndex < triangleCount * 3)
    {
        int trianglesQueued = 0;
        int vertex = 0;
        while (trianglesQueued < trianglesPerBatch && sourceIndex < triangleCount * 3)
        {
            // Copy one triangle/3 vertices
            for (int i = 0; i < 3; i++)
            {
                m_drawVertices[vertex++] = m_vertices[m_listIndices[sourceIndex++]];
            }

            trianglesQueued++;
        }

        if (trianglesQueued > 0)
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * trianglesQueued * 3, m_drawVertices.data(), GL_DYNAMIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, trianglesQueued * 3);
        }
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Sampler::Unbind(0);
    Shader::Unbind();
}

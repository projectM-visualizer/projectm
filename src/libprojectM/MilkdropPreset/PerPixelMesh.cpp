#include "PerPixelMesh.hpp"

#include "PerFrameContext.hpp"
#include "PerPixelContext.hpp"
#include "PresetState.hpp"

#include <algorithm>
#include <cmath>

PerPixelMesh::PerPixelMesh()
    : RenderItem()
{
    RenderItem::Init();
}

void PerPixelMesh::InitVertexAttrib()
{
}

void PerPixelMesh::Draw(const PresetState& presetState,
                        const PerFrameContext& perFrameContext,
                        PerPixelContext& perPixelContext,
                        MilkdropShader* warpShader)
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
        m_stripIndices.resize((m_gridSizeX + 2) * (m_gridSizeY * 2));
        m_listIndices.resize(m_gridSizeX * m_gridSizeY * 6);
    }
    else if (m_viewportWidth == presetState.renderContext.viewportSizeX &&
             m_viewportHeight == presetState.renderContext.viewportSizeY)
    {
        // Nothing changed, just go on to the dynamic calculation.
        return;
    }

    // Either viewport size or mesh size changed, reinitialize the vertices.
    int vertexIndex{0};
    float const texelOffsetX = 0.5f / static_cast<float>(presetState.renderContext.viewportSizeX);
    float const texelOffsetY = 0.5f / static_cast<float>(presetState.renderContext.viewportSizeY);

    for (int gridY = 0; gridY < m_gridSizeY; gridY++)
    {
        for (int gridX = 0; gridX < m_gridSizeX; gridX++)
        {
            auto& vertex = m_vertices.at(vertexIndex);

            vertex.x = static_cast<float>(gridX) / static_cast<float>(m_gridSizeX - 1);
            vertex.y = -((static_cast<float>(gridY) / static_cast<float>(m_gridSizeY - 1)) - 1.0f);

            // ToDo: Check if this works correctly, as it previously didn't use the aspect ratio, but some weird constant:
            //       hypotf((vertex.x - .5f) * 2.0f, (vertex.y - .5f) * 2.0f) * .7071067f;
            // Milkdrop uses sqrtf, but hypotf is probably safer.
            vertex.radius = hypotf(vertex.y * static_cast<float>(presetState.renderContext.aspectY),
                                   vertex.y * static_cast<float>(presetState.renderContext.aspectY));
            if (gridY == m_gridSizeY / 2 && gridX == m_gridSizeX / 2)
            {
                vertex.angle = 0.0f;
            }
            else
            {
                // ToDo: Check if this works as intended with the OpenGL coordinate frame.
                //       projectM previously used: atan2((yval - .5) * 2, (xval - .5) * 2)
                vertex.angle = atan2f(vertex.y * static_cast<float>(presetState.renderContext.aspectY),
                                      vertex.x * static_cast<float>(presetState.renderContext.aspectX));
            }

            vertex.uOrigin = vertex.x * 0.5f + 0.5f + texelOffsetX;
            vertex.vOrigin = vertex.y * 0.5f + 0.5f + texelOffsetY;

            vertexIndex++;
        }
    }

    // Generate triangle strips for the image quadrants.
    int vertexStripIndex{0};
    for (int quadrant = 0; quadrant < 4; quadrant++)
    {
        for (int slice = 0; slice < m_gridSizeY / 2; slice++)
        {
            for (int gridX = 0; gridX < m_gridSizeX + 2; gridX++)
            {
                int xReference = gridX / 2;
                int yReference = (gridX % 2) + slice;

                if ((quadrant & 1) != 0)
                {
                    xReference = m_gridSizeX - xReference;
                }
                if ((quadrant & 2) != 0)
                {
                    yReference = m_gridSizeY - yReference;
                }

                m_stripIndices.at(vertexStripIndex++) = xReference + yReference * (m_gridSizeX + 1);
            }
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
    // Warp stuff
    float const warpTime = presetState.renderContext.time * presetState.warpAnimSpeed;
    float const warpScaleInverse = 1.0f / presetState.warpScale;
    std::array<float, 4> const warpFactors{
        11.68f + 4.0f * cosf(warpTime * 1.413f + 10),
        8.77f + 3.0f * cosf(warpTime * 1.113f + 7),
        10.54f + 3.0f * cosf(warpTime * 1.233f + 3),
        11.49f + 4.0f * cosf(warpTime * 0.933f + 5),
    };

    // Texel alignment
    float const texelOffsetX = 0.5f / static_cast<float>(presetState.renderContext.viewportSizeX);
    float const texelOffsetY = 0.5f / static_cast<float>(presetState.renderContext.viewportSizeY);

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

                zoom = static_cast<float>(*perPixelContext.zoom);
                zoomExp = static_cast<float>(*perPixelContext.zoomexp);
                rot = static_cast<float>(*perPixelContext.rot);
                warp = static_cast<float>(*perPixelContext.warp);
                cx = static_cast<float>(*perPixelContext.cx);
                cy = static_cast<float>(*perPixelContext.cy);
                dx = static_cast<float>(*perPixelContext.dx);
                dy = static_cast<float>(*perPixelContext.dy);
                sx = static_cast<float>(*perPixelContext.sx);
                sy = static_cast<float>(*perPixelContext.sy);
            }

            float zoom2 = powf(zoom, powf(zoomExp, curVertex.radius * 2.0f - 1.0f));

            // Initial texture coordinates, with built-in zoom factor
            float zoom2Inverse = 1.0f / zoom2;
            float u = curVertex.x * presetState.renderContext.aspectX * 0.5f * zoom2Inverse + 0.5f;
            float v = curVertex.y * presetState.renderContext.aspectY * 0.5f * zoom2Inverse + 0.5f;

            // Stretch on X, Y
            u = (u - cx) / sx + cx;
            v = (v - cy) / sy + cy;

            // Warping
            u += warp * 0.0035f * sinf(warpTime * 0.333f + warpScaleInverse * (curVertex.x * warpFactors[0] - curVertex.y * warpFactors[3]));
            v += warp * 0.0035f * cosf(warpTime * 0.375f - warpScaleInverse * (curVertex.x * warpFactors[2] + curVertex.y * warpFactors[1]));
            u += warp * 0.0035f * cosf(warpTime * 0.753f - warpScaleInverse * (curVertex.x * warpFactors[1] - curVertex.y * warpFactors[2]));
            v += warp * 0.0035f * sinf(warpTime * 0.825f + warpScaleInverse * (curVertex.x * warpFactors[0] + curVertex.y * warpFactors[3]));

            // Rotation
            float u2 = u - cx;
            float v2 = v - cy;

            float cosRotation = cosf(rot);
            float sinRotation = sinf(rot);
            u = u2 * cosRotation - v2 * sinRotation + cx;
            v = u2 * sinRotation - v2 * cosRotation + cy;

            // Translation
            u -= dx;
            v -= dy;

            // Undo aspect ratio fix
            u = (u - 0.5f) * presetState.renderContext.invAspectX + 0.5f;
            v = (v - 0.5f) * presetState.renderContext.invAspectY + 0.5f;

            // Final half-texel translation
            u += texelOffsetX;
            v += texelOffsetY;

            curVertex.u = u;
            curVertex.v = v;

            vertex++;
        }
    }
}

void PerPixelMesh::BlitNoShader()
{
}

void PerPixelMesh::BlitShader(MilkdropShader* warpShader)
{
}

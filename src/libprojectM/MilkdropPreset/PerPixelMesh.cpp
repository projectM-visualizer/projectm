#include "PerPixelMesh.hpp"

#include "PerFrameContext.hpp"

#include <algorithm>
#include <cmath>

PerPixelMesh::PerPixelMesh()
    : RenderItem()
{
}

void PerPixelMesh::InitializeMesh(const PresetState& presetState)
{
    if (m_gridSizeX != presetState.gridX ||
        m_gridSizeY != presetState.gridY)
    {
        m_gridSizeX = presetState.gridX;
        m_gridSizeY = presetState.gridY;

        // Grid size has changed, reallocate vertex buffers
        m_vertices.resize((m_gridSizeX + 1) * (m_gridSizeY + 1));
        m_stripIndices.resize((m_gridSizeX + 2) * (m_gridSizeY + 2));
        m_listIndices.resize(m_gridSizeX * m_gridSizeY * 6);
    }
    else if (m_viewportWidth == presetState.viewportWidth &&
             m_viewportHeight == presetState.viewportHeight)
    {
        // Nothing changed, just go on to the dynamic calculation.
        return;
    }

    // Either viewport size or mesh size changed, reinitialize the vertices.
    int vertexIndex{0};
    float const texelOffsetX = 0.5f / static_cast<float>(presetState.viewportWidth);
    float const texelOffsetY = 0.5f / static_cast<float>(presetState.viewportHeight);

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
            vertex.radius = hypotf(vertex.y * static_cast<float>(presetState.aspectY),
                                   vertex.y * static_cast<float>(presetState.aspectY));
            if (gridY == m_gridSizeY / 2 && gridX == m_gridSizeX / 2)
            {
                vertex.angle = 0.0f;
            }
            else
            {
                // ToDo: Check if this works as intended with the OpenGL coordinate frame.
                //       projectM previously used: atan2((yval - .5) * 2, (xval - .5) * 2)
                vertex.angle = atan2f(vertex.y * static_cast<float>(presetState.aspectY),
                                      vertex.x * static_cast<float>(presetState.aspectX));
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

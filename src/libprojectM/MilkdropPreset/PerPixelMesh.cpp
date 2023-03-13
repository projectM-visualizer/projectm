#include "PerPixelMesh.hpp"

#include "MilkdropShader.hpp"
#include "PerFrameContext.hpp"
#include "PerPixelContext.hpp"
#include "PresetState.hpp"

#include <algorithm>
#include <cmath>

static const std::string perPixelVertexShaderGLSL120 = R"(
#version 120

#define pos vertex_position.xy
#define radius vertex_position.z
#define angle vertex_position.w
#define zoom transforms.x
#define zoomExp transforms.y
#define rot transforms.z
#define warp transforms.w

#define aspectX aspect.x
#define aspectY aspect.y
#define invAspectX aspect.z
#define invAspectY aspect.w

attribute vec4 vertex_position;
attribute vec4 transforms;
attribute vec2 center;
attribute vec2 distance;
attribute vec2 stretch;

uniform mat4 vertex_transformation;
uniform vec4 aspect;
uniform float warpTime;
uniform float warpScaleInverse;
uniform vec4 warpFactors;
uniform vec2 texelOffset;
uniform float decay;

varying vec4 fragment_color;
varying vec2 fragment_texture;

void main(){
    gl_Position = vertex_transformation * vec4(vec2(pos.x, -pos.y), 0.0, 1.0);

    float zoom2 = pow(zoom, pow(zoomExp, radius * 2.0 - 1.0));
    float zoom2Inverse = 1.0 / zoom2;

    // Initial texture coordinates, with built-in zoom factor
    vec2 uv = vec2(pos.x * aspectX * 0.5 * zoom2Inverse + 0.5,
                   -pos.y * aspectY * 0.5 * zoom2Inverse + 0.5);

    // Stretch on X, Y
    uv.x = (uv.x - center.x) / stretch.x + center.x;
    uv.y = (uv.y - center.y) / stretch.y + center.y;

    // Warping
    uv.x += warp * 0.0035 * sin(warpTime * 0.333 + warpScaleInverse * (pos.x * warpFactors.x - pos.y * warpFactors.w));
    uv.y += warp * 0.0035 * cos(warpTime * 0.375 - warpScaleInverse * (pos.x * warpFactors.z + pos.y * warpFactors.y));
    uv.x += warp * 0.0035 * cos(warpTime * 0.753 - warpScaleInverse * (pos.x * warpFactors.y - pos.y * warpFactors.z));
    uv.y += warp * 0.0035 * sin(warpTime * 0.825 + warpScaleInverse * (pos.x * warpFactors.x + pos.y * warpFactors.w));

    // Rotation
    vec2 uv2 = vec2(uv.x - center.x,
                    uv.y - center.y);

    float cosRotation = cos(rot);
    float sinRotation = sin(rot);
    uv.x = uv2.x * cosRotation - uv2.y * sinRotation + center.x;
    uv.y = uv2.x * sinRotation - uv2.y * cosRotation + center.y;

    // Translation
    uv -= distance;

    // Undo aspect ratio fix
    uv.x = (uv.x - 0.5) * invAspectX + 0.5;
    uv.y = (uv.y - 0.5) * invAspectY + 0.5;

    // Final half-texel translation
    uv += texelOffset;

    fragment_color = vec4(decay, decay, decay, 1.0);
    fragment_texture = uv;
}
)";

static const std::string perPixelFragmentShaderGLSL120 = R"(
#version 120

varying vec4 fragment_color;
varying vec2 fragment_texture;

uniform sampler2D texture_sampler;

void main(){
    gl_FragColor = fragment_color * texture2D(texture_sampler, fragment_texture);
})";

static const std::string perPixelVertexShaderGLSL330 = R"(
#version 330

#define pos vertex_position.xy
#define radius vertex_position.z
#define angle vertex_position.w
#define zoom transforms.x
#define zoomExp transforms.y
#define rot transforms.z
#define warp transforms.w

#define aspectX aspect.x
#define aspectY aspect.y
#define invAspectX aspect.z
#define invAspectY aspect.w

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec4 transforms;
layout(location = 2) in vec2 center;
layout(location = 3) in vec2 distance;
layout(location = 4) in vec2 stretch;

uniform mat4 vertex_transformation;
uniform vec4 aspect;
uniform float warpTime;
uniform float warpScaleInverse;
uniform vec4 warpFactors;
uniform vec2 texelOffset;
uniform float decay;

out vec4 fragment_color;
out vec2 fragment_texture;

void main() {
    gl_Position = vertex_transformation * vec4(vec2(pos.x, -pos.y), 0.0, 1.0);

    float zoom2 = pow(zoom, pow(zoomExp, radius * 2.0 - 1.0));
    float zoom2Inverse = 1.0 / zoom2;

    // Initial texture coordinates, with built-in zoom factor
    vec2 uv = vec2(pos.x * aspectX * 0.5 * zoom2Inverse + 0.5,
                   -pos.y * aspectY * 0.5 * zoom2Inverse + 0.5);

    // Stretch on X, Y
    uv.x = (uv.x - center.x) / stretch.x + center.x;
    uv.y = (uv.y - center.y) / stretch.y + center.y;

    // Warping
    uv.x += warp * 0.0035 * sin(warpTime * 0.333 + warpScaleInverse * (pos.x * warpFactors.x - pos.y * warpFactors.w));
    uv.y += warp * 0.0035 * cos(warpTime * 0.375 - warpScaleInverse * (pos.x * warpFactors.z + pos.y * warpFactors.y));
    uv.x += warp * 0.0035 * cos(warpTime * 0.753 - warpScaleInverse * (pos.x * warpFactors.y - pos.y * warpFactors.z));
    uv.y += warp * 0.0035 * sin(warpTime * 0.825 + warpScaleInverse * (pos.x * warpFactors.x + pos.y * warpFactors.w));

    // Rotation
    vec2 uv2 = vec2(uv.x - center.x,
                    uv.y - center.y);

    float cosRotation = cos(rot);
    float sinRotation = sin(rot);
    uv.x = uv2.x * cosRotation - uv2.y * sinRotation + center.x;
    uv.y = uv2.x * sinRotation - uv2.y * cosRotation + center.y;

    // Translation
    uv -= distance;

    // Undo aspect ratio fix
    uv.x = (uv.x - 0.5) * invAspectX + 0.5;
    uv.y = (uv.y - 0.5) * invAspectY + 0.5;

    // Final half-texel translation
    uv += texelOffset;

    fragment_color = vec4(decay, decay, decay, 1.0);
    fragment_texture = uv;
}
)";

static const std::string perPixelFragmentShaderGLSL330 = R"(
#version 330

in vec2 fragment_texture;
in vec4 fragment_color;

uniform sampler2D texture_sampler;

out vec4 color;

void main() {
    color = fragment_color * texture(texture_sampler, fragment_texture);
}
)";

PerPixelMesh::PerPixelMesh()
    : RenderItem()
{
    RenderItem::Init();

    m_perPixelMeshShader.CompileProgram(perPixelVertexShaderGLSL330, perPixelFragmentShaderGLSL330);
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

    // Only position & texture coordinates are per-vertex, colors are equal all over the grid (used for decay).
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, x)));         // Position, radius & angle
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, zoom)));      // zoom, zoom exponent, rotation & warp
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, centerX)));   // Center coord
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, distanceX))); // Distance
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), reinterpret_cast<void*>(offsetof(MeshVertex, stretchX)));  // Stretch

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PerPixelMesh::CompileWarpShader(const PresetState& presetState)
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
                std::cerr << "Compiled warp shader." << std::endl;
            }
            catch (ShaderException& ex)
            {
                std::cerr << "Error compiling warp shader:" << ex.message() << std::endl;
                m_warpShader.reset();
            }
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

    // Either viewport size or mesh size changed, reinitialize the vertices.
    int vertexIndex{0};
    float const texelOffsetX = 0.5f / static_cast<float>(presetState.renderContext.viewportSizeX);
    float const texelOffsetY = 0.5f / static_cast<float>(presetState.renderContext.viewportSizeY);

    for (int gridY = 0; gridY <= m_gridSizeY; gridY++)
    {
        for (int gridX = 0; gridX <= m_gridSizeX; gridX++)
        {
            auto& vertex = m_vertices.at(vertexIndex);

            vertex.x = static_cast<float>(gridX) / static_cast<float>(m_gridSizeX) * 2.0f - 1.0f;
            vertex.y = static_cast<float>(gridY) / static_cast<float>(m_gridSizeY) * 2.0f - 1.0f;

            // Milkdrop uses sqrtf, but hypotf is probably safer.
            vertex.radius = hypotf(vertex.x * static_cast<float>(presetState.renderContext.aspectX),
                                   vertex.y * static_cast<float>(presetState.renderContext.aspectY));
            if (gridY == m_gridSizeY / 2 && gridX == m_gridSizeX / 2)
            {
                vertex.angle = 0.0f;
            }
            else
            {
                vertex.angle = atan2f(vertex.y * static_cast<float>(presetState.renderContext.aspectY),
                                      vertex.x * static_cast<float>(presetState.renderContext.aspectX));
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

    glBindTexture(GL_TEXTURE_2D, presetState.mainTextureId);

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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    Sampler::Unbind(0);
    Shader::Unbind();
}

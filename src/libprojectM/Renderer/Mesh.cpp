#include "Renderer/Mesh.hpp"

namespace libprojectM {
namespace Renderer {

Mesh::Mesh()
{
    Initialize();
}

Mesh::Mesh(VertexBufferUsage usage)
    : m_vertices(usage)
    , m_colors(usage)
    , m_textureUVs(usage)
    , m_indices(usage)
{
    Initialize();
}

Mesh::Mesh(VertexBufferUsage usage, bool useColor, bool useTextureUVs)
    : m_useColorAttributes(useColor)
    , m_useUVAttributes(useTextureUVs)
    , m_vertices(usage)
    , m_colors(usage)
    , m_textureUVs(usage)
    , m_indices(usage)
{
    Initialize();
}

void Mesh::SetVertexCount(uint32_t vertexCount)
{
    m_vertices.Resize(vertexCount);

    if (m_useColorAttributes)
    {
        m_colors.Resize(vertexCount);
    }

    if (m_useUVAttributes)
    {
        m_textureUVs.Resize(vertexCount);
    }
}

void Mesh::SetUseColor(bool useColor)
{
    m_useColorAttributes = useColor;
    if (m_useColorAttributes)
    {
        m_colors.Resize(m_vertices.Size());
    }
    else
    {
        m_colors.Resize(0);
    }
}

void Mesh::SetUseUV(bool useUV)
{
    m_useUVAttributes = useUV;
    if (m_useUVAttributes)
    {
        m_textureUVs.Resize(m_vertices.Size());
    }
    else
    {
        m_textureUVs.Resize(0);
    }
}

auto Mesh::Indices() -> VertexIndexArray&
{
    return m_indices;
}

auto Mesh::Indices() const -> const VertexIndexArray&
{
    return m_indices;
}

void Mesh::Bind() const
{
    m_vertexArray.Bind();
}

void Mesh::Unbind()
{
    VertexArray::Unbind();
    VertexBuffer<Point>::Unbind();
    VertexIndexArray::Unbind();
}

void Mesh::Update()
{
    m_vertexArray.Bind();
    m_vertices.Update();
    m_colors.Update();
    m_textureUVs.Update();

    VertexBuffer<class Color>::SetEnableAttributeArray(1, m_useColorAttributes);
    VertexBuffer<TextureUV>::SetEnableAttributeArray(2, m_useUVAttributes);

    m_indices.Update();
}

void Mesh::Draw()
{
    m_vertexArray.Bind();

    // Assume each vertex is drawn once, in order.
    if (m_indices.Empty())
    {
        m_indices.Resize(m_vertices.Size());
        for (size_t index = 0; index < m_vertices.Size(); index++)
        {
            m_indices[index] = index;
        }
        m_indices.Update();
    }

    GLuint primitiveType{GL_LINES};
    switch (m_primitiveType)
    {
        case PrimitiveType::Points:
            primitiveType = GL_POINTS;
            break;
        case PrimitiveType::Lines:
            primitiveType = GL_LINES;
            break;
        case PrimitiveType::LineLoop:
            primitiveType = GL_LINE_LOOP;
            break;
        case PrimitiveType::LineStrip:
            primitiveType = GL_LINE_STRIP;
            break;
        case PrimitiveType::Triangles:
            primitiveType = GL_TRIANGLES;
            break;
        case PrimitiveType::TriangleStrip:
            primitiveType = GL_TRIANGLE_STRIP;
            break;
        case PrimitiveType::TriangleFan:
            primitiveType = GL_TRIANGLE_FAN;
            break;
    }

    glDrawElements(primitiveType, m_indices.Size(), GL_UNSIGNED_INT, nullptr);

    VertexArray::Unbind();
}

void Mesh::Initialize()
{
    m_vertexArray.Bind();

    // Set up the attribute pointers for use in shaders
    m_vertices.InitializeAttributePointer(0);
    m_colors.InitializeAttributePointer(1);
    m_textureUVs.InitializeAttributePointer(2);

    VertexBuffer<Point>::SetEnableAttributeArray(0, true);

    m_indices.Bind();

    VertexArray::Unbind();
    VertexBuffer<Point>::Unbind();
}

} // namespace Renderer
} // namespace libprojectM

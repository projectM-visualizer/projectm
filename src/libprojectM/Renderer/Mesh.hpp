#pragma once

#include "VertexArray.hpp"
#include "VertexIndexArray.hpp"


#include <Renderer/Color.hpp>
#include <Renderer/Point.hpp>
#include <Renderer/TextureUV.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <Renderer/VertexBufferUsage.hpp>

#include <cstdint>
#include <vector>

namespace libprojectM {
namespace Renderer {

/**
 * @brief A 2D mesh class for drawing lines and polygons.
 *
 * It keeps separate buffers/streams for optional attributes like color and UV coordinates.
 *
 * Vertex attributes are always stored in the following location indices for use in vertex shaders:
 * - 0: Vertex position (2 elements, X/Y)
 * - 1: Color (4 elements, R/G/B/A)
 * - 2: U/V coordinate (2 elements, U/V)
 */
class Mesh
{
public:
    enum class PrimitiveType : uint8_t
    {
        Points,
        Lines,
        LineStrip,
        LineLoop,
        Triangles,
        TriangleStrip,
        TriangleFan
    };

    Mesh();

    explicit Mesh(VertexBufferUsage usage);

    Mesh(VertexBufferUsage usage, bool useColor, bool useTextureUVs);

    virtual ~Mesh() = default;

    auto RenderPrimitiveType() const -> PrimitiveType
    {
        return m_primitiveType;
    }

    void SetRenderPrimitiveType(PrimitiveType primitiveType)
    {
        m_primitiveType = primitiveType;
    }

    auto VertexCount() const -> uint32_t
    {
        return m_vertices.Size();
    }

    void SetVertexCount(uint32_t vertexCount);

    auto Vertices() const -> const VertexBuffer<Point>&
    {
        return m_vertices;
    }

    auto Vertices() -> VertexBuffer<Point>&
    {
        return m_vertices;
    }

    auto Vertex(uint32_t index) const -> const Point&
    {
        return m_vertices[index];
    }

    auto Vertex(uint32_t index) -> Point&
    {
        return m_vertices[index];
    }

    void SetVertex(uint32_t index, const Point& vertex)
    {
        m_vertices[index] = vertex;
    }

    auto Colors() const -> const VertexBuffer<Color>&
    {
        return m_colors;
    }

    auto Colors() -> VertexBuffer<Color>&
    {
        return m_colors;
    }

    auto Color(uint32_t index) const -> const Color&
    {
        return m_colors[index];
    }

    auto Color(uint32_t index) -> class Color&
    {
        return m_colors[index];
    }

    auto UseColor() const -> bool
    {
        return m_useColorAttributes;
    }

    void SetUseColor(bool useColor);

    void SetColor(uint32_t index, const class Color& vertex)
    {
        m_colors[index] = vertex;
    }

    auto UVs() const -> const VertexBuffer<TextureUV>&
    {
        return m_textureUVs;
    }

    auto UVs() -> VertexBuffer<TextureUV>&
    {
        return m_textureUVs;
    }

    auto UV(uint32_t index) const -> const TextureUV&
    {
        return m_textureUVs[index];
    }

    auto UV(uint32_t index) -> TextureUV&
    {
        return m_textureUVs[index];
    }

    void SetUV(uint32_t index, const TextureUV& vertex)
    {
        m_textureUVs[index] = vertex;
    }

    auto UseUV() const -> bool
    {
        return m_useUVAttributes;
    }

    void SetUseUV(bool useUV);

    auto Indices() -> VertexIndexArray&;

    auto Indices() const -> const VertexIndexArray&;

    void Bind() const;

    static void Unbind();

    void Update();

    void Draw();

private:
    void Initialize();

    PrimitiveType m_primitiveType{PrimitiveType::Lines}; //!< Mesh render primitive type.

    bool m_useColorAttributes{false}; //!< If true, the color attribute array is enabled and populated.
    bool m_useUVAttributes{false};    //!< If true, the UV attribute array is enabled and populated.

    VertexArray m_vertexArray; //!< The vertex array object used to store all data of the mesh.

    VertexBuffer<Point> m_vertices;       //!< The vertex coordinates.
    VertexBuffer<class Color> m_colors;   //!< Vertex color values.
    VertexBuffer<TextureUV> m_textureUVs; //!< Vertex texture coordinates.

    VertexIndexArray m_indices; //!< The vertex indices used for rendering.
};

} // namespace Renderer
} // namespace libprojectM

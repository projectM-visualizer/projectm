#pragma once

#include "Renderer/Color.hpp"
#include "Renderer/Point.hpp"
#include "Renderer/TextureUV.hpp"
#include "Renderer/VertexArray.hpp"
#include "Renderer/VertexBuffer.hpp"
#include "Renderer/VertexBufferUsage.hpp"
#include "Renderer/VertexIndexArray.hpp"

#include <cstdint>

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
 *
 * Attribute data is stored in individual buffers. When setting the buffers via their Set() methods,
 * the user has to make sure that all buffers have at least the same length as the main vertex buffer.
 *
 * Keep in mind that only 8 attribute arrays are guaranteed to be supported by the OpenGL spec.
 *
 * The Mesh class always uses vertex indices to render, using glDrawElements. If the index buffer is
 * empty on the first draw call, it will be populated with a 1:1 mapping of the vertex buffer. If
 * the vertex count changes, the caller has to make sure that the index array is updated accordingly.
 *
 * @note When using this class, attributes indices 1 and 2 will be toggled according to the
 * attribute enable flags. When using additional vertex arrays, make sure to start with index 3.
 */
class Mesh
{
public:
    enum class PrimitiveType : uint8_t
    {
        Points,        //!< Renders each vertex as a single texel.
        Lines,         //!< Renders individual lines, one for each 2 vertices.
        LineStrip,     //!< Renders all vertices as a continuous line.
        LineLoop,      //!< Renders all vertices as a continuous line, plus an additional line from the last to first vertex.
        Triangles,     //!< Renders vertices as individual triangles, with three vertices/indices making up one triangle.
        TriangleStrip, //!< Renders vertices as a continuous strip of triangles, where 2 vertices are shared between each old and new triangle.
        TriangleFan    //!< Renders a fan of triangles, with the first vertex being used as the first vertex of all rendered triangles.
    };

    /**
     * Constructor. Creates the mesh with a default rendering mode of PrimitiveType::Lines.
     */
    Mesh();

    /**
     * Constructor. Creates the mesh with a default rendering mode of PrimitiveType::Lines.
     * Allows setting the buffer usage hint for the internal buffers.
     * @param usage The buffer usage hint.
     */
    explicit Mesh(VertexBufferUsage usage);

    /**
     * Constructor. Creates the mesh with a default rendering mode of PrimitiveType::Lines.
     * Allows setting the buffer usage hint for the internal buffers and to specify the use
     * or color and texture coordinate attribute arrays.
     * @param usage The buffer usage hint.
     * @param useColor If true, the color attribute array will be enabled by default.
     * @param useTextureUVs If true, the texture coordinate attribute array will be enabled by default.
     */
    Mesh(VertexBufferUsage usage, bool useColor, bool useTextureUVs);

    /**
     * Default destructor.
     */
    virtual ~Mesh() = default;

    /**
     * Returns the currently set primitive type which is rendered when calling Draw().
     * @return The currently set primitive type.
     */
    auto RenderPrimitiveType() const -> PrimitiveType
    {
        return m_primitiveType;
    }

    /**
     * Sets the primitive type to render when calling Draw().
     * @param primitiveType The new primitive type to render.
     */
    void SetRenderPrimitiveType(PrimitiveType primitiveType)
    {
        m_primitiveType = primitiveType;
    }

    /**
     * Returns the current number of vertices in the vertex buffer.
     * @return The number of vertices in the vertex buffer.
     */
    auto VertexCount() const -> uint32_t
    {
        return m_vertices.Size();
    }

    /**
     * @brief Sets the size of all attribute buffers to the given count.
     * Only buffers enabled at the time of the call are resized to save memory. If a buffer is
     * enabled after calling this function, either resize it manually or call this function again.
     * @param vertexCount The new vertex count.
     */
    void SetVertexCount(uint32_t vertexCount);

    /**
     * Returns a constant reference to the vertex position buffer.
     * @return A constant reference to the internal vertex (position) buffer.
     */
    auto Vertices() const -> const VertexBuffer<Point>&
    {
        return m_vertices;
    }

    /**
     * Returns a reference to the vertex position buffer.
     * @return A writeable reference to the internal vertex (position) buffer.
     */
    auto Vertices() -> VertexBuffer<Point>&
    {
        return m_vertices;
    }

    /**
     * Returns a Point instance with the position of a vertex at the given index.
     * @throws std::out_of_range thrown if the index is out of range.
     * @param index The index of the vertex position to retrieve.
     * @return A constant reference to the vertex position at the given index.
     */
    auto Vertex(uint32_t index) const -> const Point&
    {
        return m_vertices[index];
    }

    /**
     * Returns a Point instance with the position of a vertex at the given index.
     * @throws std::out_of_range thrown if the index is out of range.
     * @param index The index of the vertex position to retrieve.
     * @return A writeable reference to the vertex position at the given index.
     */
    auto Vertex(uint32_t index) -> Point&
    {
        return m_vertices[index];
    }

    /**
     * Set a given vertex index to a new value.
     * @param index The index of the vertex to set.
     * @param vertex The new data to set at the given index.
     */
    void SetVertex(uint32_t index, const Point& vertex)
    {
        m_vertices[index] = vertex;
    }

    /**
     * Returns a constant reference to the internal color buffer.
     * @return A constant reference to the internal color buffer.
     */
    auto Colors() const -> const VertexBuffer<Color>&
    {
        return m_colors;
    }

    /**
     * Returns a reference to the internal color buffer.
     * @return A writeable reference to the internal color buffer.
     */
    auto Colors() -> VertexBuffer<Color>&
    {
        return m_colors;
    }

    /**
     * Returns a Color instance with the RGBA color of a vertex at the given index.
     * @throws std::out_of_range thrown if the index is out of range.
     * @param index The index of the vertex color to retrieve.
     * @return A constant reference to the vertex color at the given index.
     */
    auto Color(uint32_t index) const -> const Color&
    {
        return m_colors[index];
    }

    /**
     * Returns a Color instance with the RGBA color of a vertex at the given index.
     * @throws std::out_of_range thrown if the index is out of range.
     * @param index The index of the vertex color to retrieve.
     * @return A writeable reference to the vertex color at the given index.
     */
    auto Color(uint32_t index) -> class Color&
    {
        return m_colors[index];
    }

    /**
     * Returns whether the color attribute array (index 1) is enabled.
     * @return true if the color attribute array is enabled, false if not.
     */
    auto UseColor() const -> bool
    {
        return m_useColorAttributes;
    }

    /**
     * @brief Enables or disables the use of the color attribute array (index 1).
     * When enabling the array, the color buffer is resized to the size of the position buffer.
     * When disabling the array, the color buffer is cleared.
     * @note Disabling the color attribute array does NOT free any previously allocated GPU buffer memory.
     * @param useColor true to enable the color attribute array, false to disable it.
     */
    void SetUseColor(bool useColor);

    /**
     * Set a given index to a new color value.
     * @param index The index of the vertex color to set.
     * @param color The new color data to set at the given index.
     */
    void SetColor(uint32_t index, const class Color& color)
    {
        m_colors[index] = color;
    }

    /**
     * Returns a constant reference to the internal texture coordinates buffer.
     * @return A constant reference to the internal texture coordinates buffer.
     */
    auto UVs() const -> const VertexBuffer<TextureUV>&
    {
        return m_textureUVs;
    }

    /**
     * Returns a reference to the internal texture coordinates buffer.
     * @return A writeable reference to the internal texture coordinates buffer.
     */
    auto UVs() -> VertexBuffer<TextureUV>&
    {
        return m_textureUVs;
    }

    /**
     * Returns a TextureUV instance with the texture coordinates of a vertex at the given index.
     * @throws std::out_of_range thrown if the index is out of range.
     * @param index The index of the vertex texture coordinates to retrieve.
     * @return A constant reference to the vertex texture coordinates at the given index.
     */
    auto UV(uint32_t index) const -> const TextureUV&
    {
        return m_textureUVs[index];
    }

    /**
     * Returns a TextureUV instance with the texture coordinates of a vertex at the given index.
     * @throws std::out_of_range thrown if the index is out of range.
     * @param index The index of the vertex texture coordinates to retrieve.
     * @return A writeable reference to the vertex texture coordinates at the given index.
     */
    auto UV(uint32_t index) -> TextureUV&
    {
        return m_textureUVs[index];
    }

    /**
     * Set a given index to new texture coordinates.
     * @param index The index of the vertex texture coordinates to set.
     * @param uvs The new texture coordinates data to set at the given index.
     */
    void SetUV(uint32_t index, const TextureUV& uvs)
    {
        m_textureUVs[index] = uvs;
    }

    /**
     * Returns whether the texture coordinates attribute array (index 2) is enabled.
     * @return true if the texture coordinates attribute array is enabled, false if not.
     */
    auto UseUV() const -> bool
    {
        return m_useUVAttributes;
    }

    /**
     * @brief Enables or disables the use of the texture coordinates attribute array (index 2).
     * When enabling the array, the texture coordinates buffer is resized to the size of the position buffer.
     * When disabling the array, the texture coordinates buffer is cleared.
     * @note Disabling the texture coordinates attribute array does NOT free any previously allocated GPU buffer memory.
     * @param useUV true to enable the texture coordinates attribute array, false to disable it.
     */
    void SetUseUV(bool useUV);

    /**
     * Returns a reference to the vertex index buffer.
     * @return A writeable reference to the vertex index buffer.
     */
    auto Indices() -> VertexIndexArray&;

    /**
     * Returns a constant reference to the vertex index buffer.
     * @return A constant reference to the vertex index buffer.
     */
    auto Indices() const -> const VertexIndexArray&;

    /**
     * Binds the vertex array object used to store the mesh vertex data.
     */
    void Bind() const;

    /**
     * Binds the default vertex array object with ID 0.
     */
    static void Unbind();

    /**
     * @brief Updates the data all enabled buffers, sending it to the GPU.
     * Calling this method is required to render any changed vertex data. As long
     * as Update() is not called, Draw() will always draw the same geometry.
     * @note This method binds the vertex array object and all buffers and leaves them bound.
     */
    void Update();

    /**
     * @brief Draws the current geometry stored in the GPU.
     * If this method is called with an empty vertex index buffer, the index buffer is filled with
     * a continuous 1:1 mapping to the vertex position buffer.
     * @note Before calling this method, the caller has to ensure that all buffers have the correct
     * length and were properly uploaded to the GPU by calling Update().
     * @note This method binds and unbinds the stored vertex array object.
     */
    void Draw();

private:
    /**
     * Binds the internal buffers to the vertex array object and initializes the array pointers.
     */
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

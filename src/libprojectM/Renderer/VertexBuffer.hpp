#pragma once

#include "Renderer/OpenGL.h"
#include "Renderer/VertexBufferUsage.hpp"

#include <cstddef>
#include <vector>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Wraps a vertex buffer object and manages its data.
 *
 * The templated storage class must meet a few requirements:
 * - It must only contain 1 to 4 members of a single type allowed by the OpenGL specification.
 * - It must implement a function with the following signature to initialize the attribute pointer:
 *   <tt>static void InitializeAttributePointer(uint32_t attributeIndex)</tt>
 * - It must be default-constructible.
 *
 * @tparam VT The data storage type for this buffer.
 */
template<class VT>
class VertexBuffer
{
public:
    /**
     * Constructor. Creates an empty buffer with the default usage hint.
     */
    VertexBuffer();

    /**
     * Constructor. Creates an empty buffer with a specific usage hint.
     * @param usage The usage hint for this buffer.
     */
    explicit VertexBuffer(VertexBufferUsage usage);

    /**
     * Destructor. Deletes the GPU buffer.
     */
    virtual ~VertexBuffer();

    /**
     * Binds the stored vertex buffer object.
     */
    void Bind() const;

    /**
     * Binds the default vertex buffer object with ID 0.
     */
    static void Unbind();

    /**
     * Initializes the attribute array pointer for this buffer.
     * @param attributeIndex The index to use for the attributes stored in this buffer.
     */
    void InitializeAttributePointer(uint32_t attributeIndex) const;

    /**
     * Enables or disabled the use of the given attribute array index.
     * @param attributeIndex The index to enable or disable.
     * @param enable true to enable the given attribute array index, false to disable.
     */
    static void SetEnableAttributeArray(uint32_t attributeIndex, bool enable);

    /**
     * Return a reference to the internal vector storing the buffer's elements.
     * @return A writeable reference to the internal buffer vector.
     */
    auto Get() -> std::vector<VT>&;

    /**
     * Return a constant reference to the internal vector storing the buffer's elements.
     * @return A constant reference to the internal buffer vector.
     */
    auto Get() const -> const std::vector<VT>&;

    /**
     * @brief Replaces the buffer with the given contents.
     * All elements are copied. To reduce overhead with large buffers, use the Get() method instead
     * and then move-assign the vector to the returned reference.
     * @param buffer The new buffer contents.
     */
    void Set(const std::vector<VT>& buffer);

    /**
     * Returns a reference to a single stored vertex attrobute data object at the given index.
     * @throws std::out_of_range Thrown if the given index is outside the vector's boundaries.
     * @param index The index for the vertex attribute data to return.
     * @return A writeable reference to a single attribute object for the given index.
     */
    auto Vertex(size_t index) -> VT&;

    /**
     * Returns a reference to a single stored vertex attrobute data object at the given index.
     * @throws std::out_of_range Thrown if the given index is outside the vector's boundaries.
     * @param index The index for the vertex attribute data to return.
     * @return A constant reference to a single attribute object for the given index.
     */
    auto Vertex(size_t index) const -> const VT&;

    /**
     * Assigns new data to the given index.
     * @throws std::out_of_range Thrown if the given index is outside the vector's boundaries.
     * @param index The index of the vertex attribute data to assign.
     * @param vertex The new data to assign to the given index.
     */
    void SetVertex(size_t index, const VT& vertex);

    /**
     * Queries if the buffer is empty.
     * @return true if the buffer is empty, false if the buffer contains at least one element.
     */
    auto Empty() const -> bool;

    /**
     * Returns the number of stored elements in this buffer.
     * @return The number of stored elements in this buffer.
     */
    auto Size() const -> size_t;

    /**
     * @brief Changes the buffer size to the given value.
     * If the buffer size is increased, new elements at the end are default-constructed.
     * @param size The new size of the buffer.
     */
    void Resize(size_t size);

    /**
     * @brief Changes the buffer size to the given value.
     * If the buffer size is increased, new elements at the end are copied from the given element in elem.
     * @param size The new size of the buffer.
     * @param elem An element to copy for newly added entries.
     */
    void Resize(size_t size, const VT& elem);

    /**
     * @brief Uploads the current buffer contents to the GPU.
     * This method won't change the GPU buffer if the buffer is empty.
     * @note This method binds the stored vertex buffer object and leaves it bound.
     */
    void Update();

    /**
     * @brief A shorthand notation to access individual buffer elements.
     * @throws std::out_of_range Thrown if the given index is outside the buffer index bounds.
     * @param index The index of the vertex attribute to access.
     * @return A writeable reference to the vertex attribute data at the given index.
     */
    auto operator[](size_t index) -> VT&;

    /**
     * @brief A shorthand notation to access individual buffer elements.
     * @throws std::out_of_range Thrown if the given index is outside the buffer index bounds.
     * @param index The index of the vertex attribute to access.
     * @return A constant reference to the vertex attribute data at the given index.
     */
    auto operator[](size_t index) const -> const VT&;

private:
    GLuint m_vboID{};   //!< The ID of the OpenGL vertex buffer object.
    size_t m_vboSize{}; //!< Stores the last number if items uploaded to the GPU.

    VertexBufferUsage m_vboUsage{VertexBufferUsage::StaticDraw}; //!< The buffer usage hint.

    std::vector<VT> m_vertices; //!< The local copy of the vertex buffer data.
};

template<class VT>
VertexBuffer<VT>::VertexBuffer()
{
    glGenBuffers(1, &m_vboID);
}

template<class VT>
VertexBuffer<VT>::VertexBuffer(VertexBufferUsage usage)
    : m_vboUsage(usage)
{
    glGenBuffers(1, &m_vboID);
}

template<class VT>
VertexBuffer<VT>::~VertexBuffer()
{
    glDeleteBuffers(1, &m_vboID);
}

template<class VT>
void VertexBuffer<VT>::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
}

template<class VT>
void VertexBuffer<VT>::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template<class VT>
void VertexBuffer<VT>::InitializeAttributePointer(uint32_t attributeIndex) const
{
    Bind();
    VT::InitializeAttributePointer(attributeIndex);
}

template<class VT>
void VertexBuffer<VT>::SetEnableAttributeArray(uint32_t attributeIndex, bool enable)
{
    if (enable)
    {
        glEnableVertexAttribArray(attributeIndex);
    }
    else
    {
        glDisableVertexAttribArray(attributeIndex);
    }
}

template<class VT>
auto VertexBuffer<VT>::Get() -> std::vector<VT>&
{
    return m_vertices;
}

template<class VT>
auto VertexBuffer<VT>::Get() const -> const std::vector<VT>&
{
    return m_vertices;
}

template<class VT>
void VertexBuffer<VT>::Set(const std::vector<VT>& buffer)
{
    m_vertices = buffer;
}

template<class VT>
auto VertexBuffer<VT>::Vertex(size_t index) -> VT&
{
    return m_vertices.at(index);
}

template<class VT>
auto VertexBuffer<VT>::Vertex(size_t index) const -> const VT&
{
    return m_vertices.at(index);
}

template<class VT>
void VertexBuffer<VT>::SetVertex(size_t index, const VT& vertex)
{
    m_vertices.at(index) = vertex;
}

template<class VT>
auto VertexBuffer<VT>::Empty() const -> bool
{
    return m_vertices.empty();
}

template<class VT>
auto VertexBuffer<VT>::Size() const -> size_t
{
    return m_vertices.size();
}

template<class VT>
void VertexBuffer<VT>::Resize(size_t size)
{
    m_vertices.resize(size);
}

template<class VT>
void VertexBuffer<VT>::Resize(size_t size, const VT& elem)
{
    m_vertices.resize(size, elem);
}

template<class VT>
void VertexBuffer<VT>::Update()
{
    Bind();

    if (m_vertices.empty())
    {
        return;
    }

    if (m_vboSize == m_vertices.size())
    {
        glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizei>(sizeof(VT) * m_vertices.size()), m_vertices.data());
    }
    else
    {
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(VT) * m_vertices.size()), m_vertices.data(), VertexBufferUsageToGL(m_vboUsage));
        m_vboSize = m_vertices.size();
    }
}

template<class VT>
auto VertexBuffer<VT>::operator[](size_t index) const -> const VT&
{
    return m_vertices.at(index);
}

template<class VT>
auto VertexBuffer<VT>::operator[](size_t index) -> VT&
{
    return m_vertices.at(index);
}

} // namespace Renderer
} // namespace libprojectM

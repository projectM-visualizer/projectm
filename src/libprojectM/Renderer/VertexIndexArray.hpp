#pragma once

#include "Renderer/VertexBufferUsage.hpp"

#include <projectM-opengl.h>

#include <cstddef>
#include <cstdint>
#include <vector>

namespace libprojectM {
namespace Renderer {

/**
 * Wraps a vertex element array buffer, containing vertex indices for drawing.
 */
class VertexIndexArray
{
public:
    /**
     * Constructor. Creates an empty index buffer with the default usage hint.
     */
    VertexIndexArray();

    /**
     * Constructor. Creates an empty index buffer with the specified usage hint.
     */
    VertexIndexArray(VertexBufferUsage usage);

    /**
     * Destructor. Deleted the index buffer.
     */
    virtual ~VertexIndexArray();

    /**
     * Binds the stored index buffer.
     */
    void Bind() const;

    /**
     * Binds the default index buffer with ID 0.
     */
    static void Unbind();

    /**
     * Return a reference to the internal vector storing the buffer's elements.
     * @return A writeable reference to the internal buffer vector.
     */
    auto Get() -> std::vector<uint32_t>&;

    /**
     * Return a constant reference to the internal vector storing the buffer's elements.
     * @return A constant reference to the internal buffer vector.
     */
    auto Get() const -> const std::vector<uint32_t>&;

    /**
     * @brief Replaces the index buffer with the given contents.
     * All elements are copied. To reduce overhead with large buffers, use the Get() method instead
     * and then move-assign the vector to the returned reference.
     * @param buffer The new index buffer contents.
     */
    void Set(const std::vector<uint32_t>& buffer);

    /**
     * Returns a reference to a single value at the given index in the buffer.
     * @throws std::out_of_range Thrown if the given index is outside the vector's boundaries.
     * @param index The index for the vertex index value to return.
     * @return A writeable reference to a value for the given index.
     */
    auto VertexIndex(size_t index) -> uint32_t;

    /**
     * Returns a reference to a single value at the given index in the buffer.
     * @throws std::out_of_range Thrown if the given index is outside the vector's boundaries.
     * @param index The index for the vertex index value to return.
     * @return A constant reference to a value for the given index.
     */
    auto VertexIndex(size_t index) const -> uint32_t;

    /**
     * Assigns a new index value to the given index.
     * @throws std::out_of_range Thrown if the given index is outside the vector's boundaries.
     * @param index The index of the vertex index value to assign.
     * @param value The new index value to assign to the given index.
     */
    void SetVertexIndex(size_t index, uint32_t value);

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
     * If the buffer size is increased, new elements at the end are set to 0.
     * @param size The new size of the index buffer.
     */
    void Resize(size_t size);

    /**
     * @brief Changes the buffer size to the given value.
     * If the buffer size is increased, new elements at the end are copied from the given value.
     * @param size The new size of the index buffer.
     * @param value A value to copy for newly added entries.
     */
    void Resize(size_t size, uint32_t value);

    /**
     * @brief Fills the buffer with values from 0 to size() - 1.
     * This is useful if each vertex is only drawn once, in the order as stored in the vertex buffer.
     */
    void MakeContinuous();

    /**
     * Uploads the current index buffer contents to the GPU.
     * @note this functions binds the index buffer object and leaves it bound.
     */
    void Update();

    /**
     * @brief A shorthand notation to access individual buffer elements.
     * @throws std::out_of_range Thrown if the given index is outside the buffer index bounds.
     * @param index The index of the vertex index value to access.
     * @return A writeable reference to the vertex index value at the given index.
     */
    auto operator[](size_t index) -> uint32_t&;

    /**
     * @brief A shorthand notation to access individual buffer elements.
     * @throws std::out_of_range Thrown if the given index is outside the buffer index bounds.
     * @param index The index of the vertex index value to access.
     * @return A constant reference to the vertex index value at the given index.
     */
    auto operator[](size_t index) const -> uint32_t;

private:
    GLuint m_veabID{};   //!< The ID of the OpenGL vertex element array buffer object.
    size_t m_veabSize{}; //!< Stores the last number if items uploaded to the GPU.

    VertexBufferUsage m_vboUsage{VertexBufferUsage::StaticDraw}; //!< The buffer usage hint.

    std::vector<uint32_t> m_indices; //!< The local copy of the vertex index buffer data.
};

} // namespace Renderer
} // namespace libprojectM

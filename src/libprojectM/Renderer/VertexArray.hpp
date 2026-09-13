#pragma once

#include "Renderer/OpenGL.h"

namespace libprojectM {
namespace Renderer {

/**
 * @brief Wraps a vertex array object.
 * Creates, destroys and binds a single VAO.
 */
class VertexArray
{
public:
    VertexArray(const VertexArray&) = delete;
    auto operator=(const VertexArray&) -> VertexArray& = delete;
    VertexArray(VertexArray&& other) noexcept : m_vaoID(other.m_vaoID) { other.m_vaoID = 0; }
    auto operator=(VertexArray&& other) noexcept -> VertexArray& {
        if (this != &other) {
            if (m_vaoID) glDeleteVertexArrays(1, &m_vaoID);
            m_vaoID = other.m_vaoID;
            other.m_vaoID = 0;
        }
        return *this;
    }

    /**
     * Constructor. Creates a new VAO.
     */
    VertexArray()
    {
        glGenVertexArrays(1, &m_vaoID);
    }

    /**
     * Destructor. Deletes the stored VAO.
     */
    virtual ~VertexArray()
    {
        glDeleteVertexArrays(1, &m_vaoID);
        m_vaoID = 0;
    }

    /**
     * Binds the stored VAO.
     */
    void Bind() const
    {
        glBindVertexArray(m_vaoID);
    }

    /**
     * Binds the default VAO with ID 0.
     */
    static void Unbind()
    {
        glBindVertexArray(0);
    }

private:
    GLuint m_vaoID{0}; //!< The vertex array object ID for this mesh's vertex data.

};


} // namespace Renderer
} // namespace libprojectM

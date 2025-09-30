#pragma once

#include "VertexBuffer.hpp"


#include <projectM-opengl.h>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Wraps a vertex array object.
 * Creates, destroys and binds a single VAO.
 */
class VertexArray
{
public:
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

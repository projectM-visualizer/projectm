#pragma once

#include "VertexBuffer.hpp"


#include <projectM-opengl.h>

namespace libprojectM {
namespace Renderer {

class VertexArray
{
public:
    VertexArray()
    {
        glGenVertexArrays(1, &m_vaoID);
    }

    virtual ~VertexArray()
    {
        glDeleteVertexArrays(1, &m_vaoID);
        m_vaoID = 0;
    }

    void Bind() const
    {
        glBindVertexArray(m_vaoID);
    }

    static void Unbind()
    {
        glBindVertexArray(0);
    }

private:
    GLuint m_vaoID{0}; //!< The vertex array object ID for this mesh's vertex data.

};


} // namespace Renderer
} // namespace libprojectM

#include "OpenGLRenderItem.hpp"

namespace libprojectM {
namespace Renderer {
namespace Backend {
namespace OpenGL {

OpenGLRenderItem::OpenGLRenderItem() = default;

OpenGLRenderItem::~OpenGLRenderItem()
{
    glDeleteBuffers(1, &m_vboID);
    glDeleteVertexArrays(1, &m_vaoID);
}

void OpenGLRenderItem::Init()
{
    glGenVertexArrays(1, &m_vaoID);
    glGenBuffers(1, &m_vboID);

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    InitVertexAttrib();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

} // namespace OpenGL
} // namespace Backend
} // namespace Renderer
} // namespace libprojectM

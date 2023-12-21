#include "RenderItem.hpp"

void RenderItem::Init()
{
    glGenVertexArrays(1, &m_vaoID);
    glGenBuffers(1, &m_vboID);

    glBindVertexArray(m_vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);

    InitVertexAttrib();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

RenderItem::~RenderItem()
{
    glDeleteBuffers(1, &m_vboID);
    glDeleteVertexArrays(1, &m_vaoID);
}

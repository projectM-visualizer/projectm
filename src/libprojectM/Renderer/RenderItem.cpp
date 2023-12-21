#include "RenderItem.hpp"

#if __has_include(<TargetConditionals.h>)
#include <TargetConditionals.h>
#define GLES_SILENCE_DEPRECATION 1
#endif

void RenderItem::Init()
{
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    glGenVertexArraysOES(1, &m_vaoID);
#else
    glGenVertexArrays(1, &m_vaoID);
#endif
    glGenBuffers(1, &m_vboID);

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
    glBindVertexArrayOES(m_vaoID);
#else
    glBindVertexArray(m_vaoID);
#endif
    glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
    InitVertexAttrib();

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#else
    glBindVertexArray(0);
#endif
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

RenderItem::~RenderItem()
{
    glDeleteBuffers(1, &m_vboID);
#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#else
    glDeleteVertexArrays(1, &m_vaoID);
#endif
}

#pragma once

#include <Renderer/RenderItem.hpp>
#include <projectM-opengl.h>

namespace libprojectM {
namespace Renderer {
namespace Backend {
namespace OpenGL {

class OpenGLRenderItem : public libprojectM::Renderer::RenderItem
{
public:
    OpenGLRenderItem();
    ~OpenGLRenderItem();

    void InitVertexAttrib();

    // Override Init to provide OpenGL-specific initialization
    void Init();

protected:
    GLuint m_vboID{0};
    GLuint m_vaoID{0};
};

} // namespace OpenGL
} // namespace Backend
} // namespace Renderer
} // namespace libprojectM

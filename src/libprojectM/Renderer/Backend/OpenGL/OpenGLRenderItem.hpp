#pragma once

#include "Renderer/RenderItem.hpp"
#include <projectM-opengl.h>

#ifdef PROJECTM_USERSPRITES_DEBUG
#pragma message("DEBUG: Including OpenGLRenderItem.hpp")
#endif

namespace libprojectM {
namespace Renderer {
namespace Backend {
namespace OpenGL {

class OpenGLRenderItem : public ::libprojectM::Renderer::RenderItem
{
public:
    OpenGLRenderItem();
    virtual ~OpenGLRenderItem();

    // Mark override for clarity and to avoid hiding warnings
    void InitVertexAttrib() override;

    // Mark override for clarity and to avoid hiding warnings
    void Init() override;

protected:
    using ::libprojectM::Renderer::RenderItem::Init; // Unhide base Init() to avoid -Woverloaded-virtual warning
    GLuint m_vboID{0};
    GLuint m_vaoID{0};
};

} // namespace OpenGL
} // namespace Backend
} // namespace Renderer
} // namespace libprojectM

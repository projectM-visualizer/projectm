#include "Renderer/VertexBufferUsage.hpp"

namespace libprojectM {
namespace Renderer {

GLuint VertexBufferUsageToGL(VertexBufferUsage usage)
{
    switch (usage)
    {
        case VertexBufferUsage::StreamDraw:
        default:
            return GL_STREAM_DRAW;
            break;
        case VertexBufferUsage::StreamRead:
            return GL_STREAM_READ;
            break;
        case VertexBufferUsage::StreamCopy:
            return GL_STREAM_COPY;
            break;
        case VertexBufferUsage::StaticDraw:
            return GL_STATIC_DRAW;
            break;
        case VertexBufferUsage::StaticRead:
            return GL_STATIC_READ;
            break;
        case VertexBufferUsage::StaticCopy:
            return GL_STATIC_COPY;
            break;
        case VertexBufferUsage::DynamicDraw:
            return GL_DYNAMIC_DRAW;
            break;
        case VertexBufferUsage::DynamicRead:
            return GL_DYNAMIC_READ;
            break;
        case VertexBufferUsage::DynamicCopy:
            return GL_DYNAMIC_COPY;
            break;
    }
}

} // namespace Renderer
} // namespace libprojectM

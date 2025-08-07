#pragma once

#include <projectM-opengl.h>

namespace libprojectM {
namespace Renderer {

enum class VertexBufferUsage : char
{
    StreamDraw,
    StreamRead,
    StreamCopy,
    StaticDraw,
    StaticRead,
    StaticCopy,
    DynamicDraw,
    DynamicRead,
    DynamicCopy
};

auto VertexBufferUsageToGL(VertexBufferUsage usage) -> GLuint;

} // namespace Renderer
} // namespace libprojectM

#pragma once

#include "Renderer/OpenGL.h"

namespace libprojectM {
namespace Renderer {

/**
 * An enum with available buffer usage hints.
 */
enum class VertexBufferUsage : char
{
    StreamDraw,  //!< The data store contents will be modified once and used at most a few times for drawing.
    StreamRead,  //!< The data store contents will be modified once and used at most a few times for reading data from GL.
    StreamCopy,  //!< The data store contents will be modified once and used at most a few times for both drawing and reading data from GL.
    StaticDraw,  //!< The data store contents will be modified once and used many times for drawing.
    StaticRead,  //!< The data store contents will be modified once and used many times for reading data from GL.
    StaticCopy,  //!< The data store contents will be modified once and used many times for both drawing and reading data from GL.
    DynamicDraw, //!< The data store contents will be modified repeatedly and used many times for drawing.
    DynamicRead, //!< The data store contents will be modified repeatedly and used many times for reading data from GL.
    DynamicCopy  //!< The data store contents will be modified repeatedly and used many times for both drawing and reading data from GL.
};

/**
 * Translates values from the VertexBufferUsage enumeration to OpenGL constants.
 * @param usage The usage hint to translate to an OpenGL constant.
 * @return The OpenGL constant for the given usage hint enum value.
 */
auto VertexBufferUsageToGL(VertexBufferUsage usage) -> GLuint;

} // namespace Renderer
} // namespace libprojectM

/**
 * @file GLStateGuard.hpp
 * @brief RAII guard that saves and restores OpenGL state around a rendering scope.
 *
 * This is used to ensure libprojectM doesn't leak GL state into the host
 * application's rendering pipeline. This is especially important when hosted
 * inside a shared OpenGL context such as Qt's QOpenGLWidget.
 */
#pragma once

#include "OpenGL.h"

namespace libprojectM {
namespace Renderer {

/**
 * @brief RAII guard that captures OpenGL state on construction and restores it on destruction.
 *
 * Create an instance at the beginning of a rendering scope. When the instance goes out
 * of scope (or is explicitly destroyed), all captured state is restored.
 */
class GLStateGuard
{
public:
    /**
     * @brief Captures the current OpenGL state.
     */
    GLStateGuard()
    {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_framebuffer);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &m_vertexArray);
        glGetIntegerv(GL_CURRENT_PROGRAM, &m_program);
        glGetIntegerv(GL_ACTIVE_TEXTURE, &m_activeTexture);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &m_arrayBuffer);
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &m_elementArrayBuffer);
        glGetBooleanv(GL_BLEND, &m_blend);
        glGetBooleanv(GL_DEPTH_TEST, &m_depthTest);
        glGetIntegerv(GL_BLEND_SRC_RGB, &m_blendSrcRgb);
        glGetIntegerv(GL_BLEND_DST_RGB, &m_blendDstRgb);
#ifndef USE_GLES
        glGetBooleanv(GL_LINE_SMOOTH, &m_lineSmooth);
#endif
    }

    /**
     * @brief Restores the OpenGL state captured at construction time.
     */
    ~GLStateGuard()
    {
        // Restore framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

        // Restore VAO (must be done before restoring buffer bindings)
        glBindVertexArray(m_vertexArray);

        // Restore shader program
        glUseProgram(m_program);

        // Restore active texture unit
        glActiveTexture(m_activeTexture);

        // Restore buffer bindings
        glBindBuffer(GL_ARRAY_BUFFER, m_arrayBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementArrayBuffer);

        // Restore blend state
        if (m_blend)
        {
            glEnable(GL_BLEND);
        }
        else
        {
            glDisable(GL_BLEND);
        }
        glBlendFunc(m_blendSrcRgb, m_blendDstRgb);

        // Restore depth test state
        if (m_depthTest)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

#ifndef USE_GLES
        // Restore line smooth state
        if (m_lineSmooth)
        {
            glEnable(GL_LINE_SMOOTH);
        }
        else
        {
            glDisable(GL_LINE_SMOOTH);
        }
#endif

        // Reset color mask to default (projectM may have changed it via MaskDrawBuffer)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // Unbind any sampler objects left bound by projectM on common texture units
        for (GLuint unit = 0; unit < 8; unit++)
        {
            glBindSampler(unit, 0);
        }
    }

    GLStateGuard(const GLStateGuard&) = delete;
    GLStateGuard(GLStateGuard&&) = delete;
    auto operator=(const GLStateGuard&) -> GLStateGuard& = delete;
    auto operator=(GLStateGuard&&) -> GLStateGuard& = delete;

private:
    GLint m_framebuffer{};
    GLint m_vertexArray{};
    GLint m_program{};
    GLint m_activeTexture{};
    GLint m_arrayBuffer{};
    GLint m_elementArrayBuffer{};
    GLboolean m_blend{};
    GLboolean m_depthTest{};
    GLint m_blendSrcRgb{};
    GLint m_blendDstRgb{};
#ifndef USE_GLES
    GLboolean m_lineSmooth{};
#endif
};

} // namespace Renderer
} // namespace libprojectM

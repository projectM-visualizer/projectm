#pragma once

#include "LineGeometry.hpp"

#include <Renderer/RenderContext.hpp>
#include <Renderer/Shader.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

#include <memory>

namespace libprojectM {
namespace MilkdropPreset {

/**
 * @brief The quad lines' vertex offset in render pixels, which decides pixel-border ties.
 *
 * A hard-edged band that lies exactly on a pixel border (a flat line in silence) has pixel centres
 * on both of its edges; the rasterizer's top-left rule then picks one row. GL leaves the row a GL
 * line lights in that case to the implementation. On desktop GL the top-left rule already picks the
 * row the GL lines light, so no offset is needed. On GLES (checked on a Mali-G52) the GL lines light
 * the other row, so the quads move by a 1/64 px fraction: enough to decide the tie, too little to
 * change any pixel that is not on an edge.
 */
auto LineTieBias() -> glm::vec2;

/**
 * @brief Draws line strips as quads, one instance per segment, with miter joins (issue #682).
 *
 * Upload() a LineBatch, Begin() once for its transformation, then Draw() its strips and End().
 * Other draws may run between Begin() and Draw(): Draw() binds its own program and buffers. The
 * caller sets blending.
 *
 * The program is compiled on first use and shared through the render context's shader cache. If
 * it fails to compile or link, Prepare() returns false and callers draw GL lines instead.
 */
class LineRenderer
{
public:
    LineRenderer() = default;
    ~LineRenderer();

    LineRenderer(const LineRenderer&) = delete;
    auto operator=(const LineRenderer&) -> LineRenderer& = delete;

    /**
     * @brief Gets (or compiles) the line program.
     * @param context The render context holding the shader cache.
     * @return False if the program cannot be compiled or linked; nothing is drawn then.
     */
    auto Prepare(const Renderer::RenderContext& context) -> bool;

    void Upload(const LineBatch& batch);
    void Begin(const glm::mat4& transformation, const Renderer::RenderContext& context);
    void Draw(const LineBatch::Strip& strip, const LineStyle& style);
    void End();

private:
    std::shared_ptr<Renderer::Shader> m_shader; //!< Line program, from the shader cache.
    bool m_failed{false};                       //!< The program did not compile or link; draw GL lines.
    GLuint m_vaoId{0};                          //!< Vertex array with six per-instance attributes.
    GLuint m_vboId{0};                          //!< Padded strip points.
    bool m_ready{false};                        //!< Begin() found a program, uploaded points and a non-empty viewport.

    // The per-draw uniforms' values as last set on the program; Begin() forgets them, so the first
    // Draw() after it sets them again. Draw() skips sets that would not change anything.
    float m_lastHalfWidth{0.0f};
    bool m_halfWidthSet{false};
    glm::vec2 m_lastPassOffset{0.0f, 0.0f};
    bool m_passOffsetSet{false};
};

/**
 * @brief Lets scaled dots set their own size on desktop GL while it lives.
 *
 * Desktop core-profile GL ignores gl_PointSize unless GL_PROGRAM_POINT_SIZE is enabled; GLES always
 * honours it. Enables it (when asked and it is off) and disables it again when destroyed, so other
 * draws keep the host's setting. Does nothing on GLES.
 */
class ScopedProgramPointSize
{
public:
    explicit ScopedProgramPointSize(bool enable);
    ~ScopedProgramPointSize();

    ScopedProgramPointSize(const ScopedProgramPointSize&) = delete;
    auto operator=(const ScopedProgramPointSize&) -> ScopedProgramPointSize& = delete;

private:
    bool m_enabled{false}; //!< This object enabled GL_PROGRAM_POINT_SIZE.
};

} // namespace MilkdropPreset
} // namespace libprojectM

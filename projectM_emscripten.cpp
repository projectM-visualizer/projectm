#include "omp.h"
#include <unistd.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/html5.h>
#include <projectM-4/playlist.h>
#include <projectM-4/projectM.h>
#include <emscripten/html5_webgl.h>
#define GL_GLEXT_PROTOTYPES
#define GL_FRAGMENT_PRECISION_HIGH
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLES3/gl31.h>
// #include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3.h>

#define GL_CONTEXT_PROFILE_MASK 0x9126
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#define CONTEXT_FLAG_NO_ERROR_BIT_KHR 0x00000008
#define GL_ANISOTROPIC_FILTER 0x3000
#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <cstdint>
#include <vector>
#include <limits>
#include <cmath>
#include <algorithm>

using namespace emscripten;

// =============================================================================
// Phase 2: Dual Ping-Pong FBO Architecture with Floating-Point Texture Support
// =============================================================================

/**
 * @brief Available floating-point texture formats for FBO color attachments.
 *
 * Preference order:
 *   RGBA32F – full 32-bit float per channel (requires EXT_color_buffer_float)
 *   RGBA16F – 16-bit half-float per channel (requires EXT_color_buffer_half_float)
 *   RGBA8   – 8-bit normalized (always available; shaders must clamp output to [0,1])
 */
enum class FboFloatFormat
{
    RGBA32F, //!< GL_RGBA32F – preferred for recursive warp feedback loops
    RGBA16F, //!< GL_RGBA16F – mobile-friendly fallback
    RGBA8    //!< GL_RGBA8   – last resort; negative alpha corruption possible without clamping
};

/**
 * @brief Dual ping-pong framebuffer manager for isolated preset transitions.
 *
 * Maintains four framebuffer objects so that two preset pipelines can render
 * concurrently without sharing any texture state:
 *
 *   Slot 0 (A_Read)  – Preset A feedback source (history texture)
 *   Slot 1 (A_Write) – Preset A render target   (current frame)
 *   Slot 2 (B_Read)  – Preset B feedback source [lazily allocated]
 *   Slot 3 (B_Write) – Preset B render target   [lazily allocated]
 *
 * Each frame the Read/Write roles within each preset are swapped (ping-pong),
 * giving each preset its own isolated feedback loop.
 *
 * Preset B FBOs are only allocated when a transition is actively initiated and
 * are released (or promoted to Preset A) when the transition completes.
 */
class DualPingPongFramebuffer
{
public:
    static constexpr int kARead  = 0; //!< Preset A read  (history) slot
    static constexpr int kAWrite = 1; //!< Preset A write (current) slot
    static constexpr int kBRead  = 2; //!< Preset B read  (history) slot
    static constexpr int kBWrite = 3; //!< Preset B write (current) slot

    DualPingPongFramebuffer() = default;

    ~DualPingPongFramebuffer()
    {
        ReleaseAll();
    }

    /**
     * @brief Detects the best available float texture format by probing WebGL extensions.
     *
     * Call once after the WebGL context has been made current and before any FBO
     * allocation. Priority: GL_RGBA32F (EXT_color_buffer_float) >
     * GL_RGBA16F (EXT_color_buffer_half_float) > GL_RGBA8.
     *
     * @param ctx The active Emscripten WebGL context handle.
     */
    void DetectFormat(EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx)
    {
        if (emscripten_webgl_enable_extension(ctx, "EXT_color_buffer_float") == EM_TRUE)
        {
            m_format = FboFloatFormat::RGBA32F;
            // Enable bilinear filtering on float textures when available.
            emscripten_webgl_enable_extension(ctx, "OES_texture_float_linear");
            printf("DualFBO: Using GL_RGBA32F float textures.\n");
        }
        else if (emscripten_webgl_enable_extension(ctx, "EXT_color_buffer_half_float") == EM_TRUE)
        {
            m_format = FboFloatFormat::RGBA16F;
            emscripten_webgl_enable_extension(ctx, "OES_texture_half_float_linear");
            printf("DualFBO: Using GL_RGBA16F half-float textures.\n");
        }
        else
        {
            m_format = FboFloatFormat::RGBA8;
            printf("DualFBO: Float textures unavailable, falling back to GL_RGBA8.\n");
            printf("DualFBO: WARNING – preset shaders should clamp output to [0,1].\n");
        }
    }

    /**
     * @brief Allocates the Preset A ping-pong FBO pair.
     *
     * Must be called once after the WebGL context is ready and DetectFormat()
     * has been invoked. Safe to call again after a Resize() – dimensions are
     * updated but no re-allocation occurs.
     *
     * @param width  Framebuffer width in pixels.
     * @param height Framebuffer height in pixels.
     * @return true on success, false if FBO creation failed.
     */
    bool AllocatePresetA(int width, int height)
    {
        if (m_presetAAllocated)
        {
            Resize(width, height);
            return true;
        }

        m_width  = width;
        m_height = height;

        if (!CreateFBO(kARead, width, height) || !CreateFBO(kAWrite, width, height))
        {
            ReleaseFBO(kARead);
            ReleaseFBO(kAWrite);
            fprintf(stderr, "DualFBO: Failed to allocate Preset A FBOs.\n");
            return false;
        }

        m_presetAAllocated = true;
        printf("DualFBO: Preset A FBOs allocated (%dx%d).\n", width, height);
        return true;
    }

    /**
     * @brief Lazily allocates the Preset B ping-pong FBO pair.
     *
     * Call when a transition is initiated. Does nothing if Preset B is already
     * allocated.
     *
     * @param width  Framebuffer width in pixels.
     * @param height Framebuffer height in pixels.
     * @return true on success, false if FBO creation failed.
     */
    bool AllocatePresetB(int width, int height)
    {
        if (m_presetBAllocated)
        {
            return true;
        }

        if (!CreateFBO(kBRead, width, height) || !CreateFBO(kBWrite, width, height))
        {
            ReleaseFBO(kBRead);
            ReleaseFBO(kBWrite);
            fprintf(stderr, "DualFBO: Failed to allocate Preset B FBOs.\n");
            return false;
        }

        m_presetBAllocated = true;
        printf("DualFBO: Preset B FBOs allocated (%dx%d).\n", width, height);
        return true;
    }

    /**
     * @brief Releases the Preset B FBOs without affecting Preset A.
     *
     * Call when a transition is cancelled or after PromoteBtoA() has
     * transferred ownership.
     */
    void ReleasePresetB()
    {
        if (!m_presetBAllocated)
        {
            return;
        }
        ReleaseFBO(kBRead);
        ReleaseFBO(kBWrite);
        m_presetBAllocated = false;
        printf("DualFBO: Preset B FBOs released.\n");
    }

    /**
     * @brief Releases all allocated FBOs. Called automatically by the destructor.
     */
    void ReleaseAll()
    {
        ReleasePresetB();
        if (m_presetAAllocated)
        {
            ReleaseFBO(kARead);
            ReleaseFBO(kAWrite);
            m_presetAAllocated = false;
            printf("DualFBO: Preset A FBOs released.\n");
        }
    }

    /**
     * @brief Promotes Preset B's FBOs into the Preset A slots after a transition completes.
     *
     * Old Preset A FBOs are deleted; Preset B's FBOs take their place.
     * After this call Preset B is no longer allocated and Preset A holds the
     * former Preset B resources.
     */
    void PromoteBtoA()
    {
        if (!m_presetBAllocated)
        {
            return;
        }

        // Release old Preset A resources.
        if (m_presetAAllocated)
        {
            ReleaseFBO(kARead);
            ReleaseFBO(kAWrite);
        }

        // Move Preset B into Preset A slots (transfer ownership).
        m_fbos[kARead]      = m_fbos[kBRead];
        m_fbos[kAWrite]     = m_fbos[kBWrite];
        m_textures[kARead]  = m_textures[kBRead];
        m_textures[kAWrite] = m_textures[kBWrite];

        // Clear Preset B slots (ownership transferred; do not delete).
        m_fbos[kBRead]      = 0;
        m_fbos[kBWrite]     = 0;
        m_textures[kBRead]  = 0;
        m_textures[kBWrite] = 0;

        m_presetAAllocated = true;
        m_presetBAllocated = false;
        printf("DualFBO: Preset B promoted to Preset A.\n");
    }

    /**
     * @brief Swaps the Read/Write FBOs for Preset A (ping-pong).
     *
     * Call once per frame while Preset A is rendering so that the previous
     * frame's output becomes the new frame's history texture.
     */
    void SwapPresetA()
    {
        std::swap(m_fbos[kARead],      m_fbos[kAWrite]);
        std::swap(m_textures[kARead],  m_textures[kAWrite]);
    }

    /**
     * @brief Swaps the Read/Write FBOs for Preset B (ping-pong).
     *
     * Call once per frame during a transition.
     */
    void SwapPresetB()
    {
        std::swap(m_fbos[kBRead],      m_fbos[kBWrite]);
        std::swap(m_textures[kBRead],  m_textures[kBWrite]);
    }

    /**
     * @brief Resizes all allocated FBOs to new dimensions.
     *
     * Recreates all textures at the new size; existing framebuffer contents
     * are lost. The FBO handles themselves are reused.
     *
     * @param width  New framebuffer width in pixels.
     * @param height New framebuffer height in pixels.
     */
    void Resize(int width, int height)
    {
        if (width == m_width && height == m_height)
        {
            return;
        }

        m_width  = width;
        m_height = height;

        if (m_presetAAllocated)
        {
            ResizeFBOTexture(kARead,  width, height);
            ResizeFBOTexture(kAWrite, width, height);
        }
        if (m_presetBAllocated)
        {
            ResizeFBOTexture(kBRead,  width, height);
            ResizeFBOTexture(kBWrite, width, height);
        }

        printf("DualFBO: Resized to %dx%d.\n", width, height);
    }

    // -------------------------------------------------------------------------
    // Accessor functions – return raw OpenGL handles for use in render passes.
    // -------------------------------------------------------------------------
    GLuint GetAReadFBO()    const { return m_fbos[kARead]; }
    GLuint GetAWriteFBO()   const { return m_fbos[kAWrite]; }
    GLuint GetAReadTex()    const { return m_textures[kARead]; }
    GLuint GetAWriteTex()   const { return m_textures[kAWrite]; }

    GLuint GetBReadFBO()    const { return m_fbos[kBRead]; }
    GLuint GetBWriteFBO()   const { return m_fbos[kBWrite]; }
    GLuint GetBReadTex()    const { return m_textures[kBRead]; }
    GLuint GetBWriteTex()   const { return m_textures[kBWrite]; }

    bool IsPresetAAllocated() const { return m_presetAAllocated; }
    bool IsPresetBAllocated() const { return m_presetBAllocated; }

    FboFloatFormat GetFormat() const { return m_format; }
    int Width()  const { return m_width; }
    int Height() const { return m_height; }

private:
    // -------------------------------------------------------------------------
    // Helper: return the GL internal format for the detected float format.
    // -------------------------------------------------------------------------
    GLint GetGLInternalFormat() const
    {
        switch (m_format)
        {
            case FboFloatFormat::RGBA32F: return GL_RGBA32F;
            case FboFloatFormat::RGBA16F: return GL_RGBA16F;
            case FboFloatFormat::RGBA8:   return GL_RGBA8;
            default:
                fprintf(stderr, "DualFBO: Unknown FboFloatFormat; falling back to GL_RGBA8.\n");
                return GL_RGBA8;
        }
    }

    GLenum GetGLFormat() const { return GL_RGBA; }

    GLenum GetGLType() const
    {
        switch (m_format)
        {
            case FboFloatFormat::RGBA32F: return GL_FLOAT;
            case FboFloatFormat::RGBA16F: return GL_HALF_FLOAT;
            case FboFloatFormat::RGBA8:   return GL_UNSIGNED_BYTE;
            default:
                fprintf(stderr, "DualFBO: Unknown FboFloatFormat; falling back to GL_UNSIGNED_BYTE.\n");
                return GL_UNSIGNED_BYTE;
        }
    }

    /**
     * @brief Creates a single FBO with a colour texture attachment at the given slot.
     *
     * @param index  Slot index (kARead, kAWrite, kBRead, or kBWrite).
     * @param width  Texture width in pixels.
     * @param height Texture height in pixels.
     * @return true on success; false if the framebuffer status check failed.
     */
    bool CreateFBO(int index, int width, int height)
    {
        glGenTextures(1, &m_textures[index]);
        glBindTexture(GL_TEXTURE_2D, m_textures[index]);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GetGLInternalFormat(), width, height, 0,
                     GetGLFormat(), GetGLType(), nullptr);
        ConfigureTextureSampling(m_textures[index]);

        glGenFramebuffers(1, &m_fbos[index]);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbos[index]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, m_textures[index], 0);

        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            fprintf(stderr, "DualFBO: Framebuffer slot %d incomplete (status=0x%x).\n",
                    index, status);
            glDeleteTextures(1, &m_textures[index]);
            glDeleteFramebuffers(1, &m_fbos[index]);
            m_textures[index] = 0;
            m_fbos[index]     = 0;
            return false;
        }

        return true;
    }

    /**
     * @brief Deletes the FBO and its texture at the given slot and resets handles to 0.
     */
    void ReleaseFBO(int index)
    {
        if (m_fbos[index] != 0)
        {
            glDeleteFramebuffers(1, &m_fbos[index]);
            m_fbos[index] = 0;
        }
        if (m_textures[index] != 0)
        {
            glDeleteTextures(1, &m_textures[index]);
            m_textures[index] = 0;
        }
    }

    /**
     * @brief Reallocates the colour texture for a slot at new dimensions.
     *
     * The FBO handle itself is reused; only the texture storage is replaced
     * and re-attached.
     */
    void ResizeFBOTexture(int index, int width, int height)
    {
        if (m_textures[index] == 0)
        {
            return;
        }
        glBindTexture(GL_TEXTURE_2D, m_textures[index]);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GetGLInternalFormat(), width, height, 0,
                     GetGLFormat(), GetGLType(), nullptr);
        ConfigureTextureSampling(m_textures[index]);

        // Re-attach the resized texture to the FBO.
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbos[index]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, m_textures[index], 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /**
     * @brief Sets the standard sampler parameters (linear filtering, edge clamp) on a
     *        currently-bound 2D texture. Assumes the texture is already bound.
     *
     * @param texId The texture whose parameters to configure (used only for clarity;
     *              the call operates on the currently bound GL_TEXTURE_2D target).
     */
    void ConfigureTextureSampling(GLuint texId)
    {
        (void)texId; // The texture must be bound before calling this helper.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    GLuint m_fbos[4]     = {0, 0, 0, 0}; //!< FBO IDs:     [A_Read, A_Write, B_Read, B_Write]
    GLuint m_textures[4] = {0, 0, 0, 0}; //!< Texture IDs: [A_Read, A_Write, B_Read, B_Write]

    int m_width  = 0; //!< Current FBO texture width in pixels
    int m_height = 0; //!< Current FBO texture height in pixels

    bool m_presetAAllocated = false; //!< Whether Preset A FBOs are currently allocated
    bool m_presetBAllocated = false; //!< Whether Preset B FBOs are currently allocated

    FboFloatFormat m_format = FboFloatFormat::RGBA8; //!< Detected colour format for textures
};

// Global dual ping-pong FBO manager instance (Emscripten/WASM build only).
DualPingPongFramebuffer g_dualFbo;

// =============================================================================
// Phase 3: GLStateGuard – RAII WebGL state save/restore for preset isolation
// =============================================================================

/**
 * @brief RAII guard that snapshots relevant WebGL state on construction and
 *        restores it on destruction.
 *
 * Usage:
 * @code
 *     {
 *         GLStateGuard guard;
 *         // … render preset A …
 *     } // state automatically restored here
 * @endcode
 *
 * Saved/restored state:
 *   - GL_BLEND enabled flag
 *   - Blend function (src/dst RGB + Alpha)
 *   - GL_DEPTH_WRITEMASK
 *   - GL_VIEWPORT
 *   - GL_SCISSOR_TEST enabled flag
 *   - GL_SCISSOR_BOX
 *   - GL_ACTIVE_TEXTURE unit
 *   - GL_TEXTURE_BINDING_2D on texture unit 0
 *   - GL_FRAMEBUFFER_BINDING
 */
class GLStateGuard
{
public:
    GLStateGuard()
    {
        // --- Blend state ---
        m_blendEnabled = glIsEnabled(GL_BLEND);
        glGetIntegerv(GL_BLEND_SRC_RGB,   &m_blendSrcRGB);
        glGetIntegerv(GL_BLEND_DST_RGB,   &m_blendDstRGB);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &m_blendSrcAlpha);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &m_blendDstAlpha);

        // --- Depth mask ---
        glGetBooleanv(GL_DEPTH_WRITEMASK, &m_depthMask);

        // --- Viewport ---
        glGetIntegerv(GL_VIEWPORT, m_viewport);

        // --- Scissor ---
        m_scissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
        glGetIntegerv(GL_SCISSOR_BOX, m_scissorBox);

        // --- Active texture unit ---
        glGetIntegerv(GL_ACTIVE_TEXTURE, &m_activeTexture);

        // --- Texture binding on unit 0 ---
        glActiveTexture(GL_TEXTURE0);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_tex0Binding);
        // Restore the originally active texture unit immediately.
        glActiveTexture(static_cast<GLenum>(m_activeTexture));

        // --- FBO binding ---
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_fboBinding);
    }

    ~GLStateGuard()
    {
        // --- FBO binding ---
        glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(m_fboBinding));

        // --- Blend state ---
        if (m_blendEnabled)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
        glBlendFuncSeparate(static_cast<GLenum>(m_blendSrcRGB),
                            static_cast<GLenum>(m_blendDstRGB),
                            static_cast<GLenum>(m_blendSrcAlpha),
                            static_cast<GLenum>(m_blendDstAlpha));

        // --- Depth mask ---
        glDepthMask(m_depthMask);

        // --- Viewport ---
        glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

        // --- Scissor ---
        if (m_scissorEnabled)
            glEnable(GL_SCISSOR_TEST);
        else
            glDisable(GL_SCISSOR_TEST);
        glScissor(m_scissorBox[0], m_scissorBox[1], m_scissorBox[2], m_scissorBox[3]);

        // --- Texture unit 0 binding ---
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(m_tex0Binding));

        // --- Active texture unit ---
        glActiveTexture(static_cast<GLenum>(m_activeTexture));
    }

    // Non-copyable, non-movable.
    GLStateGuard(const GLStateGuard&)            = delete;
    GLStateGuard& operator=(const GLStateGuard&) = delete;
    GLStateGuard(GLStateGuard&&)                 = delete;
    GLStateGuard& operator=(GLStateGuard&&)      = delete;

private:
    GLboolean m_blendEnabled   = GL_FALSE;
    GLint     m_blendSrcRGB    = GL_ONE;
    GLint     m_blendDstRGB    = GL_ZERO;
    GLint     m_blendSrcAlpha  = GL_ONE;
    GLint     m_blendDstAlpha  = GL_ZERO;
    GLboolean m_depthMask      = GL_TRUE;
    GLint     m_viewport[4]    = {0, 0, 0, 0};
    GLboolean m_scissorEnabled = GL_FALSE;
    GLint     m_scissorBox[4]  = {0, 0, 0, 0};
    GLint     m_activeTexture  = GL_TEXTURE0;
    GLint     m_tex0Binding    = 0;
    GLint     m_fboBinding     = 0;
};

/**
 * @brief Forcefully resets OpenGL blend, texture, and FBO state between two
 *        preset pipeline phases.
 *
 * Call after finishing Preset A's draw and before starting Preset B's draw to
 * prevent Preset A's additive blend/texture state from contaminating Preset B.
 *
 * Resets:
 *   - Blending disabled (GL_ONE, GL_ZERO – opaque replace mode)
 *   - Texture units 0-7 unbound (GL_TEXTURE_2D)
 *   - FBO unbound (default framebuffer)
 *   - Depth mask enabled (default)
 */
static void gl_reset_state_between_pipelines()
{
    // Disable blending; reset to opaque replace mode.
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);

    // Unbind textures on the first 8 units (covers all units used by projectM shaders).
    for (int unit = 0; unit < 8; ++unit)
    {
        glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + unit));
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glActiveTexture(GL_TEXTURE0); // Leave active unit at 0 as a clean default.

    // Reset depth mask.
    glDepthMask(GL_TRUE);

    // Unbind any FBO (render to default framebuffer).
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// =============================================================================

EGLContext ctxegl;
EGLDisplay display;
EGLSurface surface;
EGLConfig eglconfig=NULL;
EGLint config_size,major,minor,atb_pos;
EGLint numSamples;
EGLint numSamplesNV;
EGLint numBuffersNV;
EGLint numGreen;
EGLint numRed;
EGLint numBlue;
EGLint numAlpha;
EGLint numDepth;
EGLint numStencil;
EGLint numBuffer;
EGLint numMBuffers;
EGLint colorSpace;
EGLint colorFormat;
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl_ctx;

typedef struct {
projectm_handle projectm_engine;
projectm_playlist_handle playlist;
EM_BOOL loading;
} AppData;

projectm_handle pm;
AppData app_data;
projectm_playlist_handle playlist={};

bool g_is_streaming_audio = false;

extern "C" {
EMSCRIPTEN_KEEPALIVE
void create_sprite() {
const char* new_sprite_code =
        "[preset01]"
        "img='textures/rv_IP_20250421_060250.png';"
        "per_frame_1=blendmode=1;"
        "per_frame_2=x = 0.5;"         // Center X
        "per_frame_3=y = 0.5;"         // Center Y
        "per_frame_4=z = 0.0;"         // Center Y
        "per_frame_5=scaling = 1.0;"   // Make it huge (twice the screen height)
        "per_pixel_2=a = 1;"         // Fully opaque
        "per_pixel_3=r = 1.0;"         // Bright Red
        "per_pixel_4=g = 0.0;"
        "per_pixel_5=b = 1.0;";
        
projectm_sprite_create(app_data.projectm_engine, "milkdrop", new_sprite_code);
return;
}

EMSCRIPTEN_KEEPALIVE
uintptr_t get_projectm_handle() { 
return reinterpret_cast<uintptr_t>(app_data.projectm_engine);
}
} // extern "C"

EM_JS(void, js_feed_stream_data_to_projectm, (uintptr_t pm_handle, int buffer_size), {
    const analyser = window.projectMStreamAnalyser;
    const pcmBuffer = window.projectMStreamBuffer;
    if (!analyser || !pcmBuffer || pcmBuffer.length !== buffer_size) {
        return;
    }
    analyser.getFloatTimeDomainData(pcmBuffer);

    // Optimization: projectM's internal analysis buffer is 576 samples (AudioBufferSamples).
    // Sending more than this just overwrites the older data in the ring buffer before analysis.
    // We send only the most recent 576 samples to minimize overhead while keeping the buffer fresh.
    const projectm_buffer_size = 576;
    if (pcmBuffer.length > projectm_buffer_size) {
        // Send subarray of the last 576 samples
        const optimizedBuffer = pcmBuffer.subarray(pcmBuffer.length - projectm_buffer_size);
        Module.ccall('projectm_pcm_add_float_wrapper', null, ['number', 'array', 'number', 'number'], [pm_handle, optimizedBuffer, projectm_buffer_size, 1]);
    } else {
        Module.ccall('projectm_pcm_add_float_wrapper', null, ['number', 'array', 'number', 'number'], [pm_handle, pcmBuffer, pcmBuffer.length, 1]);
    }
});

EM_JS(void, js_initialize_stream_analyser, (), {
    const audioContext = window.projectMAudioContext_Global_Cpp;
    const audioElement = document.getElementById('audio-stream-element');
    if (!audioContext || !audioElement) {
        console.error("JS Stream Init: AudioContext or audio element not found.");
        return;
    }
    const analyser = audioContext.createAnalyser();


    analyser.fftSize = 2048; // A common size for detailed analysis


    const source = audioContext.createMediaElementSource(audioElement);
    source.connect(analyser);
    analyser.connect(audioContext.destination);
    window.projectMStreamAnalyser = analyser;
    window.projectMStreamBuffer = new Float32Array(analyser.fftSize);
    console.log("JS Stream Init: Media element and analyser connected.");
});

void load_preset_callback_example(bool is_hard_cut, unsigned int index,void* user_data) {
// AppData* app_data = (AppData*)user_data;
projectm_playlist_handle playlist = app_data.playlist;
uint32_t indx = projectm_playlist_play_next(playlist, false);
return;
}

EM_JS(void, js_update_preset_name, (const char* name), {
    const presetName = UTF8ToString(name);
    if (window.updatePresetDisplay) {
        window.updatePresetDisplay(presetName);
    }
});

void load_preset_callback_done(bool is_hard_cut, unsigned int index,void* user_data) {
float randomDelay=(emscripten_random()*30.0)+27.0;
projectm_set_preset_duration(app_data.projectm_engine, randomDelay);
app_data.loading=EM_FALSE;

uint32_t pos = projectm_playlist_get_position(app_data.playlist);
char* preset_path = projectm_playlist_item(app_data.playlist, pos);
if (preset_path) {
    js_update_preset_name(preset_path);
    projectm_playlist_free_string(preset_path);
}
return;
}

void projectm_pcm_add_float_from_js_array_wrapper(
uintptr_t pm_handle_value,
emscripten::val js_audio_array_val,
unsigned int num_samples_per_channel,
int channels_enum_value) {
projectm_handle current_pm_handle = app_data.projectm_engine;
if (!current_pm_handle) {
fprintf(stderr, "Error: projectM handle is null in from_js_array_wrapper.\n");
return;
}

std::vector<float> cpp_audio_buffer = emscripten::vecFromJSArray<float>(js_audio_array_val);
if (channels_enum_value <= 0 || num_samples_per_channel == 0) {
fprintf(stderr, "Error: Invalid channel count (%d) or samples_per_channel (%u).\n",
channels_enum_value, num_samples_per_channel);
return;
}

size_t expected_total_elements = static_cast<size_t>(num_samples_per_channel) * static_cast<size_t>(channels_enum_value);
if (cpp_audio_buffer.size() != expected_total_elements) {
fprintf(stderr, "Error: Audio data size mismatch. Expected %zu elements, got %zu elements from JS array.\n",
expected_total_elements, cpp_audio_buffer.size());
return;
}

projectm_pcm_add_float(current_pm_handle, cpp_audio_buffer.data(), num_samples_per_channel, static_cast<projectm_channels>(channels_enum_value));
return;
}

EM_JS(void, js_initialize_worklet_system_once, (uintptr_t pm_handle_for_addpcm), {
    if (window.projectMAudioContext_Global_Cpp) { return; }
    try {
        const audioContext = new (window.AudioContext || window.webkitAudioContext)();
        window.projectMAudioContext_Global_Cpp = audioContext;
        console.log("JS Audio Init: Web Audio context created.");
        (async () => {
            await audioContext.audioWorklet.addModule('projectm_audio_processor.js');
            const workletNode = new AudioWorkletNode(audioContext, 'projectm-audio-processor');
            window.projectMWorkletNode_Global_Cpp = workletNode;
            workletNode.port.onmessage = (event) => {
                if (event.data.type === 'pcmData' && Module._projectm_pcm_add_float_wrapper) {
                    Module.ccall('projectm_pcm_add_float_wrapper', null, ['number', 'array', 'number', 'number'],
                        [pm_handle_for_addpcm, event.data.audioData, event.data.samplesPerChannel, event.data.channelsForPM]);
                }
            };
            workletNode.connect(audioContext.destination);
            console.log("JS Audio Init: AudioWorkletNode created and connected permanently.");
        })();
    } catch(e) {
        console.error("JS Audio Init: Failed to initialize worklet system:", e);
    }
    return;
});

EM_JS(void, js_load_song_into_worklet, (const char* path_in_vfs, bool loop, bool startPlaying), {
    const filePath = UTF8ToString(path_in_vfs);
    const audioContext = window.projectMAudioContext_Global_Cpp;
    const workletNode = window.projectMWorkletNode_Global_Cpp;
    if (!audioContext || !workletNode) { return; }
    
    async function decodeAndSend() {
        try {
            const fileDataUint8Array = FS.readFile(filePath);
            console.log(`JS Load Song: Read ${fileDataUint8Array.length} bytes from ${filePath}.`);
            if (fileDataUint8Array.length === 0) { return; }
            
            const audioDataArrayBuffer = fileDataUint8Array.buffer.slice(
                fileDataUint8Array.byteOffset, fileDataUint8Array.byteOffset + fileDataUint8Array.byteLength
            );
            
            if (audioContext.state === 'suspended') { await audioContext.resume(); }
            
            const decodedBuffer = await audioContext.decodeAudioData(audioDataArrayBuffer);
            console.log(`JS Load Song: Decoded buffer. Duration: ${decodedBuffer.duration.toFixed(2)}s. Sending to worklet.`);
            
            const rawChannelData = Array.from({length: decodedBuffer.numberOfChannels}, (_, i) => decodedBuffer.getChannelData(i));
            
            workletNode.port.postMessage({
                type: 'loadWavData',
                channelData: rawChannelData,
                sampleRate: decodedBuffer.sampleRate,
                loop: loop,
                startPlaying: startPlaying // Now consistently using 'startPlaying'
            });
        } catch(e) {
            console.error("JS Load Song: Error during decode and send:", e);
        }
    }
    decodeAndSend();
    return;
});

extern "C" {

EMSCRIPTEN_KEEPALIVE
void pl(const char* song_path_in_vfs) {
printf("C++: pl() called for unique path: %s\n", song_path_in_vfs);
js_load_song_into_worklet(song_path_in_vfs, true, true);
return;
}

EMSCRIPTEN_KEEPALIVE
void set_audio_source_to_stream(bool is_streaming) {
g_is_streaming_audio = is_streaming;
printf("C++: Audio source set to stream: %s\n", is_streaming ? "true" : "false");
}

EM_JS(void, js_stop_worklet_playback, (), {
    const workletNode = window.projectMWorkletNode_Global_Cpp;
    if (workletNode) {
        workletNode.port.postMessage({ type: 'stopPlayback' });
    }
});

EMSCRIPTEN_KEEPALIVE
void stop_worklet_playback() {
    js_stop_worklet_playback();
}

} // extern "C"

void renderLoop(){
if(app_data.loading==EM_TRUE){
return;
}
if (g_is_streaming_audio) {
js_feed_stream_data_to_projectm(
reinterpret_cast<uintptr_t>(app_data.projectm_engine),


            2048 // This MUST match the analyser.fftSize


);
}
// Phase 3: Wrap the render call in a GLStateGuard so that any blend, texture,
// or FBO state set by the preset shader is restored before handing control back
// to the browser's WebGL layer.
{
    GLStateGuard guard;
    projectm_opengl_render_frame(pm);
}
eglSwapBuffers(display,surface);
return;
}

extern "C" {
EMSCRIPTEN_KEEPALIVE
void start_render(int width, int height){
// glClearColor( 1.0, 1.0, 1.0, 0.0 );
glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
printf("Setting window size: %i x %i\n", width, height);
glViewport(0,0,8192,8192);  //  viewport/scissor after UsePrg runs at full resolution
glViewport(0,0,width,height);  //  viewport/scissor after UsePrg runs at full resolution
glEnable(GL_SCISSOR_TEST);
glScissor(0,0,width,height);
glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,GL_NICEST);
glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);
glDisable(GL_DITHER);
glFrontFace(GL_CW);
glCullFace(GL_BACK);
app_data.loading=EM_FALSE;
projectm_set_window_size(pm,width,height);
// Phase 2: Allocate Preset A ping-pong FBOs now that the viewport
// dimensions are known. DetectFormat() was already called in init().
g_dualFbo.AllocatePresetA(width, height);
emscripten_set_main_loop((void (*)())renderLoop,0,0);


emscripten_set_main_loop_timing(2,1);


return;
}
} // extern "C"

void _on_preset_switch_failed(const char *preset_filename, const char *message, void *user_data) {
printf("Preset switch failed (%s): %s\n", preset_filename, message);
return;
}

extern "C" {

void add_audio_data(uint8_t* data, int len) {
projectm_pcm_add_uint8(pm, data, len, PROJECTM_MONO);
return;
}

}

EM_JS(char*, js_get_random_preset_path, (), {
    const randIndex = Math.floor(Math.random()*20);
    var jsString = '/presets/preset_'+randIndex+'.milk';
    return stringToNewUTF8(jsString);
});

void on_preset_switch_requested(bool is_hard_cut, void* user_data) {
printf("projectM is requesting a preset switch (hard_cut: %s)!\n", is_hard_cut ? "true" : "false");
uint32_t indx = projectm_playlist_play_next(app_data.playlist, false);
return;
}

EM_JS(void,getCustomShader,(),{
var pth=document.querySelector('#milkPath2').innerHTML;
var presetName = pth.split('/').pop();
if (window.updatePresetDisplay) { window.updatePresetDisplay(presetName); }
console.log('Getting preset: '+pth);
const ff=new XMLHttpRequest();
ff.open('GET',pth,true);
ff.responseType='arraybuffer';
var statEl5 = document.querySelector('#stat');
if (statEl5) { statEl5.innerHTML='Downloading Shader'; statEl5.style.backgroundColor='yellow'; }
ff.addEventListener("load",function(){
let sarrayBuffer=ff.response;
if(sarrayBuffer){
let sfil=new Uint8ClampedArray(sarrayBuffer);
FS.writeFile("/presets/preset_custom.milk",sfil);
setTimeout(function(){
Module.ccall('load_preset_file', null, ['string'], ["/presets/preset_custom.milk"]);
var statEl6 = document.querySelector('#stat');
if (statEl6) { statEl6.innerHTML='Downloaded Shader'; statEl6.style.backgroundColor='blue'; }
},20);
}
});
ff.send(null);
return;
});

EM_JS(void,getShader,(int num),{
var pth=document.querySelector('#milkPath').innerHTML;
var presetName = pth.split('/').pop();
if (window.updatePresetDisplay) { window.updatePresetDisplay(presetName); }
console.log('Getting preset: '+pth);
const ff=new XMLHttpRequest();
ff.open('GET',pth,true);
ff.responseType='arraybuffer';
var statEl5 = document.querySelector('#stat');
if (statEl5) { statEl5.innerHTML='Downloading Shader'; statEl5.style.backgroundColor='yellow'; }
ff.addEventListener("load",function(){
let sarrayBuffer=ff.response;
if(sarrayBuffer){
let sfil=new Uint8ClampedArray(sarrayBuffer);
FS.writeFile("/presets/preset_"+num+".milk",sfil);
setTimeout(function(){
document.querySelector('#stat').innerHTML='Downloaded Shader';
document.querySelector('#stat').style.backgroundColor='blue';
},20);
}
});
ff.send(null);
return;
});

EM_JS(void, js_init_projectm_dom, (), {
if (window.projectMDOMInitialized) return;
window.projectMDOMInitialized = true;

function vfsPathExists(path) {
    try {
        FS.stat(path);
        return true;
    } catch (e) {
        return false;
    }
}

function vfsMkdir(path) {
    try {
        FS.mkdir(path);
    } catch (e) {
        // Ignore if already exists or other FS errors
    }
}

vfsMkdir('/presets');
vfsMkdir('/textures');
vfsMkdir('/snd');
// Canvas sizing is handled by the JS ResizeObserver before init();
// Do not clobber it here.
var $sngs=[];
var $shds=[];
var $texs=[];
var $customMilk=[];

function getBasePath(id, fallback) {
    var el = document.querySelector(id);
    if (el && el.innerHTML && el.innerHTML.trim().length > 0) {
        var path = el.innerHTML.trim();
        if (path.charAt(path.length - 1) !== '/') {
            path += '/';
        }
        return path;
    }
    return fallback;
}

function textures(xml, textureBase){
    const nparser = new DOMParser();
    const htmlDocs = nparser.parseFromString(xml.responseText, 'text/html');
    const preList = htmlDocs.getElementsByTagName('pre')[0].getElementsByTagName('a');
    $texs[0] = preList.length;
    console.log('scanned: ' + $texs[0] + ' textures from ' + textureBase);
    for (var i = 5; i < preList.length; i++) {
        var fname = preList[i].getAttribute('href');
        var fileName = preList[i].innerText.trim();
        var fullUrl = new URL(fname, textureBase).href;
        $texs[i] = fullUrl;
        console.log('$texs[' + i + ']: ', $texs[i]);
        (function(filename, url) {
            const ff = new XMLHttpRequest();
            ff.open('GET', url, true);
            ff.responseType = 'arraybuffer';
            var statEl = document.querySelector('#stat');
            if (statEl) { statEl.innerHTML = 'Downloading Texture'; statEl.style.backgroundColor = 'yellow'; }
            ff.addEventListener("load", function(){
                let sarrayBuffer = ff.response;
                if (sarrayBuffer) {
                    let sfil = new Uint8ClampedArray(sarrayBuffer);
                    FS.writeFile("/textures/" + filename, sfil);
                    console.log('got texture: ' + filename + ' from ' + url);
                    setTimeout(function(){
                        var statEl2 = document.querySelector('#stat');
                        if (statEl2) { statEl2.innerHTML = 'Downloaded Texture'; statEl2.style.backgroundColor = 'blue'; }
                    }, 500);
                }
            });
            ff.send(null);
        })(fileName, fullUrl);
    }
}

function scanTextures(){
    var textureBase = getBasePath('#textureDir', 'textures/');
    if (!textureBase.startsWith('http://') && !textureBase.startsWith('https://')) {
        textureBase = new URL(textureBase, window.location.href).href;
    }
    const nxhttp = new XMLHttpRequest();
    nxhttp.onreadystatechange = function(){
        if (this.readyState == 4 && this.status == 200) {
            console.log('scanning textures from: ' + textureBase);
            textures(this, textureBase);
        }
    };
    nxhttp.open('GET', textureBase, true);
    nxhttp.send();
}

function customMilkShds(xml){
const nparser=new DOMParser();
const htmlDocs=nparser.parseFromString(xml.responseText,'text/html');
const preList=htmlDocs.getElementsByTagName('pre')[0].getElementsByTagName('a');
var baseUrl='https://glsl.1ink.us/custom_milk/';
$customMilk=[];
for(var i=0;i<preList.length;i++){
var txxt=preList[i].getAttribute('href');
var fullUrl=new URL(txxt,baseUrl).href;
if(fullUrl.indexOf('.milk')!==-1){
$customMilk.push(fullUrl);
}
}
console.log('Scanned '+$customMilk.length+' custom milk presets.');
if($customMilk.length>0){
setTimeout(function(){ getCustomMilkShaders(); },3000);
}
}

function scanCustomMilk(){
const nxhttp=new XMLHttpRequest();
nxhttp.onreadystatechange=function(){
if(this.readyState==4&&this.status==200){ customMilkShds(this); }
};
nxhttp.open('GET','https://glsl.1ink.us/custom_milk/',true);
nxhttp.send();
}

function getCustomMilkShaders(){
var completed=0;
var total=$customMilk.length;
var statEl3=document.querySelector('#stat');
if(statEl3){statEl3.innerHTML='Downloading Custom Presets';statEl3.style.backgroundColor='yellow';}
for(var i=0;i<total;i++){
(function(src,idx){
const ff=new XMLHttpRequest();
ff.open('GET',src,true);
ff.responseType='arraybuffer';
ff.addEventListener("load",function(){
var buf=ff.response;
if(buf){
FS.writeFile("/presets/custmilk_"+idx+".milk",new Uint8ClampedArray(buf));
console.log('Got custom preset: custmilk_'+idx+'.milk from '+src);
}
completed++;
if(completed===total){
console.log('Custom milk presets downloaded (not adding to auto-change playlist).');
if(statEl3){statEl3.innerHTML='Custom Presets Ready';statEl3.style.backgroundColor='blue';}
}
});
ff.addEventListener("error",function(){
console.warn('Failed to download custom preset: '+src);
completed++;
if(completed===total){
console.log('Custom milk presets downloaded (not adding to auto-change playlist).');
if(statEl3){statEl3.innerHTML='Custom Presets Ready';statEl3.style.backgroundColor='blue';}
}
});
ff.send(null);
})($customMilk[i],i);
}
}

function loadRandomCustomMilk(){
if($customMilk.length===0){
console.log('No custom milk presets available yet.');
document.querySelector('#stat').innerHTML='Custom presets loading...';
document.querySelector('#stat').style.backgroundColor='orange';
return;
}
var idx=Math.floor(Math.random()*$customMilk.length);
var fname='/presets/custmilk_'+idx+'.milk';
var originalName = $customMilk[idx].split('/').pop();
Module.ccall('load_preset_file', null, ['string'], [fname]);
if (window.updatePresetDisplay) { window.updatePresetDisplay(originalName); }
document.querySelector('#stat').innerHTML='Loaded: custmilk_'+idx+'.milk';
document.querySelector('#stat').style.backgroundColor='green';
console.log('Loading random custom milk: '+fname);
}

var $milk=[];
var $milkLrg=[];
var $milkMed=[];
var $milkSml=[];

function parseMilkDir(xml,baseUrl,array){
const nparser=new DOMParser();
const htmlDocs=nparser.parseFromString(xml.responseText,'text/html');
const preList=htmlDocs.getElementsByTagName('pre')[0].getElementsByTagName('a');
for(var i=1;i<preList.length;i++){
var txxt=preList[i].getAttribute('href');
var fullUrl=new URL(txxt,baseUrl).href;
array.push(fullUrl);
}
console.log('Scanned '+array.length+' presets from '+baseUrl);
}

function scanMilkDir(url,array){
const nxhttp=new XMLHttpRequest();
nxhttp.onreadystatechange=function(){
if(this.readyState==4&&this.status==200){
parseMilkDir(this,url,array);
}};
nxhttp.open('GET',url,true);
nxhttp.send();
}

function sngs(xml, songBase){
    const nparser = new DOMParser();
    const htmlDocs = nparser.parseFromString(xml.responseText, 'text/html');
    const preList = htmlDocs.getElementsByTagName('pre')[0].getElementsByTagName('a');
    $sngs[0] = preList.length;
    console.log('scanned: ' + $sngs[0] + ' songs from ' + songBase);
    for (var i = 1; i < preList.length; i++) {
        var fname = preList[i].getAttribute('href');
        var fullUrl = new URL(fname, songBase).href;
        $sngs[i] = fullUrl;
    }
}

function scanSongs(){
    var songBase = getBasePath('#songDir', 'songs/');
    if (!songBase.startsWith('http://') && !songBase.startsWith('https://')) {
        songBase = new URL(songBase, window.location.href).href;
    }
    const nxhttp = new XMLHttpRequest();
    nxhttp.onreadystatechange = function(){
        if (this.readyState == 4 && this.status == 200) {
            sngs(this, songBase);
        }
    };
    nxhttp.open('GET', songBase, true);
    nxhttp.send();
}

var lastSongFileName = '';

const fll = new BroadcastChannel('file');
fll.addEventListener('message', ea => {
    const uniqueFileName = `/snd/song_${Date.now()}.wav`;
    console.log(`JS Event: Received new song. Writing to unique path: ${uniqueFileName}`);
    const fill = new Uint8Array(ea.data.data);
    FS.writeFile(uniqueFileName, fill);
    if (lastSongFileName && vfsPathExists(lastSongFileName)) {
        FS.unlink(lastSongFileName);
        console.log(`JS Event: Cleaned up previous song file: ${lastSongFileName}`);
    }
    lastSongFileName = uniqueFileName;
    setTimeout(function() {
        Module.ccall(
            'pl',                   // C function name
            null,                   // return type
            ['string'],             // argument types
            [uniqueFileName]        // arguments
        );
const shutDown=new BroadcastChannel('shutDown');
shutDown.postMessage({data:222});
    }, 250); // Shorter timeout should be fine
});

function getShader(pth,fname){
const ff=new XMLHttpRequest();
ff.open('GET',pth,true);
ff.responseType='arraybuffer';
var statEl5 = document.querySelector('#stat');
if (statEl5) { statEl5.innerHTML='Downloading Shader'; statEl5.style.backgroundColor='yellow'; }
ff.addEventListener("load",function(){
let sarrayBuffer=ff.response;
if(sarrayBuffer){
let sfil=new Uint8ClampedArray(sarrayBuffer);
FS.writeFile(fname,sfil);
console.log('got preset: '+fname);
document.querySelector('#stat').innerHTML='Downloaded Shader';
document.querySelector('#stat').style.backgroundColor='blue';
const presetFileNameToLoad = fname;
console.log("JS: Attempting to load pre-downloaded: " + presetFileNameToLoad);
try {
const content = FS.readFile(presetFileNameToLoad, { encoding: 'utf8' });
console.log("JS: Content of " + presetFileNameToLoad + " (first 200 chars):", content.substring(0,200));
if (content.length === 0) {
console.error("JS: File " + presetFileNameToLoad + " is EMPTY!");
}
} catch (e) {
console.error("JS: Failed to read file " + presetFileNameToLoad + " from FS:", e);
return;
}
}
});
ff.send(null);
}

function snd(){
    if ($sngs.length < 6 || !$sngs[0]) {
        console.log('No songs available yet.');
        return;
    }
    let randSong = Math.floor(($sngs[0] - 5) * Math.random());
    let songSrc = $sngs[randSong + 5];
    console.log('Song: ', songSrc);
    document.querySelector('#track').src = songSrc;
    const sng = new BroadcastChannel('sng');
    sng.postMessage({data: songSrc});
}

document.querySelector('#musicBtn').addEventListener('click',function(){
window.open('./flac');
setTimeout(function(){
snd();
},1550);
});

var milkBtnEl = document.querySelector('#milkBtn');
if (milkBtnEl) {
    milkBtnEl.addEventListener('click',function(){
        loadRandomCustomMilk();
    });
}

document.querySelector('#customMilkBtn').addEventListener('click',function(){
loadRandomCustomMilk();
});

var createSpriteBtnEl = document.querySelector('#createSpriteBtn');
if (createSpriteBtnEl) {
    createSpriteBtnEl.addEventListener('click',function(){
        Module._createSprite();
    });
}

var pth=document.querySelector('#milkPath').innerHTML;
scanTextures();
scanSongs();
scanCustomMilk();
var meshSizeEl = document.querySelector('#meshSize');
if (meshSizeEl) {
    meshSizeEl.addEventListener('change', (event) => {
        let meshValue = event.target.value;
        // Split the value into two numbers
        let values = meshValue.split(',').map(Number);
        console.log('Setting Mesh:', values[0], values[1]);
        Module._setMesh(values[0], values[1]);
    });
}


//  const meshValue = document.querySelector('#meshSize').value;
   // Split the value into two numbers
// const values = meshValue.split(',').map(Number);
// console.log('Setting Mesh:', values[0], values[1]);
// Module.setMesh(values[0], values[1]);


});

extern "C" {
EMSCRIPTEN_KEEPALIVE
int init() {
if (pm) return 0;
js_init_projectm_dom();
EmscriptenWebGLContextAttributes webgl_attrs;
emscripten_webgl_init_context_attributes(&webgl_attrs);
webgl_attrs.majorVersion = 2;
webgl_attrs.minorVersion = 0;
webgl_attrs.alpha = EM_TRUE;
webgl_attrs.stencil = EM_TRUE;
webgl_attrs.depth = EM_TRUE;
webgl_attrs.antialias = EM_TRUE;
webgl_attrs.premultipliedAlpha=EM_TRUE;
webgl_attrs.preserveDrawingBuffer=EM_FALSE;
webgl_attrs.enableExtensionsByDefault=EM_TRUE;
webgl_attrs.powerPreference=EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
display=eglGetDisplay(EGL_DEFAULT_DISPLAY);
eglInitialize(display,&major,&minor);
// eglconfig is not yet initialized here; query attributes after eglChooseConfig selects a config.

static EGLint ctx_att[]={
EGL_CONTEXT_CLIENT_TYPE,EGL_OPENGL_ES_API,
EGL_CONTEXT_CLIENT_VERSION,3,
EGL_CONTEXT_MAJOR_VERSION_KHR,3,
EGL_CONTEXT_MINOR_VERSION_KHR,0,
// EGL_CONTEXT_FLAGS_KHR,EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE_BIT_KHR,
EGL_CONTEXT_PRIORITY_LEVEL_IMG,EGL_CONTEXT_PRIORITY_REALTIME_NV,
// EGL_CONTEXT_PRIORITY_LEVEL_IMG,EGL_CONTEXT_PRIORITY_HIGH_IMG,
EGL_NONE
};

static EGLint att_lst2[]={
EGL_GL_COLORSPACE_KHR,colorSpace,
EGL_NONE
};

static EGLint att_lst[]={
EGL_COLOR_COMPONENT_TYPE_EXT,EGL_COLOR_COMPONENT_TYPE_FLOAT_EXT,
// EGL_COLOR_COMPONENT_TYPE_EXT,EGL_COLOR_COMPONENT_TYPE_FIXED_EXT,
// EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR,EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT_KHR,
// EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR,EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT_KHR,
EGL_RENDERABLE_TYPE,EGL_OPENGL_ES3_BIT,
// EGL_RENDERABLE_TYPE,EGL_OPENGL_BIT,  // EGL 1.5 needed  (WASM cannot Window surface)
// EGL_RENDERABLE_TYPE,EGL_NONE,
// EGL_CONFORMANT,EGL_OPENGL_BIT,
// EGL_CONFORMANT,EGL_NONE,
//  EGL_CONFIG_CAVEAT,EGL_NONE,
EGL_CONTEXT_OPENGL_ROBUST_ACCESS_EXT,EGL_TRUE,
EGL_CONTEXT_OPENGL_NO_ERROR_KHR,EGL_TRUE,
// EGL_DEPTH_ENCODING_NV,EGL_DEPTH_ENCODING_NONLINEAR_NV,
// EGL_RENDER_BUFFER,EGL_TRIPLE_BUFFER_NV,
EGL_RENDER_BUFFER,EGL_QUADRUPLE_BUFFER_NV, //   available in OpenGL
// EGL_SURFACE_TYPE,EGL_MULTISAMPLE_RESOLVE_BOX_BIT,
EGL_SURFACE_TYPE,EGL_SWAP_BEHAVIOR_PRESERVED_BIT|EGL_MULTISAMPLE_RESOLVE_BOX_BIT,
EGL_MULTISAMPLE_RESOLVE,EGL_MULTISAMPLE_RESOLVE_BOX,
//  EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE,EGL_TRUE, // EGL 1.5 "...the context will only support OpenGL ES 3.0 and later features."
EGL_COLOR_FORMAT_HI,colorFormat, //  available in OpenGL
// EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY,EGL_NO_RESET_NOTIFICATION,
// EGL_NATIVE_RENDERABLE,EGL_TRUE,
EGL_COLOR_BUFFER_TYPE,EGL_RGB_BUFFER,
EGL_LUMINANCE_SIZE,0, // available in OpenGL
EGL_RED_SIZE,numRed,
EGL_GREEN_SIZE,numGreen,
EGL_BLUE_SIZE,numBlue,
EGL_ALPHA_SIZE,numAlpha,
EGL_DEPTH_SIZE,numDepth,
EGL_STENCIL_SIZE,numStencil,
EGL_BUFFER_SIZE,numBuffer,
EGL_COVERAGE_BUFFERS_NV,numBuffersNV, // available in GLES 3.1
EGL_COVERAGE_SAMPLES_NV,numSamplesNV,
EGL_SAMPLE_BUFFERS,numMBuffers,
EGL_SAMPLES,numSamples,
EGL_NONE
};

eglChooseConfig(display,att_lst,&eglconfig,1,&config_size);
ctxegl=eglCreateContext(display,eglconfig,EGL_NO_CONTEXT,ctx_att);
surface=eglCreateWindowSurface(display,eglconfig,(NativeWindowType)0,att_lst2);
// eglBindAPI(EGL_OPENGL_ES_API);
eglBindAPI(EGL_OPENGL_API);
gl_ctx = emscripten_webgl_create_context("#mcanvas", &webgl_attrs);
if (!gl_ctx) {
fprintf(stderr, "Failed to create WebGL context\n");
return 1;
}
EMSCRIPTEN_RESULT em_res = emscripten_webgl_make_context_current(gl_ctx);
eglMakeCurrent(display,surface,surface,ctxegl);
emscripten_webgl_make_context_current(gl_ctx);
glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,GL_NICEST);
glHint(GL_GENERATE_MIPMAP_HINT,GL_NICEST);
if (em_res != EMSCRIPTEN_RESULT_SUCCESS) {
fprintf(stderr, "Failed to activate the WebGL context for rendering\n");
return 1;
}

// These are probably redundant since all GL extensions are enabled by default
// https://github.com/emscripten-core/emscripten/blob/1b01a9ef2b60184eb70616bbb294cf33d011bbb2/src/settings.js#L481
// https://emscripten.org/docs/api_reference/html5.h.html#c.EmscriptenWebGLContextAttributes.enableExtensionsByDefault
//
// enable floating-point texture support for motion vector grid
// https://github.com/projectM-visualizer/projectm/blob/master/docs/emscripten.rst#initializing-emscriptens-opengl-context
// https://emscripten.org/docs/api_reference/html5.h.html#c.emscripten_webgl_enable_extension

emscripten_webgl_enable_extension(gl_ctx, "OES_texture_float");
// projectM uses half-float textures for the motion vector grid to store
// the displacement of the previous frame's warp mesh. WebGL 2.0 sadly
// doesn't support this texture format by default (while OpenGL ES 3 does)
// so we have to enable the following WebGL extensions.

emscripten_webgl_enable_extension(gl_ctx, "OES_texture_half_float");
emscripten_webgl_enable_extension(gl_ctx, "OES_texture_half_float_linear");
emscripten_webgl_enable_extension(gl_ctx,"EXT_color_buffer_float"); // GLES float
emscripten_webgl_enable_extension(gl_ctx,"EXT_float_blend"); // GLES float

// Phase 2: Detect the best available floating-point texture format for the
// dual ping-pong FBO system. DetectFormat() checks EXT_color_buffer_float
// (RGBA32F), EXT_color_buffer_half_float (RGBA16F), and falls back to RGBA8.
// This must be called after the WebGL context is made current so that
// extension availability can be probed reliably.
g_dualFbo.DetectFormat(gl_ctx);

pm = projectm_create();
app_data.projectm_engine = pm;
playlist = projectm_playlist_create(pm);
app_data.playlist = playlist;
const char * loc="/presets/";
projectm_playlist_add_path(playlist,loc,true,true);
projectm_playlist_set_preset_switched_event_callback(playlist,&load_preset_callback_done,&app_data);
if (!pm) {
fprintf(stderr, "Failed to create projectM handle\n");
return 1;
}
const char* texture_search_paths[] = {"textures"};
projectm_set_texture_search_paths(pm, texture_search_paths, 1);
projectm_set_fps(pm, 60);
projectm_set_preset_duration(pm, 30.0);
projectm_set_soft_cut_duration(pm, 17.0);
// projectm_set_hard_cut_duration(pm, 48.0);
// projectm_set_hard_cut_enabled(pm, true);
projectm_set_beat_sensitivity(pm, 1.50);
projectm_playlist_set_shuffle(playlist,true);
projectm_set_preset_switch_failed_event_callback(pm, &_on_preset_switch_failed, nullptr);
projectm_set_preset_switch_requested_event_callback(pm, &on_preset_switch_requested, &app_data);
// projectm_playlist_connect(app_data.playlist,app_data.projectm_engine);
printf("  --==  projectM initialized!  ==--\n");
js_initialize_worklet_system_once(reinterpret_cast<uintptr_t>(app_data.projectm_engine));
js_initialize_stream_analyser();
return 0;
}
} // extern "C"

extern "C" {
EMSCRIPTEN_KEEPALIVE
void add_preset_path(){
const char * loc="/presets/";
char preset_file[256];
for (int i = 0; i <= 100; ++i) {
snprintf(preset_file, sizeof(preset_file), "/presets/preset_%d.milk", i);
projectm_playlist_add_preset(app_data.playlist, preset_file, false);
}
return;
}

EMSCRIPTEN_KEEPALIVE
void add_existing_vfs_presets(){
char preset_file[256];
int added = 0;
for (int i = 0; i <= 100; ++i) {
snprintf(preset_file, sizeof(preset_file), "/presets/preset_%d.milk", i);
if (access(preset_file, F_OK) == 0) {
projectm_playlist_add_preset(app_data.playlist, preset_file, false);
added++;
}
}
printf("Added %d existing VFS presets to playlist.\n", added);
return;
}

EMSCRIPTEN_KEEPALIVE
void add_preset_file(const char* path) {
if (!app_data.playlist) return;
projectm_playlist_add_preset(app_data.playlist, path, false);
return;
}

EMSCRIPTEN_KEEPALIVE
void add_custom_milk_paths(int count){
char preset_file[256];
int added=0;
for(int i=0;i<count;++i){
snprintf(preset_file,sizeof(preset_file),"/presets/custmilk_%d.milk",i);
if(access(preset_file,F_OK)==0){
projectm_playlist_add_preset(app_data.playlist,preset_file,false);
added++;
}
}
printf("Added %d of %d custom milk presets to playlist.\n",added,count);
return;
}

EMSCRIPTEN_KEEPALIVE
void set_mesh(int w,int h){
projectm_set_mesh_size(pm,w,h);
return;
}

EMSCRIPTEN_KEEPALIVE
void destruct() {
if (pm) {
projectm_destroy(pm);
}
pm = NULL;
// Phase 2: Release dual FBO resources before destroying the WebGL context
// to avoid calling OpenGL functions with an invalid context.
g_dualFbo.ReleaseAll();
if (gl_ctx) emscripten_webgl_destroy_context(gl_ctx);
gl_ctx = NULL;
return;
}

EMSCRIPTEN_KEEPALIVE
void switch_preset() {
if (!app_data.playlist) return;
projectm_playlist_play_next(app_data.playlist, false);
return;
}

EMSCRIPTEN_KEEPALIVE
void set_aspect_correction(bool enabled) {
if (!pm) return;
projectm_set_aspect_correction(pm, enabled);
return;
}

EMSCRIPTEN_KEEPALIVE
void set_preset_locked(bool locked) {
if (!pm) return;
projectm_set_preset_locked(pm, locked);
printf("Preset lock set to: %s\n", locked ? "true" : "false");
return;
}
} // extern "C"

extern "C" {
EMSCRIPTEN_KEEPALIVE
void load_preset_file(const char* filename) {
    if (!pm) return;

    // Phase 3: Route all preset switches through the playlist manager so that
    // the engine's built-in transition cleanup routines are always triggered
    // (dual FBO re-init, glClearColor reset, blend state isolation).
    if (app_data.playlist) {
        // Search the existing playlist for this filename.
        uint32_t count = projectm_playlist_size(app_data.playlist);
        int32_t foundIdx = -1;
        for (uint32_t i = 0; i < count; ++i) {
            char* item = projectm_playlist_item(app_data.playlist, i);
            if (item) {
                bool match = (strcmp(item, filename) == 0);
                projectm_playlist_free_string(item);
                if (match) {
                    foundIdx = static_cast<int32_t>(i);
                    break;
                }
            }
        }
        if (foundIdx < 0) {
            // Add to playlist so the manager can track it.
            projectm_playlist_add_preset(app_data.playlist, filename, false);
            foundIdx = static_cast<int32_t>(projectm_playlist_size(app_data.playlist) - 1);
        }
        // Switch via the playlist manager (hard_cut=false → soft transition).
        projectm_playlist_set_position(app_data.playlist,
                                       static_cast<uint32_t>(foundIdx), false);
        return;
    }

    // Fallback: no playlist attached yet – load directly.
    projectm_load_preset_file(pm, filename, true);
}
} // extern "C"

extern "C" {
EMSCRIPTEN_KEEPALIVE
void render_frame() {
if (!pm) return;
projectm_opengl_render_frame(pm);
return;
}

EMSCRIPTEN_KEEPALIVE
void set_window_size(int width, int height) {
if (!pm) return;
emscripten_set_canvas_element_size("#mcanvas", width, height);
emscripten_set_canvas_element_size("#scanvas", width, height);
glViewport(0,0,width,height);
glScissor(0,0,width,height);
projectm_set_window_size(pm, width, height);
// Phase 2: Resize all allocated dual ping-pong FBOs to match the new viewport.
g_dualFbo.Resize(width, height);
return;
}

EMSCRIPTEN_KEEPALIVE
void projectm_pcm_add_float_wrapper(uintptr_t pm_handle_value, float* audio_data, unsigned int num_samples_per_channel, int channels_enum_value) {
    (void)pm_handle_value;
    projectm_handle current_pm_handle = app_data.projectm_engine;
    if (!current_pm_handle) {
        fprintf(stderr, "Error: projectM handle is null in pcm_add_float_wrapper.\n");
        return;
    }
    projectm_pcm_add_float(current_pm_handle, audio_data, num_samples_per_channel, static_cast<projectm_channels>(channels_enum_value));
}
} // extern "C"

// =============================================================================
// Phase 2 + Phase 3: Dual ping-pong FBO lifecycle C API (EMSCRIPTEN_KEEPALIVE exports)
//
// These functions are the public interface for JavaScript / the transition
// layer to drive the dual FBO system:
//
//   dual_fbo_begin_transition()  – allocate Preset B FBOs (lazy, call once)
//   dual_fbo_end_transition()    – promote Preset B → Preset A (transition done)
//   dual_fbo_cancel_transition() – release Preset B FBOs without promoting
//   dual_fbo_swap_preset_a()     – ping-pong Preset A Read/Write each frame
//   dual_fbo_swap_preset_b()     – ping-pong Preset B Read/Write each frame
//   dual_fbo_get_a_read_fbo()    – FBO ID to bind as render target (Preset A)
//   dual_fbo_get_a_read_tex()    – texture ID to sample as history (Preset A)
//   dual_fbo_get_b_write_fbo()   – FBO ID to bind as render target (Preset B)
//   dual_fbo_get_b_read_tex()    – texture ID to sample as history (Preset B)
//   dual_fbo_is_preset_b_allocated() – query whether transition is active
//   dual_fbo_get_format()        – 0=RGBA32F, 1=RGBA16F, 2=RGBA8
//
// Phase 3 isolated render helpers (save/restore full GL state):
//   dual_fbo_render_preset_a()   – render Preset A into A_Write FBO w/ state guard
//   dual_fbo_render_preset_b()   – force-reset GL, render Preset B into B_Write FBO
// =============================================================================
extern "C" {

/**
 * @brief Lazily allocates Preset B ping-pong FBOs to start a transition.
 *
 * Uses the current Preset A dimensions.  Safe to call when a transition is
 * already active (no-op in that case).
 *
 * @return true on success.
 */
EMSCRIPTEN_KEEPALIVE
bool dual_fbo_begin_transition()
{
    int w = g_dualFbo.Width();
    int h = g_dualFbo.Height();
    if (w <= 0 || h <= 0)
    {
        fprintf(stderr, "DualFBO: Cannot begin transition – Preset A FBOs not allocated.\n");
        return false;
    }
    return g_dualFbo.AllocatePresetB(w, h);
}

/**
 * @brief Completes a transition by promoting Preset B's FBOs into Preset A.
 *
 * Old Preset A resources are freed; Preset B's resources become the new
 * Preset A. After this call Preset B is no longer allocated.
 */
EMSCRIPTEN_KEEPALIVE
void dual_fbo_end_transition()
{
    g_dualFbo.PromoteBtoA();
}

/**
 * @brief Cancels an in-progress transition and releases Preset B FBOs.
 */
EMSCRIPTEN_KEEPALIVE
void dual_fbo_cancel_transition()
{
    g_dualFbo.ReleasePresetB();
}

/**
 * @brief Swaps Preset A's Read/Write FBOs (call once per rendered frame).
 *
 * After the swap the former Write texture becomes the Read (history) texture
 * for the next frame, implementing the ping-pong feedback loop.
 */
EMSCRIPTEN_KEEPALIVE
void dual_fbo_swap_preset_a()
{
    g_dualFbo.SwapPresetA();
}

/**
 * @brief Swaps Preset B's Read/Write FBOs (call once per rendered frame during a transition).
 */
EMSCRIPTEN_KEEPALIVE
void dual_fbo_swap_preset_b()
{
    g_dualFbo.SwapPresetB();
}

// ---- Accessor functions (return raw OpenGL / WebGL handles) ----------------

/** @brief Returns the Preset A Read FBO ID (bind as render target). */
EMSCRIPTEN_KEEPALIVE
GLuint dual_fbo_get_a_read_fbo()   { return g_dualFbo.GetAReadFBO(); }

/** @brief Returns the Preset A Write FBO ID. */
EMSCRIPTEN_KEEPALIVE
GLuint dual_fbo_get_a_write_fbo()  { return g_dualFbo.GetAWriteFBO(); }

/** @brief Returns the Preset A Read texture ID (sample as history/feedback). */
EMSCRIPTEN_KEEPALIVE
GLuint dual_fbo_get_a_read_tex()   { return g_dualFbo.GetAReadTex(); }

/** @brief Returns the Preset A Write texture ID. */
EMSCRIPTEN_KEEPALIVE
GLuint dual_fbo_get_a_write_tex()  { return g_dualFbo.GetAWriteTex(); }

/** @brief Returns the Preset B Read FBO ID (bind as render target). */
EMSCRIPTEN_KEEPALIVE
GLuint dual_fbo_get_b_read_fbo()   { return g_dualFbo.GetBReadFBO(); }

/** @brief Returns the Preset B Write FBO ID. */
EMSCRIPTEN_KEEPALIVE
GLuint dual_fbo_get_b_write_fbo()  { return g_dualFbo.GetBWriteFBO(); }

/** @brief Returns the Preset B Read texture ID (sample as history/feedback). */
EMSCRIPTEN_KEEPALIVE
GLuint dual_fbo_get_b_read_tex()   { return g_dualFbo.GetBReadTex(); }

/** @brief Returns the Preset B Write texture ID. */
EMSCRIPTEN_KEEPALIVE
GLuint dual_fbo_get_b_write_tex()  { return g_dualFbo.GetBWriteTex(); }

/**
 * @brief Returns true if the Preset B FBOs are currently allocated
 *        (i.e., a transition is in progress).
 */
EMSCRIPTEN_KEEPALIVE
bool dual_fbo_is_preset_b_allocated()
{
    return g_dualFbo.IsPresetBAllocated();
}

/**
 * @brief Returns the active FBO colour format as an integer.
 *
 * Values:
 *   0 = RGBA32F (GL_RGBA32F, 32-bit float)
 *   1 = RGBA16F (GL_RGBA16F, 16-bit half-float)
 *   2 = RGBA8   (GL_RGBA8, 8-bit normalized – shaders should clamp output)
 */
EMSCRIPTEN_KEEPALIVE
int dual_fbo_get_format()
{
    return static_cast<int>(g_dualFbo.GetFormat());
}

// ---- Phase 3: isolated render helpers --------------------------------------

/**
 * @brief Renders the current projectM frame for Preset A with full GL state
 *        save/restore isolation.
 *
 * Binds Preset A's Write FBO before rendering so that projectM's output is
 * captured into the dual FBO system. Saves all relevant GL state before the
 * render call and restores it afterward to prevent leakage into subsequent
 * pipeline phases or into the browser's WebGL layer.
 *
 * After this call, swap Preset A's ping-pong FBOs with dual_fbo_swap_preset_a().
 */
EMSCRIPTEN_KEEPALIVE
void dual_fbo_render_preset_a()
{
    if (!pm || !g_dualFbo.IsPresetAAllocated())
    {
        return;
    }
    GLStateGuard guard;
    glBindFramebuffer(GL_FRAMEBUFFER, g_dualFbo.GetAWriteFBO());
    projectm_opengl_render_frame(pm);
}

/**
 * @brief Renders the current projectM frame for Preset B with full GL state
 *        save/restore isolation.
 *
 * Must be called after dual_fbo_render_preset_a() during a transition frame.
 * Before rendering, gl_reset_state_between_pipelines() is called to eliminate
 * any additive blend or texture state left by Preset A's draw – this prevents
 * the "white screen" / additive bleed artefacts.
 *
 * After this call, swap Preset B's ping-pong FBOs with dual_fbo_swap_preset_b().
 */
EMSCRIPTEN_KEEPALIVE
void dual_fbo_render_preset_b()
{
    if (!pm || !g_dualFbo.IsPresetBAllocated())
    {
        return;
    }
    // Force-reset GL state left by Preset A's draw before entering Preset B's pipeline.
    gl_reset_state_between_pipelines();
    GLStateGuard guard;
    glBindFramebuffer(GL_FRAMEBUFFER, g_dualFbo.GetBWriteFBO());
    projectm_opengl_render_frame(pm);
}

} // extern "C"

int main(){
init();
return 0;
}

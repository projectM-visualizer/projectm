/**
 * @file Shader.hpp
 * @brief Implements an interface to a single shader program instance.
 */
#pragma once

#include "Renderer/Texture.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x4.hpp>

#include <map>
#include <string>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Shader compilation exception.
 */
class ShaderException : public std::exception
{
public:
    ShaderException(std::string message)
        : m_message(std::move(message))
    {
    }

    virtual ~ShaderException() = default;

    const char* what() const noexcept override
    {
        return m_message.c_str();
    }

    const std::string& message() const
    {
        return m_message;
    }

private:
    std::string m_message;
};


/**
 * @brief Base class containing a shader program, consisting of a vertex and fragment shader.
 */
class Shader
{
public:
    /**
     * GLSL version structure
     */
    struct GlslVersion {
        int major{}; //!< Major OpenGL shading language version
        int minor{}; //!< Minor OpenGL shading language version
    };

    /**
     * Creates a new shader.
     */
    Shader();

    /**
     * Destructor.
     */
    ~Shader();

    /**
     * @brief Compiles a vertex and fragment shader into a program.
     * @throws ShaderException Thrown if compilation of a shader or program linking failed.
     * @param vertexShaderSource The vertex shader source.
     * @param fragmentShaderSource The fragment shader source.
     */
    void CompileProgram(const std::string& vertexShaderSource,
                        const std::string& fragmentShaderSource);

    /**
     * Deferred GL compilation (GL_KHR_parallel_shader_compile):
     *
     * ANGLE backgrounds glCompileShader/glLinkProgram on a worker pool
     * (GL_KHR_parallel_shader_compile), but ANY program-observing call — including the
     * glDetachShader right after glLinkProgram in CompileProgram — synchronously resolves
     * the link on the calling thread. While the defer flag is set (only around preset
     * preload), CompileProgram skips the status queries AND the detach/delete cleanup,
     * leaving the link in flight; callers poll PendingCompileReady() (non-blocking
     * GL_COMPLETION_STATUS_KHR) and call FinalizeCompile() once ready. Bind()/Validate()/
     * SetUniform* finalize lazily as a safety net, so a pending shader is always safe to
     * use — first use just blocks until the background compile finishes.
     */

    /**
     * @brief Globally enables/disables deferred compilation for subsequent CompileProgram calls.
     * Render/GL thread only. Only set around preset preloading.
     */
    static void SetDeferCompilation(bool defer);

    /**
     * @brief Returns true if this shader has a deferred, not-yet-finalized link.
     */
    auto HasPendingCompile() const -> bool;

    /**
     * @brief Non-blocking: true once the deferred link has completed in the background
     * (or if there is no pending compile). Does NOT resolve the link.
     */
    auto PendingCompileReady() const -> bool;

    /**
     * @brief Performs the deferred status check + shader cleanup for a pending compile.
     * No-op if nothing is pending. Blocks if the background compile is still running.
     * @throws ShaderException Thrown if the deferred compilation or linking failed.
     */
    void FinalizeCompile() const;

    /**
     * @brief Validates that the program can run in the current state.
     * @param validationMessage The error message if validation failed.
     * @return true if the shader program is valid and can run, false if it broken.
     */
    bool Validate(std::string& validationMessage) const;

    /**
     * Binds the program into the current context.
     */
    void Bind() const;

    /**
     * Unbinds the program.
     */
    static void Unbind();

    /**
     * @brief Sets a single float uniform.
     * The program must be bound before calling this method!
     * @param uniform The uniform name
     * @param value The value to set.
     */
    void SetUniformFloat(const char* uniform, float value) const;

    /**
     * @brief Sets a single integer uniform.
     * The program must be bound before calling this method!
     * @param uniform The uniform name
     * @param value The value to set.
     */
    void SetUniformInt(const char* uniform, int value) const;

    /**
     * @brief Sets a float vec2 uniform.
     * The program must be bound before calling this method!
     * @param uniform The uniform name
     * @param values The values to set.
     */
    void SetUniformFloat2(const char* uniform, const glm::vec2& values) const;

    /**
     * @brief Sets an int vec2 uniform.
     * The program must be bound before calling this method!
     * @param uniform The uniform name
     * @param values The values to set.
     */
    void SetUniformInt2(const char* uniform, const glm::ivec2& values) const;

    /**
     * @brief Sets a float vec3 uniform.
     * The program must be bound before calling this method!
     * @param uniform The uniform name
     * @param values The values to set.
     */
    void SetUniformFloat3(const char* uniform, const glm::vec3& values) const;

    /**
     * @brief Sets an int vec3 uniform.
     * The program must be bound before calling this method!
     * @param uniform The uniform name
     * @param values The values to set.
     */
    void SetUniformInt3(const char* uniform, const glm::ivec3& values) const;

    /**
     * @brief Sets a float vec4 uniform.
     * The program must be bound before calling this method!
     * @param uniform The uniform name
     * @param values The values to set.
     */
    void SetUniformFloat4(const char* uniform, const glm::vec4& values) const;

    /**
     * @brief Sets an int vec4 uniform.
     * The program must be bound before calling this method!
     * @param uniform The uniform name
     * @param values The values to set.
     */
    void SetUniformInt4(const char* uniform, const glm::ivec4& values) const;

    /**
     * @brief Sets a float 3x4 matrix uniform.
     * The program must be bound before calling this method!
     * @param uniform The uniform name
     * @param values The matrix to set.
     */
    void SetUniformMat3x4(const char* uniform, const glm::mat3x4& values) const;

    /**
     * @brief Sets a float 4x4 matrix uniform.
     * The program must be bound before calling this method!
     * @param uniform The uniform name
     * @param values The matrix to set.
     */
    void SetUniformMat4x4(const char* uniform, const glm::mat4x4& values) const;

    /**
     * @brief Parses the shading language version string returned from OpenGL.
     * If this function does not return a good version (e.g. "major" not >0), then OpenGL is probably
     * not properly initialized or the context not made current.
     * @return The parsed version, or {0,0} if the version could not be parsed.
     */
    static auto GetShaderLanguageVersion() -> GlslVersion;

private:
    /**
     * @brief Compiles a single shader.
     * @throws ShaderException Thrown if compilation of the shader failed.
     * @param source The shader source.
     * @param type The shader type, e.g. GL_VERTEX_SHADER.
     * @return The shader ID.
     */
    auto CompileShader(const std::string& source, GLenum type) -> GLuint;

    /**
     * @brief Lazily finalizes a pending deferred compile, swallowing (but logging) errors.
     * Safety net for Bind()/Validate()/SetUniform* so render paths never throw.
     */
    void EnsureFinalizedNoThrow() const noexcept;

    GLuint m_shaderProgram{}; //!< The program ID.

    // Deferred-compile state. Mutable because the lazy
    // finalize safety net runs from const accessors (Bind/SetUniform are const).
    mutable bool m_pendingLink{false};           //!< A deferred link has not been finalized yet.
    mutable GLuint m_pendingVertexShader{};      //!< Attached vertex shader awaiting detach/delete.
    mutable GLuint m_pendingFragmentShader{};    //!< Attached fragment shader awaiting detach/delete.
    mutable std::string m_pendingVertexSource;   //!< Kept for the error log if the deferred link fails.
    mutable std::string m_pendingFragmentSource; //!< Kept for the error log if the deferred link fails.
};

} // namespace Renderer
} // namespace libprojectM

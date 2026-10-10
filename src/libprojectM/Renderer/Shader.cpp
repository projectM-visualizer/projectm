#include "Shader.hpp"

#include <Logging.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

// Non-blocking link-completion poll (GL_KHR_parallel_shader_compile).
// Value per the KHR extension spec.
#ifndef GL_COMPLETION_STATUS_KHR
#define GL_COMPLETION_STATUS_KHR 0x91B1
#endif

namespace libprojectM {
namespace Renderer {

namespace {
// When set (render/GL thread only, around preset
// preload), CompileProgram leaves the link in flight instead of resolving it.
bool s_deferCompilation{false};
} // namespace

Shader::Shader()
    : m_shaderProgram(glCreateProgram())
{
}

Shader::~Shader()
{
    if (m_shaderProgram)
    {
        // Note: deleting a program with a pending background link resolves (blocks on)
        // the link inside ANGLE first. Acceptable: only reached when a preload is
        // discarded, and by then the compile is normally long finished.
        glDeleteProgram(m_shaderProgram);
    }
    if (m_pendingVertexShader)
    {
        glDeleteShader(m_pendingVertexShader);
    }
    if (m_pendingFragmentShader)
    {
        glDeleteShader(m_pendingFragmentShader);
    }
}

void Shader::SetDeferCompilation(bool defer)
{
    s_deferCompilation = defer;
}

auto Shader::HasPendingCompile() const -> bool
{
    return m_pendingLink;
}

auto Shader::PendingCompileReady() const -> bool
{
    if (!m_pendingLink)
    {
        return true;
    }

    // Explicitly non-resolving in ANGLE: reports whether the background link (including
    // the Metal shader-library subtasks) has finished, without waiting for it.
    GLint completed{GL_FALSE};
    glGetProgramiv(m_shaderProgram, GL_COMPLETION_STATUS_KHR, &completed);
    return completed == GL_TRUE;
}

void Shader::FinalizeCompile() const
{
    if (!m_pendingLink)
    {
        return;
    }
    m_pendingLink = false;

    // The detach/delete below is what resolves the link inside ANGLE (blocking if the
    // background compile is still running — callers gate on PendingCompileReady()).
    glDetachShader(m_shaderProgram, m_pendingVertexShader);
    glDetachShader(m_shaderProgram, m_pendingFragmentShader);
    glDeleteShader(m_pendingVertexShader);
    glDeleteShader(m_pendingFragmentShader);
    m_pendingVertexShader = 0;
    m_pendingFragmentShader = 0;

    std::string vertexShaderSource;
    std::string fragmentShaderSource;
    std::swap(vertexShaderSource, m_pendingVertexSource);
    std::swap(fragmentShaderSource, m_pendingFragmentSource);

    GLint programLinked{};
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &programLinked);
    if (programLinked == GL_TRUE)
    {
        return;
    }

    GLint infoLogLength{};
    glGetProgramiv(m_shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> message(infoLogLength + 1);
    glGetProgramInfoLog(m_shaderProgram, infoLogLength, nullptr, message.data());

    std::string linkError = "[Shader] Error linking deferred shader program: " + std::string(message.data());
    LOG_ERROR(linkError);
    LOG_DEBUG("[Shader] Vertex shader source: " + vertexShaderSource);
    LOG_DEBUG("[Shader] Fragment shader source: " + fragmentShaderSource);
    throw ShaderException(linkError);
}

void Shader::EnsureFinalizedNoThrow() const noexcept
{
    if (!m_pendingLink)
    {
        return;
    }
    try
    {
        FinalizeCompile();
    }
    catch (const ShaderException& ex)
    {
        // Safety net only: real preload paths finalize explicitly (with fallback
        // handling) before first use. A shader that fails here draws nothing.
        LOG_ERROR(std::string("[Shader] Deferred compile failed at first use: ") + ex.message());
    }
}

void Shader::CompileProgram(const std::string& vertexShaderSource,
                            const std::string& fragmentShaderSource)
{
    // Recompiling a shader that still has a deferred link in flight: settle it first.
    EnsureFinalizedNoThrow();

    auto vertexShader = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
    auto fragmentShader = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);

    if (s_deferCompilation)
    {
        // Leave the link running on the driver's worker pool. Everything past
        // glLinkProgram that touches the program — INCLUDING the glDetachShader in the
        // normal path below — may synchronously resolve the link on this thread (ANGLE
        // does), so all of it moves to FinalizeCompile().
        glLinkProgram(m_shaderProgram);

        m_pendingLink = true;
        m_pendingVertexShader = vertexShader;
        m_pendingFragmentShader = fragmentShader;
        m_pendingVertexSource = vertexShaderSource;
        m_pendingFragmentSource = fragmentShaderSource;
        return;
    }

    glLinkProgram(m_shaderProgram);

    // Shader objects are no longer needed after linking, free the memory.
    glDetachShader(m_shaderProgram, vertexShader);
    glDetachShader(m_shaderProgram, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint programLinked;
    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &programLinked);
    if (programLinked == GL_TRUE)
    {
        return;
    }

    GLint infoLogLength{};
    glGetProgramiv(m_shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> message(infoLogLength + 1);
    glGetProgramInfoLog(m_shaderProgram, infoLogLength, nullptr, message.data());

    std::string linkError = "[Shader] Error linking compiled shader program: " + std::string(message.data());
    LOG_ERROR(linkError);
    LOG_DEBUG("[Shader] Vertex shader source: " + vertexShaderSource);
    LOG_DEBUG("[Shader] Fragment shader source: " + fragmentShaderSource);
    throw ShaderException(linkError);
}

bool Shader::Validate(std::string& validationMessage) const
{
    EnsureFinalizedNoThrow();

    GLint result{GL_FALSE};
    int infoLogLength;

    glValidateProgram(m_shaderProgram);

    glGetProgramiv(m_shaderProgram, GL_VALIDATE_STATUS, &result);
    glGetProgramiv(m_shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
        std::vector<char> validationErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(m_shaderProgram, infoLogLength, nullptr, validationErrorMessage.data());
        validationMessage = std::string(validationErrorMessage.data());
    }

    return result;
}

void Shader::Bind() const
{
    // Every render path binds before setting uniforms, so finalizing here covers the
    // deferred-compile safety net for all program uses.
    EnsureFinalizedNoThrow();

    if (m_shaderProgram > 0)
    {
        glUseProgram(m_shaderProgram);
    }
}

void Shader::Unbind()
{
    glUseProgram(0);
}

void Shader::SetUniformFloat(const char* uniform, float value) const
{
    auto location = glGetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    glUniform1fv(location, 1, &value);
}

void Shader::SetUniformInt(const char* uniform, int value) const
{
    auto location = glGetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    glUniform1iv(location, 1, &value);
}

void Shader::SetUniformFloat2(const char* uniform, const glm::vec2& values) const
{
    auto location = glGetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    glUniform2fv(location, 1, glm::value_ptr(values));
}

void Shader::SetUniformInt2(const char* uniform, const glm::ivec2& values) const
{
    auto location = glGetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    glUniform2iv(location, 1, glm::value_ptr(values));
}

void Shader::SetUniformFloat3(const char* uniform, const glm::vec3& values) const
{
    auto location = glGetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    glUniform3fv(location, 1, glm::value_ptr(values));
}

void Shader::SetUniformInt3(const char* uniform, const glm::ivec3& values) const
{
    auto location = glGetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    glUniform3iv(location, 1, glm::value_ptr(values));
}

void Shader::SetUniformFloat4(const char* uniform, const glm::vec4& values) const
{
    auto location = glGetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    glUniform4fv(location, 1, glm::value_ptr(values));
}

void Shader::SetUniformInt4(const char* uniform, const glm::ivec4& values) const
{
    auto location = glGetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    glUniform4iv(location, 1, glm::value_ptr(values));
}

void Shader::SetUniformMat3x4(const char* uniform, const glm::mat3x4& values) const
{
    auto location = glGetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(values));
}

void Shader::SetUniformMat4x4(const char* uniform, const glm::mat4x4& values) const
{
    auto location = glGetUniformLocation(m_shaderProgram, uniform);
    if (location < 0)
    {
        return;
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(values));
}

GLuint Shader::CompileShader(const std::string& source, GLenum type)
{
    GLint shaderCompiled{};

    auto shader = glCreateShader(type);
    const auto* shaderSourceCStr = source.c_str();
    glShaderSource(shader, 1, &shaderSourceCStr, nullptr);

    glCompileShader(shader);

    if (s_deferCompilation)
    {
        // The status query would synchronously resolve the background translate job.
        // Compile errors surface as a link failure in FinalizeCompile() instead.
        return shader;
    }

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderCompiled);
    if (shaderCompiled == GL_TRUE)
    {
        return shader;
    }

    GLint infoLogLength{};
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> message(infoLogLength + 1);
    glGetShaderInfoLog(shader, infoLogLength, nullptr, message.data());
    glDeleteShader(shader);

    std::string compileError = "[Shader] Error compiling " + std::string(type == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader: " + std::string(message.data());
    LOG_ERROR(compileError);
    LOG_DEBUG("[Shader] Failed source: " + source);
    throw ShaderException(compileError);
}

auto Shader::GetShaderLanguageVersion() -> Shader::GlslVersion
{
    const char* shaderLanguageVersion = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));

    if (shaderLanguageVersion == nullptr)
    {
        return {};
    }

    std::string shaderLanguageVersionString(shaderLanguageVersion);

    // Some OpenGL implementations add non-standard-conforming text in front, e.g. WebGL, which returns "OpenGL ES GLSL ES 3.00 ..."
    // Find the first digit and start there.
    auto firstDigit = shaderLanguageVersionString.find_first_of("0123456789");
    if (firstDigit != std::string::npos && firstDigit != 0)
    {
        shaderLanguageVersionString = shaderLanguageVersionString.substr(firstDigit);
    }

    // Cut off the vendor-specific information, if any
    auto spacePos = shaderLanguageVersionString.find(' ');
    if (spacePos != std::string::npos)
    {
        shaderLanguageVersionString.resize(spacePos);
    }

    auto dotPos = shaderLanguageVersionString.find('.');
    if (dotPos == std::string::npos)
    {
        return {};
    }

    int versionMajor = std::stoi(shaderLanguageVersionString.substr(0, dotPos));
    int versionMinor = std::stoi(shaderLanguageVersionString.substr(dotPos + 1));

    return {versionMajor, versionMinor};
}

} // namespace Renderer
} // namespace libprojectM

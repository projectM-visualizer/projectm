#include "Shader.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace libprojectM {
namespace Renderer {

Shader::Shader()
    : m_shaderProgram(glCreateProgram())
{
}

Shader::~Shader()
{
    if (m_shaderProgram)
    {
        glDeleteProgram(m_shaderProgram);
    }
}

void Shader::CompileProgram(const std::string& vertexShaderSource,
                            const std::string& fragmentShaderSource)
{
    auto vertexShader = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
    auto fragmentShader = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

    glAttachShader(m_shaderProgram, vertexShader);
    glAttachShader(m_shaderProgram, fragmentShader);

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

    throw ShaderException("Error compiling shader: " + std::string(message.data()));
}

bool Shader::Validate(std::string& validationMessage) const
{
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

    throw ShaderException("Error compiling shader: " + std::string(message.data()));
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

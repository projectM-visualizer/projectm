
#pragma once

#include "Renderer/Texture.hpp"

#include <glm/mat3x4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <map>
#include <string>

namespace libprojectM {
namespace Renderer {


class ShaderException : public std::exception
{
public:
    inline ShaderException(std::string message)
        : m_message(std::move(message))
    {
    }

    virtual ~ShaderException() = default;

    const std::string& message() const
    {
        return m_message;
    }

private:
    std::string m_message;
};



class Shader
{
public:

    struct GlslVersion {
        int major{};
        int minor{};
    };


    Shader();


    ~Shader();


    void CompileProgram(const std::string& vertexShaderSource,
                        const std::string& fragmentShaderSource);


    bool Validate(std::string& validationMessage) const;


    void Bind() const;


    static void Unbind();


    void SetUniformFloat(const char* uniform, float value) const;


    void SetUniformInt(const char* uniform, int value) const;


    void SetUniformFloat2(const char* uniform, const glm::vec2& values) const;


    void SetUniformInt2(const char* uniform, const glm::ivec2& values) const;


    void SetUniformFloat3(const char* uniform, const glm::vec3& values) const;


    void SetUniformInt3(const char* uniform, const glm::ivec3& values) const;


    void SetUniformFloat4(const char* uniform, const glm::vec4& values) const;


    void SetUniformInt4(const char* uniform, const glm::ivec4& values) const;


    void SetUniformMat3x4(const char* uniform, const glm::mat3x4& values) const;


    void SetUniformMat4x4(const char* uniform, const glm::mat4x4& values) const;


    static auto GetShaderLanguageVersion() -> GlslVersion;

private:

    auto CompileShader(const std::string& source, GLenum type) -> GLuint;

    GLuint m_shaderProgram{};
};

}
}

/**
 * @file Shader.hpp
 * @brief Implements an interface to a single shader program instance.
 */
#pragma once

#include "Texture.hpp"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x4.hpp>

#include <map>
#include <string>

/**
 * @brief Shader compilation exception.
 */
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


/**
 * @brief Base class containing a shader program, consisting of a vertex and fragment shader.
 */
class Shader
{
public:
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
     * @brief Sets a float vec3 uniform.
     * The program must be bound before calling this method!
     * @param uniform The uniform name
     * @param values The values to set.
     */
    void SetUniformFloat3(const char* uniform, const glm::vec3& values) const;

    /**
     * @brief Sets a float vec4 uniform.
     * The program must be bound before calling this method!
     * @param uniform The uniform name
     * @param values The values to set.
     */
    void SetUniformFloat4(const char* uniform, const glm::vec4& values) const;

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

private:
    /**
     * @brief Compiles a single shader.
     * @throws ShaderException Thrown if compilation of the shader failed.
     * @param source The shader source.
     * @param type The shader type, e.g. GL_VERTEX_SHADER.
     * @return The shader ID.
     */
    auto CompileShader(const std::string& source, GLenum type) -> GLuint;

    GLuint m_shaderProgram{}; //!< The program ID.
};

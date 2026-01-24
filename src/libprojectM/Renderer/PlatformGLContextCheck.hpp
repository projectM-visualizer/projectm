#pragma once

#include <cstdint>
#include <string>

namespace libprojectM {
namespace Renderer {
namespace Platform {

/**
 * @brief Identifies the GL API exposed by the current context.
 */
enum class GLApi : std::uint8_t
{
    Any = 0,
    OpenGL = 1,
    OpenGLES = 2
};

/**
 * @brief Requirements to validate against the current GL context.
 */
struct GLContextRequirements
{
    GLApi api{GLApi::Any};           //!< Required API (Any/OpenGL/OpenGLES)
    int minMajor{};                  //!< Minimum major version
    int minMinor{};                  //!< Minimum minor version
    bool requireCoreProfile{false};  //!< Desktop GL only; ignored for GLES
};

/**
 * @brief Compact information about the current GL context.
 */
struct GLContextInfo
{
    GLApi api{GLApi::Any};    //!< Detected API (WebGL is treated as GLES-like on Emscripten)
    int major{};              //!< Detected major version
    int minor{};              //!< Detected minor version
    std::string versionStr;   //!< GL_VERSION
    std::string glslStr;      //!< GL_SHADING_LANGUAGE_VERSION
    std::string vendor;       //!< GL_VENDOR
    std::string renderer;     //!< GL_RENDERER
    std::string profile;      //!< "core", "compat", "unknown", "n/a"
    std::string flags;        //!< "debug,fwd,robust", "none", "n/a"
};

/**
 * @brief Result of a GL context check.
 */
struct GLContextCheckResult
{
    bool success{false};            //!< True if requirements are met
    std::string reason;             //!< Failure reason (empty on success)
    GLContextRequirements req;      //!< Requirements used
    GLContextInfo info;             //!< Detected context info (best-effort)
};

/**
 * @brief GL context requirement verification check using a builder pattern.
 */
class GLContextCheck final
{
public:
    /**
     * @brief Builder for GLContextRequirements.
     */
    class Builder final
    {
    public:
        /**
         * @brief Constructs a builder with default (permissive) requirements.
         */
        Builder();

        /**
         * @brief Sets the required API (Any/OpenGL/OpenGLES).
         */
        auto WithApi(GLApi api) -> Builder&;

        /**
         * @brief Sets the minimum required version.
         */
        auto WithMinimumVersion(int major, int minor) -> Builder&;

        /**
         * @brief Requires a desktop OpenGL core profile. Ignored for GLES.
         */
        auto WithRequireCoreProfile(bool required) -> Builder&;

        /**
         * @brief Performs the check and returns a filled result struct.
         *
         * @note Precondition: a GL/GLES context must be current on the calling thread and the
         * required entry points (at minimum glGetString/glGetError) must be callable.
         */
        [[nodiscard]] auto Check() const -> GLContextCheckResult;

    private:
        GLContextRequirements m_req; //!< Requirements being built
    };

    /**
     * @brief Formats GL context info as a compact summary string.
     */
    [[nodiscard]] static auto FormatCompactLine(const GLContextInfo& info) -> std::string;

};

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

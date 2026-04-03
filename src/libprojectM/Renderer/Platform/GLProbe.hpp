#pragma once

#include <cstdint>
#include <string>
#include <vector>

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
struct GLRequirements {
    GLApi api{GLApi::Any};                       //!< Required API (Any/OpenGL/OpenGLES)
    int minMajor{};                              //!< Minimum major version
    int minMinor{};                              //!< Minimum minor version
    bool requireCoreProfile{false};              //!< Desktop GL only; ignored for GLES
    int minShaderMajor{};                        //!< Minimum shading language major version (GLSL/ESSL)
    int minShaderMinor{};                        //!< Minimum shading language minor version (GLSL/ESSL)
    std::vector<std::string> requiredExtensions; //!< GL extensions that must be present
};

/**
 * @brief Compact information about the current GL context.
 */
struct GLInfo {
    GLApi api{GLApi::Any};               //!< Detected API (WebGL is treated as GLES-like on Emscripten)
    int major{};                         //!< Detected major version
    int minor{};                         //!< Detected minor version
    std::string versionStr;              //!< GL_VERSION
    std::string glslStr;                 //!< GL_SHADING_LANGUAGE_VERSION
    std::string vendor;                  //!< GL_VENDOR
    std::string renderer;                //!< GL_RENDERER
    std::string profile;                 //!< "core", "compat", "unknown", "n/a"
    std::string flags;                   //!< "debug,fwd,robust", "none", "n/a"
    std::vector<std::string> extensions; //!< List of supported GL extensions
};

/**
 * @brief Result of a GL context check.
 */
struct GLProbeResult {
    bool success{false}; //!< True if requirements are met
    std::string reason;  //!< Failure reason (empty on success)
    GLRequirements req;  //!< Requirements used
    GLInfo info;         //!< Detected context info
};

/**
 * @brief GL requirements verification check using a builder pattern.
 * @note GL function pointers can be supplied by the user, GLResolver will be used if not present.
 *       Does not dependent on GLAD resolved GL functions in order to allow checks before GLAD is loaded.
 */
class GLProbe final
{
public:
    /**
     * @brief GL function pointer parameter type.
     */
    struct GLFunctions {
        /**
         * Opaque procedure address.
         */
        using ProcAddress = void*;

        ProcAddress getString{};   //!< glGetString
        ProcAddress getError{};    //!< glGetError
        ProcAddress getIntegerv{}; //!< glGetIntegerv
        ProcAddress getStringi{};  //!< glGetStringi (GL 3.0+ / GLES 3.0+; optional)
    };

    /**
     * @brief Builder for retrieving GL infos.
     */
    class InfoBuilder final
    {
    public:
        InfoBuilder() = default;

        /**
         * @brief Sets the GL function pointers to use for the checks.
         *        Optional, uses the resolver to discover functions if not provided.
         */
        auto WithGLFunctions(const GLFunctions& glFunctions) -> InfoBuilder&;

        /**
         * @brief Retrieve GL infos.
         * @param info Info struct to fill.
         * @param reason Error information.
         *
         * @return true if gl information was retrieved.
         */
        auto Build(GLInfo& info, std::string& reason) -> bool;

    private:
        GLFunctions m_gl; //!< GL functions used for checks.
    };

    /**
     * @brief Builder for composing GLContextRequirements.
     */
    class CheckBuilder final
    {
    public:
        /**
         * @brief Constructs a builder with default (permissive) requirements.
         */
        CheckBuilder();

        /**
         * @brief Sets the GL function pointers to use for the checks.
         * Optional, uses the resolver to discover functions if not provided.
         */
        auto WithGLFunctions(const GLFunctions& glFunctions) -> CheckBuilder&;

        /**
         * @brief Sets the required API (Any/OpenGL/OpenGLES).
         */
        auto WithApi(GLApi api) -> CheckBuilder&;

        /**
         * @brief Sets the minimum required version.
         */
        auto WithMinimumVersion(int major, int minor) -> CheckBuilder&;

        /**
         * @brief Requires a minimum GL shading language version (GLSL for OpenGL, ESSL for GLES).
         *
         * @note This checks GL_SHADING_LANGUAGE_VERSION and parses the first major.minor pair.
         *       If the driver does not report a shading language version, the check fails.
         */
        auto WithMinimumShaderLanguageVersion(int major, int minor) -> CheckBuilder&;

        /**
         * @brief Requires a desktop OpenGL core profile. Ignored for GLES.
         */
        auto WithRequireCoreProfile(bool required) -> CheckBuilder&;

        /**
         * @brief Adds a single required GL extension to the check.
         *
         * The extension name should be the full GL extension string (e.g. "GL_ARB_compute_shader").
         * Multiple calls accumulate required extensions.
         *
         * @param extensionName Full GL extension name.
         */
        auto WithRequiredExtension(const std::string& extensionName) -> CheckBuilder&;

        /**
         * @brief Adds multiple required GL extensions to the check.
         *
         * @param extensionNames List of full GL extension names.
         */
        auto WithRequiredExtensions(const std::vector<std::string>& extensionNames) -> CheckBuilder&;

        /**
         * @brief Performs the check and returns a filled result struct.
         *
         * @note Precondition: a GL/GLES context must be current on the calling thread and the
         * required entry points (at minimum glGetString/glGetError) must be callable.
         */
        [[nodiscard]] auto Check() const -> GLProbeResult;

    private:
        GLFunctions m_gl;     //!< GL functions used for checks.
        GLRequirements m_req; //!< Requirements being built.
    };

    /**
     * @brief Formats GL context info as a compact summary string.
     */
    [[nodiscard]] static auto FormatCompactLine(const GLInfo& info) -> std::string;
};

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

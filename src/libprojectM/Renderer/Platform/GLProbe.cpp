#include "GLProbe.hpp"

#include "../OpenGL.h"
#include "DynamicLibrary.hpp"
#include "GLResolver.hpp"

#include <cstdio>
#include <cstring>
#include <sstream>
#include <vector>

namespace libprojectM {
namespace Renderer {
namespace Platform {

namespace {

/**
 * @brief OpenGL integer query tokens used with glGetIntegerv for context/version inspection.
 *
 * These are numeric values for `GL_MAJOR_VERSION`, `GL_MINOR_VERSION`, `GL_CONTEXT_FLAGS`,
 * and `GL_CONTEXT_PROFILE_MASK`. They are provided here as constants to avoid depending on
 * a particular GL header version exposing the named macros on all platforms.
 */
enum : std::uint16_t
{
    PM_GL_MAJOR_VERSION = 0x821B,
    PM_GL_MINOR_VERSION = 0x821C,
    PM_GL_CONTEXT_FLAGS = 0x821E,
    PM_GL_NUM_EXTENSIONS = 0x821D
#ifndef USE_GLES
    ,
    PM_GL_CONTEXT_PROFILE_MASK = 0x9126
#endif
};

/**
 * @brief Bits reported by `PM_GL_CONTEXT_PROFILE_MASK` describing the context profile.
 *
 * Matches the `GL_CONTEXT_CORE_PROFILE_BIT` and `GL_CONTEXT_COMPATIBILITY_PROFILE_BIT` values.
 */
enum : std::uint32_t
{
    PM_GL_CONTEXT_CORE_PROFILE_BIT = 0x00000001u,
    PM_GL_CONTEXT_COMPATIBILITY_PROFILE_BIT = 0x00000002u
};

/**
 * @brief Bits reported by `PM_GL_CONTEXT_FLAGS` describing context behavior.
 *
 * Matches the `GL_CONTEXT_FLAG_*_BIT` values.
 */
enum : std::uint32_t
{
    PM_GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT = 0x00000001u,
    PM_GL_CONTEXT_FLAG_DEBUG_BIT = 0x00000002u,
    PM_GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT = 0x00000004u
};

/**
 * @brief Typed OpenGL entrypoints needed by GL version/profile/flags probing.
 *
 * This struct converts opaque proc addresses (e.g. from GLAD or a resolver) into typed function
 * pointers. Only a minimal subset is required:
 * - glGetString and glGetError are mandatory for probing.
 * - glGetIntegerv is optional; when unavailable, version probing can fall back to parsing
 *   `GL_VERSION`.
 */
struct ResolvedGLFunctions {
    using GetStringFn = decltype(+glGetString);
    using GetErrorFn = decltype(+glGetError);
    using GetIntegervFn = decltype(+glGetIntegerv);
    using GetStringiFn = const GLubyte* (*) (GLenum, GLuint);

    GetStringFn getString{};     //!< Typed pointer to glGetString (required).
    GetErrorFn getError{};       //!< Typed pointer to glGetError (required).
    GetIntegervFn getIntegerv{}; //!< Typed pointer to glGetIntegerv (optional).
    GetStringiFn getStringi{};   //!< Typed pointer to glGetStringi (optional, GL 3.0+).
};

/**
 * @brief Resolves the required OpenGL entrypoints into typed function pointers.
 *
 * Uses the caller-provided handles first. If any required handles are missing and the
 * GL resolver is loaded, missing entrypoints are queried via the resolver. Opaque
 * addresses are converted into typed function pointers using SymbolToFunction.
 *
 * @param handles Opaque/typeless entrypoints (may contain null pointers).
 * @param[out] out Destination for typed function pointers.
 * @param[out] reason Failure reason (set on failure; cleared on success).
 * @return True if the required entrypoints were resolved; false otherwise.
 */
auto ResolveGLFunctions(const GLProbe::GLFunctions& handles,
                        ResolvedGLFunctions& out,
                        std::string& reason) -> bool
{
    // Use explicit void* (GLFunctions::ProcAddress) to avoid auto* deduction
    // ambiguity across compilers when the source type is already void*.
    void* getString = handles.getString;
    void* getError = handles.getError;
    void* getIntegerv = handles.getIntegerv;
    void* getStringi = handles.getStringi;

    if ((getString == nullptr || getError == nullptr) && !GLResolver::Instance().IsInitialized())
    {
        reason = "GL entrypoints not configured and GLResolver is not loaded";
        return false;
    }

    if (getString == nullptr)
    {
        getString = GLResolver::Instance().GetProcAddress("glGetString");
    }

    if (getError == nullptr)
    {
        getError = GLResolver::Instance().GetProcAddress("glGetError");
    }

    if (getIntegerv == nullptr)
    {
        getIntegerv = GLResolver::Instance().GetProcAddress("glGetIntegerv");
    }

    if (getStringi == nullptr)
    {
        getStringi = GLResolver::Instance().GetProcAddress("glGetStringi");
    }

    // Convert opaque procedure addresses into typed function pointers.
    out.getString = SymbolToFunction<ResolvedGLFunctions::GetStringFn>(getString);
    out.getError = SymbolToFunction<ResolvedGLFunctions::GetErrorFn>(getError);
    out.getIntegerv = SymbolToFunction<ResolvedGLFunctions::GetIntegervFn>(getIntegerv);
    out.getStringi = SymbolToFunction<ResolvedGLFunctions::GetStringiFn>(getStringi);

    if (out.getString == nullptr || out.getError == nullptr)
    {
        reason = "GL entrypoints not available";
        return false;
    }

    // glGetIntegerv is optional for the check (we can parse GL_VERSION as fallback).
    reason.clear();
    return true;
}

/**
 * @brief Clears (drains) pending GL errors to establish a clean error baseline.
 *
 * Repeatedly calls glGetError up to a bounded number of iterations to avoid an
 * infinite loop in the presence of a misbehaving driver or entrypoint.
 *
 * @param gl Resolved GL entrypoints (gl.getError must be non-null to have an effect).
 */
auto ClearGlErrors(const ResolvedGLFunctions& gl) -> void
{
    if (gl.getError == nullptr)
    {
        return;
    }

    for (int i = 0; i < 32; ++i)
    {
        const auto err = gl.getError();
        if (err == GL_NO_ERROR)
        {
            break;
        }
    }
}

/**
 * @brief Converts a GL `GLubyte*` string pointer into a safe C string.
 *
 * @param str GL-provided string pointer (may be null).
 * @return A non-null C string pointer. Returns an empty string when @p str is null.
 */
auto SafeStr(const unsigned char* str) -> const char*
{
    if (str != nullptr)
    {
        return reinterpret_cast<const char*>(str);
    }
    return "";
}

/**
 * @brief Returns true if @p str begins with @p prefix.
 *
 * Null-safe: returns false if either argument is null.
 *
 * @param str Input string (may be null).
 * @param prefix Prefix to match (may be null).
 * @return True if @p str starts with @p prefix; false otherwise.
 */
auto StartsWith(const char* str, const char* prefix) -> bool
{
    if (str == nullptr || prefix == nullptr)
    {
        return false;
    }
    return std::strncmp(str, prefix, std::strlen(prefix)) == 0;
}

/**
 * @brief Sanitizes a string for logging by replacing control whitespace with spaces.
 *
 * Replaces `\\n`, `\\r`, and `\\t` with a single space to keep log output single-line
 * and avoid log injection/formatting issues.
 *
 * @param input Input string.
 * @return Sanitized copy of @p input.
 */
auto SanitizeString(const std::string& input) -> std::string
{
    std::string out = input;
    for (auto& c : out)
    {
        if (c == '\n' || c == '\r' || c == '\t')
        {
            c = ' ';
        }
    }
    return out;
}

/**
 * @brief Formats a major/minor version pair as "major.minor".
 *
 * @param major Major version.
 * @param minor Minor version.
 * @return Version string in dotted form.
 */
auto FormatVersion(int major, int minor) -> std::string
{
    std::ostringstream oss;
    oss << major << "." << minor;
    return oss.str();
}

/**
 * @brief Returns a short human-readable API name for a GL API enum.
 *
 * @param api API enum value.
 * @return "GLES" for OpenGLES, "GL" for OpenGL, and "Any" for unknown/unspecified.
 */
auto ApiString(GLApi api) -> const char*
{
    switch (api)
    {
        case GLApi::OpenGLES: {
            return "GLES";
        }
        case GLApi::OpenGL: {
            return "GL";
        }
        default: {
            return "Any";
        }
    }
}

/**
 * @brief Validates that the minimal GL entrypoints required for probing are present.
 *
 * @param gl Resolved GL entrypoints.
 * @param[out] reason Failure reason when returning false.
 * @return True if glGetString and glGetError are available; false otherwise.
 */
auto HasBasicGLEntrypoints(const ResolvedGLFunctions& gl, std::string& reason) -> bool
{
    if (gl.getString == nullptr || gl.getError == nullptr)
    {
        reason = "GL entrypoints not loaded";
        return false;
    }
    return true;
}

/**
 * @brief Queries major/minor GL version via glGetIntegerv.
 *
 * If glGetIntegerv is unavailable, returns false and sets @p major/@p minor to 0.
 * This function clears prior GL errors before querying and requires glGetError to
 * report no error after the queries.
 *
 * @param gl Resolved GL entrypoints.
 * @param[out] major Major version (0 on failure).
 * @param[out] minor Minor version (0 on failure).
 * @return True if major/minor were queried successfully and major > 0; false otherwise.
 */
auto QueryMajorMinor(const ResolvedGLFunctions& gl, int& major, int& minor) -> bool
{
    if (gl.getIntegerv == nullptr)
    {
        major = 0;
        minor = 0;
        return false;
    }

    major = 0;
    minor = 0;

    ClearGlErrors(gl);

    gl.getIntegerv(PM_GL_MAJOR_VERSION, &major);

    if (gl.getError() != GL_NO_ERROR)
    {
        major = 0;
        return false;
    }

    gl.getIntegerv(PM_GL_MINOR_VERSION, &minor);

    if (gl.getError() != GL_NO_ERROR)
    {
        minor = 0;
        return false;
    }

    return major > 0;
}

/**
 * @brief Parses a GL version string into major/minor integers.
 *
 * For GLES contexts, the function optionally scans for the substring "OpenGL ES" and
 * parses the first "X.Y" pattern after it. For non-GLES contexts, it parses the first
 * numeric "X.Y" sequence found in the string.
 *
 * @param str Version string (typically from glGetString(GL_VERSION)).
 * @param isGLES True if the string is expected to describe an OpenGL ES/WebGL context.
 * @param[out] major Parsed major version.
 * @param[out] minor Parsed minor version.
 * @return True if parsing succeeded and major > 0; false otherwise.
 */
auto ParseVersionString(const char* str, bool isGLES, int& major, int& minor) -> bool
{
    if (str == nullptr || *str == 0)
    {
        return false;
    }

    const char* p = str;

    if (isGLES)
    {
        const char* found = std::strstr(str, "OpenGL ES");
        if (found != nullptr)
        {
            p = found + std::strlen("OpenGL ES");
        }
    }

    while ((*p != 0) && (*p < '0' || *p > '9'))
    {
        ++p;
    }

    return (std::sscanf(p, "%d.%d", &major, &minor) == 2) && (major > 0);
}

/**
 * @brief Compares a version (major/minor) against a required minimum version.
 *
 * @param major Detected major version.
 * @param minor Detected minor version.
 * @param reqMajor Required major version.
 * @param reqMinor Required minor version.
 * @return True if (major, minor) is at least (reqMajor, reqMinor); false otherwise.
 */
auto VersionAtLeast(int major, int minor, int reqMajor, int reqMinor) -> bool
{
    if (major != reqMajor)
    {
        return major > reqMajor;
    }
    return minor >= reqMinor;
}

/**
 * @brief Returns a human-readable context profile string ("core", "compat", "unknown", "n/a").
 *
 * Uses glGetIntegerv(PM_GL_CONTEXT_PROFILE_MASK) when available. If glGetIntegerv is
 * missing or the query produces a GL error, returns "n/a".
 *
 * @param gl Resolved GL entrypoints.
 * @return Profile string for logging/diagnostics.
 */
auto ProfileString(const ResolvedGLFunctions& gl) -> std::string
{
#ifdef USE_GLES
    (void) gl;
    return "n/a";
#else

    if (gl.getIntegerv == nullptr)
    {
        return "n/a";
    }

    int mask = 0;

    ClearGlErrors(gl);

    gl.getIntegerv(PM_GL_CONTEXT_PROFILE_MASK, &mask);

    if (gl.getError() != GL_NO_ERROR)
    {
        return "n/a";
    }

    if ((mask & PM_GL_CONTEXT_CORE_PROFILE_BIT) != 0)
    {
        return "core";
    }

    if ((mask & PM_GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) != 0)
    {
        return "compat";
    }

    return "unknown";

#endif // #ifdef USE_GLES
}

/**
 * @brief Returns a human-readable comma-separated GL context flags string.
 *
 * Uses glGetIntegerv(PM_GL_CONTEXT_FLAGS) when available. If glGetIntegerv is missing
 * or the query produces a GL error, returns "n/a". If no known flags are set, returns "none".
 *
 * @param gl Resolved GL entrypoints.
 * @return Flags string for logging/diagnostics (e.g. "debug,fwd", "none", "n/a").
 */
auto FlagsString(const ResolvedGLFunctions& gl) -> std::string
{
    if (gl.getIntegerv == nullptr)
    {
        return "n/a";
    }

    int flags = 0;

    ClearGlErrors(gl);

    gl.getIntegerv(PM_GL_CONTEXT_FLAGS, &flags);

    if (gl.getError() != GL_NO_ERROR)
    {
        return "n/a";
    }

    std::vector<std::string> bits;

    if ((flags & PM_GL_CONTEXT_FLAG_DEBUG_BIT) != 0)
    {
        bits.push_back("debug");
    }

    if ((flags & PM_GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT) != 0)
    {
        bits.push_back("fwd");
    }

    if ((flags & PM_GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT) != 0)
    {
        bits.push_back("robust");
    }

    if (bits.empty())
    {
        return "none";
    }

    std::ostringstream oss;
    for (size_t i = 0; i < bits.size(); ++i)
    {
        if (i != 0)
        {
            oss << ",";
        }
        oss << bits[i];
    }
    return oss.str();
}

/**
 * @brief Queries the list of supported GL extensions from the current context.
 *
 * On GL 3.0+ / GLES 3.0+ contexts, uses glGetStringi(GL_EXTENSIONS, i) in a loop
 * (the indexed query). Falls back to parsing the space-delimited string returned by
 * glGetString(GL_EXTENSIONS) on older contexts or when glGetStringi is unavailable.
 *
 * @param gl Resolved GL entrypoints.
 * @param[out] extensions Destination vector; cleared and repopulated on success.
 */
auto QueryExtensions(const ResolvedGLFunctions& gl, std::vector<std::string>& extensions) -> void
{
    extensions.clear();

    // Prefer indexed query (GL 3.0+ / GLES 3.0+).
    if (gl.getStringi != nullptr && gl.getIntegerv != nullptr)
    {
        int numExtensions = 0;

        ClearGlErrors(gl);

        gl.getIntegerv(PM_GL_NUM_EXTENSIONS, &numExtensions);

        if (gl.getError() == GL_NO_ERROR && numExtensions > 0)
        {
            extensions.reserve(static_cast<std::size_t>(numExtensions));
            for (int i = 0; i < numExtensions; ++i)
            {
                const char* ext = SafeStr(gl.getStringi(GL_EXTENSIONS, static_cast<GLuint>(i)));
                if (*ext != '\0')
                {
                    extensions.emplace_back(ext);
                }
            }
            return;
        }
    }

    // Fallback: parse the monolithic GL_EXTENSIONS string.
    if (gl.getString != nullptr)
    {
        ClearGlErrors(gl);

        const char* allExt = SafeStr(gl.getString(GL_EXTENSIONS));
        if (*allExt != '\0')
        {
            std::istringstream stream(allExt);
            std::string token;
            while (stream >> token)
            {
                if (!token.empty())
                {
                    extensions.push_back(std::move(token));
                }
            }
        }
    }
}

/**
 * @brief Populates a GLInfo struct by querying the current context.
 *
 * Requires glGetString and glGetError to be available, and a current GL context to be
 * bound on the calling thread. The function reads:
 * - API kind (OpenGL vs OpenGL ES/WebGL)
 * - Vendor/renderer strings
 * - GLSL version string (if available)
 * - Major/minor version (via glGetIntegerv when available, otherwise by parsing GL_VERSION)
 * - Profile and flags (best-effort; "n/a" when unavailable)
 *
 * On Emscripten, WebGL is treated as a GLES-like surface. Additionally, if the reported
 * version string is in "WebGL 2.0" form and parsing yields 2.0, the version is lifted
 * to 3.0 for minimum-version requirement checks.
 *
 * @param gl Resolved GL entrypoints.
 * @param[out] info Destination for queried GL properties.
 * @param[out] reason Failure reason (set on failure; cleared on success).
 * @return True on success; false if no current context is available or version probing fails.
 */
auto QueryInfo(const ResolvedGLFunctions& gl, GLInfo& info, std::string& reason) -> bool
{
    if (!HasBasicGLEntrypoints(gl, reason))
    {
        return false;
    }

    ClearGlErrors(gl);

    const char* ver = SafeStr(gl.getString(GL_VERSION));
    if (*ver == 0)
    {
        reason = "No current GL context (glGetString(GL_VERSION) returned null/empty)";
        return false;
    }

    const bool isGLES =
#if defined(__EMSCRIPTEN__)
        // WebGL is a GLES-like API surface
        true;
#else
        StartsWith(ver, "OpenGL ES") ||
        std::strstr(ver, "OpenGL ES") != nullptr ||
        std::strstr(ver, "WebGL") != nullptr;
#endif

    info.api = isGLES ? GLApi::OpenGLES : GLApi::OpenGL;
    info.versionStr = ver;

    info.vendor = SanitizeString(SafeStr(gl.getString(GL_VENDOR)));
    info.renderer = SanitizeString(SafeStr(gl.getString(GL_RENDERER)));

    const char* glsl = SafeStr(gl.getString(GL_SHADING_LANGUAGE_VERSION));
    if (*glsl != 0)
    {
        info.glslStr = SanitizeString(glsl);
    }
    else
    {
        info.glslStr.clear();
    }

    if (!QueryMajorMinor(gl, info.major, info.minor))
    {
        if (!ParseVersionString(ver, isGLES, info.major, info.minor))
        {
            reason = std::string("Unable to determine GL version from GL_VERSION=\"") + SanitizeString(ver) + "\"";
            return false;
        }
    }

#if defined(__EMSCRIPTEN__)
    // Emscripten can be configured to return WebGL-format version strings (e.g. "WebGL 2.0")
    // instead of ES-format strings. For the purposes of *minimum version checking* (not feature
    // completeness), WebGL 2 most closely maps to a GLES 3.0-class API surface.
    // If version parsing yields 2.0 from a WebGL 2.0 string, lift it to 3.0 for requirement checks.
    if (std::strstr(ver, "WebGL 2") != nullptr)
    {
        if (info.major < 3)
        {
            info.major = 3;
            info.minor = 0;
        }
    }
#endif

    info.profile = ProfileString(gl);
    info.flags = FlagsString(gl);

    QueryExtensions(gl, info.extensions);

    reason.clear();
    return true;
}

} /* anonymous namespace */

auto GLProbe::InfoBuilder::WithGLFunctions(const GLFunctions& glFunctions) -> InfoBuilder&
{
    m_gl = glFunctions;
    return *this;
}

auto GLProbe::InfoBuilder::Build(GLInfo& info, std::string& reason) -> bool
{
    std::string ret;

    ResolvedGLFunctions gl;
    if (!ResolveGLFunctions(m_gl, gl, ret))
    {
        reason = ret;
        return false;
    }

    if (!QueryInfo(gl, info, ret))
    {
        reason = ret;
        return false;
    }

    reason.clear();
    return true;
}

GLProbe::CheckBuilder::CheckBuilder()
{
    m_req.api = GLApi::Any;
    m_req.minMajor = 0;
    m_req.minMinor = 0;
    m_req.requireCoreProfile = false;
    m_req.minShaderMajor = 0;
    m_req.minShaderMinor = 0;
}

auto GLProbe::CheckBuilder::WithGLFunctions(const GLFunctions& glFunctions) -> CheckBuilder&
{
    m_gl = glFunctions;
    return *this;
}

auto GLProbe::CheckBuilder::WithApi(GLApi api) -> CheckBuilder&
{
    m_req.api = api;
    return *this;
}

auto GLProbe::CheckBuilder::WithMinimumVersion(int major, int minor) -> CheckBuilder&
{
    m_req.minMajor = major;
    m_req.minMinor = minor;
    return *this;
}

auto GLProbe::CheckBuilder::WithMinimumShaderLanguageVersion(int major, int minor) -> CheckBuilder&
{
    m_req.minShaderMajor = major;
    m_req.minShaderMinor = minor;
    return *this;
}

auto GLProbe::CheckBuilder::WithRequireCoreProfile(bool required) -> CheckBuilder&
{
    m_req.requireCoreProfile = required;
    return *this;
}

auto GLProbe::CheckBuilder::WithRequiredExtension(const std::string& extensionName) -> CheckBuilder&
{
    if (!extensionName.empty())
    {
        m_req.requiredExtensions.push_back(extensionName);
    }
    return *this;
}

auto GLProbe::CheckBuilder::WithRequiredExtensions(const std::vector<std::string>& extensionNames) -> CheckBuilder&
{
    for (const auto& name : extensionNames)
    {
        if (!name.empty())
        {
            m_req.requiredExtensions.push_back(name);
        }
    }
    return *this;
}

auto GLProbe::CheckBuilder::Check() const -> GLProbeResult
{
    GLProbeResult result;
    result.req = m_req;

    std::string reason;

    ResolvedGLFunctions gl;
    if (!ResolveGLFunctions(m_gl, gl, reason))
    {
        result.success = false;
        result.reason = reason;
        return result;
    }

    if (!QueryInfo(gl, result.info, reason))
    {
        result.success = false;
        result.reason = reason;
        return result;
    }

    if (m_req.api != GLApi::Any && result.info.api != m_req.api)
    {
        result.success = false;
        result.reason = std::string("Wrong API: ") + ApiString(result.info.api);
        return result;
    }

    if (!VersionAtLeast(result.info.major, result.info.minor, m_req.minMajor, m_req.minMinor))
    {
        result.success = false;
        result.reason = std::string("Version too low: ") + FormatVersion(result.info.major, result.info.minor);
        return result;
    }

    if (m_req.minShaderMajor > 0 || m_req.minShaderMinor > 0)
    {
        if (result.info.glslStr.empty())
        {
            result.success = false;
            result.reason = "No shading language version reported";
            return result;
        }

        int glslMajor = 0;
        int glslMinor = 0;
        const bool isGLES = (result.info.api == GLApi::OpenGLES);
        if (!ParseVersionString(result.info.glslStr.c_str(), isGLES, glslMajor, glslMinor))
        {
            result.success = false;
            result.reason = std::string("Unable to parse shading language version: ") + result.info.glslStr;
            return result;
        }

        if (!VersionAtLeast(glslMajor, glslMinor, m_req.minShaderMajor, m_req.minShaderMinor))
        {
            result.success = false;
            result.reason = std::string("Shading language version too low: ") + FormatVersion(glslMajor, glslMinor);
            return result;
        }
    }

    if (m_req.requireCoreProfile &&
        result.info.api == GLApi::OpenGL &&
        result.info.profile != "core")
    {
        result.success = false;
        result.reason = "Core profile required";
        return result;
    }

    if (!m_req.requiredExtensions.empty())
    {
        for (const auto& reqExt : m_req.requiredExtensions)
        {
            bool found = false;
            for (const auto& ext : result.info.extensions)
            {
                if (ext == reqExt)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                result.success = false;
                result.reason = std::string("Required GL extension not available: ") + reqExt;
                return result;
            }
        }
    }

    result.success = true;
    return result;
}

auto GLProbe::FormatCompactLine(const GLInfo& info) -> std::string
{
    std::ostringstream oss;

    oss << "api=\"" << (info.api == GLApi::Any ? "None" : ApiString(info.api)) << "\""
        << " ver=\"" << FormatVersion(info.major, info.minor) << "\""
        << " profile=\"" << info.profile << "\""
        << " flags=\"" << info.flags << "\"";

    if (!info.glslStr.empty())
    {
        oss << " glsl=\"" << info.glslStr << "\"";
    }

    if (!info.vendor.empty())
    {
        oss << " vendor=\"" << info.vendor << "\"";
    }

    if (!info.renderer.empty())
    {
        oss << " renderer=\"" << info.renderer << "\"";
    }

    return oss.str();
}

} /* namespace Platform */
} /* namespace Renderer */
} /* namespace libprojectM */

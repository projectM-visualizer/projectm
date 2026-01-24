#include "PlatformGLContextCheck.hpp"

#include "OpenGL.h"
#include <Logging.hpp>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <vector>

namespace libprojectM {
namespace Renderer {
namespace Platform {

namespace {

enum : std::uint16_t
{
    PM_GL_MAJOR_VERSION        = 0x821B,
    PM_GL_MINOR_VERSION        = 0x821C,
    PM_GL_CONTEXT_FLAGS        = 0x821E,
    PM_GL_CONTEXT_PROFILE_MASK = 0x9126
};

enum : std::uint32_t
{
    PM_GL_CONTEXT_CORE_PROFILE_BIT          = 0x00000001u,
    PM_GL_CONTEXT_COMPATIBILITY_PROFILE_BIT = 0x00000002u
};

enum : std::uint32_t
{
    PM_GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT = 0x00000001u,
    PM_GL_CONTEXT_FLAG_DEBUG_BIT              = 0x00000002u,
    PM_GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT      = 0x00000004u
};

auto SafeStr(const unsigned char* str) -> const char*
{
    if (str != nullptr)
    {
        return reinterpret_cast<const char*>(str);
    }
    return "";
}

auto StartsWith(const char* str, const char* prefix) -> bool
{
    if (str == nullptr || prefix == nullptr)
    {
        return false;
    }
    return std::strncmp(str, prefix, std::strlen(prefix)) == 0;
}

auto SanitizeString(const std::string& input) -> std::string
{
    std::string out = input;
    for (size_t i = 0; i < out.size(); ++i)
    {
        char& c = out[i];
        if (c == '\n' || c == '\r' || c == '\t')
        {
            c = ' ';
        }
    }
    return out;
}

auto FormatVersion(int major, int minor) -> std::string
{
    std::ostringstream oss;
    oss << major << "." << minor;
    return oss.str();
}

auto ApiString(GLApi api) -> const char*
{
    switch (api)
    {
        case GLApi::OpenGLES:
        {
            return "GLES";
        }
        case GLApi::OpenGL:
        {
            return "GL";
        }
        default:
        {
            return "Any";
        }
    }
}

auto HasBasicGLEntrypoints(std::string& reason) -> bool
{
    if (glGetString == nullptr || glGetError == nullptr)
    {
        reason = "GL entrypoints not loaded (call gladLoadGL/GLES with a current context first)";
        return false;
    }
    return true;
}

static void ClearGlErrors()
{
    if (glGetError == nullptr)
    {
        return;
    }

    // OpenGL error state is sticky until drained. Avoid false negatives in probes.
    // Cap the loop to prevent getting stuck on misbehaving drivers.
    for (int i = 0; i < 32; ++i)
    {
        const GLenum err = glGetError();
        if (err == GL_NO_ERROR)
        {
            break;
        }
    }
}

auto QueryMajorMinor(int& major, int& minor) -> bool
{
    if (glGetIntegerv == nullptr)
    {
        major = 0;
        minor = 0;
        return false;
    }

    major = 0;
    minor = 0;

    ClearGlErrors();

    glGetIntegerv(PM_GL_MAJOR_VERSION, &major);
    glGetIntegerv(PM_GL_MINOR_VERSION, &minor);

    if (glGetError() != GL_NO_ERROR)
    {
        return false;
    }

    return major > 0;
}

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

auto VersionAtLeast(int major, int minor, int reqMajor, int reqMinor) -> bool
{
    if (major != reqMajor)
    {
        return major > reqMajor;
    }
    return minor >= reqMinor;
}

auto ProfileString() -> std::string
{
    if (glGetIntegerv == nullptr)
    {
        return "n/a";
    }

    int mask = 0;

    ClearGlErrors();

    glGetIntegerv(PM_GL_CONTEXT_PROFILE_MASK, &mask);

    if (glGetError() != GL_NO_ERROR)
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
}

auto FlagsString() -> std::string
{
    if (glGetIntegerv == nullptr)
    {
        return "n/a";
    }

    int flags = 0;

    ClearGlErrors();

    glGetIntegerv(PM_GL_CONTEXT_FLAGS, &flags);

    if (glGetError() != GL_NO_ERROR)
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

auto QueryInfo(GLContextInfo& info, std::string& reason) -> bool
{
    if (!HasBasicGLEntrypoints(reason))
    {
        return false;
    }

    ClearGlErrors();

    const char* ver = SafeStr(glGetString(GL_VERSION));
    if (*ver == 0)
    {
        reason = "No current GL context";
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

    info.vendor = SanitizeString(SafeStr(glGetString(GL_VENDOR)));
    info.renderer = SanitizeString(SafeStr(glGetString(GL_RENDERER)));

    const char* glsl = SafeStr(glGetString(GL_SHADING_LANGUAGE_VERSION));
    if (*glsl != 0)
    {
        info.glslStr = SanitizeString(glsl);
    }
    else
    {
        info.glslStr.clear();
    }

    if (!QueryMajorMinor(info.major, info.minor))
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

    info.profile = ProfileString();
    info.flags = FlagsString();

    reason.clear();
    return true;
}

} /* anonymous namespace */

GLContextCheck::Builder::Builder()
    : m_req()
{
    m_req.api = GLApi::Any;
    m_req.minMajor = 0;
    m_req.minMinor = 0;
    m_req.requireCoreProfile = false;
}

auto GLContextCheck::Builder::WithApi(GLApi api) -> Builder&
{
    m_req.api = api;
    return *this;
}

auto GLContextCheck::Builder::WithMinimumVersion(int major, int minor) -> Builder&
{
    m_req.minMajor = major;
    m_req.minMinor = minor;
    return *this;
}

auto GLContextCheck::Builder::WithRequireCoreProfile(bool required) -> Builder&
{
    m_req.requireCoreProfile = required;
    return *this;
}

auto GLContextCheck::Builder::Check() const -> GLContextCheckResult
{
    GLContextCheckResult result;
    result.success = false;
    result.reason.clear();
    result.req = m_req;

    result.info.api = GLApi::Any;
    result.info.major = 0;
    result.info.minor = 0;
    result.info.versionStr.clear();
    result.info.glslStr.clear();
    result.info.vendor.clear();
    result.info.renderer.clear();
    result.info.profile.clear();
    result.info.flags.clear();

    std::string reason;
    if (!QueryInfo(result.info, reason))
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
        result.reason = "Version too low: " + FormatVersion(result.info.major, result.info.minor);
        return result;
    }

    if (m_req.requireCoreProfile &&
        result.info.api == GLApi::OpenGL &&
        result.info.profile != "core")
    {
        result.success = false;
        result.reason = "Core profile required";
        return result;
    }

    result.success = true;
    result.reason.clear();
    return result;
}

auto GLContextCheck::FormatCompactLine(const GLContextInfo& info) -> std::string
{
    std::ostringstream oss;

    oss << "api=\"" << ApiString(info.api) << "\""
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

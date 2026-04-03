#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)

#include "DynamicLibrary.hpp"

#include "Logging.hpp"

#include <cstdlib>
#include <dlfcn.h>
#include <string>

namespace libprojectM {
namespace Renderer {
namespace Platform {

namespace {

#ifdef __APPLE__

/**
 * Removes all trailing whitespaces for the given string.
 *
 * @param str Input string.
 */
auto TrimTrailingWhitespace(std::string& str) -> void
{
    while (!str.empty())
    {
        const char c = str.back();
        if (c == '\r' || c == '\n' || c == ' ' || c == '\t')
        {
            str.pop_back();
            continue;
        }
        break;
    }
}

/**
 * @brief Checks whether a library name is a bare file name (no path separators or special tokens).
 *
 * Only bare names are eligible for PROJECTM_GLRESOLVER_DYLIB_DIR prepending.
 *
 * @param n The library name to check.
 * @return true if the name is bare (no path components).
 */
auto IsBareLibraryName(const std::string& n) -> bool
{
    if (n.empty())
    {
        return false;
    }
    if (n.find('/') != std::string::npos || n.find('\\') != std::string::npos)
    {
        return false;
    }
    if (n.rfind("@rpath/", 0) == 0 || n.rfind("@loader_path/", 0) == 0 || n.rfind("@executable_path/", 0) == 0)
    {
        return false;
    }
    return true;
}

/**
 * @brief Attempts to open a library using the PROJECTM_GLRESOLVER_DYLIB_DIR environment variable.
 *
 * @param name The library name to open.
 * @param[out] handle Set to the dlopen handle on success, nullptr on failure.
 * @param[out] loadedName Set to the full path on success.
 * @return true if the library was opened via the dylib dir override.
 */
auto TryOpenViaDylibDir(const char* name, void*& handle, std::string& loadedName) -> bool
{
    const char* const extraDir = std::getenv("PROJECTM_GLRESOLVER_DYLIB_DIR");
    if (extraDir == nullptr || extraDir[0] == '\0')
    {
        return false;
    }

    if (!IsBareLibraryName(name))
    {
        if (EnableGLResolverTraceLogging())
        {
            LOG_INFO(std::string("[DynLibrary] PROJECTM_GLRESOLVER_DYLIB_DIR ignored for non-bare name: ") + name);
        }
        // Do not attempt directory prepending when the name already encodes a path or @rpath token.
        return false;
    }

    // Allow whitespace in env var values (common when set in shells or .env files).
    std::string baseDir(extraDir);
    TrimTrailingWhitespace(baseDir);

    std::string full(baseDir);
    if (EnableGLResolverTraceLogging())
    {
        LOG_INFO(std::string("[DynLibrary] using PROJECTM_GLRESOLVER_DYLIB_DIR=\"") + baseDir + "\" to locate: " + name);
    }
    if (!full.empty() && full.back() != '/')
    {
        full.push_back('/');
    }
    full += name;
    ::dlerror();

    handle = ::dlopen(full.c_str(), RTLD_NOW | RTLD_LOCAL);

    if (handle != nullptr)
    {
        loadedName = full;
        if (EnableGLResolverTraceLogging())
        {
            LOG_INFO(std::string("[DynLibrary] Opened  lib=\"") + loadedName + "\"");
        }
        return true;
    }

    return false;
}

#endif // __APPLE__

} // namespace


auto DynamicLibrary::Open(const char* const* names, std::string& reason) -> bool
{
    Close();

    if (names == nullptr)
    {
        reason = "No library names provided";
        return false;
    }

    for (const char* const* namePtr = names; *namePtr; ++namePtr)
    {
        const char* name = *namePtr;
        if (name == nullptr)
        {
            continue;
        }

        if (EnableGLResolverTraceLogging())
        {
            LOG_INFO(std::string("[DynLibrary] Try     lib=\"") + name + "\"");
        }

        ::dlerror(); // clear any prior error
        m_handle = ::dlopen(name, RTLD_NOW | RTLD_LOCAL);

#ifdef __APPLE__

        // Allow callers to specify an explicit search directory for bundled dylibs
        // (e.g., ANGLE's libEGL.dylib / libGLESv2.dylib inside a macOS app bundle).
        // This is an optional override for deployments where @rpath-based discovery is not sufficient.
        // It is intentionally scoped to macOS to avoid changing loader semantics elsewhere.
        if (m_handle == nullptr)
        {
            if (TryOpenViaDylibDir(name, m_handle, m_loadedName))
            {
                return true;
            }
        }

#endif // __APPLE__

        if (m_handle != nullptr)
        {
            m_loadedName = name;
            if (EnableGLResolverTraceLogging())
            {
                LOG_INFO(std::string("[DynLibrary] Opened  lib=\"") + m_loadedName + "\"");
            }
            return true;
        }

        const char* err = ::dlerror();
        if (err != nullptr)
        {
            reason = "dlopen failed for ";
            reason += name;
            reason += ": ";
            reason += err;
        }

    } // for loop

    if (!reason.empty() && EnableGLResolverTraceLogging())
    {
        LOG_INFO(std::string("[DynLibrary] Failed  ") + reason);
    }
    return false;
}


void DynamicLibrary::Close()
{
    if (m_handle == nullptr)
    {
        return;
    }

    ::dlclose(m_handle);

    m_handle = nullptr;
    m_loadedName.clear();
}


auto DynamicLibrary::GetSymbol(const char* name) const -> void*
{
    if (m_handle == nullptr || name == nullptr)
    {
        return nullptr;
    }

    // clear any prior error
    ::dlerror();
    void* sym = ::dlsym(m_handle, name);
    const char* err = ::dlerror();
    if (err != nullptr)
    {
        return nullptr;
    }
    return sym;
}


auto DynamicLibrary::FindGlobalSymbol(const char* name) -> void*
{
    if (name == nullptr)
    {
        return nullptr;
    }

    // clear any prior error
    ::dlerror();
    void* sym = ::dlsym(RTLD_DEFAULT, name);
    const char* err = ::dlerror();
    if (err != nullptr)
    {
        return nullptr;
    }
    return sym;
}

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

#endif // !defined(_WIN32) && !defined(__EMSCRIPTEN__)

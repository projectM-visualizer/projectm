#include "DynamicLibrary.hpp"

#include <cstdlib>

namespace libprojectM {
namespace Renderer {
namespace Platform {

auto EnvFlagEnabled(const char* name, bool defaultValue) -> bool
{
    if (name == nullptr)
    {
        return defaultValue;
    }
    const char* v = std::getenv(name);
    if (v == nullptr || v[0] == '\0')
    {
        return defaultValue;
    }

    // Lowercase first char is enough for common values.
    const char c0 = (v[0] >= 'A' && v[0] <= 'Z') ? static_cast<char>(v[0] - 'A' + 'a') : v[0];
    if (c0 == '1' || c0 == 'y' || c0 == 't')
    {
        return true;
    }
    if (c0 == '0' || c0 == 'n' || c0 == 'f')
    {
        return false;
    }

    // Accept "on"/"off".
    if ((c0 == 'o') && (v[1] != '\0'))
    {
        const char c1 = (v[1] >= 'A' && v[1] <= 'Z') ? static_cast<char>(v[1] - 'A' + 'a') : v[1];
        if (c1 == 'n')
        {
            return true;
        }
        if (c1 == 'f')
        {
            return false;
        }
    }

    return defaultValue;
}

auto EnableGLResolverTraceLogging() -> bool
{
    static const bool enabled = EnvFlagEnabled("PROJECTM_GLRESOLVER_TRACE_LOGGING", false);
    return enabled;
}

DynamicLibrary::~DynamicLibrary()
{
    if (m_closeOnDestruct)
    {
        Close();
    }
}

DynamicLibrary::DynamicLibrary(DynamicLibrary&& other) noexcept
    : m_handle(other.m_handle)
    , m_loadedName(std::move(other.m_loadedName))
    , m_closeOnDestruct(other.m_closeOnDestruct)
{
    other.m_handle = nullptr;
    other.m_loadedName.clear();
    other.m_closeOnDestruct = false;
}

auto DynamicLibrary::operator=(DynamicLibrary&& other) noexcept -> DynamicLibrary&
{
    if (this != &other)
    {
        Close();
        m_handle = other.m_handle;
        m_loadedName = std::move(other.m_loadedName);
        m_closeOnDestruct = other.m_closeOnDestruct;
        other.m_handle = nullptr;
        other.m_loadedName.clear();
        other.m_closeOnDestruct = false;
    }
    return *this;
}

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

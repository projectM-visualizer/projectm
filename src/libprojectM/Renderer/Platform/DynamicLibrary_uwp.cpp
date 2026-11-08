#include "DynamicLibrary.hpp"

// Windows Store (UWP) apps can't use LoadLibrary(Ex), GetModuleHandle or GetSystemDirectory,
// which DynamicLibrary_win32.cpp relies on. A UWP process can only load DLLs that ship in its own
// package, which is where an EGL/GLES provider such as ANGLE is deployed, so LoadPackagedLibrary
// is all that's needed.
#ifdef PLATFORM_WINDOWS_STORE_APP

#include "PlatformLibraryNames.hpp"

#include <array>
#include <cstring>
#include <string>
#include <windows.h>

namespace libprojectM {
namespace Renderer {
namespace Platform {

namespace {

/**
 * @brief Converts a Windows FARPROC into the generic symbol representation (void*).
 *
 * Copies the bit pattern instead of casting between function and object pointers.
 */
auto WinProcToSymbol(FARPROC proc) noexcept -> void*
{
    if (proc == nullptr || sizeof(proc) != sizeof(void*))
    {
        return nullptr;
    }

    void* sym = nullptr;
    std::memcpy(&sym, &proc, sizeof(void*));
    return sym;
}

/**
 * @brief Loads a DLL from the app package.
 * @param dllName The DLL file name. Library names are plain ASCII.
 * @return The module handle, or nullptr if it couldn't be loaded.
 */
auto TryLoadPackaged(const char* dllName) -> HMODULE
{
    const std::string narrowName(dllName);
    const std::wstring wideName(narrowName.begin(), narrowName.end());
    return ::LoadPackagedLibrary(wideName.c_str(), 0);
}

} // namespace

auto DynamicLibrary::Open(const char* const* names, std::string& reason) -> bool
{
    Close();

    if (names == nullptr)
    {
        reason = "No library names provided";
        return false;
    }

    for (const char* const* namePtr = names; *namePtr != nullptr; ++namePtr)
    {
        const HMODULE handle = TryLoadPackaged(*namePtr);
        if (handle != nullptr)
        {
            m_handle = static_cast<LibHandle>(handle);
            m_loadedName = *namePtr;
            return true;
        }

        reason = "LoadPackagedLibrary failed for ";
        reason += *namePtr;
        reason += " (error ";
        reason += std::to_string(static_cast<unsigned long>(::GetLastError()));
        reason += ")";
    }

    return false;
}

void DynamicLibrary::Close()
{
    if (m_handle == nullptr)
    {
        return;
    }

    ::FreeLibrary(static_cast<HMODULE>(m_handle));

    m_handle = nullptr;
    m_loadedName.clear();
}

auto DynamicLibrary::GetSymbol(const char* name) const -> void*
{
    if (m_handle == nullptr || name == nullptr)
    {
        return nullptr;
    }

    return WinProcToSymbol(::GetProcAddress(static_cast<HMODULE>(m_handle), name));
}

auto DynamicLibrary::FindGlobalSymbol(const char* name) -> void*
{
    if (name == nullptr)
    {
        return nullptr;
    }

    // UWP apps can't look up already loaded modules with GetModuleHandle. Loading a packaged DLL
    // that's already loaded only adds a reference. The handles are cached and stay loaded for the
    // lifetime of the process, like the module cache in DynamicLibrary_win32.cpp.
    static const auto eglModules = []() {
        std::array<HMODULE, kAllEglModuleNames.size()> modules{};
        for (std::size_t i = 0; i < kAllEglModuleNames.size(); ++i)
        {
            modules[i] = TryLoadPackaged(kAllEglModuleNames[i]);
        }
        return modules;
    }();

    for (const HMODULE module : eglModules)
    {
        if (module != nullptr)
        {
            if (void* ptr = WinProcToSymbol(::GetProcAddress(module, name)))
            {
                return ptr;
            }
        }
    }

    return nullptr;
}

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

#endif // PLATFORM_WINDOWS_STORE_APP

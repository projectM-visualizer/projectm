#ifdef _WIN32

#include "DynamicLibrary.hpp"
#include "PlatformLibraryNames.hpp"

#include "Logging.hpp"

#include <array>
#include <cstring>
#include <mutex>
#include <string>
#include <windows.h>

// -------------------------------------------------------------------------
// Windows DLL safe-search flags
// -------------------------------------------------------------------------
// Some toolchains/Windows SDKs may not define these constants even though
// the OS loader supports them (Windows 7 w/ KB2533623+, Win8+). We define
// them locally when absent to allow using LoadLibraryEx with safe search.
#ifndef LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
#define LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR 0x00000100
#endif
#ifndef LOAD_LIBRARY_SEARCH_APPLICATION_DIR
#define LOAD_LIBRARY_SEARCH_APPLICATION_DIR 0x00000200
#endif
#ifndef LOAD_LIBRARY_SEARCH_USER_DIRS
#define LOAD_LIBRARY_SEARCH_USER_DIRS 0x00000400
#endif
#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
#define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif
#ifndef LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
#define LOAD_LIBRARY_SEARCH_DEFAULT_DIRS 0x00001000
#endif

// -------------------------------------------------------------------------
// Legacy DLL search fallback
// -------------------------------------------------------------------------
//
// If the OS loader does not support LOAD_LIBRARY_SEARCH_* flags (ERROR_INVALID_PARAMETER),
// this loader tries to load from explicit safe locations (application directory and
// System32 for known system DLLs).
//
// As a last resort, some applications may still want to fall back to LoadLibrary(name)
// (which can consult legacy search paths such as the process current working directory).
// This is disabled by default for security hardening.
//
// Define GLRESOLVER_ALLOW_UNSAFE_DLL_SEARCH=1 to re-enable the legacy fallback.
#ifndef GLRESOLVER_ALLOW_UNSAFE_DLL_SEARCH
#define GLRESOLVER_ALLOW_UNSAFE_DLL_SEARCH 0
#endif

namespace libprojectM {
namespace Renderer {
namespace Platform {

namespace {

/**
 * @brief Converts a Windows FARPROC into the generic symbol representation (void*) .
 *
 * Windows GetProcAddress returns a function pointer type (FARPROC). To avoid relying on a direct
 * reinterpret_cast between function pointers and object pointers, this helper copies the bit pattern
 * into a void* storage location when the sizes match.
 */
auto WinProcToSymbol(FARPROC proc) noexcept -> void*
{
    if (proc == nullptr)
    {
        return nullptr;
    }

    if (sizeof(proc) != sizeof(void*))
    {
        return nullptr;
    }

    void* sym = nullptr;
    std::memcpy(&sym, &proc, sizeof(void*));
    return sym;
}

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
 * @brief Builds a full path by combining the application directory with a DLL name.
 *
 * @param dllName The DLL file name to append.
 * @return The full path, or an empty string on failure.
 */
auto BuildAppDirPath(const char* dllName) -> std::string
{
    char exePath[MAX_PATH] = {};
    const DWORD n = ::GetModuleFileNameA(nullptr, exePath, static_cast<DWORD>(sizeof(exePath)));
    if (n == 0u || n >= sizeof(exePath))
    {
        return std::string();
    }

    // Strip to directory.
    bool found = false;
    for (DWORD i = n; i > 0u; --i)
    {
        const char c = exePath[i - 1u];
        if (c == '\\' || c == '/')
        {
            exePath[i] = '\0';
            found = true;
            break;
        }
    }

    if (!found)
    {
        // No directory separator found; cannot construct a meaningful path.
        return std::string();
    }

    std::string full(exePath);
    full += dllName;
    return full;
}

/**
 * @brief Builds a full path by combining the System32 directory with a DLL name.
 *
 * @param dllName The DLL file name to append.
 * @return The full path, or an empty string on failure.
 */
auto BuildSystem32Path(const char* dllName) -> std::string
{
    char sysDir[MAX_PATH] = {};
    const UINT n = ::GetSystemDirectoryA(sysDir, static_cast<UINT>(sizeof(sysDir)));
    if (n == 0u || n >= sizeof(sysDir))
    {
        return std::string();
    }
    std::string full(sysDir);
    full += "\\";
    full += dllName;
    return full;
}

/**
 * @brief Attempts to load a DLL via LoadLibraryExA with the given flags.
 *
 * @param dllName The DLL name or path to load.
 * @param flags LoadLibraryEx flags.
 * @return Module handle, or nullptr on failure.
 */
auto TryLoadEx(const char* dllName, DWORD flags) -> HMODULE
{
    ::SetLastError(0);
    return ::LoadLibraryExA(dllName, nullptr, flags);
}

/**
 * @brief Attempts to load a DLL via LoadLibraryA (legacy, no safe-search flags).
 *
 * @param dllName The DLL name or path to load.
 * @return Module handle, or nullptr on failure.
 */
auto TryLoad(const char* dllName) -> HMODULE
{
    ::SetLastError(0);
    return ::LoadLibraryA(dllName);
}

/**
 * @brief Legacy fallback loader for explicit paths when LOAD_LIBRARY_SEARCH_* flags are unavailable.
 *
 * Prefer LOAD_WITH_ALTERED_SEARCH_PATH for absolute paths so dependent DLLs are
 * resolved relative to the loaded module directory.
 *
 * @param dllPath The DLL path to load.
 * @return Module handle, or nullptr on failure.
 */
auto TryLoadExplicitPathFallback(const char* dllPath) -> HMODULE
{
    if (dllPath == nullptr)
    {
        return nullptr;
    }

    // Absolute path heuristics for Win32:
    //  - "C:\\path\\to\\library.dll"  (drive)
    //  - "\\\\server\\share\\library.dll" (UNC)
    const bool isDriveAbs = (std::strlen(dllPath) > 2u) && (dllPath[1] == ':') &&
                            (dllPath[2] == '\\' || dllPath[2] == '/');
    const bool isUncAbs = (dllPath[0] == '\\' && dllPath[1] == '\\');

    if (isDriveAbs || isUncAbs)
    {
        return TryLoadEx(dllPath, LOAD_WITH_ALTERED_SEARCH_PATH);
    }

    // Relative paths are still explicit, but LOAD_WITH_ALTERED_SEARCH_PATH has
    // undefined behavior for relative lpFileName.
    return TryLoad(dllPath);
}

/**
 * @brief Formats a Windows error code into a human-readable reason string.
 *
 * @param name The DLL name that failed to load.
 * @param reason Output string for the error description.
 */
auto FormatWindowsLoadError(const char* name, std::string& reason) -> void
{
    const DWORD err = ::GetLastError();

    char* msg = nullptr;
    const DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    const DWORD lang = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

    const DWORD len = ::FormatMessageA(flags,
                                       nullptr,
                                       err,
                                       lang,
                                       reinterpret_cast<LPSTR>(&msg),
                                       0,
                                       nullptr);

    reason = "LoadLibrary failed for ";
    reason += name;
    reason += " (";
    reason += std::to_string(static_cast<unsigned long>(err));
    reason += "): ";
    if (len != 0 && msg != nullptr)
    {
        reason += msg;
        TrimTrailingWhitespace(reason);
    }
    if (msg != nullptr)
    {
        ::LocalFree(msg);
    }
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

        // DLL loading policy:
        //  - Prefer LoadLibraryExA with LOAD_LIBRARY_SEARCH_* flags to avoid CWD/PATH hijacking.
        //  - If the OS loader doesn't support the flags (ERROR_INVALID_PARAMETER), fall back to:
        //      * application directory (explicit full path based on the running module)
        //      * System32 (for known system DLLs like opengl32.dll)
        //      * optionally, legacy LoadLibraryA(name) (disabled by default; see GLRESOLVER_ALLOW_UNSAFE_DLL_SEARCH).
        //
        // NOTE: We intentionally do not call SetDefaultDllDirectories() here. It changes
        // the process-wide DLL search behavior and can surprise a host application.
        // Instead, we rely on per-call LoadLibraryExA LOAD_LIBRARY_SEARCH_* flags when available.

        // Detect whether the name contains a path. If so, LoadLibraryA is already safe (absolute/relative path is explicit).
        HMODULE handle = nullptr;
        const bool hasPath = (std::strchr(name, '\\') != nullptr) || (std::strchr(name, '/') != nullptr);

        if (hasPath)
        {
            // Prefer safe search flags for dependency resolution relative to the DLL location.
            handle = TryLoadEx(name, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
            if (handle == nullptr && ::GetLastError() == ERROR_INVALID_PARAMETER)
            {
                // Older OS loader: use altered search path for absolute paths to improve
                // dependent DLL resolution.
                handle = TryLoadExplicitPathFallback(name);
            }
        }
        else
        {
            // Bare name: avoid CWD/PATH when possible.
            //
            // NOTE: When loading app-bundled DLLs (e.g. ANGLE's libEGL/libGLESv2), loading by
            // explicit full path improves dependency resolution by allowing the loader to search
            // the DLL's directory for its dependent DLLs (LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR).
            // This keeps the search path restricted without changing process-wide state.
            const bool isSystemOpenGL32 = (_stricmp(name, "opengl32.dll") == 0);

            // For system DLLs like opengl32.dll, never prefer the application directory.
            // Loading system DLLs from app-local paths enables DLL preloading/hijacking.
            std::string appFull;
            if (!isSystemOpenGL32)
            {
                appFull = BuildAppDirPath(name);
                if (!appFull.empty())
                {
                    handle = TryLoadEx(appFull.c_str(), LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
                    if (handle == nullptr && ::GetLastError() == ERROR_INVALID_PARAMETER)
                    {
                        // Older OS loader: use altered search path for absolute paths to improve
                        // dependent DLL resolution.
                        handle = TryLoadExplicitPathFallback(appFull.c_str());
                    }
                }
            }

            if (handle == nullptr)
            {
                if (isSystemOpenGL32)
                {
                    handle = TryLoadEx(name, LOAD_LIBRARY_SEARCH_SYSTEM32);
                }
                else
                {
                    handle = TryLoadEx(name, LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
                }
            }

            if (handle == nullptr && ::GetLastError() == ERROR_INVALID_PARAMETER)
            {
                // Flags unsupported: manual safe search.
                // 1) Application directory
                if (handle == nullptr && !appFull.empty())
                {
                    handle = TryLoadExplicitPathFallback(appFull.c_str());
                }

                // 2) System32 for known system DLLs
                if (handle == nullptr && _stricmp(name, "opengl32.dll") == 0)
                {
                    const std::string sysFull = BuildSystem32Path("opengl32.dll");
                    if (!sysFull.empty())
                    {
                        handle = TryLoadExplicitPathFallback(sysFull.c_str());
                    }
                }

                // 3) Legacy fallback (disabled by default).
                //
                // NOTE: LoadLibrary(name) without LOAD_LIBRARY_SEARCH_* flags can consult legacy
                // search paths (including the current working directory) depending on process
                // configuration. See Microsoft guidance on DLL search order hardening.
                if (handle == nullptr && GLRESOLVER_ALLOW_UNSAFE_DLL_SEARCH != 0)
                {
                    static std::once_flag s_warnOnce;
                    std::call_once(s_warnOnce, []() {
                        LOG_WARN("[DynLibrary] Unsafe DLL search fallback is enabled (GLRESOLVER_ALLOW_UNSAFE_DLL_SEARCH!=0). "
                                 "This may consult legacy loader search paths; prefer configuring an explicit library directory.");
                    });

                    handle = TryLoad(name);
                }
            }
            else if (handle == nullptr)
            {
                // Flags supported but the restricted search didn't find it. Fall back to default dirs.
                handle = TryLoadEx(name, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
            }
        }

        m_handle = static_cast<LibHandle>(handle);

        if (m_handle != nullptr)
        {
            m_loadedName = name;
            if (EnableGLResolverTraceLogging())
            {
                LOG_INFO(std::string("[DynLibrary] Opened  lib=\"") + m_loadedName + "\"");
            }
            return true;
        }

        FormatWindowsLoadError(name, reason);

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

    // Cache module handles to avoid repeated GetModuleHandleA calls on every symbol
    // lookup. This function is called in the hot path during GLAD initialization.
    // The cache is populated once and is safe because modules
    // that are already loaded remain loaded for the process lifetime in this resolver.
    struct CachedModules {
        HMODULE mainModule{};
        HMODULE glModule{};
        std::array<HMODULE, kAllEglModuleNames.size()> eglModules{};
        bool initialized{false};
    };

    static CachedModules cache = []() {
        CachedModules c;
        c.mainModule = ::GetModuleHandleA(nullptr);
        c.glModule = ::GetModuleHandleA("opengl32.dll");
        for (std::size_t i = 0; i < kAllEglModuleNames.size(); ++i)
        {
            c.eglModules[i] = ::GetModuleHandleA(kAllEglModuleNames[i]);
        }
        c.initialized = true;
        return c;
    }();

    // Search the main executable first.
    if (cache.mainModule != nullptr)
    {
        if (void* ptr = WinProcToSymbol(::GetProcAddress(cache.mainModule, name)))
        {
            return ptr;
        }
    }

    // If the host has already loaded EGL/GLES provider DLLs (e.g., ANGLE), probe those modules as well.
    // This is an enhancement for applications embedding this library where we may not have
    // opened the provider libraries ourselves.
    for (std::size_t i = 0; i < cache.eglModules.size(); ++i)
    {
        if (cache.eglModules[i] != nullptr)
        {
            if (void* ptr = WinProcToSymbol(::GetProcAddress(cache.eglModules[i], name)))
            {
                return ptr;
            }
        }
    }

    // Then search the default OpenGL module.
    if (cache.glModule != nullptr)
    {
        if (void* ptr = WinProcToSymbol(::GetProcAddress(cache.glModule, name)))
        {
            return ptr;
        }
    }

    return nullptr;
}

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

#endif // _WIN32

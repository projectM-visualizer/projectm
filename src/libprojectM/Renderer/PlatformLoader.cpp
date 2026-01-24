
#include "PlatformLoader.hpp"

#ifndef __EMSCRIPTEN__

#ifdef _WIN32

#include <array>
#include <cstddef>

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
// Optional legacy DLL search fallback
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
// Define PLATFORM_ALLOW_UNSAFE_DLL_SEARCH=1 to re-enable the legacy fallback.
#ifndef PLATFORM_ALLOW_UNSAFE_DLL_SEARCH

#define PLATFORM_ALLOW_UNSAFE_DLL_SEARCH 0

#endif

#else // #ifdef _WIN32

#include <dlfcn.h>

#endif // #ifdef _WIN32

#endif

namespace libprojectM {
namespace Renderer {
namespace Platform {


namespace {

#ifdef _WIN32

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

#endif

} // namespace

#ifndef __EMSCRIPTEN__

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

#ifdef _WIN32

            // DLL loading policy:
            //  - Prefer LoadLibraryExA with LOAD_LIBRARY_SEARCH_* flags to avoid CWD/PATH hijacking.
            //  - If the OS loader doesn't support the flags (ERROR_INVALID_PARAMETER), fall back to:
            //      * application directory (explicit full path based on the running module)
            //      * System32 (for known system DLLs like opengl32.dll)
            //      * optionally, legacy LoadLibraryA(name) (disabled by default; see PLATFORM_ALLOW_UNSAFE_DLL_SEARCH).
            //
            // NOTE: We intentionally do not call SetDefaultDllDirectories() here. It changes
            // the process-wide DLL search behavior and can surprise a host application.
            // Instead, we rely on per-call LoadLibraryExA LOAD_LIBRARY_SEARCH_* flags when available.

            auto buildAppDirPath = [](const char* dllName) -> std::string {
                char exePath[MAX_PATH] = {};
                const DWORD n = ::GetModuleFileNameA(nullptr, exePath, static_cast<DWORD>(sizeof(exePath)));
                if (n == 0u || n >= sizeof(exePath))
                {
                    return std::string();
                }

                // Strip to directory.
                for (DWORD i = n; i > 0u; --i)
                {
                    const char c = exePath[i - 1u];
                    if (c == '\\' || c == '/')
                    {
                        exePath[i] = '\0';
                        break;
                    }
                }

                std::string full(exePath);
                full += dllName;
                return full;
            };

            auto buildSystem32Path = [](const char* dllName) -> std::string {
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
            };

            auto tryLoadEx = [&](const char* dllName, DWORD flags) -> HMODULE {
                ::SetLastError(0);
                return ::LoadLibraryExA(dllName, nullptr, flags);
            };

            auto tryLoad = [&](const char* dllName) -> HMODULE {
                ::SetLastError(0);
                return ::LoadLibraryA(dllName);
            };

            // Best-effort legacy fallback when LOAD_LIBRARY_SEARCH_* flags are unavailable.
            // Prefer LOAD_WITH_ALTERED_SEARCH_PATH for absolute paths so dependent DLLs are
            // resolved relative to the loaded module directory.
            auto tryLoadExplicitPathFallback = [&](const char* dllPath) -> HMODULE {
                if (dllPath == nullptr)
                {
                    return nullptr;
                }

                // Absolute path heuristics for Win32:
                //  - "C:\\..."  (drive)
                //  - "\\\\server\\share..." (UNC)
                const bool isDriveAbs = (std::strlen(dllPath) > 2u) && (dllPath[1] == ':') &&
                                        (dllPath[2] == '\\' || dllPath[2] == '/');
                const bool isUncAbs = (dllPath[0] == '\\' && dllPath[1] == '\\');

                if (isDriveAbs || isUncAbs)
                {
                    return tryLoadEx(dllPath, LOAD_WITH_ALTERED_SEARCH_PATH);
                }

                // Relative paths are still explicit, but LOAD_WITH_ALTERED_SEARCH_PATH has
                // undefined behavior for relative lpFileName.
                return tryLoad(dllPath);
            };

            // Detect whether the name contains a path. If so, LoadLibraryA is already safe (absolute/relative path is explicit).
            HMODULE handle = nullptr;
            const bool hasPath = (std::strchr(name, '\\') != nullptr) || (std::strchr(name, '/') != nullptr);

            if (hasPath)
            {
                // Prefer safe search flags for dependency resolution relative to the DLL location.
                handle = tryLoadEx(name, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
                if (handle == nullptr && ::GetLastError() == ERROR_INVALID_PARAMETER)
                {
                    // Older OS loader: use altered search path for absolute paths to improve
                    // dependent DLL resolution.
                    handle = tryLoadExplicitPathFallback(name);
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
                    appFull = buildAppDirPath(name);
                    if (!appFull.empty())
                    {
                        handle = tryLoadEx(appFull.c_str(), LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
                        if (handle == nullptr && ::GetLastError() == ERROR_INVALID_PARAMETER)
                        {
                            // Older OS loader: use altered search path for absolute paths to improve
                            // dependent DLL resolution.
                            handle = tryLoadExplicitPathFallback(appFull.c_str());
                        }
                    }
                }

                if (handle == nullptr)
                {
                    if (isSystemOpenGL32)
                    {
                        handle = tryLoadEx(name, LOAD_LIBRARY_SEARCH_SYSTEM32);
                    }
                    else
                    {
                        handle = tryLoadEx(name, LOAD_LIBRARY_SEARCH_APPLICATION_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
                    }
                }

                if (handle == nullptr && ::GetLastError() == ERROR_INVALID_PARAMETER)
                {
                    // Flags unsupported: best-effort manual safe search.
                    // 1) Application directory
                    if (handle == nullptr && !appFull.empty())
                    {
                        handle = tryLoadExplicitPathFallback(appFull.c_str());
                    }

                    // 2) System32 for known system DLLs
                    if (handle == nullptr && _stricmp(name, "opengl32.dll") == 0)
                    {
                        const std::string sysFull = buildSystem32Path("opengl32.dll");
                        if (!sysFull.empty())
                        {
                            handle = tryLoadExplicitPathFallback(sysFull.c_str());
                        }
                    }

                    // 3) Legacy fallback (disabled by default).
                    //
                    // NOTE: LoadLibrary(name) without LOAD_LIBRARY_SEARCH_* flags can consult legacy
                    // search paths (including the current working directory) depending on process
                    // configuration. See Microsoft guidance on DLL search order hardening.
                    if (handle == nullptr && PLATFORM_ALLOW_UNSAFE_DLL_SEARCH != 0)
                    {
                        handle = tryLoad(name);
                    }
                }
                else if (handle == nullptr)
                {
                    // Flags supported but the restricted search didn't find it. Fall back to default dirs.
                    handle = tryLoadEx(name, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
                }
            }

            m_handle = handle;

#else

            ::dlerror(); // clear any prior error
            m_handle = ::dlopen(name, RTLD_NOW | RTLD_LOCAL);

#endif

            if (m_handle != nullptr)
            {
                m_loadedName = name;
                return true;
            }

#ifdef _WIN32

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

#else

        const char* err = ::dlerror();
        if (err != nullptr)
        {
            reason = "dlopen failed for ";
            reason += name;
            reason += ": ";
            reason += err;
        }

#endif

        } // for loop

        return false;
    }

    /**
     * @brief Closes the library if it is open.
     */
    void DynamicLibrary::Close()
    {
        if (m_handle == nullptr)
        {
            return;
        }

#ifdef _WIN32

        ::FreeLibrary(m_handle);

#else

        ::dlclose(m_handle);

#endif

        m_handle = nullptr;
        m_loadedName.clear();
    }

    /**
     * @brief Resolves a symbol from this library.
     * @param name The symbol name.
     *
     * @return Symbol address or nullptr.
     */
    auto DynamicLibrary::GetSymbol(const char* name) const -> void*
    {
        if (m_handle == nullptr || name == nullptr)
        {
            return nullptr;
        }

#ifdef _WIN32

        return WinProcToSymbol(::GetProcAddress(m_handle, name));

#else

        // clear any prior error
        ::dlerror();
        void* sym = ::dlsym(m_handle, name);
        const char* err = ::dlerror();
        if (err != nullptr)
        {
            return nullptr;
        }
        return sym;

#endif

    }

    /**
     * @brief Looks up a symbol in the global process scope.
     * @param name Symbol name.
     *
     * @return Symbol address or nullptr.
     */
    auto DynamicLibrary::FindGlobalSymbol(const char* name) -> void*
    {
        if (name == nullptr)
        {
            return nullptr;
        }

#ifdef _WIN32

        // Search the main executable first.
        if (HMODULE mainModule = ::GetModuleHandleA(nullptr))
        {
            if (void* ptr = WinProcToSymbol(::GetProcAddress(mainModule, name)))
            {
                return ptr;
            }
        }

        // If the host has already loaded EGL/GLES provider DLLs (e.g., ANGLE), probe those modules as well.
        // This is a best-effort enhancement for applications embedding this library where we may not have
        // opened the provider libraries ourselves.
        {
            static constexpr std::array<const char*, 6> moduleNames =
            {
                "libEGL.dll",
                "EGL.dll",
                "libGLESv2.dll",
                "GLESv2.dll",
                "libGLESv3.dll",
                "GLESv3.dll"
            };

            for (const auto& m : moduleNames)
            {
                if (HMODULE mod = ::GetModuleHandleA(m))
                {
                    if (void* ptr = WinProcToSymbol(::GetProcAddress(mod, name)))
                    {
                        return ptr;
                    }
                }
            }
        }

        // Then search the default OpenGL module.
        if (HMODULE glModule = ::GetModuleHandleA("opengl32.dll"))
        {
            if (void* ptr = WinProcToSymbol(::GetProcAddress(glModule, name)))
            {
                return ptr;
            }
        }

        return nullptr;

#else

        // clear any prior error
        ::dlerror();
        void* sym = ::dlsym(RTLD_DEFAULT, name);
        const char* err = ::dlerror();
        if (err != nullptr)
        {
            return nullptr;
        }
        return sym;

#endif

    }

#endif // #ifndef __EMSCRIPTEN__

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM


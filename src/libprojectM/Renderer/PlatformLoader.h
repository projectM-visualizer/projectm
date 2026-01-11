#pragma once

#include <cstddef>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace libprojectM
{
namespace Renderer
{
namespace Platform
{

/**
 * @brief Platform library handle type.
 */
#ifdef _WIN32
using LibHandle = HMODULE;
#else
using LibHandle = void*;
#endif

/**
 * @brief RAII wrapper around a dynamic library handle.
 */
class DynamicLibrary
{
public:
    DynamicLibrary() = default;

    /**
     * @brief Constructs a DynamicLibrary by attempting to open the first available library name.
     * @param names Null-terminated list of candidate library names.
     */
    explicit DynamicLibrary(const char* const* names)
    {
        Open(names);
    }

    ~DynamicLibrary()
    {
        Close();
    }

    DynamicLibrary(const DynamicLibrary&) = delete;
    auto operator=(const DynamicLibrary&) -> DynamicLibrary& = delete;

    DynamicLibrary(DynamicLibrary&& other) noexcept
        : m_handle(other.m_handle)
    {
        other.m_handle = nullptr;
    }

    auto operator=(DynamicLibrary&& other) noexcept -> DynamicLibrary&
    {
        if (this != &other)
        {
            Close();
            m_handle = other.m_handle;
            other.m_handle = nullptr;
        }
        return *this;
    }

    /**
     * @brief Attempts to open the first library from the given candidate list.
     * @param names Null-terminated list of candidate library names.
     *
     * @return true if a library was opened, false otherwise.
     */
    auto Open(const char* const* names) -> bool
    {
        Close();

        if (names == nullptr)
        {
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
            m_handle = ::LoadLibraryA(name);
#else
            m_handle = ::dlopen(name, RTLD_LAZY | RTLD_LOCAL);
#endif

            if (m_handle != nullptr)
            {
                return true;
            }
        }

        return false;
    }

    /**
     * @brief Closes the library if it is open.
     */
    void Close()
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
    }

    /**
     * @brief Returns true if the library handle is valid.
     */
    [[nodiscard]] auto IsOpen() const -> bool
    {
        return m_handle != nullptr;
    }

    /**
     * @brief Returns the raw library handle.
     */
    [[nodiscard]] auto Handle() const -> LibHandle
    {
        return m_handle;
    }

    /**
     * @brief Resolves a symbol from this library.
     * @param name The symbol name.
     *
     * @return Symbol address or nullptr.
     */
    auto GetSymbol(const char* name) const -> void*
    {
        if (m_handle == nullptr || name == nullptr)
        {
            return nullptr;
        }

#ifdef _WIN32
        return reinterpret_cast<void*>(::GetProcAddress(m_handle, name));
#else
        return ::dlsym(m_handle, name);
#endif
    }

    /**
     * @brief Looks up a symbol in the global process scope.
     * @param name Symbol name.
     *
     * @return Symbol address or nullptr.
     */
    static auto FindGlobalSymbol(const char* name) -> void*
    {
        if (name == nullptr)
        {
            return nullptr;
        }

#ifdef _WIN32
        // Search the main executable first.
        if (HMODULE mainModule = ::GetModuleHandleA(nullptr))
        {
            if (void* ptr = reinterpret_cast<void*>(::GetProcAddress(mainModule, name)))
            {
                return ptr;
            }
        }

        // Then search the default OpenGL module.
        if (HMODULE glModule = ::GetModuleHandleA("opengl32.dll"))
        {
            if (void* ptr = reinterpret_cast<void*>(::GetProcAddress(glModule, name)))
            {
                return ptr;
            }
        }

        return nullptr;
#else
        return ::dlsym(RTLD_DEFAULT, name);
#endif
    }

private:
    LibHandle m_handle{};          //!< Library handle used to access the system library.
};

/**
 * @brief Checks whether the current context is EGL-based.
 * @param eglLib The loaded EGL library.
 */
auto IsCurrentEgl(const DynamicLibrary& eglLib) -> bool;

/**
 * @brief Checks whether the current context is GLX-based (Linux/Unix).
 * @param glLib The loaded GL library.
 */
#ifndef _WIN32

auto IsCurrentGlx(const DynamicLibrary& glLib) -> bool;

#else

/**
 * @brief Checks whether the current context is WGL-based (Windows).
 */
auto IsCurrentWgl() -> bool;

#endif

// ---------------- Inline implementations -----------------------------------

inline auto IsCurrentEgl(const DynamicLibrary& eglLib) -> bool
{
    if (!eglLib.IsOpen())
    {
        return false;
    }

    using EglGetCurrentContext = void* (*)();
    auto func = reinterpret_cast<EglGetCurrentContext>(eglLib.GetSymbol("eglGetCurrentContext"));
    return func != nullptr && func() != nullptr;
}

#ifndef _WIN32
inline auto IsCurrentGlx(const DynamicLibrary& glLib) -> bool
{
    if (!glLib.IsOpen())
    {
        return false;
    }

    using GlxGetCurrentContext = void* (*)();

    auto func = reinterpret_cast<GlxGetCurrentContext>(glLib.GetSymbol("glXGetCurrentContextARB"));
    if (func == nullptr)
    {
        func = reinterpret_cast<GlxGetCurrentContext>(glLib.GetSymbol("glXGetCurrentContext"));
    }

    return func != nullptr && func() != nullptr;
}

#else
inline auto IsCurrentWgl() -> bool
{
    HMODULE glModule = ::GetModuleHandleA("opengl32.dll");
    if (!glModule)
    {
        return false;
    }

    using WglGetCurrentContext = void* (WINAPI*)(void);
    auto func = reinterpret_cast<WglGetCurrentContext>(::GetProcAddress(glModule, "wglGetCurrentContext"));
    return func != nullptr && func() != nullptr;
}

#endif

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

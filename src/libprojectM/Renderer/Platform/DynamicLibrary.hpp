#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>

#ifndef __EMSCRIPTEN__

#ifdef _WIN32
#include <windows.h>
#else // #ifdef _WIN32
#include <dlfcn.h>
#endif // #ifdef _WIN32

#endif // #ifndef __EMSCRIPTEN__

// -------------------------------------------------------------------------
// Minimal EGL calling-convention support
// -------------------------------------------------------------------------
//
// We avoid including EGL headers in this loader. On 32-bit Windows, EGL entry
// points use the stdcall calling convention (via EGLAPIENTRY/KHRONOS_APIENTRY).
// On 64-bit Windows the calling convention is ignored.
//
// This macro is used in our local EGL function pointer typedefs to ensure we
// call into the provider (ANGLE / driver EGL) using the correct ABI.
#if defined(_WIN32) && !defined(_WIN64)
#define PLATFORM_EGLAPIENTRY __stdcall
#else
#define PLATFORM_EGLAPIENTRY
#endif

// -------------------------------------------------------------------------
// Optional loader diagnostics
// -------------------------------------------------------------------------
//
// When enabled, the loader prints diagnostics for unusual ABI
// situations (e.g., platforms where data pointers and function pointers have
// different representations/sizes). Disabled by default to avoid noisy output.
#ifndef GLRESOLVER_LOADER_DIAGNOSTICS
#define GLRESOLVER_LOADER_DIAGNOSTICS 0
#endif

namespace libprojectM {
namespace Renderer {
namespace Platform {

/**
 * @brief Platform library handle type.
 */
#ifdef _WIN32
using LibHandle = HMODULE;
#else
using LibHandle = void*;
#endif

#ifdef _WIN32

// Windows GL/EGL library name candidates

constexpr std::array<const char*, 3> kNativeEglNames = {
    "libEGL.dll",
    "EGL.dll",
    nullptr};

constexpr std::array<const char*, 2> kNativeGlNames = {
    "opengl32.dll",
    nullptr};

constexpr std::array<const char*, 5> kNativeGlesNames = {
    "libGLESv3.dll",
    "GLESv3.dll",
    "libGLESv2.dll",
    "GLESv2.dll",
    nullptr};

/**
 * EGL names for global module lookup.
 */
static constexpr std::array<const char*, 6> kAllEglModuleNames = {
    "libEGL.dll",
    "EGL.dll",
    "libGLESv2.dll",
    "GLESv2.dll",
    "libGLESv3.dll",
    "GLESv3.dll"};

#elif defined(__APPLE__)

// macOS GL/EGL library name candidates
// macOS native OpenGL uses CGL (OpenGL.framework). ANGLE (and other portability layers).
// commonly provide EGL/GLES dylibs in the application bundle / @rpath.

constexpr std::array<const char*, 6> kNativeEglNames = {
    "@rpath/libEGL.dylib",
    "@rpath/libEGL.1.dylib",
    "libEGL.dylib",
    "libEGL.1.dylib",
    "EGL",
    nullptr};

constexpr std::array<const char*, 2> kNativeGlNames = {
    "/System/Library/Frameworks/OpenGL.framework/OpenGL",
    nullptr};

constexpr std::array<const char*, 7> kNativeGlesNames = {
    "@rpath/libGLESv3.dylib",
    "@rpath/libGLESv2.dylib",
    "@rpath/libGLESv2_with_capture.dylib",
    "libGLESv3.dylib",
    "libGLESv2.dylib",
    "libGLESv2_with_capture.dylib",
    nullptr};

#elif defined(__ANDROID__)

// Android EGL + GLES (no desktop libGL / GLX) library name candidates

constexpr std::array<const char*, 2> kNativeEglNames = {
    "libEGL.so",
    nullptr};

constexpr std::array<const char*, 3> kNativeGlesNames = {
    "libGLESv3.so",
    "libGLESv2.so",
    nullptr};

#else // #ifdef _WIN32

// Unix GL/EGL library name candidates

constexpr std::array<const char*, 3> kNativeEglNames = {
    "libEGL.so.1",
    "libEGL.so",
    nullptr};

/**
 * Linux / GLES:
 * Prefer libGLESv3/libGLESv2 sonames. Core GLES entry points are expected
 * to be available as library exports. eglGetProcAddress is not guaranteed
 * to return core symbols unless EGL_KHR_get_all_proc_addresses (or its
 * client variant) is advertised.
 */
constexpr std::array<const char*, 6> kNativeGlesNames = {
    "libGLESv3.so.3",
    "libGLESv3.so",
    "libGLESv2.so.2",
    "libGLESv2.so.1",
    "libGLESv2.so",
    nullptr};

/**
 * Linux / GLVND note:
 * Many modern distributions use GLVND, which splits OpenGL entry points (libOpenGL) from
 * window-system glue such as GLX (libGLX). Prefer GLVND-facing libs first, but keep legacy
 * libGL names in the candidate list for compatibility with older or non-GLVND stacks.
 */
constexpr std::array<const char*, 6> kNativeGlNames = {
    "libOpenGL.so.1", // GLVND OpenGL dispatcher (core gl* entry points)
    "libOpenGL.so.0", // older GLVND soname
    "libGL.so.1",     // legacy/compat umbrella (often provided by GLVND)
    "libGL.so.0",     // sometimes shipped as .so.0
    "libGL.so",
    nullptr};

constexpr std::array<const char*, 3> kNativeGlxNames = {
    "libGLX.so.1", // GLVND GLX dispatcher (glXGetProcAddress*)
    "libGLX.so.0", // older GLVND soname
    nullptr};

#endif // #ifdef _WIN32

#if GLRESOLVER_LOADER_DIAGNOSTICS

inline auto ReportFnPtrSizeMismatch(const char* where, std::size_t fnSize, std::size_t ptrSize) -> void
{
    std::fprintf(stderr, "[PlatformLoader] %s: sizeof(Fn)=%zu sizeof(void*)=%zu; cannot convert symbol/function pointer\n",
                 (where != nullptr ? where : "(unknown)"),
                 static_cast<std::size_t>(fnSize),
                 static_cast<std::size_t>(ptrSize));
}

#endif

/**
 * @brief Converts a symbol pointer (void*) into a function pointer type .
 *
 * dlsym/GetProcAddress return untyped procedure addresses (void* on POSIX, FARPROC on Windows). Converting those to function
 * pointers via reinterpret_cast is not guaranteed to be portable in ISO C++.
 *
 * This helper uses memcpy to transfer the representation into a function pointer type. This is widely supported on common
 * platforms/ABIs (and is compatible with typical POSIX dlsym usage), but it is still a best-effort technique rather than a strict
 * ISO C++ guarantee.
 * If the platform uses different sizes for data pointers and function pointers, the
 * conversion fails and returns nullptr.
 *
 * @tparam Fn Function pointer type.
 * @param symbol Symbol pointer as void*.
 * @return Function pointer or nullptr.
 */
template<typename Fn>
auto SymbolToFunction(void* symbol) -> Fn
{
    static_assert(std::is_pointer<Fn>::value, "Fn must be a pointer type");
    static_assert(std::is_function<typename std::remove_pointer<Fn>::type>::value,
                  "Fn must be a function pointer type");

    if (symbol == nullptr)
    {
        return nullptr;
    }

    if (sizeof(Fn) != sizeof(void*))
    {

#if GLRESOLVER_LOADER_DIAGNOSTICS
        ReportFnPtrSizeMismatch("SymbolToFunction", sizeof(Fn), sizeof(void*));
#endif

        return nullptr;
    }

    Fn func = nullptr;
    std::memcpy(&func, &symbol, sizeof(Fn));
    return func;
}

/**
 * @brief Converts a function pointer into a symbol pointer representation .
 *
 * The inverse of SymbolToFunction(). This is used at API boundaries where legacy
 * interfaces represent procedure addresses as void*.
 *
 * @tparam Fn Function pointer type.
 * @param func Function pointer.
 * @return Symbol pointer as void* or nullptr if not representable.
 */
template<typename Fn>
auto FunctionToSymbol(Fn func) -> void*
{
    static_assert(std::is_pointer<Fn>::value, "Fn must be a pointer type");
    static_assert(std::is_function<typename std::remove_pointer<Fn>::type>::value,
                  "Fn must be a function pointer type");

    if (func == nullptr)
    {
        return nullptr;
    }

    if (sizeof(Fn) != sizeof(void*))
    {

#if GLRESOLVER_LOADER_DIAGNOSTICS
        ReportFnPtrSizeMismatch("FunctionToSymbol", sizeof(Fn), sizeof(void*));
#endif

        return nullptr;
    }

    void* symbol = nullptr;
    std::memcpy(&symbol, &func, sizeof(void*));
    return symbol;
}

/**
 * @brief Converts a function pointer into an integer representation .
 *
 * Useful for validating platform-specific sentinel values (e.g. Windows WGL).
 * Returns 0 if the conversion is not representable.
 */
template<typename Fn>
auto FunctionToInteger(Fn func) -> std::uintptr_t
{
    static_assert(std::is_pointer<Fn>::value, "Fn must be a pointer type");
    static_assert(std::is_function<typename std::remove_pointer<Fn>::type>::value,
                  "Fn must be a function pointer type");

    if (func == nullptr)
    {
        return 0;
    }

    if (sizeof(Fn) != sizeof(void*))
    {

#if GLRESOLVER_LOADER_DIAGNOSTICS
        ReportFnPtrSizeMismatch("FunctionToInteger", sizeof(Fn), sizeof(void*));
#endif

        return 0;
    }

    if (sizeof(std::uintptr_t) != sizeof(void*))
    {
        return 0;
    }

    std::uintptr_t value = 0;
    std::memcpy(&value, &func, sizeof(std::uintptr_t));
    return value;
}

/**
 * @brief Reads a bool-ish feature flag from an environment variable. Truthy: 1, y, yes, t, true, on (case-insensitive). Falsy: 0, n, no, f, false, off.
 *
 * Interprets the environment variable named @p name as a boolean value using a
 * simple, case-insensitive prefix check:
 * - **true** if the first character is one of: `1`, `y`, `t` (e.g. "1", "yes", "true")
 * - **false** if the first character is one of: `0`, `n`, `f` (e.g. "0", "no", "false")
 * - Special-case: values beginning with **"on"** -> true, **"off"** -> false (case-insensitive)
 *
 * If @p name is null, the variable is not set, or the value is empty/unknown,
 * the function returns @p defaultValue.
 *
 * @param name Environment variable name.
 * @param defaultValue Value to return when the variable is missing, empty, or unrecognized.
 * @return Parsed boolean value, or @p defaultValue if not determinable.
 */
auto EnvFlagEnabled(const char* name, bool defaultValue) -> bool;

/**
 * @brief Reads feature flag from PROJECTM_GLRESOLVER_TRACE_LOGGING environment variable.
 *
 * @return true if trace logging is enabled.
 */
auto EnableGLResolverTraceLogging() -> bool;

#ifdef _WIN32

/**
 * @brief Converts a Windows FARPROC into the generic symbol representation (void*) .
 *
 * Windows GetProcAddress returns a function pointer type (FARPROC). To avoid relying on a direct
 * reinterpret_cast between function pointers and object pointers, this helper copies the bit pattern
 * into a void* storage location when the sizes match.
 */
inline auto WinProcToSymbol(FARPROC proc) noexcept -> void*
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

#endif

#ifdef __EMSCRIPTEN__

// -------------------------------------------------------------------------
// Emscripten stub implementation
// -------------------------------------------------------------------------
class DynamicLibrary
{
public:
    DynamicLibrary() = default;
    ~DynamicLibrary() = default;

    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    DynamicLibrary(DynamicLibrary&&) noexcept = default;
    DynamicLibrary& operator=(DynamicLibrary&&) noexcept = default;

    inline auto Open(const char* const*, std::string&) -> bool
    {
        return false;
    }

    inline auto Close() -> void
    {
    }

    inline auto IsOpen() const -> bool
    {
        return false;
    }

    inline auto GetSymbol(const char*) const -> void*
    {
        return nullptr;
    }

    inline auto LoadedName() const -> const std::string&
    {
        static const std::string empty;
        return empty;
    }

    inline auto Handle() const -> void*
    {
        return nullptr;
    }

    inline auto SetCloseOnDestruct(bool) -> void
    {
    }

    static inline auto FindGlobalSymbol(const char*) -> void*
    {
        return nullptr;
    }
};

#else // #ifdef __EMSCRIPTEN__

// -------------------------------------------------------------------------
// Native implementation (Windows / POSIX)
// -------------------------------------------------------------------------

/**
 * @brief Wrapper around a dynamic library handle. Once opened, the library needs to be closed by calling Close() if needed, it is not closed automatically by destruction.
 */
class DynamicLibrary
{
public:
    DynamicLibrary() = default;

    ~DynamicLibrary();

    DynamicLibrary(const DynamicLibrary&) = delete;

    auto operator=(const DynamicLibrary&) -> DynamicLibrary& = delete;

    DynamicLibrary(DynamicLibrary&& other) noexcept;

    auto operator=(DynamicLibrary&& other) noexcept -> DynamicLibrary&;

    /**
     * @brief Attempts to open the first library from the given candidate list.
     * @param names Null-terminated list of candidate library names.
     * @param reason User error message.
     *
     * @return true if a library was opened, false otherwise.
     */
    auto Open(const char* const* names, std::string& reason) -> bool;

    /**
     * @brief Closes the library if it is open.
     */
    void Close();

    /**
     * @brief Returns true if the library handle is valid.
     */
    [[nodiscard]] auto IsOpen() const -> bool
    {
        return m_handle != nullptr;
    }

    /**
     * @brief Returns the name of the successfully opened library (empty if none).
     */
    [[nodiscard]] auto LoadedName() const -> const std::string&
    {
        return m_loadedName;
    }

    /**
     * @brief Returns the raw library handle.
     */
    [[nodiscard]] auto Handle() const -> LibHandle
    {
        return m_handle;
    }

    /**
     * @brief Controls whether the library is closed in the destructor.
     *
     * The default is false to avoid unloading GL/driver libraries during process teardown.
     * Enable only for short-lived helper loads where unloading is safe and desired.
     */
    auto SetCloseOnDestruct(bool enabled) -> void
    {
        m_closeOnDestruct = enabled;
    }

    /**
     * @brief Resolves a symbol from this library.
     * @param name The symbol name.
     *
     * @return Symbol address or nullptr.
     */
    auto GetSymbol(const char* name) const -> void*;

    /**
     * @brief Looks up a symbol in the global process scope.
     * @param name Symbol name.
     *
     * @return Symbol address or nullptr.
     */
    static auto FindGlobalSymbol(const char* name) -> void*;

private:
    LibHandle m_handle{};          //!< Library handle used to access the system library.
    std::string m_loadedName;      //!< Successfully opened library name.
    bool m_closeOnDestruct{false}; //!< If true, Close() is called from the destructor.
};

/**
 * @brief Retrieves a function pointer from a dynamic library or the global symbol table.
 *
 * Attempts to resolve a symbol named @p funcName as a function of type @p Fn.
 * If the library is open, it first searches the library itself; if that fails,
 * it falls back to searching the global symbol table.
 *
 * When the symbol is found, it is converted to the requested function type using
 * SymbolToFunction(). If the conversion fails or the symbol cannot be located,
 * a descriptive error message may be written to @p reason.
 *
 * @tparam Fn Function pointer type to convert the resolved symbol to.
 *
 * @param lib Dynamic library to search for the symbol.
 * @param funcName Name of the symbol to resolve.
 * @param reason Output parameter populated with a failure description when resolution fails.
 *
 * @return A function pointer of type @p Fn if the symbol is found and successfully converted;
 *         otherwise, nullptr.
 */
template<typename Fn>
auto GetProcAddressFromLibraryFallback(const DynamicLibrary& lib, const char* funcName, std::string& reason) -> Fn
{
    void* sym = nullptr;
    if (lib.IsOpen())
    {
        sym = lib.GetSymbol(funcName);
    }
    if (sym == nullptr)
    {
        sym = DynamicLibrary::FindGlobalSymbol(funcName);
    }
    if (sym != nullptr)
    {
        auto typed = SymbolToFunction<Fn>(sym);
        if (typed == nullptr)
        {
            reason = std::string(funcName) + " found in " + lib.LoadedName() + " but could not be converted to a function pointer";
        }
        return typed;
    }

    if (lib.IsOpen())
    {
        reason = std::string(funcName) + " not found (" + lib.LoadedName() + " loaded but missing symbol)";
    }

    return nullptr;
}

/**
 * @brief Resolve a function pointer by searching up to two already-open dynamic libraries, then falling back
 *        to a global symbol lookup.
 *
 * This helper attempts to locate a symbol by name and return it as a strongly-typed function pointer
 * of type @p Fn.
 *
 * Lookup order:
 * 1. If @p lib1 is open, query @p funcName1 and (optionally) @p funcName2 from @p lib1.
 * 2. If not found and @p lib2 is open, query @p funcName1 and (optionally) @p funcName2 from @p lib2.
 * 3. If still not found, attempt a process-wide (global) lookup via DynamicLibrary::FindGlobalSymbol()
 *    for @p funcName1 and (optionally) @p funcName2. This is useful when the host process has already
 *    loaded a relevant library (e.g., GLX/EGL) and exported the symbol globally.
 *
 * If a symbol is found but cannot be converted to a function pointer of type @p Fn (via SymbolToFunction),
 * the function returns nullptr and updates @p reason accordingly.
 *
 * @tparam Fn Function pointer type to convert the resolved symbol to.
 *
 * @param lib1        Primary dynamic library handle to search (preferred when available).
 * @param lib2        Secondary dynamic library handle to search if not found in @p lib1.
 * @param funcName1   Primary symbol name to resolve. Must not be nullptr.
 * @param funcName2   Optional alternate symbol name to try if @p funcName1 is not found (may be nullptr).
 * @param reason      Output string describing why resolution failed. On success, the content is unchanged.
 *
 * @return A function pointer of type @p Fn if resolution and conversion succeed; otherwise nullptr.
 *
 * @note If @p funcName1 is nullptr, this function returns nullptr immediately and does not modify @p reason.
 */
template<typename Fn>
auto GetProcAddressFromLibrariesFallback(const DynamicLibrary& lib1, const DynamicLibrary& lib2, const char* funcName1, const char* funcName2, std::string& reason) -> Fn
{
    void* sym = nullptr;

    if (funcName1 == nullptr)
    {
        return nullptr;
    }

    // Prefer explicit library handles when we have them.
    if (lib1.IsOpen())
    {
        sym = lib1.GetSymbol(funcName1);
        if (sym == nullptr && funcName2 != nullptr)
        {
            sym = lib1.GetSymbol(funcName2);
        }
    }
    if (sym == nullptr && lib2.IsOpen())
    {
        sym = lib2.GetSymbol(funcName1);
        if (sym == nullptr && funcName2 != nullptr)
        {
            sym = lib2.GetSymbol(funcName2);
        }
    }

    // Finally, try global lookup.
    if (sym == nullptr)
    {
        sym = DynamicLibrary::FindGlobalSymbol(funcName1);
        if (sym == nullptr && funcName2 != nullptr)
        {
            sym = DynamicLibrary::FindGlobalSymbol(funcName2);
        }
    }

    if (sym != nullptr)
    {
        Fn typed = SymbolToFunction<Fn>(sym);
        if (typed == nullptr)
        {
            reason = std::string(funcName1);
            if (funcName2 != nullptr)
            {
                reason += " / ";
                reason += funcName2;
            }
            reason += " found but could not be converted to a function pointer";
        }
        return typed;
    }

    std::string openLibs;
    if (lib1.IsOpen())
    {
        openLibs = lib1.LoadedName();
    }
    if (lib2.IsOpen())
    {
        if (!openLibs.empty())
        {
            openLibs += " / ";
        }
        openLibs += lib2.LoadedName();
    }
    reason = std::string(funcName1);
    if (funcName2 != nullptr)
    {
        reason += " / ";
        reason += funcName2;
    }
    if (!openLibs.empty())
    {
        reason += " not found (" + openLibs + " loaded but missing symbol)";
    }
    return nullptr;
}

#endif // #ifdef __EMSCRIPTEN__ #else

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

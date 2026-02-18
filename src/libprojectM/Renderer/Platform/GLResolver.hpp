#pragma once

#include "DynamicLibrary.hpp"

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace libprojectM {
namespace Renderer {
namespace Platform {

/**
 * @brief Backend describing which API/provider the current context appears to be using.
 */
enum class Backend : std::uint8_t
{
    /**
     * Backend detection was not successful or no specific loader is needed (e.g. Apple).
     */
    None = 0,

    /**
     * Detected EGL backend (EGL-based context; used for desktop GL via EGL or for GLES/ANGLE, depending on the build and context).
     */
    EGL = 1,

    /**
     * Detected GLX backend (GL build only).
     */
    GLX = 2,

    /**
     * Detected WGL backend (GL build only).
     */
    WGL = 3,

    /**
     * WebGL proc resolver (Emscripten only).
     */
    WebGL = 4,

    /**
     * MacOS native CGL.
     */
    CGL = 5
};

/**
 * @brief Optional user resolver callback.
 *
 * If provided, it is consulted first when resolving procedure addresses.
 * Return nullptr to allow the resolver to continue probing.
 */
using UserResolver = void* (*) (const char* name, void* userData);

/**
 * @brief Universal cross-platform runtime GL/GLES procedure resolver.
 *
 * @note GL API (either GL or GLES) is selected at compile-time.
 * @note Supported backends/wrappers: EGL (including ANGLE), GLX (including libGLVND), WGL,
 *       macOS CGL, WebGL (Emscripten), plus an optional user resolver.
 *
 *
 * Lifecycle:
 *
 *  - The resolver is a process-singleton.
 *
 *  - GL libraries that have been opened are not unloaded to avoid conflicts with the host app.
 *    They will be released by OS during process tear-down.
 *
 *
 * Initialization:
 *
 *  - Must be called after a context is created and made current on the calling thread.
 *
 *  - Thread-safe; intended to be called during startup before any resolution occurs.
 *
 *  - If multiple backends appear to be current, the resolver applies a platform policy (EGL-preferred on most platforms; CGL-preferred on macOS by default when CGL is current).
 *
 *
 * Environment variables:
 *
 *
 * - PROJECTM_GLRESOLVER_DYLIB_DIR=path
 *     macOS-only: if dlopen() fails for a *bare* library name (no path separators and no @rpath/@loader_path tokens),
 *     retry dlopen() with @p path prepended. This is intended as an escape hatch for app-bundle deployments where
 *     @rpath-based discovery is not sufficient; prefer LC_RPATH/install-name based deployment when possible.
 *
 *  - PROJECTM_GLRESOLVER_MACOS_PREFER_CGL=0
 *      On macOS, prefer CGL when both EGL and CGL appear current. Default is 1 (prefer CGL).
 *
 *  - PROJECTM_GLRESOLVER_STRICT_CONTEXT_GATE=0
 *      Disable the per-call check that the detected backend is still current when resolving symbols.
 *      Default is 1 (enabled). Useful for debugging unusual context switching in host apps.
 *
 *  - PROJECTM_GLRESOLVER_TRACE_LOGGING=0
 *      Enable trace logging for resolver details.
 *      Default is 1 (enabled). Useful for debugging the resolver path.
 *
 *  - PROJECTM_GLRESOLVER_EGL_ALLOW_CORE_GETPROCADDRESS_FALLBACK=1
 *      Enable a non-portable fallback that asks eglGetProcAddress() for core (non-extension) "gl*"
 *      entry points when EGL_KHR_get_all_proc_addresses / EGL_KHR_client_get_all_proc_addresses
 *      is not advertised. Default is 0 (disabled).
 *
 * - PROJECTM_GLRESOLVER_GLX_ALLOW_CORE_GETPROCADDRESS_FALLBACK=1
 *     Enable GLX fallback for resolving non-extension gl* names via glXGetProcAddress*.
 *     Disabled by default.
 *
 *
 * Compile-time switches:
 *
 * - GLRESOLVER_ALLOW_UNSAFE_DLL_SEARCH=1 (legacy Windows only)
 *     If the OS loader does not support LOAD_LIBRARY_SEARCH_* flags (ERROR_INVALID_PARAMETER),
 *     this loader tries to load from explicit safe locations (application directory and
 *     System32 for known system DLLs).
 *     As a last resort, some applications may still want to fall back to LoadLibrary(name)
 *     (which can consult legacy search paths such as the process current working directory).
 *     This is disabled by default for security hardening.
 *
 * - GLRESOLVER_LOADER_DIAGNOSTICS=1
 *     When enabled, the loader prints diagnostics for unusual ABI situations. Default is disabled.
 *
 * - USE_GLES
 *   Switch for compile-time API selection:
 *
 *   - If USE_GLES is defined, loads OpenGL ES entry points and
 *     expects a current EGL+GLES or WebGL context.
 *
 *   - Otherwise, loads desktop OpenGL entry points and
 *     expects a current desktop GL context (WGL/GLX/CGL or EGL+GL).
 *
 *
 * Resolution order (non-Emscripten):
 *
 *  1) User resolver callback (if provided, resolution continues if user provider returns a nullptr)
 *
 *  2) Backend provider:
 *      - EGL: eglGetProcAddress
 *        - Queried for all symbols only when EGL_KHR_get_all_proc_addresses or
 *          EGL_KHR_client_get_all_proc_addresses is advertised.
 *        - Otherwise queried only for extension-style names during the provider step.
 *      - GLX: glXGetProcAddressARB / glXGetProcAddress
 *        - Queried only for glX* or extension-style names.
 *      - WGL: wglGetProcAddress
 *        - Filters sentinel values; prefers exported symbols for core OpenGL 1.1 entry points.
 *
 *  3) Global symbol scope lookup (dlsym(RTLD_DEFAULT) / GetProcAddress on already-loaded modules)
 *
 *  4) Direct exports from explicitly opened libraries (EGL/GL/GLX)
 *
 *  5) GetProcAddress fallback
 *     - EGL: Try to resolve function via eglGetProcAddress as fallback.
 *            Optional, enabled via PROJECTM_GLRESOLVER_EGL_ALLOW_CORE_GETPROCADDRESS_FALLBACK.
 *     - GLX: Try to resolve function via glXGetProcAddress as fallback.
 *            Optional, enabled via PROJECTM_GLRESOLVER_GLX_ALLOW_CORE_GETPROCADDRESS_FALLBACK.
 *
 * Resolution order (Emscripten/WebGL):
 *
 *  1) User resolver callback (if provided)
 *
 *  2) emscripten_webgl2_get_proc_address / emscripten_webgl_get_proc_address
 *     (prefers the current context major version)
 *
 * ## Loader Flow (non-Emscripten)
 *
 * GLResolver::Initialize(userResolver?, userData?)
 *   |
 *   +-- OpenNativeLibraries()
 *   |     +-- open EGL library
 *   |     |     Windows:  libEGL.dll | EGL.dll
 *   |     |     macOS:    @rpath/libEGL.dylib | @rpath/libEGL.1.dylib | libEGL.dylib | libEGL.1.dylib | EGL
 *   |     |     Linux:    libEGL.so.1 | libEGL.so
 *   |     |     Android:  libEGL.so
 *   |     +-- open GL library
 *   |     |     Desktop GL build:
 *   |     |       Windows: opengl32.dll
 *   |     |       macOS:   /System/Library/Frameworks/OpenGL.framework/OpenGL
 *   |     |       Linux:   libOpenGL.so.* (GLVND) | libGL.so.* (legacy/compat)
 *   |     |     GLES build:
 *   |     |       Windows: libGLESv3.dll | GLESv3.dll | libGLESv2.dll | GLESv2.dll
 *   |     |       macOS:   @rpath/libGLESv3.dylib | @rpath/libGLESv2.dylib | libGLESv3.dylib | libGLESv2.dylib
 *   |     |       Linux:   libGLESv3.so.* | libGLESv2.so.*
 *   |     |       Android: libGLESv3.so/libGLESv2.so
 *   |     +-- open GLX library (Linux/Unix)
 *   |           libGLX.so.*
 *   |
 *   +-- ResolveProviderFunctions()
 *   |     +-- EGL: eglGetProcAddress, eglGetCurrentContext, and probe
 *   |     |       EGL_KHR_*_get_all_proc_addresses via eglQueryString
 *   |     +-- WGL (Windows): wglGetProcAddress, wglGetCurrentContext
 *   |     +-- CGL (macOS): CGLGetCurrentContext
 *   |     +-- GLX (Linux/Unix): glXGetProcAddress* and glXGetCurrentContext
 *   |
 *   +-- ProbeCurrentContext()                        (uses the resolved *GetCurrentContext)
 *   |
 *   +-- HasCurrentContext()                          (fails if no current context is present)
 *   |
 *   +-- DetectBackend()                              (prefers EGL when available; macOS: if both EGL and CGL appear current, prefers EGL unless eglGetProcAddress is unavailable and PROJECTM_GLRESOLVER_MACOS_PREFER_CGL=1)
 *
 * ## GetProcAddress Flow
 *
 * GLResolver::GetProcAddress(name)
 *   |
 *   +-- Validate if the currently-bound context matches the detected backend.
 *   |
 *   +-- User resolver (if any)
 *   |
 *   +-- Provider getProcAddress (backend-aware; preferred for extensions / GLVND dispatch):
 *   |     EGL  -> eglGetProcAddress (used for extension-style names unless EGL_KHR_*_get_all_proc_addresses is advertised)
 *   |     GLX  -> glXGetProcAddress* (used for extension-style names only by default; see PROJECTM_GLRESOLVER_GLX_ALLOW_CORE_GETPROCADDRESS_FALLBACK)
 *   |     WGL  -> wglGetProcAddress (filters sentinel return values; prefers opengl32.dll exports for core OpenGL 1.1 entry points)
 *   |     WebGL-> emscripten_webgl*_get_proc_address (Emscripten only; tries WebGL2 first when the current context is >= 2)
 *   |
 *   +-- If Emscripten -> All available lookups have been tried -> Not Found (nullptr)
 *   |
 *   +-- DynamicLibrary::FindGlobalSymbol(name)
 *   |
 *   +-- Direct exports from explicitly opened libraries (if any):
 *   |     m_eglLib.GetSymbol(name)
 *   |     m_glLib.GetSymbol(name)
 *   |     m_glxLib.GetSymbol(name)
 *   |
 *   +-- Late provider fallbacks (opt-in; for stacks that do not export some core symbols):
 *   |     EGL  -> eglGetProcAddress(core gl*) when PROJECTM_GLRESOLVER_EGL_ALLOW_CORE_GETPROCADDRESS_FALLBACK=1
 *   |     GLX  -> glXGetProcAddress*(core gl*) when PROJECTM_GLRESOLVER_GLX_ALLOW_CORE_GETPROCADDRESS_FALLBACK=1
 *   |
 *  \/
 * Not Found (nullptr)
 */
class GLResolver
{
public:
    GLResolver() = default;

    /**
     * Process-lifetime singleton: destructor runs during process teardown.
     * Do not call into GL or unload GL driver libraries here:
     *  - GL context may already be destroyed on this thread.
     *  - Other threads may still be running.
     * OS will reclaim mappings on exit.
     */
    ~GLResolver() = default;

    GLResolver(const GLResolver&) = delete;
    auto operator=(const GLResolver&) -> GLResolver& = delete;
    GLResolver(GLResolver&&) = delete;
    auto operator=(GLResolver&&) -> GLResolver& = delete;

    /**
     * @brief Returns the process-wide resolver instance.
     */
    static auto Instance() -> GLResolver&;

    /**
     * @brief Initializes the resolver.
     *
     * This method must be called at least once before GetProcAddress() is used.
     *
     * Thread-safety: Initialize() is internally synchronized. Intended usage is to initialize
     * once during startup before any resolution occurs. GetProcAddress() should not be called until Initialize() has completed.
     *
     * May be called multiple times; initialization work is performed only when needed.
     *
     * @param resolver Optional user resolver callback.
     * @param userData Optional user pointer passed to resolver.
     * @return true if GLAD successfully loaded a backend, false otherwise.
     */
    auto Initialize(UserResolver resolver = nullptr, void* userData = nullptr) -> bool;

    /**
     * @brief Returns true if the resolver was successfully initialized.
     */
    auto IsInitialized() const -> bool;

    /**
     * @brief Returns the backend detected during the last successful Initialize() call.
     */
    auto CurrentBackend() const -> Backend;

    /**
     * @brief Returns true if a user resolver is configured.
     */
    auto HasUserResolver() const -> bool;

    /**
     * @brief Resolves a function pointer by consulting all sources in priority order.
     *
     * @note Intended usage assumes Initialize() has completed successfully before this is called.
     *
     * @note GLX policy: some implementations (notably Mesa/GLVND setups) may return a non-null
     *       pointer for unknown names from glXGetProcAddress*. To reduce the risk of calling
     *       invalid stubs, this resolver only consults glXGetProcAddress* for GLX entry points
     *       (glX*) and extension-style symbols. Desktop core GL symbols are resolved via direct
     *       exports (dlsym / libOpenGL / libGL) through the later lookup path.
     *
     * @param name Function name.
     * @return Procedure address or nullptr.
     */
    auto GetProcAddress(const char* name) const -> void*;

    /**
     * @brief Resolves a function pointer by consulting all sources in priority order from a static context.
     *
     * @param name Function name.
     * @return Procedure address or nullptr.
     */
    static auto GetProcAddressThunk(const char* name) -> void*;

    /**
     * @brief Verify that the resolver is ready to use from the current thread.
     *
     * @param reason Return user readable reason.
     * @return true if the resolver can be used from the current thread.
     */
    auto VerifyBeforeUse(std::string& reason) const -> bool;

private:
    // Basic EGL access signatures.

    using EglProc = void(PLATFORM_EGLAPIENTRY*)();
    using EglGetProcAddressFn = EglProc(PLATFORM_EGLAPIENTRY*)(const char* name);
    using EglGetCurrentContextFn = void*(PLATFORM_EGLAPIENTRY*) ();

#ifdef _WIN32

    // Basic WGL access signatures.

    using WglGetProcAddressFn = PROC(WINAPI*)(LPCSTR);
    using WglGetCurrentContextFn = HGLRC(WINAPI*)();

#elif defined(__APPLE__)

    // Basic CGL access signatures.

    using CglGetCurrentContextFn = void* (*) ();

#elif !defined(__ANDROID__)

    // Basic GLX access signatures.

    /**
     * glXGetProcAddress/glXGetProcAddressARB return a function pointer.
    */
    using GlxGetProcAddressFn = void (*(*) (const unsigned char*) )();
    using GlxGetCurrentContextFn = void* (*) ();

#endif

    /**
     * Current GL context probe results.
     */
    struct CurrentContextProbe {
        bool eglLibOpened{false};               //!< True if an EGL library was opened.
        bool eglAvailable{false};               //!< True if EGL entry points were resolved.
        bool eglCurrent{false};                 //!< True if an EGL context appears current.
        bool eglGetProcAddressAvailable{false}; //!< True if eglGetProcAddress is available (non-null).

        bool glxLibOpened{false}; //!< True if a GLX library was opened.
        bool glxAvailable{false}; //!< True if GLX entry points were resolved.
        bool glxCurrent{false};   //!< True if a GLX context appears current.

        bool wglLibOpened{false}; //!< True if a WGL library was opened.
        bool wglAvailable{false}; //!< True if WGL entry points were resolved.
        bool wglCurrent{false};   //!< True if a WGL context appears current.

        bool cglLibOpened{false};
        bool cglAvailable{false}; //!< True if CGL entry points were resolved.
        bool cglCurrent{false};   //!< True if a CGL context appears current.

        bool webglAvailable{false}; //!< True if WebGL entry points were resolved.
        bool webglCurrent{false};   //!< True if a WebGL context appears current.
    };

    /**
     * All values needed for the resolver are encapsulated in this struct to keep them immutable later.
     */
    struct ResolverState {
        Backend m_backend{Backend::None}; //!< Detected GL backend.

        UserResolver m_userResolver{nullptr}; //!< User provided function resolver. Optional, may be null.
        void* m_userData{nullptr};            //!< User data to pass to user provided function resolver.

        DynamicLibrary m_eglLib; //!< EGL library handle. Optional, may be empty.
        DynamicLibrary m_glLib;  //!< GL library handle. Optional, may be empty.
        DynamicLibrary m_glxLib; //!< GLX library handle. Optional, may be empty.

        EglGetProcAddressFn m_eglGetProcAddress{nullptr};       //!< eglGetProcAddress handle.
        bool m_eglGetAllProcAddresses{false};                   //!< True if EGL_KHR_get_all_proc_addresses (or client variant) is advertised.
        EglGetCurrentContextFn m_eglGetCurrentContext{nullptr}; //!< eglGetCurrentContext handle.

#ifdef _WIN32

        WglGetProcAddressFn m_wglGetProcAddress{nullptr};       //!< wglGetProcAddress handle.
        WglGetCurrentContextFn m_wglGetCurrentContext{nullptr}; //!< wglGetCurrentContext handle.

#elif defined(__APPLE__)

        CglGetCurrentContextFn m_cglGetCurrentContext{nullptr}; //!< CGLGetCurrentContext handle.

#elif !defined(__ANDROID__)

        GlxGetProcAddressFn m_glxGetProcAddress{nullptr};       //!< glXGetProcAddress* handle.
        GlxGetCurrentContextFn m_glxGetCurrentContext{nullptr}; //!< glXGetCurrentContext handle.

#else

#endif

    }; // struct ResolverState

    // use static methods to prohibit access to members, force state constness
    static auto OpenNativeLibraries(ResolverState& state) -> void;
    static auto ResolveProviderFunctions(ResolverState& state) -> void;
    static auto ProbeCurrentContext(const ResolverState& state) -> CurrentContextProbe;
    static auto HasCurrentContext(const CurrentContextProbe& probe, std::string& outReason) -> bool;
    static auto DetectBackend(const CurrentContextProbe& probe) -> Backend;
    static auto VerifyBackendIsCurrent(Backend backend, const CurrentContextProbe& currentContext) -> bool;
    static auto VerifyBeforeUse(const ResolverState& state, std::string& reason) -> bool;
    static auto ResolveProcAddress(const ResolverState& state, const char* name) -> void*;

    mutable std::mutex m_mutex;               //!< Mutex to synchronize initialization and access.
    bool m_loaded{false};                     //!< True if the resolver is initialized.
    bool m_initializing{false};               //!< True while an Initialize() attempt is in-flight.
    mutable std::condition_variable m_initCv; //!< Signals completion of Initialize().

    std::shared_ptr<const ResolverState> m_state; //!< Resolver properties and resolved pointers.
};

/**
 * @brief Converts a Backend enum value to a human-readable string.
 *
 * @param backend Backend enum value.
 * @return Constant string representation of the backend.
 */
inline auto BackendToString(Backend backend) -> const char*
{
    switch (backend)
    {
        case Backend::None: {
            return "None";
        }
        case Backend::EGL: {
            return "EGL";
        }
        case Backend::GLX: {
            return "GLX";
        }
        case Backend::WGL: {
            return "WGL";
        }
        case Backend::WebGL: {
            return "WebGL";
        }
        case Backend::CGL: {
            return "CGL";
        }
        default: {
            return "Unknown";
        }
    }
}

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

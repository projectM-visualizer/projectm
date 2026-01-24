#pragma once

#include "PlatformGLContextCheck.hpp"
#include "PlatformLoader.hpp"

#include <condition_variable>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

// Optional GLX fallback for resolving non-extension gl* names via glXGetProcAddress*.
//
// Default behavior is conservative: only extension-style names are resolved via
// glXGetProcAddress*, because some implementations return non-null for unknown
// symbols which may crash when called.
//
// If support for legacy/non-GLVND stacks is required where some core entry points are not
// exported from libGL/libOpenGL, you may enable this as a last-resort fallback.
#ifndef PLATFORM_GLX_ALLOW_CORE_GETPROCADDRESS_FALLBACK
#define PLATFORM_GLX_ALLOW_CORE_GETPROCADDRESS_FALLBACK 0
#endif

namespace libprojectM
{
namespace Renderer
{
namespace Platform
{

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
using UserResolver = void* (*)(const char* name, void* userData);

/**
 * @brief Universal cross-platform runtime GL/GLES procedure resolver for GLAD2 (non-MX).
 *
 * Compile-time API selection:
 *  - If USE_GLES is defined, loads OpenGL ES entry points via gladLoadGLES2() and
 *    expects a current EGL+GLES or WebGL context.
 *  - Otherwise, loads desktop OpenGL entry points via gladLoadGL() and expects a
 *    current desktop GL context (WGL/GLX/CGL or EGL+GL).
 *
 * Supported backends/wrappers: EGL (including ANGLE), GLX (including libGLVND), WGL,
 * macOS CGL, WebGL (Emscripten), plus an optional user resolver.
 *
 * Lifecycle:
 *  - The resolver is a process-singleton.
 *  - GL libraries that have been opened are not unloaded to avoid conflicts with the host app.
 *    They will be released by OS during process tear-down.
 *
 * Initialization:
 *  - Must be called after a context is created and made current on the calling thread.
 *  - Thread-safe; intended to be called during startup before any resolution occurs.
 *  - If multiple backends appear to be current, EGL is preferred.
 *
 * Resolution order (non-Emscripten):
 *  1) User resolver callback (if provided, resolution continues if user provider returns a nullptr)
 *  2) Backend provider:
 *      - EGL: eglGetProcAddress
 *        - Queried for all symbols only when EGL_KHR_get_all_proc_addresses or
 *          EGL_KHR_client_get_all_proc_addresses is advertised.
 *        - Otherwise queried only for extension-style names during the provider step.
 *      - GLX: glXGetProcAddressARB / glXGetProcAddress
 *        - Queried only for glX* or extension-style names.
 *      - WGL: wglGetProcAddress
 *        - Filters sentinel values; prefers exported symbols for core OpenGL 1.1 entry points.
 *  3) Global symbol scope lookup (dlsym(RTLD_DEFAULT) / GetProcAddress on already-loaded modules)
 *  4) Direct exports from explicitly opened libraries (EGL/GL/GLX)
 *  5) GetProcAddress fallback
 *     - EGL: Try to resolve function via eglGetProcAddress as fallback.
 *            Always enabled.
 *     - GLX: Try to resolve function via glXGetProcAddress as fallback.
 *            Optional, enabled via PLATFORM_GLX_ALLOW_CORE_GETPROCADDRESS_FALLBACK.
 *
 * Resolution order (Emscripten/WebGL):
 *  1) User resolver callback (if provided)
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
 *   |     |     macOS:    libEGL.dylib | libEGL.1.dylib | EGL
 *   |     |     Linux:    libEGL.so.1 | libEGL.so
 *   |     |     Android:  libEGL.so
 *   |     +-- open GL library
 *   |     |     Desktop GL build:
 *   |     |       Windows: opengl32.dll
 *   |     |       macOS:   OpenGL.framework/OpenGL
 *   |     |       Linux:   libGL.so.* and/or libOpenGL.so.* (GLVND)
 *   |     |     GLES build:
 *   |     |       Windows: libGLESv3.dll/libGLESv2.dll
 *   |     |       macOS:   libGLESv3.dylib/libGLESv2.dylib
 *   |     |       Linux:   libGLESv3.so.* and/or libGLESv2.so.*
 *   |     |       Android: libGLESv3.so/libGLESv2.so
 *   |     +-- open GLX library (Linux/Unix, best-effort)
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
 *   +-- DetectBackend()                              (prefers EGL, then WGL/CGL/GLX depending on platform)
 *   |
 *   +-- gladLoadGL(...) / gladLoadGLES2(...)         (GLAD calls back into GLResolver::GetProcAddress)
 *   |
 *   +-- CheckGLRequirements()
 *   |     Desktop GL: OpenGL    >= 3.3
 *   |     GLES:       OpenGL ES >= 3.0
 *   |
 *   +-- if ready:
 *         SOIL_GL_SetResolver(&GLResolver::GladResolverThunk)
 *         SOIL_GL_Init()
 *
 * ## GetProcAddress Flow
 *
 * GLResolver::GetProcAddress(name)
 *   |
 *   +-- Validate if the currently-bound context matches the detected backend.
 *   |
 *   +-- User resolver (if any)
 *   |
 *   +-- Provider getProcAddress (backend-aware)
 *   |     EGL  -> eglGetProcAddress (extension-only unless EGL_KHR_*_get_all_proc_addresses)
 *   |     GLX  -> glXGetProcAddress* (glX* and extension-style only)
 *   |     WGL  -> wglGetProcAddress (filters sentinels; may prefer exported symbols)
 *   |     WebGL-> emscripten_webgl*_get_proc_address (Emscripten only)
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
 *   +-- Late best-effort eglGetProcAddress fallback (EGL only)
 *   |                    glXGetProcAddress fallback (GLX only, disabled per default)
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
    auto IsLoaded() const -> bool;

    /**
     * @brief Returns the backend detected during the last successful Initialize() call.
     *
     */
    auto CurrentBackend() const -> Backend;

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
    static auto GladResolverThunk(const char* name) -> void*;

private:

    // Basic EGL access signatures.

    using EglProc = void (PLATFORM_EGLAPIENTRY*)();
    using EglGetProcAddressFn = EglProc (PLATFORM_EGLAPIENTRY*)(const char* name);
    using EglGetCurrentContextFn = void* (PLATFORM_EGLAPIENTRY*)();

#ifdef _WIN32

    // Basic WGL access signatures.

    using WglGetProcAddressFn = PROC (WINAPI*)(LPCSTR);
    using WglGetCurrentContextFn = HGLRC (WINAPI*)();

#elif defined(__APPLE__)

    // Basic CGL access signatures.

    using CglGetCurrentContextFn = void* (*)();

#elif !defined(__ANDROID__)

    // Basic GLX access signatures.

    /**
     * glXGetProcAddress/glXGetProcAddressARB return a function pointer.
    */
    using GlxGetProcAddressFn = void (*(*)(const unsigned char*))();
    using GlxGetCurrentContextFn = void* (*)();

#endif

    /**
     * Current GL context probe results.
     */
    struct CurrentContextProbe
    {
        bool eglLibOpened{false};
        bool eglAvailable{false};
        bool eglCurrent{false};

        bool glxLibOpened{false};
        bool glxAvailable{false};
        bool glxCurrent{false};

        bool wglLibOpened{false};
        bool wglAvailable{false};
        bool wglCurrent{false};

        bool cglLibOpened{false};
        bool cglAvailable{false};
        bool cglCurrent{false};

        bool webglAvailable{false};
        bool webglCurrent{false};
    };

    /**
     * All values needed for the resolver are encapsulated in this struct, so they can be copied easily.
     */
    struct ResolverState
    {
        Backend m_backend{ Backend::None };                             //!< Detected GL backend.

        UserResolver m_userResolver{nullptr};                           //!< User provided function resolver. Optional, may be null.
        void* m_userData{nullptr};                                      //!< User data to pass to user provided function resolver.

        EglGetProcAddressFn m_eglGetProcAddress{nullptr};               //!< eglGetProcAddress handle.
        bool m_eglGetAllProcAddresses{false};                           //!< True if EGL_KHR_get_all_proc_addresses (or client variant) is advertised.
        EglGetCurrentContextFn m_eglGetCurrentContext{nullptr};         //!< eglGetCurrentContext handle.

#ifdef _WIN32

        WglGetProcAddressFn m_wglGetProcAddress{nullptr};               //!< wglGetProcAddress handle.
        WglGetCurrentContextFn m_wglGetCurrentContext{nullptr};         //!< wglGetCurrentContext handle.

#elif defined(__APPLE__)

        CglGetCurrentContextFn m_cglGetCurrentContext{nullptr};         //!< CGLGetCurrentContext handle.

#elif !defined(__ANDROID__)

        GlxGetProcAddressFn m_glxGetProcAddress{nullptr};               //!< glXGetProcAddress* handle.
        GlxGetCurrentContextFn m_glxGetCurrentContext{nullptr};         //!< glXGetCurrentContext handle.

#else

#endif

    }; // struct ResolverState

    auto OpenNativeLibraries() -> void;
    auto ResolveProviderFunctions() -> void;
    auto ProbeCurrentContext() const -> CurrentContextProbe;
    auto HasCurrentContext(const CurrentContextProbe& probe, std::string& outReason) -> bool;
    auto DetectBackend(const CurrentContextProbe& probe) -> void;
    auto CheckGLRequirementsUnlocked() -> GLContextCheckResult;
    bool VerifyBackendIsCurrent(const ResolverState& state, const CurrentContextProbe& currentContext) const;
    auto static LoadGladUnlocked(const ResolverState& state) -> bool;
    static auto ResolveUnlocked(const char* name, const ResolverState& state) -> void*;

    mutable std::mutex m_mutex;                                     //!< Mutex to synchronize initialization and access.
    bool m_loaded{false};                                           //!< True if the resolver is initialized.
    bool m_initializing{false};                                     //!< True while an Initialize() attempt is in-flight.
    mutable std::condition_variable m_initCv;                       //!< Signals completion of Initialize().

    DynamicLibrary m_eglLib;                                        //!< EGL library handle. Optional, may be empty.
    DynamicLibrary m_glLib;                                         //!< GL library handle. Optional, may be empty.
    DynamicLibrary m_glxLib;                                        //!< GLX library handle. Optional, may be empty.

    ResolverState m_state;                                          //!< Resolver properties and resolved pointers.
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
        case Backend::None:
        {
            return "None";
        }
        case Backend::EGL:
        {
            return "EGL";
        }
        case Backend::GLX:
        {
            return "GLX";
        }
        case Backend::WGL:
        {
            return "WGL";
        }
        case Backend::WebGL:
        {
            return "WebGL";
        }
        case Backend::CGL:
        {
            return "CGL";
        }
        default:
        {
            return "Unknown";
        }
    }
}

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

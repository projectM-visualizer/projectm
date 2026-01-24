
#include "PlatformGLResolver.hpp"

#include "OpenGL.h"
#include "PlatformGLContextCheck.hpp"

#include "SOIL2/SOIL2.h"
#include "SOIL2/SOIL2_gl_bridge.h"

#include <Logging.hpp>

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <cstdlib>
#include <limits>
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#include <emscripten/html5_webgl.h>
#endif

namespace libprojectM {
namespace Renderer {
namespace Platform {

namespace {

/**
 * @brief Checks whether a space-separated token list contains an exact token match.
 *
 * This utility is intended for parsing OpenGL/EGL/GLX extension strings, which are
 * specified as a sequence of extension names separated by ASCII spaces. The match
 * is performed on whole tokens only (not substrings) to avoid false positives
 * (e.g. @c "FOO" will not match @c "FOOBAR").
 *
 * The scan skips over consecutive spaces and compares each token's length and
 * contents against @p token.
 *
 * @param list  NUL-terminated string containing space-separated tokens (may be nullptr).
 * @param token NUL-terminated token to search for (may be nullptr). An empty token
 *              never matches.
 *
 * @return true if @p list contains a token exactly equal to @p token; false otherwise.
 *
 * @note This function treats only the space character (' ') as a separator, matching
 *       the common extension-string format used by OpenGL-family APIs.
 * @note Comparison is case-sensitive.
 * @warning The inputs must be valid NUL-terminated strings when non-null.
 */
auto HasSpaceSeparatedToken(const char* list, const char* token) -> bool
{
    if (list == nullptr || token == nullptr)
    {
        return false;
    }

    const std::size_t tokenLen = std::strlen(token);
    if (tokenLen == 0u)
    {
        return false;
    }

    // Extension strings are defined as space-separated tokens.
    // Match whole tokens only to avoid false positives (e.g. "FOO" matching "FOOBAR").
    const char* p = list;
    while (*p != 0)
    {
        while (*p == ' ')
        {
            ++p;
        }
        if (*p == 0)
        {
            break;
        }

        const char* start = p;
        while (*p != 0 && *p != ' ')
        {
            ++p;
        }
        const auto len = static_cast<std::size_t>(p - start);
        if (len == tokenLen && std::strncmp(start, token, tokenLen) == 0)
        {
            return true;
        }
    }

    return false;
}

#ifndef __EMSCRIPTEN__

/**
 * @brief Heuristically determines whether a name looks like an OpenGL-style extension symbol.
 *
 * OpenGL-family extension entry points commonly end with a vendor or standards-body
 * suffix (e.g. @c ARB, @c EXT, @c KHR, @c OES, @c NV). This function performs a
 * lightweight suffix check against a curated set of common suffixes and returns
 * true if any suffix matches the end of @p name.
 *
 * This is a heuristic used to decide whether an entry point is *likely* to be an
 * extension name (and therefore safe/appropriate to resolve using mechanisms like
 * @c wglGetProcAddress / @c glXGetProcAddress / @c eglGetProcAddress in some
 * backend policies).
 *
 * @param name NUL-terminated symbol name to test. Must not be nullptr.
 *
 * @return true if @p name ends with one of the known extension suffixes; false otherwise.
 *
 * @note Comparison is case-sensitive and expects the conventional uppercase suffixes.
 * @warning Passing nullptr is undefined behavior (the function calls @c std::strlen(name)).
 */
auto IsLikelyExtensionName(const char* name) -> bool
{
    // Common extension suffixes.
    static constexpr std::array<const char*, 32> kSuffixes = {
        "ARB", "EXT", "KHR", "OES", "NV", "NVX", "AMD", "APPLE",
        "ANGLE", "INTEL", "MESA", "QCOM", "IMG", "ARM", "ATI", "IBM",
        "SUN", "SGI", "SGIX", "OML", "GREMEDY", "HP", "3DFX", "S3",
        "PVR", "VIV", "OVR", "NOK", "MSFT", "SEC", "DMP", "FJ"};

    const auto len = std::strlen(name);
    for (const char* suffix : kSuffixes)
    {
        const auto slen = std::strlen(suffix);
        if (len >= slen && std::strcmp(name + (len - slen), suffix) == 0)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Returns true if a GL/EGL symbol name is likely to be an extension entry point.
 */
auto ShouldUseEglGetProcAddressForName(const char* name) -> bool
{
    if (name == nullptr)
    {
        return false;
    }

    return IsLikelyExtensionName(name);
}

#endif // __EMSCRIPTEN__

#if !defined(_WIN32) && !defined(__APPLE__) && !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)

/**
 * @brief Returns true if a GL symbol name is likely to be an extension or GLX entry point.
 *
 * GLX loaders can return a non-null pointer for unknown functions. A conservative
 * mitigation is to only accept glXGetProcAddress* results for names that appear to
 * be extension entry points (ARB/EXT/KHR/etc.), while resolving core entry points
 * via direct exports (dlsym / libOpenGL/libGL).
 */
auto ShouldUseGlxGetProcAddressForName(const char* name) -> bool
{
    if (name == nullptr)
    {
        return false;
    }

    // GLX entry points themselves are always resolved via GLX libraries.
    if (std::strncmp(name, "glX", 3) == 0)
    {
        return true;
    }

    if (IsLikelyExtensionName(name))
    {
        return true;
    }

    return false;
}

#endif

} // anonymous namespace


auto GLResolver::Instance() -> GLResolver&
{
    static GLResolver instance;
    return instance;
}

auto GLResolver::CheckGLRequirementsUnlocked() -> GLContextCheckResult
{
    GLContextCheck::Builder glCheck;

#ifdef USE_GLES

    glCheck
        .WithApi(GLApi::OpenGLES)
        .WithMinimumVersion(3, 0)
        .WithRequireCoreProfile(false);

#else

    glCheck
        .WithApi(GLApi::OpenGL)
        .WithMinimumVersion(3, 3)
        // Accept both core and compatibility contexts. A 3.3+ compatibility context is a valid
        // configuration on many drivers/stacks, and profile filtering would reject it unnecessarily.
        .WithRequireCoreProfile(false);

#endif

    return glCheck.Check();
}

auto GLResolver::Initialize(UserResolver resolver, void* userData) -> bool
{

    // Prevent concurrent Initialize()
    std::unique_lock<std::mutex> lock(m_mutex);

    if (m_loaded)
    {
        return true;
    }

    m_initCv.wait(lock, [this]() { return !m_initializing; });

    if (m_loaded)
    {
        return true;
    }

    m_initializing = true;

    m_state.m_userResolver = resolver;
    m_state.m_userData = userData;

    // Find source for gl functions.
#ifndef __EMSCRIPTEN__

    OpenNativeLibraries();
    ResolveProviderFunctions();

#endif

    // Try to find a current gl context.
    auto currentContext = ProbeCurrentContext();

    // Precondition: caller must have a current context on this thread.
    {
        std::string reason;
        if (!HasCurrentContext(currentContext, reason))
        {
            m_loaded = false;
            m_state.m_backend = Backend::None;

            m_initializing = false;
            m_initCv.notify_all();
            lock.unlock();

            LOG_ERROR(std::string("[GLResolver] No current GL context present: ") + reason);
            return false;
        }
    }

    // Determine backend from current context.
    DetectBackend(currentContext);

    // Emit a diagnostics line for troubleshooting.
    {
        std::string diag = std::string("[GLResolver] Resolver policy: backend=\"") +
                           BackendToString(m_state.m_backend) + "\""
#ifndef __EMSCRIPTEN__

                           +
                           " egl=\"" + m_eglLib.LoadedName() + "\"" +
                           " gl=\"" + m_glLib.LoadedName() + "\"" +
                           " glx=\"" + m_glxLib.LoadedName() + "\"" +
                           " egl_get_proc=\"" + (m_state.m_eglGetProcAddress != nullptr ? "yes" : "no") + "\"" +
                           " egl_all_proc=\"" + (m_state.m_eglGetAllProcAddresses ? "yes" : "no") + "\""

#endif
            ;

#ifdef _WIN32

        diag += std::string(" wgl_get_proc=\"") + (m_state.m_wglGetProcAddress != nullptr ? "yes" : "no") + "\"";

#elif !defined(__APPLE__) && !defined(__ANDROID__)

        diag += std::string(" glx_get_proc=\"") + (m_state.m_glxGetProcAddress != nullptr ? "yes" : "no") + "\"";

#if PLATFORM_GLX_ALLOW_CORE_GETPROCADDRESS_FALLBACK

        diag += " glx_policy=\"ext+fallback\"";

#else

        diag += " glx_policy=\"ext-only\"";

#endif

#endif

        diag += std::string(" user_resolver=\"") + (m_state.m_userResolver != nullptr ? "yes" : "no") + "\"";

        LOG_DEBUG(diag);
    }

    // The process needs to yield a backend. Unlikely to happen if a context was current.
    if (m_state.m_backend == Backend::None)
    {
        m_loaded = false;
        m_initializing = false;
        m_initCv.notify_all();
        lock.unlock();

        LOG_ERROR(std::string("[GLResolver] Could not select a valid backend."));
        return false;
    }

    // Do not hold m_mutex while calling into GLAD.
    lock.unlock();
    const bool loaded = LoadGladUnlocked(m_state);
    lock.lock();

    if (!loaded)
    {
        m_loaded = false;
    }
    else
    {
        lock.unlock();
        const auto glDetails = CheckGLRequirementsUnlocked();
        lock.lock();

        LOG_INFO(std::string("[GLResolver] GL Info: ") +
                 GLContextCheck::FormatCompactLine(glDetails.info) +
                 " backend=\"" + BackendToString(m_state.m_backend) + "\"" +
                 " user_resolver=\"" + (m_state.m_userResolver != nullptr ? "yes" : "no") + "\"");

        if (!glDetails.success)
        {
            LOG_ERROR(std::string("[GLResolver] GL requirements check failed: ") + glDetails.reason);
        }

        lock.unlock();

        const bool ready = glDetails.success;
        if (ready)
        {
            SOIL_GL_SetResolver(&GLResolver::GladResolverThunk);
            SOIL_GL_Init();
        }

        lock.lock();
        m_loaded = ready;
    }

    m_initializing = false;
    m_initCv.notify_all();
    lock.unlock();

    return m_loaded;
}

auto GLResolver::IsLoaded() const -> bool
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    return m_loaded;
}

auto GLResolver::CurrentBackend() const -> Backend
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    return m_state.m_backend;
}

bool GLResolver::VerifyBackendIsCurrent(const ResolverState& state, const CurrentContextProbe& currentContext) const
{
    switch (state.m_backend)
    {
        case Backend::CGL:
            if (!currentContext.cglCurrent)
            {
                LOG_ERROR(std::string("[GLResolver] CGL context not available."));
                return false;
            }
            break;
        case Backend::EGL:
            if (!currentContext.eglCurrent)
            {
                LOG_ERROR(std::string("[GLResolver] EGL context not available."));
                return false;
            }
            break;
        case Backend::GLX:
            if (!currentContext.glxCurrent)
            {
                LOG_ERROR(std::string("[GLResolver] GLX context not available."));
                return false;
            }
            break;
        case Backend::WGL:
            if (!currentContext.wglCurrent)
            {
                LOG_ERROR(std::string("[GLResolver] WGL context not available."));
                return false;
            }
            break;
        case Backend::WebGL:
            if (!currentContext.webglCurrent)
            {
                LOG_ERROR(std::string("[GLResolver] WebGL context not available."));
                return false;
            }
            break;
        default:
            LOG_ERROR(std::string("[GLResolver] Initialization did not choose a valid backend."));
            return false;
    }
    return true;
}

auto GLResolver::GetProcAddress(const char* name) const -> void*
{
    if (name == nullptr)
    {
        return nullptr;
    }

    // Avoid holding m_mutex while calling user callbacks or driver/loader code.
    // Only hold m_mutex while reading internal state and probing the current context.
    std::unique_lock<std::mutex> lock(m_mutex);

    if (!m_loaded && !m_initializing)
    {
        LOG_ERROR(std::string("[GLResolver] GetProcAddress called without initialization."));
        return nullptr;
    }

    // Local state copy to use once the lock is released.
    const auto state = m_state;

    // Gate to detected backend.
    const auto currentContext = ProbeCurrentContext();

    lock.unlock();

    if (!VerifyBackendIsCurrent(state, currentContext))
    {
        return nullptr;
    }

    void* resolved = ResolveUnlocked(name, state);

    if (resolved != nullptr)
    {
        return resolved;
    }

#ifndef __EMSCRIPTEN__

    // Global symbol table (works if the process already linked/loaded GL libs).
    // NOTE: After initialization completes, native libraries are not mutated; m_mutex not needed here.
    void* global = DynamicLibrary::FindGlobalSymbol(name);
    if (global != nullptr)
    {
        return global;
    }

    // Direct library symbol lookup.
    if (m_eglLib.IsOpen())
    {
        void* ptr = m_eglLib.GetSymbol(name);
        if (ptr != nullptr)
        {
            return ptr;
        }
    }
    if (m_glLib.IsOpen())
    {
        void* ptr = m_glLib.GetSymbol(name);
        if (ptr != nullptr)
        {
            return ptr;
        }
    }
    if (m_glxLib.IsOpen())
    {
        void* ptr = m_glxLib.GetSymbol(name);
        if (ptr != nullptr)
        {
            return ptr;
        }
    }

#if !defined(_WIN32) && !defined(__ANDROID__) && !defined(__APPLE__)

#if PLATFORM_GLX_ALLOW_CORE_GETPROCADDRESS_FALLBACK

    // Optional GLX fallback for non-extension names:
    // Some stacks require glXGetProcAddress* even for core entry points, but some
    // implementations return non-null for unknown symbols. This is OFF by default.
    if ((state.m_backend == Backend::GLX || state.m_backend == Backend::None) &&
        state.m_glxGetProcAddress != nullptr &&
        !ShouldUseGlxGetProcAddressForName(name))
    {
        auto proc = state.m_glxGetProcAddress(reinterpret_cast<const unsigned char*>(name));
        if (proc != nullptr)
        {
            return FunctionToSymbol(proc);
        }
    }

#endif

#endif

    // Pragmatic EGL fallback: some providers expose core client API entry points only via
    // eglGetProcAddress despite not advertising EGL_KHR_*get_all_proc_addresses. The spec
    // does not require this behavior, so we keep a strict policy first and only try this
    // after all export/global lookups fail.
    if ((state.m_backend == Backend::EGL || state.m_backend == Backend::None) &&
        state.m_eglGetProcAddress != nullptr &&
        !state.m_eglGetAllProcAddresses &&
        !ShouldUseEglGetProcAddressForName(name))
    {
        const EglProc proc = state.m_eglGetProcAddress(name);
        if (proc != nullptr)
        {
            return FunctionToSymbol(proc);
        }
    }

#endif

    return nullptr;
}

void GLResolver::OpenNativeLibraries()
{
    // Best-effort: macOS or minimal EGL setups may fail to open.
    std::string reason;

#ifdef _WIN32

    static constexpr std::array<const char*, 3> kEglNames = {"libEGL.dll", "EGL.dll", nullptr};

    static constexpr std::array<const char*, 2> kGlDesktopNames = {"opengl32.dll", nullptr};
    static constexpr std::array<const char*, 5> kGlGlesNames = {"libGLESv3.dll", "GLESv3.dll", "libGLESv2.dll", "GLESv2.dll", nullptr};

#elif defined(__APPLE__)

    // macOS native OpenGL uses CGL (OpenGL.framework). ANGLE (and other portability layers).
    // commonly provide EGL/GLES dylibs in the application bundle / @rpath.
    static constexpr std::array<const char*, 4> kEglNames = {"libEGL.dylib", "libEGL.1.dylib", "EGL", nullptr};

    static constexpr std::array<const char*, 2> kGlCglNames = {
        "/System/Library/Frameworks/OpenGL.framework/OpenGL",
        nullptr};

    static constexpr std::array<const char*, 3> kGlGlesNames = {
        "libGLESv3.dylib",
        "libGLESv2.dylib",
        nullptr};

#elif defined(__ANDROID__)

    // Android uses EGL + GLES (no desktop libGL / GLX).
    static constexpr std::array<const char*, 2> kEglNames = {"libEGL.so", nullptr};
    static constexpr std::array<const char*, 3> kGlNames = {"libGLESv3.so", "libGLESv2.so", nullptr};

#else

    static constexpr std::array<const char*, 3> kEglNames = {"libEGL.so.1", "libEGL.so", nullptr};

#if defined(USE_GLES)

    // Linux / GLES:
    // Prefer libGLESv3/libGLESv2 sonames. Core GLES entry points are expected
    // to be available as library exports. eglGetProcAddress is not guaranteed
    // to return core symbols unless EGL_KHR_get_all_proc_addresses (or its
    // client variant) is advertised.
    static constexpr std::array<const char*, 6> kGlNames = {
        "libGLESv3.so.3",
        "libGLESv3.so",
        "libGLESv2.so.2",
        "libGLESv2.so.1",
        "libGLESv2.so",
        nullptr};

#else

    static constexpr std::array<const char*, 6> kGlNames = {
        "libOpenGL.so.1", // GLVND OpenGL dispatcher (core gl* entry points)
        "libOpenGL.so.0", // older GLVND soname
        "libGL.so.1",     // legacy/compat umbrella (often provided by GLVND)
        "libGL.so.0",     // sometimes shipped as .so.0
        "libGL.so",
        nullptr};

#endif

    // Linux / GLVND note:
    // Some environments (especially minimal/container) may not ship libGL.so.1 but do ship GLVND libs.
    // Keep legacy libGL first for backwards compatibility, but fall back to GLVND-facing libs if needed.
    static constexpr std::array<const char*, 3> kGlxNames = {
        "libGLX.so.1", // GLVND GLX dispatcher (glXGetProcAddress*)
        "libGLX.so.0", // older GLVND soname
        nullptr};
    reason = "";
    const bool glxOpened = m_glxLib.Open(kGlxNames.data(), reason);
    if (!glxOpened)
    {
        LOG_DEBUG(std::string("[GLResolver] Failed to open GLX library: ") + reason);
    }

#endif

    reason = "";
    const bool eglOpened = m_eglLib.Open(kEglNames.data(), reason);
    if (!eglOpened)
    {
        LOG_DEBUG(std::string("[GLResolver] Failed to open EGL library: ") + reason);
    }

#ifdef _WIN32

#if defined(USE_GLES)

    const auto* glNames = kGlGlesNames.data();

#else

    // Desktop OpenGL build: always prefer opengl32.dll.
    // Note: ANGLE typically exposes GLES via EGL, and is expected to be used with USE_GLES builds.
    const auto* glNames = kGlDesktopNames.data();

#endif

    reason = "";
    const bool glOpened = m_glLib.Open(glNames, reason);

#elif defined(__APPLE__)

#if defined(USE_GLES)

    const auto* glNames = kGlGlesNames.data();

#else

    // Desktop OpenGL build: always prefer OpenGL.framework.
    // EGL/GLES stacks (e.g. ANGLE) are expected to be used with USE_GLES builds.
    const auto* glNames = kGlCglNames.data();

#endif

    reason = "";
    const bool glOpened = m_glLib.Open(glNames, reason);

#else

    reason = "";
    const bool glOpened = m_glLib.Open(kGlNames.data(), reason);

#endif

    if (!glOpened)
    {
        LOG_DEBUG(std::string("[GLResolver] Failed to open GL library: ") + reason);
    }
}

void GLResolver::ResolveProviderFunctions()
{
    // EGL
    {
        // Note: eglGetProcAddress is the canonical mechanism for resolving EGL/GLES entry points,
        // but some older EGL implementations/spec interpretations may not return addresses for all
        // core functions. This resolver therefore also falls back to global and direct library
        // exports in GetProcAddress().
        void* sym = nullptr;
        if (m_eglLib.IsOpen())
        {
            sym = m_eglLib.GetSymbol("eglGetProcAddress");
        }
        if (sym == nullptr)
        {
            sym = DynamicLibrary::FindGlobalSymbol("eglGetProcAddress");
        }
        if (sym != nullptr)
        {
            m_state.m_eglGetProcAddress = SymbolToFunction<EglGetProcAddressFn>(sym);
            if (m_state.m_eglGetProcAddress == nullptr)
            {
                LOG_DEBUG("[GLResolver] eglGetProcAddress found but could not be converted to a function pointer");
            }
        }
        else if (m_eglLib.IsOpen())
        {
            LOG_DEBUG("[GLResolver] eglGetProcAddress not found (EGL loaded but missing symbol)");
        }
    }
    // eglGetCurrentContext (used for backend probing)
    {
        void* sym = nullptr;
        if (m_eglLib.IsOpen())
        {
            sym = m_eglLib.GetSymbol("eglGetCurrentContext");
        }
        if (sym == nullptr)
        {
            sym = DynamicLibrary::FindGlobalSymbol("eglGetCurrentContext");
        }
        m_state.m_eglGetCurrentContext = SymbolToFunction<EglGetCurrentContextFn>(sym);
    }

    // Detect EGL_KHR_get_all_proc_addresses / EGL_KHR_client_get_all_proc_addresses.
    // When advertised, eglGetProcAddress is allowed to return addresses for all client API functions,
    // including core entry points.
    // - Client extension: query via eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS)
    // - Display extension: query via eglQueryString(current_display, EGL_EXTENSIONS)
    // See: Khronos registry extension text.
    {
        m_state.m_eglGetAllProcAddresses = false;

        // Avoid depending on platform EGL headers here; use opaque handles and well-known constants.
        // Note: on 32-bit Windows, EGL entry points use stdcall (EGLAPIENTRY).
        using EglDisplay = void*;
        using EglQueryStringFn = const char*(PLATFORM_EGLAPIENTRY*) (EglDisplay, int);
        using EglGetCurrentDisplayFn = EglDisplay(PLATFORM_EGLAPIENTRY*)();
        using EglGetErrorFn = int(PLATFORM_EGLAPIENTRY*)();

        constexpr int kEglExtensions = 0x3055; // EGL_EXTENSIONS
        constexpr int kEglSuccess = 0x3000;    // EGL_SUCCESS
        constexpr int kEglBadDisplay = 0x3008; // EGL_BAD_DISPLAY
        EglDisplay kEglNoDisplay = nullptr;    // EGL_NO_DISPLAY

        void* querySym = nullptr;
        if (m_eglLib.IsOpen())
        {
            querySym = m_eglLib.GetSymbol("eglQueryString");
        }
        if (querySym == nullptr)
        {
            querySym = DynamicLibrary::FindGlobalSymbol("eglQueryString");
        }

        auto eglQueryStringFn = SymbolToFunction<EglQueryStringFn>(querySym);
        if (eglQueryStringFn != nullptr)
        {
            const char* clientExt = eglQueryStringFn(kEglNoDisplay, kEglExtensions);
            if (clientExt != nullptr)
            {
                // Client extensions are only queryable when EGL_EXT_client_extensions is supported.
                // When available, EGL_KHR_client_get_all_proc_addresses implies that eglGetProcAddress
                // may return addresses for all client API functions (including core entry points).
                if (HasSpaceSeparatedToken(clientExt, "EGL_KHR_client_get_all_proc_addresses"))
                {
                    m_state.m_eglGetAllProcAddresses = true;
                }
            }
            else
            {
                // If EGL_EXT_client_extensions is not supported, this call is expected to fail.
                // Best-effort: drain eglGetError() for deterministic debug logs.
                void* errSym = nullptr;
                if (m_eglLib.IsOpen())
                {
                    errSym = m_eglLib.GetSymbol("eglGetError");
                }
                if (errSym == nullptr)
                {
                    errSym = DynamicLibrary::FindGlobalSymbol("eglGetError");
                }
                auto eglGetErrorFn = SymbolToFunction<EglGetErrorFn>(errSym);
                if (eglGetErrorFn != nullptr)
                {
                    const int err = eglGetErrorFn();
                    if (err != kEglSuccess && err != kEglBadDisplay)
                    {
                        char hexBuf[16] = {};
                        std::snprintf(hexBuf, sizeof(hexBuf), "%x", static_cast<unsigned int>(err));
                        LOG_DEBUG(std::string("[GLResolver] eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS) failed with EGL error=0x") + hexBuf);
                    }
                }
            }

            // Display extension: requires a display. Best-effort: use eglGetCurrentDisplay if available.
            void* dpySym = nullptr;
            if (m_eglLib.IsOpen())
            {
                dpySym = m_eglLib.GetSymbol("eglGetCurrentDisplay");
            }
            if (dpySym == nullptr)
            {
                dpySym = DynamicLibrary::FindGlobalSymbol("eglGetCurrentDisplay");
            }

            auto eglGetCurrentDisplayFn = SymbolToFunction<EglGetCurrentDisplayFn>(dpySym);
            if (eglGetCurrentDisplayFn != nullptr)
            {
                EglDisplay dpy = eglGetCurrentDisplayFn();
                if (dpy != nullptr)
                {
                    const char* displayExt = eglQueryStringFn(dpy, kEglExtensions);
                    if (displayExt != nullptr)
                    {
                        if (HasSpaceSeparatedToken(displayExt, "EGL_KHR_get_all_proc_addresses"))
                        {
                            m_state.m_eglGetAllProcAddresses = true;
                        }
                    }
                }
            }
        }

        LOG_DEBUG(std::string("[GLResolver] EGL get_all_proc_addresses=") + (m_state.m_eglGetAllProcAddresses ? "yes" : "no"));
    }

#ifdef _WIN32
    // WGL
    {
        void* sym = nullptr;
        if (m_glLib.IsOpen())
        {
            sym = m_glLib.GetSymbol("wglGetProcAddress");
        }
        if (sym == nullptr)
        {
            sym = DynamicLibrary::FindGlobalSymbol("wglGetProcAddress");
        }
        if (sym != nullptr)
        {
            m_state.m_wglGetProcAddress = SymbolToFunction<WglGetProcAddressFn>(sym);
            if (m_state.m_wglGetProcAddress == nullptr)
            {
                LOG_DEBUG("[GLResolver] wglGetProcAddress found but could not be converted to a function pointer");
            }
        }
        else if (m_glLib.IsOpen())
        {
            LOG_DEBUG("[GLResolver] wglGetProcAddress not found (GL library loaded but missing symbol)");
        }
    }
    {
        void* sym = nullptr;
        if (m_glLib.IsOpen())
        {
            sym = m_glLib.GetSymbol("wglGetCurrentContext");
        }
        if (sym == nullptr)
        {
            sym = DynamicLibrary::FindGlobalSymbol("wglGetCurrentContext");
        }
        if (sym != nullptr)
        {
            m_state.m_wglGetCurrentContext = SymbolToFunction<WglGetCurrentContextFn>(sym);
            if (m_state.m_wglGetCurrentContext == nullptr)
            {
                LOG_DEBUG("[GLResolver] m_wglGetCurrentContextAddress found but could not be converted to a function pointer");
            }
        }
    }
#elif defined(__APPLE__)
    using CglGetCurrentContextFn = void* (*) ();

    void* sym = nullptr;
    if (m_glLib.IsOpen())
    {
        sym = m_glLib.GetSymbol("CGLGetCurrentContext");
    }
    if (sym == nullptr)
    {
        sym = DynamicLibrary::FindGlobalSymbol("CGLGetCurrentContext");
    }

    m_state.m_cglGetCurrentContext = SymbolToFunction<CglGetCurrentContextFn>(sym);
#elif !defined(__ANDROID__)
    // GLX
    {
        void* sym = nullptr;

        // Prefer explicit library handles when we have them.
        if (m_glxLib.IsOpen())
        {
            sym = m_glxLib.GetSymbol("glXGetProcAddressARB");
            if (sym == nullptr)
            {
                sym = m_glxLib.GetSymbol("glXGetProcAddress");
            }
        }
        if (sym == nullptr && m_glLib.IsOpen())
        {
            sym = m_glLib.GetSymbol("glXGetProcAddressARB");
            if (sym == nullptr)
            {
                sym = m_glLib.GetSymbol("glXGetProcAddress");
            }
        }

        // Finally, try global lookup (host might already have GLX loaded).
        if (sym == nullptr)
        {
            sym = DynamicLibrary::FindGlobalSymbol("glXGetProcAddressARB");
            if (sym == nullptr)
            {
                sym = DynamicLibrary::FindGlobalSymbol("glXGetProcAddress");
            }
        }

        if (sym != nullptr)
        {
            m_state.m_glxGetProcAddress = SymbolToFunction<GlxGetProcAddressFn>(sym);
            if (m_state.m_glxGetProcAddress == nullptr)
            {
                LOG_DEBUG("[GLResolver] glXGetProcAddress* found but could not be converted to a function pointer");
            }
        }
        else if (m_glxLib.IsOpen() || m_glLib.IsOpen())
        {
            LOG_DEBUG("[GLResolver] glXGetProcAddress* not found (GLX/GL loaded but missing symbol)");
        }
    }
    {
        void* sym = nullptr;
        if (m_glxLib.IsOpen())
        {
            sym = m_glxLib.GetSymbol("glXGetCurrentContext");
        }
        if (sym == nullptr && m_glLib.IsOpen())
        {
            sym = m_glLib.GetSymbol("glXGetCurrentContext");
        }
        if (sym == nullptr)
        {
            // If GLX is already loaded/linked by the host, probe via global symbols (GLVND setups).
            sym = DynamicLibrary::FindGlobalSymbol("glXGetCurrentContext");
        }

        m_state.m_glxGetCurrentContext = SymbolToFunction<GlxGetCurrentContextFn>(sym);
    }
#endif

    LOG_DEBUG(std::string("[GLResolver] EGL  handle=") +
              std::to_string(reinterpret_cast<std::uintptr_t>(m_eglLib.Handle())) +
              " lib=\"" + m_eglLib.LoadedName() + "\"");

    LOG_DEBUG(std::string("[GLResolver] GL   handle=") +
              std::to_string(reinterpret_cast<std::uintptr_t>(m_glLib.Handle())) +
              " lib=\"" + m_glLib.LoadedName() + "\"");

    LOG_DEBUG(std::string("[GLResolver] GLX  handle=") +
              std::to_string(reinterpret_cast<std::uintptr_t>(m_glxLib.Handle())) +
              " lib=\"" + m_glxLib.LoadedName() + "\"");
}

auto GLResolver::ProbeCurrentContext() const -> CurrentContextProbe
{
    CurrentContextProbe result;

#ifdef __EMSCRIPTEN__

    result.webglAvailable = true;

    const EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_get_current_context();

    result.webglCurrent = ctx != 0;

    return result;

#else

    // EGL (including ANGLE, GLES, and desktop EGL)
    result.eglLibOpened = m_eglLib.IsOpen();

    if (m_state.m_eglGetCurrentContext != nullptr)
    {
        result.eglAvailable = true;
        result.eglCurrent = m_state.m_eglGetCurrentContext() != nullptr;
    }

#ifdef _WIN32

    // WGL
    result.wglLibOpened = m_glLib.IsOpen();

    if (m_state.m_wglGetCurrentContext != nullptr)
    {
        result.wglAvailable = true;
        result.wglCurrent = m_state.m_wglGetCurrentContext() != nullptr;
    }

#elif defined(__APPLE__)

    // CGL (macOS native OpenGL)
    result.cglLibOpened = m_glLib.IsOpen();

    if (m_state.m_cglGetCurrentContext != nullptr)
    {
        result.cglAvailable = true;
        result.cglCurrent = m_state.m_cglGetCurrentContext() != nullptr;
    }

#elif !defined(__ANDROID__)

    // GLX (Linux/Unix via libGLX/libGL or GLVND dispatch)
    result.glxLibOpened = m_glxLib.IsOpen() || m_glLib.IsOpen();

    if (m_state.m_glxGetCurrentContext != nullptr)
    {
        result.glxAvailable = true;
        result.glxCurrent = m_state.m_glxGetCurrentContext() != nullptr;
    }

#endif

    return result;

#endif // __EMSCRIPTEN__
}

auto GLResolver::HasCurrentContext(const CurrentContextProbe& probe, std::string& outReason) -> bool
{

#ifdef __EMSCRIPTEN__

    if (probe.webglCurrent)
    {
        return true;
    }
    outReason = "WebGL: no current context";
    return false;

#else

    if (probe.eglCurrent || probe.glxCurrent || probe.wglCurrent || probe.cglCurrent)
    {
        return true;
    }

    std::string reason;

    if (probe.eglAvailable)
    {
        reason += "EGL: no current context; ";
    }
    else if (probe.eglLibOpened)
    {
        reason += "EGL: eglGetCurrentContext missing; ";
    }

#if !defined(_WIN32) && !defined(__APPLE__) && !defined(__ANDROID__)

    if (probe.glxAvailable)
    {
        reason += "GLX: no current context; ";
    }
    else if (probe.glxLibOpened)
    {
        reason += "GLX: glXGetCurrentContext missing; ";
    }

#endif

#ifdef _WIN32

    if (probe.wglAvailable)
    {
        reason += "WGL: no current context; ";
    }
    else if (probe.wglLibOpened)
    {
        reason += "WGL: wglGetCurrentContext missing; ";
    }
    else
    {
        reason += "WGL: opengl32.dll not loaded; ";
    }

#endif

#if defined(__APPLE__)

    if (probe.cglAvailable)
    {
        reason += "CGL: no current context; ";
    }
    else if (probe.cglLibOpened)
    {
        reason += "CGL: CGLGetCurrentContext missing; ";
    }
    else
    {
        reason += "CGL: CGLGetCurrentContext symbol not available; ";
    }

#endif

    if (reason.empty())
    {
        // If nothing was available, libraries may not have been opened yet.
        reason = "No platform current-context query available (libraries not loaded?)";
    }
    else
    {
        // Trim trailing "; "
        if (reason.size() >= 2)
        {
            reason.resize(reason.size() - 2);
        }
    }

    outReason = reason;
    return false;

#endif // #ifdef __EMSCRIPTEN__
}

auto GLResolver::DetectBackend(const CurrentContextProbe& probe) -> void
{

#ifdef __EMSCRIPTEN__

    if (probe.webglCurrent)
    {
        m_state.m_backend = Backend::WebGL;
    }
    return;

#else

    if (probe.eglCurrent)
    {
        m_state.m_backend = Backend::EGL;
        return;
    }

#ifdef _WIN32

    if (probe.wglCurrent)
    {
        m_state.m_backend = Backend::WGL;
        return;
    }

#elif defined(__APPLE__)

    if (probe.cglCurrent)
    {
        m_state.m_backend = Backend::CGL;
        return;
    }

#elif !defined(__ANDROID__)

    if (probe.glxCurrent)
    {
        m_state.m_backend = Backend::GLX;
        return;
    }

#endif

    m_state.m_backend = Backend::None;

#endif
}


auto GLResolver::GladResolverThunk(const char* name) -> void*
{
    return Instance().GetProcAddress(name);
}

namespace {

/**
 * Resolves a GL function by name using GladResolverThunk.
 * Adapt external void* handle to GLAD type
 *
 * @param name GL function name.
 *
 * @return Function pointer as GLADapiproc.
 */
auto GladBridgeResolverThunk(const char* name) -> GLADapiproc
{
    return SymbolToFunction<GLADapiproc>(GLResolver::GladResolverThunk(name));
}

} // namespace

auto GLResolver::LoadGladUnlocked(const ResolverState& state) -> bool
{

#ifndef USE_GLES

    const int result = gladLoadGL(&GladBridgeResolverThunk);
    if (result != 0)
    {
        LOG_DEBUG("[GLResolver] gladLoadGL() succeeded");
        return true;
    }
    LOG_ERROR(std::string("[GLResolver] gladLoadGL() failed (backend=") +
              BackendToString(state.m_backend) + ")");
    return false;

#else

    const int result = gladLoadGLES2(&GladBridgeResolverThunk);
    if (result != 0)
    {
        LOG_DEBUG("[GLResolver] gladLoadGLES2() succeeded");
        return true;
    }
    LOG_ERROR(std::string("[GLResolver] gladLoadGLES2() failed (backend=") +
              BackendToString(state.m_backend) + ")");

    return false;

#endif
}

auto GLResolver::ResolveUnlocked(const char* name, const ResolverState& state) -> void*
{
    // 1) User resolver
    if (state.m_userResolver != nullptr)
    {
        void* ptr = state.m_userResolver(name, state.m_userData);
        if (ptr != nullptr)
        {
            return ptr;
        }
    }

#ifdef __EMSCRIPTEN__

    // 2) Emscripten (WebGL)
    // Try to resolve using the function appropriate for the current context version first.
    void* ptr = nullptr;

    const EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_get_current_context();
    int ctxMajor = 0;
    if (ctx != 0)
    {
        EmscriptenWebGLContextAttributes attrs;
        if (emscripten_webgl_get_context_attributes(ctx, &attrs) == EMSCRIPTEN_RESULT_SUCCESS)
        {
            ctxMajor = attrs.majorVersion;
        }
    }

    if (ctxMajor >= 2)
    {
        ptr = emscripten_webgl2_get_proc_address(name);
        if (ptr != nullptr)
        {
            return ptr;
        }
        ptr = emscripten_webgl_get_proc_address(name);
        if (ptr != nullptr)
        {
            return ptr;
        }
    }
    else
    {
        ptr = emscripten_webgl_get_proc_address(name);
        if (ptr != nullptr)
        {
            return ptr;
        }
        ptr = emscripten_webgl2_get_proc_address(name);
        if (ptr != nullptr)
        {
            return ptr;
        }
    }

    return nullptr;

#else

    // 2) Platform provider getProcAddress (preferred for extensions, GLVND dispatch)
    if ((state.m_backend == Backend::EGL || state.m_backend == Backend::None) && state.m_eglGetProcAddress != nullptr)
    {
        if (state.m_eglGetAllProcAddresses || ShouldUseEglGetProcAddressForName(name))
        {
            // eglGetProcAddress returns a function pointer type; convert only at the boundary.
            EglProc proc = state.m_eglGetProcAddress(name);
            if (proc != nullptr)
            {
                return FunctionToSymbol(proc);
            }
        }
    }

#ifndef _WIN32

#if !defined(__ANDROID__) && !defined(__APPLE__)
    if ((state.m_backend == Backend::GLX || state.m_backend == Backend::None) && state.m_glxGetProcAddress != nullptr)
    {
        // GLX policy: only accept glXGetProcAddress* results for names that
        // look like extension entry points. Core symbols are resolved via direct exports
        // (libOpenGL/libGL) through the later global/library lookup path.
        //
        // Rationale: some GLX implementations may return a non-null pointer even for
        // unsupported or unknown symbols, which can crash when called.
        // See: https://dri.freedesktop.org/wiki/glXGetProcAddressNeverReturnsNULL/
        if (ShouldUseGlxGetProcAddressForName(name))
        {
            auto proc = state.m_glxGetProcAddress(reinterpret_cast<const unsigned char*>(name));
            if (proc != nullptr)
            {
                return FunctionToSymbol(proc);
            }
        }
    }

#endif

#else // #ifndef _WIN32

    if ((state.m_backend == Backend::WGL || state.m_backend == Backend::None) && state.m_wglGetProcAddress != nullptr)
    {
        PROC proc = state.m_wglGetProcAddress(name);
        if (proc != nullptr)
        {
            // wglGetProcAddress can return special sentinel values (1,2,3,-1) for core symbols.
            // Treat those as invalid and allow fallback to GetProcAddress/dlsym paths.
            const std::uintptr_t raw = FunctionToInteger(proc);
            // FunctionToInteger returns 0 on non-representable conversions (e.g. function pointers wider than void*).
            // Treat that as invalid and allow fallback to exported symbols.
            if (raw != 0u && raw != 1u && raw != 2u && raw != 3u && raw != std::numeric_limits<std::uintptr_t>::max())
            {
                // Prefer exports from opengl32.dll for core OpenGL 1.1 entry points.
                // In the wild, wglGetProcAddress may return a non-null pointer for some core symbols
                // that is not callable, so we check the export path first.
                // (Exports will be nullptr for extension/non-exported entry points, so this is cheap.)
                void* exportPtr = DynamicLibrary::FindGlobalSymbol(name);
                if (exportPtr != nullptr)
                {
                    return exportPtr;
                }
                return FunctionToSymbol(proc);
            }
        }
    }

#endif

    return nullptr;

#endif
}

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

#pragma once

#include "PlatformLoader.h"

#include <cstdint>
#include <memory>
#include <mutex>

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
     * Detected EGL backend (GL build), or GLES backend (ENABLE_GLES=ON build).
     */
    EglGles,

    /**
     * Detected GLX backend (GL build only).
     */
    GlxGl,

    /**
     * Detected WGL backend (GL build only).
     */
    WglGl,

    /**
     * User resolver is used, no backend detection.
     */
    UserResolver
};

/**
 * @brief Optional user resolver callback.
 *
 * If provided, it is consulted first when resolving procedure addresses.
 * Return nullptr to allow the resolver to continue probing.
 */
using UserResolver = void* (*)(const char* name, void* userData);

/**
 * @brief Cross-platform runtime GL/GLES resolver.
 *
 * This resolver:
 *  - Must be initialized after a GL/GLES context has been created and made current.
 *  - Probes for EGL/GLX/WGL by checking for a current context.
 *  - Uses GLAD2 non-MX entrypoints (gladLoadGL / gladLoadGLES2) via a universal resolver.
 *  - Resolves symbols using the following order:
 *      1) User resolver callback (if any)
 *      2) Global symbol table (RTLD_DEFAULT / main module)
 *      3) eglGetProcAddress / glXGetProcAddress* / wglGetProcAddress (when available)
 *      4) Symbols from opened libEGL / libGL / opengl32
 */
class GLResolver
{
public:
    /**
     * Opaque handle for gl functions.
     */
    using GLapiproc = void*;

    GLResolver() = default;
    ~GLResolver();

    GLResolver(const GLResolver&) = delete;
    auto operator=(const GLResolver&) -> GLResolver& = delete;
    GLResolver(GLResolver&&) = delete;
    auto operator=(GLResolver&&) -> GLResolver& = delete;

    /**
     * @brief Returns the process-wide resolver instance.
     */
    static auto Instance() -> std::shared_ptr<GLResolver>;

    /**
     * @brief Initializes the resolver.
     *
     * @param resolver Optional user resolver callback.
     * @param userData Optional user pointer passed to resolver.
     * @return true if GLAD successfully loaded a backend, false otherwise.
     */
    auto Initialize(UserResolver resolver = nullptr, void* userData = nullptr) -> bool;

    /**
     * @brief Shuts down the resolver and releases library handles.
     */
    void Shutdown();

    /**
     * @brief Returns true if the resolver was successfully initialized.
     */
    auto IsLoaded() const -> bool;

    /**
     * @brief Returns the currently detected backend.
     */
    auto CurrentBackend() const -> Backend;

    /**
     * @brief Resolves a function pointer by consulting all sources in priority order.
     *
     * @param name Function name.
     * @return Procedure address or nullptr.
     */
    auto GetProcAddress(const char* name) const -> GLapiproc;

    /**
     * @brief Resolves a function pointer by consulting all sources in priority order from a static context.
     *
     * @param name Function name.
     * @return Procedure address or nullptr.
     */
    static auto GladResolverThunk(const char* name) -> GLapiproc;
private:
    /**
     * GL resolver function pointer type.
     */
    using GetProcFunc = GLapiproc (*)(const char* name);

    void OpenNativeLibraries();
    void ResolveProviderFunctions();
    void DetectBackend();
    void SetBackendDefault();
    auto LoadGlad() -> bool;
    auto Resolve(const char* name) const -> GLapiproc;

    mutable std::mutex m_mutex;                   //!< Mutex to synchronize initialization and access.
    bool m_loaded{false};                         //!< True if the resolver is initialized.
    Backend m_backend{ Backend::None };           //!< Detected GL backend.

    UserResolver m_userResolver{nullptr};         //!< User provided function resolver.
    void* m_userData{nullptr};                    //!< User data to pass to user provided function resolver.

    DynamicLibrary m_eglLib;                      //!< EGL library handle. Optional, may not be assigned.
    DynamicLibrary m_glLib;                       //!< Detected GL backend. Optional, may not be assigned.

    GetProcFunc m_eglGetProcAddress{nullptr};     //!< Function pointer to EGL proc resolver function.
    GetProcFunc m_glxGetProcAddress{nullptr};     //!< Function pointer to GLX proc resolver function.
    GetProcFunc m_wglGetProcAddress{nullptr};     //!< Function pointer to WGL proc resolver function.
};

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

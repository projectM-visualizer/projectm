// Cross-platform runtime GL/GLES loader using GLAD2 C API (non-MX).
//
// Provides a universal resolver to find GL function pointers.

#include "PlatformGLResolver.hpp"

#include "OpenGL.h"
#include "SOIL2/SOIL2_gl_bridge.h"
#include "SOIL2/SOIL2.h"

#include <Logging.hpp>

#include <array>
#include <cstdio>

namespace libprojectM {

namespace Renderer {

namespace Platform {

GLResolver::~GLResolver()
{
    // make sure handles are released
    m_eglLib.Close();
    m_glLib.Close();
}

auto GLResolver::Instance() -> std::shared_ptr<GLResolver>
{
    // observe resolver instance with a weak pointer
    // so that it is shared, but released once the last
    // client shared_ptr is released
    static std::weak_ptr<GLResolver> sharedInstance;
    auto instance = sharedInstance.lock();
    if (!instance)
    {
        instance = std::make_shared<GLResolver>();
        sharedInstance = instance;
    }
    return instance;
}

void GLResolver::SetBackendDefault()
{
    if (m_backend == Backend::None)
    {
#ifdef USE_GLES
        m_backend = Backend::EglGles;
#else

#ifdef _WIN32
        m_backend = Backend::WglGl;
#else // _WIN32
        m_backend = Backend::GlxGl;
#endif // _WIN32

#endif // USE_GLES
    }
}

auto GLResolver::Initialize(UserResolver resolver, void* userData) -> bool
{
    const std::lock_guard<std::mutex> lock(m_mutex);

    if (m_loaded)
    {
        return true;
    }

    m_userResolver = resolver;
    m_userData = userData;

    if (m_userResolver == nullptr)
    {
        // need to find source for gl functions
        // open libs and detect backend
        OpenNativeLibraries();
        ResolveProviderFunctions();
        DetectBackend();
    }
    else
    {
        // user resolver was provided
        // we *should* be able to get all gl functions from the resolver
        // using user resolver + global symbol fallback
        m_backend = Backend::UserResolver;
    }

    if (LoadGlad())
    {
        // set default in case detection failed, but loading succeeded
        SetBackendDefault();

        // init SOIL2 gl functions
        SOIL_GL_SetResolver(&GLResolver::GladResolverThunk);
        SOIL_GL_Init();

        m_loaded = true;
    }

    return m_loaded;
}

void GLResolver::Shutdown()
{
    const std::lock_guard<std::mutex> lock(m_mutex);

    m_loaded = false;
    m_backend = Backend::None;

    m_userResolver = nullptr;
    m_userData = nullptr;

    m_eglGetProcAddress = nullptr;
    m_glxGetProcAddress = nullptr;
    m_wglGetProcAddress = nullptr;

    m_eglLib.Close();
    m_glLib.Close();
}

auto GLResolver::IsLoaded() const -> bool
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    return m_loaded;
}

auto GLResolver::CurrentBackend() const -> Backend
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    return m_backend;
}

auto GLResolver::GetProcAddress(const char* name) const -> GLapiproc
{
    // NOTE: This method is used during GLAD loading. Avoid taking the mutex here to
    // prevent deadlocks if GLAD calls back into us while Initialize() is holding the lock
    return Resolve(name);
}

void GLResolver::OpenNativeLibraries()
{
    // Best-effort: macOS or minimal EGL setups may fail to open

#ifdef _WIN32
    static constexpr std::array<const char*, 3> kEglNames = {"libEGL.dll", "EGL.dll", nullptr};
    static constexpr std::array<const char*, 2> kGlNames = {"opengl32.dll", nullptr};
#elif defined(__APPLE__)
    static constexpr std::array<const char*, 2> kEglNames = {"libEGL.dylib", nullptr};
    static constexpr std::array<const char*, 2> kGlNames = {"/System/Library/Frameworks/OpenGL.framework/OpenGL", nullptr};
#else
    static constexpr std::array<const char*, 3> kEglNames = {"libEGL.so.1", "libEGL.so", nullptr};
    static constexpr std::array<const char*, 3> kGlNames = {"libGL.so.1", "libGL.so", nullptr};
#endif

    m_eglLib.Open(kEglNames.data());
    m_glLib.Open(kGlNames.data());
}

void GLResolver::ResolveProviderFunctions()
{
    // EGL
    if (m_eglLib.IsOpen())
    {
        void* sym = m_eglLib.GetSymbol("eglGetProcAddress");
        if (sym == nullptr)
        {
            sym = DynamicLibrary::FindGlobalSymbol("eglGetProcAddress");
        }
        m_eglGetProcAddress = reinterpret_cast<GetProcFunc>(sym);
    }

    // GLX / WGL
    if (m_glLib.IsOpen())
    {
#ifdef _WIN32
        void* sym = m_glLib.GetSymbol("wglGetProcAddress");
        if (!sym)
        {
            sym = DynamicLibrary::FindGlobalSymbol("wglGetProcAddress");
        }
        m_wglGetProcAddress = reinterpret_cast<GetProcFunc>(sym);
#else
        void* sym = m_glLib.GetSymbol("glXGetProcAddressARB");
        if (sym == nullptr)
        {
            sym = m_glLib.GetSymbol("glXGetProcAddress");
        }
        if (sym == nullptr)
        {
            sym = DynamicLibrary::FindGlobalSymbol("glXGetProcAddress");
        }
        m_glxGetProcAddress = reinterpret_cast<GetProcFunc>(sym);
#endif
    }

    {
        std::array<char, 256> buf{};
        std::snprintf(buf.data(), buf.size(), "[GLResolver] Library handles: egl=%p gl=%p",
            reinterpret_cast<void*>(m_eglLib.Handle()),
            reinterpret_cast<void*>(m_glLib.Handle()));
        LOG_DEBUG(std::string(buf.data()));
    }

}

void GLResolver::DetectBackend()
{
    // Detect current context provider
    // This is best-effort: on some platforms (e.g. macOS/CGL) it may report "unknown"

    const bool usingEgl = m_eglLib.IsOpen() && IsCurrentEgl(m_eglLib);

#ifndef _WIN32
    const bool usingGlx = m_glLib.IsOpen() && IsCurrentGlx(m_glLib);
#else
    const bool usingWgl = IsCurrentWgl();
#endif

    if (usingEgl)
    {
        LOG_DEBUG("[GLResolver] Current context: EGL");
        m_backend = Backend::EglGles;
        return;
    }

#ifndef _WIN32
    if (usingGlx)
    {
        LOG_DEBUG("[GLResolver] Current context: GLX");
        m_backend = Backend::GlxGl;
        return;
    }
#else
    if (usingWgl)
    {
        LOG_DEBUG("[GLResolver] Current context: WGL");
        m_backend = Backend::WglGl;
        return;
    }
#endif

    LOG_DEBUG("[GLResolver] Current context: (unknown, will try generic loader)");
    m_backend = Backend::None;
}

auto GLResolver::GladResolverThunk(const char* name) -> GLapiproc
{
    return Instance()->Resolve(name);
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
auto gladBridgeResolverThunk(const char* name) -> GLADapiproc
{
    return reinterpret_cast<GLADapiproc>(GLResolver::GladResolverThunk(name));
}

}

auto GLResolver::LoadGlad() -> bool
{
    int result = 0;

#ifndef USE_GLES
    result = gladLoadGL(&gladBridgeResolverThunk);
    if (result != 0)
    {
        LOG_DEBUG("[GLResolver] gladLoadGL() succeeded");
        return true;
    }
    LOG_FATAL("[GLResolver] gladLoadGL() failed");
    return false;
#else
    result = gladLoadGLES2(&gladBridgeResolverThunk);
    if (result != 0)
    {
        LOG_DEBUG("[GLResolver] gladLoadGLES2() succeeded");
        return true;
    }
    LOG_FATAL("[GLResolver] gladLoadGLES2() failed");
    return false;
#endif
}

auto GLResolver::Resolve(const char* name) const -> GLapiproc
{
    if (name == nullptr)
    {
        return nullptr;
    }

    // 1) User resolver
    if (m_userResolver != nullptr)
    {
        if (void* ptr = m_userResolver(name, m_userData))
        {
            return ptr;
        }
    }

    // 2) Global symbol table
    if (void* ptr = DynamicLibrary::FindGlobalSymbol(name))
    {
        return ptr;
    }

    // 3) Platform provider getProcAddress
    if (m_eglGetProcAddress != nullptr)
    {
        if (void* ptr = m_eglGetProcAddress(name))
        {
            return ptr;
        }
    }
    if (m_glxGetProcAddress != nullptr)
    {
        if (void* ptr = m_glxGetProcAddress(name))
        {
            return ptr;
        }
    }
    if (m_wglGetProcAddress != nullptr)
    {
        if (void* ptr = m_wglGetProcAddress(name))
        {
            return ptr;
        }
    }

    // 4) Direct library symbol lookup
    if (m_eglLib.IsOpen())
    {
        if (void* ptr = m_eglLib.GetSymbol(name))
        {
            return ptr;
        }
    }
    if (m_glLib.IsOpen())
    {
        if (void* ptr = m_glLib.GetSymbol(name))
        {
            return ptr;
        }
    }

    return nullptr;
}

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

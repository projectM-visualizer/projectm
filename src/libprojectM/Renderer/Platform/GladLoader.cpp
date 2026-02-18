
#include "GladLoader.hpp"

#include "../OpenGL.h"
#include "GLProbe.hpp"
#include "GLResolver.hpp"
#include "Logging.hpp"

#include <string>

namespace {

/**
 * @brief Resolves a GL function by name using GladResolverThunk.
 * @note Adapts opaque void* handle to GLAD type
 *
 * @param name GL function name.
 *
 * @return Function pointer as GLADapiproc.
 */
auto GladBridgeGetProcAddressThunk(const char* name) -> GLADapiproc
{
    return libprojectM::Renderer::Platform::SymbolToFunction<GLADapiproc>(libprojectM::Renderer::Platform::GLResolver::GetProcAddressThunk(name));
}

} // namespace

namespace libprojectM {
namespace Renderer {
namespace Platform {

auto GladLoader::Instance() -> GladLoader&
{
    static GladLoader instance;
    return instance;
}

auto GladLoader::CheckGLRequirements() -> bool
{
#ifdef __EMSCRIPTEN__
    return true;
#else

    GLProbe::CheckBuilder glCheck;

#ifdef USE_GLES

    glCheck
        .WithApi(GLApi::OpenGLES)
        .WithMinimumVersion(3, 2)
        .WithMinimumShaderLanguageVersion(3, 20)
        .WithRequireCoreProfile(false);

#else

    glCheck
        .WithApi(GLApi::OpenGL)
        .WithMinimumVersion(3, 3)
        .WithMinimumShaderLanguageVersion(3, 30)
        // Accept both core and compatibility contexts. A 3.3+ compatibility context is a valid
        // configuration on many drivers/stacks, and profile filtering would reject it unnecessarily.
        .WithRequireCoreProfile(false);

#endif

    auto glDetails = glCheck.Check();

    LOG_INFO(std::string("[GladLoader] GLInfo  ") +
             GLProbe::FormatCompactLine(glDetails.info) +
             " backend=\"" + BackendToString(GLResolver::Instance().CurrentBackend()) + "\"" +
             " user_resolver=\"" + (GLResolver::Instance().HasUserResolver() ? "yes" : "no") + "\"");

    if (!glDetails.success)
    {
        LOG_ERROR(std::string("[GladLoader] GL requirements check failed: ") + glDetails.reason);
    }

    return glDetails.success;

#endif // #ifndef __EMSCRIPTEN__
}


auto GladLoader::Initialize() -> bool
{
    // make sure GLResolver is ready to use for this thread
    {
        std::string reason;
        if (!GLResolver::Instance().VerifyBeforeUse(reason))
        {
            LOG_ERROR(std::string("[GladLoader] GLResolver cannot be used: ") + reason);
            return false;
        }
    }

    // Always validate context requirements for the calling thread
    if (!CheckGLRequirements())
    {
        return false;
    }

    // check if GLAD is already initialized
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_isLoaded)
        {
            return true;
        }
    }

#ifndef USE_GLES

    const int result = gladLoadGL(&GladBridgeGetProcAddressThunk);
    if (result != 0)
    {
        if (EnableGLResolverTraceLogging())
        {
            LOG_INFO("[GladLoader] GLAD    GL API was loaded successfully");
        }
        std::unique_lock<std::mutex> lock(m_mutex);
        m_isLoaded = true;
        return true;
    }

    LOG_ERROR(std::string("[GladLoader] GLAD    Loading GL API failed (backend=") +
              BackendToString(GLResolver::Instance().CurrentBackend()) + ")");
    return false;

#else // #ifndef USE_GLES

    const int result = gladLoadGLES2(&GladBridgeGetProcAddressThunk);
    if (result != 0)
    {
        if (EnableGLResolverTraceLogging())
        {
            LOG_INFO("[GladLoader] GLAD    GLES2 API was loaded successfully");
        }
        std::unique_lock<std::mutex> lock(m_mutex);
        m_isLoaded = true;
        return true;
    }

    LOG_ERROR(std::string("[GladLoader] GLAD    Loading GLES2 API failed (backend=") +
              BackendToString(GLResolver::Instance().CurrentBackend()) + ")");

    return false;

#endif // #ifndef USE_GLES else
}

} // namespace Platform
} // namespace Renderer
} // namespace libprojectM

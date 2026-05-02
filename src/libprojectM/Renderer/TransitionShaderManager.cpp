#include "Renderer/TransitionShaderManager.hpp"

#include "BuiltInTransitionsResources.hpp"

namespace libprojectM {
namespace Renderer {

TransitionShaderManager::TransitionShaderManager()
    : m_mersenneTwister(m_randomDevice())
{
    // Compile all candidate shaders and keep only those that succeeded.
    std::vector<std::shared_ptr<Shader>> candidates = {
        CompileTransitionShader(kTransitionShaderBuiltInCircleGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInCubeRotateGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInDreamyGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInGlitchGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInHeatWaveGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInKaleidoscopeGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInMosaicZoomGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInMotionBlurGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInPageCurlGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInPixelateGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInPlasmaGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInSliceSwipeGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInSweepGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInTileFlipGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInWarpGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInWaterDropGlsl330),
        CompileTransitionShader(kTransitionShaderBuiltInZoomBlurGlsl330),
    };

    for (auto& shader : candidates)
    {
        if (shader != nullptr)
        {
            m_transitionShaders.push_back(std::move(shader));
        }
    }

    // Always compile SimpleBlend separately as a guaranteed fallback used when
    // every entry in m_transitionShaders failed to compile.
    m_fallbackShader = CompileTransitionShader(kTransitionShaderBuiltInSimpleBlendGlsl330);
}

auto TransitionShaderManager::RandomTransition() -> std::shared_ptr<Shader>
{
    if (!m_transitionShaders.empty())
    {
        return m_transitionShaders.at(m_mersenneTwister() % m_transitionShaders.size());
    }

    // All custom shaders failed to compile — use the SimpleBlend fallback.
    return m_fallbackShader;
}

auto TransitionShaderManager::CompileTransitionShader(const std::string& shaderBodyCode) -> std::shared_ptr<Shader>
{
#ifdef USE_GLES
    // GLES also requires a precision specifier for variables and 3D samplers
    constexpr char versionHeader[] = "#version 300 es\n\nprecision highp float;\nprecision highp int;\nprecision highp sampler2D;\nprecision highp sampler3D;\n";
#else
    constexpr char versionHeader[] = "#version 330\n\n";
#endif

    std::string fragmentShaderSource(static_cast<const char*>(versionHeader));
    fragmentShaderSource.append(kTransitionShaderHeaderGlsl330);
    fragmentShaderSource.append("\n");
    fragmentShaderSource.append(shaderBodyCode);
    fragmentShaderSource.append("\n");
    fragmentShaderSource.append(kTransitionShaderMainGlsl330);

    try
    {
        auto transitionShader = std::make_shared<Shader>();
        transitionShader->CompileProgram(static_cast<const char*>(versionHeader) + kTransitionVertexShaderGlsl330, fragmentShaderSource);
        return transitionShader;
    }
    catch (const ShaderException&)
    {
        // ToDo: Log proper shader compile error once logging API is in place
        return {};
    }
}

} // namespace Renderer
} // namespace libprojectM

#include "Renderer/TransitionShaderManager.hpp"

#include "BuiltInTransitionsResources.hpp"

namespace libprojectM {
namespace Renderer {

TransitionShaderManager::TransitionShaderManager()
    : m_mersenneTwister(m_randomDevice())
{
    // Compile all candidate shaders and keep only those that succeeded.
    // Each entry is {shaderBody, passCount}. Most are single-pass (1).
    // PageCurl and MultiPassTest use 2 passes for enhanced visual quality.
    struct Candidate
    {
        const std::string& body;
        int passCount;
    };

    std::vector<Candidate> candidates;
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInCircleGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInCubeRotateGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInDreamyGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInGlitchGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInHeatWaveGlsl330, 2});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInKaleidoscopeGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInMosaicZoomGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInMotionBlurGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInMultiPassTestGlsl330, 2});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInPageCurlGlsl330, 2});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInPixelateGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInPlasmaGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInSliceSwipeGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInSweepGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInTileFlipGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInWarpGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInWaterDropGlsl330, 1});
    candidates.emplace_back(Candidate{kTransitionShaderBuiltInZoomBlurGlsl330, 1});

    for (auto& entry : candidates)
    {
        auto shader = CompileTransitionShader(entry.body);
        if (shader != nullptr)
        {
            m_transitionShaders.push_back({std::move(shader), entry.passCount});
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
        return m_transitionShaders.at(m_mersenneTwister() % m_transitionShaders.size()).shader;
    }

    // All custom shaders failed to compile — use the SimpleBlend fallback.
    return m_fallbackShader;
}

auto TransitionShaderManager::GetPassCount(const std::shared_ptr<Shader>& shader) const -> int
{
    if (!shader)
    {
        return 1;
    }

    for (const auto& entry : m_transitionShaders)
    {
        if (entry.shader == shader)
        {
            return entry.passCount;
        }
    }

    // Fallback shader or unknown — default to single-pass.
    return m_fallbackPassCount;
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

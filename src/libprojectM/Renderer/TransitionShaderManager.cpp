#include "TransitionShaderManager.hpp"

#include "BuiltInTransitionsResources.hpp"

TransitionShaderManager::TransitionShaderManager()
    : m_mersenneTwister(m_randomDevice())
{
    m_transitionShaders.push_back(CompileTransitionShader(kTransitionShaderBuiltInSimpleBlendGlsl330));
    m_transitionShaders.push_back(CompileTransitionShader(kTransitionShaderBuiltInSweepGlsl330));
}

auto TransitionShaderManager::RandomTransition() -> std::shared_ptr<Shader>
{
    if (m_transitionShaders.empty())
    {
        return {};
    }

    return m_transitionShaders.at(m_mersenneTwister() % m_transitionShaders.size());
}

auto TransitionShaderManager::CompileTransitionShader(const std::string& shaderBodyCode) -> std::shared_ptr<Shader>
{
#if USE_GLES
    std::string versionHeader{"#version 300 es\n\n"};
#else
    std::string versionHeader{"#version 330\n\n"};
#endif

    std::string fragmentShaderSource(versionHeader);
    fragmentShaderSource.append(kTransitionShaderHeaderGlsl330);
    fragmentShaderSource.append("\n");
    fragmentShaderSource.append(shaderBodyCode);
    fragmentShaderSource.append("\n");
    fragmentShaderSource.append(kTransitionShaderMainGlsl330);

    try
    {
        auto transitionShader = std::make_shared<Shader>();
        transitionShader->CompileProgram(versionHeader + kTransitionVertexShaderGlsl330, fragmentShaderSource);
        return transitionShader;
    }
    catch (const ShaderException& ex)
    {
        // ToDo: Log proper shader compile error once logging API is in place
        return {};
    }
}

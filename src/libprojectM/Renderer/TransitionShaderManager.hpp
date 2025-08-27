#pragma once

#include "Renderer/Shader.hpp"

#include <random>

namespace libprojectM {
namespace Renderer {


class TransitionShaderManager
{
public:
    TransitionShaderManager();


    auto RandomTransition() -> std::shared_ptr<Shader>;

private:

    static auto CompileTransitionShader(const std::string& shaderBodyCode) -> std::shared_ptr<Shader>;

    std::vector<std::shared_ptr<Shader>> m_transitionShaders;

    std::random_device m_randomDevice;
    std::mt19937 m_mersenneTwister;
};

}
}

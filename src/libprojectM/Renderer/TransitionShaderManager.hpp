#pragma once

#include "Renderer/Shader.hpp"

#include <random>

namespace libprojectM {
namespace Renderer {

/**
 * @brief Manages all available transition shaders.
 */
class TransitionShaderManager
{
public:
    TransitionShaderManager();

    /**
     * @brief Selects a random transition shader from the list.
     * @return A shared pointer to a transition shader.
     */
    auto RandomTransition() -> std::shared_ptr<Shader>;

private:
    /**
     * @brief Compiles a single transition shader program.
     * @param shaderBodyCode The mainImage() fragment shader code, without any headers etc.
     */
    static auto CompileTransitionShader(const std::string& shaderBodyCode) -> std::shared_ptr<Shader>;

    std::vector<std::shared_ptr<Shader>> m_transitionShaders; //!< Currently loaded and compiled transition shaders.

    std::random_device m_randomDevice; //!< Seed for the random number generator
    std::mt19937 m_mersenneTwister; //!< Random engine to select shader
};

} // namespace Renderer
} // namespace libprojectM

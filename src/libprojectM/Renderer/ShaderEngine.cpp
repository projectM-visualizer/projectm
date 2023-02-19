#include "ShaderEngine.hpp"

#include "StaticGlShaders.h"
#include "Texture.hpp"
#include <glm/gtc/type_ptr.hpp>

ShaderEngine::ShaderEngine()
    : presetCompShaderLoaded(false)
    , presetWarpShaderLoaded(false)
{
    std::shared_ptr<StaticGlShaders> static_gl_shaders = StaticGlShaders::Get();

    m_untexturedDraw.CompileProgram(static_gl_shaders->GetV2fC4fVertexShader(),
                                    static_gl_shaders->GetV2fC4fFragmentShader());
    m_texturedDraw.CompileProgram(static_gl_shaders->GetV2fC4fT2fVertexShader(),
                                  static_gl_shaders->GetV2fC4fT2fFragmentShader());
}

ShaderEngine::~ShaderEngine()
{
    disablePresetShaders();
}

// deactivate preset shaders
void ShaderEngine::disablePresetShaders()
{
    if (presetCompShaderLoaded)
        glDeleteProgram(programID_presetComp);

    if (presetWarpShaderLoaded)
        glDeleteProgram(programID_presetWarp);

    presetCompShaderLoaded = false;
    presetWarpShaderLoaded = false;
}

void ShaderEngine::reset()
{
    disablePresetShaders();
}

// use the appropriate shader program for rendering the interpolation.
// it will use the preset shader if available, otherwise the textured shader
bool ShaderEngine::enableWarpShader(Shader& shader, const Pipeline& pipeline, const PipelineContext& pipelineContext, const glm::mat4& mat_ortho)
{
    if (presetWarpShaderLoaded)
    {
        glUseProgram(programID_presetWarp);

        shader.Bind();
        shader.BindTextures();
        glUniformMatrix4fv(uniform_vertex_transf_warp_shader, 1, GL_FALSE, glm::value_ptr(mat_ortho));

#if OGL_DEBUG
        validateProgram(programID_presetWarp);
#endif

        return true;
    }

    m_texturedDraw.Bind();
    m_texturedDraw.SetUniformMat4x4("vertex_transformation", mat_ortho);
    m_texturedDraw.SetUniformInt("texture_sampler", 0);

    return false;
}

bool ShaderEngine::enableCompositeShader(Shader& shader, const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
    if (presetCompShaderLoaded)
    {
        glUseProgram(programID_presetComp);

#if OGL_DEBUG
        validateProgram(programID_presetComp);
#endif

        return true;
    }

    m_texturedDraw.Bind();

    return false;
}

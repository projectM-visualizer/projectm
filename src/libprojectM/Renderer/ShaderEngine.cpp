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

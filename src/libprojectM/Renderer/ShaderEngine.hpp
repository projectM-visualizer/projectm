#pragma once

#include "Shader.hpp"
#include "TextureManager.hpp"
#include "projectM-opengl.h"

#include "Audio/BeatDetect.hpp"

#include <glm/mat4x4.hpp>

class ShaderEngine
{
public:
    ShaderEngine();
    virtual ~ShaderEngine();

    void reset();

    Shader m_untexturedDraw;
    Shader m_texturedDraw;

private:
    GLint uniform_vertex_transf_warp_shader;

    void disablePresetShaders();

    // programs generated from preset shader code
    GLuint programID_presetComp, programID_presetWarp;

    bool presetCompShaderLoaded, presetWarpShaderLoaded;
};

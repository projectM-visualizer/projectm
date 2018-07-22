/*
 * ShaderEngine.hpp
 *
 *  Created on: Jul 18, 2008
 *      Author: pete
 */

#ifndef SHADERENGINE_HPP_
#define SHADERENGINE_HPP_

#include "Common.hpp"
#include "projectM-opengl.h"

class ShaderEngine;
#include "Pipeline.hpp"
#include "PipelineContext.hpp"
#include "TextureManager.hpp"
#include "BeatDetect.hpp"

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include "Shader.hpp"


class ShaderEngine
{
public:
    enum PresentShaderType
    {
        PresentCompositeShader,
        PresentWarpShader,
        PresentBlur1Shader,
        PresentBlur2Shader,
    };

    ShaderEngine();
    virtual ~ShaderEngine();
    void loadPresetShaders(Pipeline &pipeline);
    bool enableWarpShader(Shader &shader, const Pipeline &pipeline, const PipelineContext &pipelineContext, const glm::mat4 & mat_ortho);
    bool enableCompositeShader(Shader &shader, const Pipeline &pipeline, const PipelineContext &pipelineContext);
    void RenderBlurTextures(const Pipeline  &pipeline, const PipelineContext &pipelineContext);
    void setParams(const int _texsizeX, const int texsizeY, BeatDetect *beatDetect, TextureManager *_textureManager);
    void reset();

    static GLuint CompileShaderProgram(const std::string & VertexShaderCode, const std::string & FragmentShaderCode, const std::string & shaderTypeString);
    static bool checkCompileStatus(GLuint shader, const std::string & shaderTitle);
    static bool linkProgram(GLuint programID);

    static GLint Uniform_V2F_C4F_VertexTranformation() { return UNIFORM_V2F_C4F_VERTEX_TRANFORMATION; }
    static GLint Uniform_V2F_C4F_VertexPointSize() { return UNIFORM_V2F_C4F_VERTEX_POINT_SIZE; }
    static GLint Uniform_V2F_C4F_T2F_VertexTranformation() { return UNIFORM_V2F_C4F_T2F_VERTEX_TRANFORMATION; }
    static GLint Uniform_V2F_C4F_T2F_FragTextureSampler() { return UNIFORM_V2F_C4F_T2F_FRAG_TEXTURE_SAMPLER; }

    GLuint programID_v2f_c4f;
    GLuint programID_v2f_c4f_t2f;

    const static std::string v2f_c4f_vert;
    const static std::string v2f_c4f_frag;
    const static std::string v2f_c4f_t2f_vert;
    const static std::string v2f_c4f_t2f_frag;

private:
    int texsizeX;
    int texsizeY;
    float aspectX;
    float aspectY;
    BeatDetect *beatDetect;
    TextureManager *textureManager;
    GLint uniform_vertex_transf_warp_shader;

    GLuint programID_warp_fallback;
    GLuint programID_comp_fallback;
    GLuint programID_blur1;
    GLuint programID_blur2;

    bool blur1_enabled;
    bool blur2_enabled;
    bool blur3_enabled;
    GLuint blur1_tex;
    GLuint blur2_tex;
    GLuint blur3_tex;

    float rand_preset[4];

    void SetupShaderQVariables(GLuint program, const Pipeline &q);
    void SetupShaderVariables(GLuint program, const Pipeline &pipeline, const PipelineContext &pipelineContext);
    void SetupTextures(GLuint program, const Shader &shader);
    GLuint compilePresetShader(const ShaderEngine::PresentShaderType shaderType, Shader &shader, const std::string &shaderFilename);
    void disablePresetShaders();
    GLuint loadPresetShader(const PresentShaderType shaderType, Shader &shader, std::string &shaderFilename);

    void deletePresetShader(Shader &shader);

    
    // programs generated from preset shader code
    GLuint programID_presetComp, programID_presetWarp;

    bool presetCompShaderLoaded, presetWarpShaderLoaded;

    static GLint UNIFORM_V2F_C4F_VERTEX_TRANFORMATION;
    static GLint UNIFORM_V2F_C4F_VERTEX_POINT_SIZE;
    static GLint UNIFORM_V2F_C4F_T2F_VERTEX_TRANFORMATION;
    static GLint UNIFORM_V2F_C4F_T2F_FRAG_TEXTURE_SAMPLER;
};

#endif /* SHADERENGINE_HPP_ */

/*
 * ShaderEngine.hpp
 *
 *  Created on: Jul 18, 2008
 *      Author: pete
 */

#ifndef SHADERENGINE_HPP_
#define SHADERENGINE_HPP_

#define PRESET_SHADERS_ENABLED 1

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
    void enableWarpShader(Shader &shader, const Pipeline &pipeline, const PipelineContext &pipelineContext);
    void enableCompositeShader(Shader &shader, const Pipeline &pipeline, const PipelineContext &pipelineContext);
    void RenderBlurTextures(const Pipeline  &pipeline, const PipelineContext &pipelineContext, const int texsize);
    void setAspect(float aspect);
    void setParams(const int texsize, const unsigned int texId, const float aspect, BeatDetect *beatDetect, TextureManager *textureManager);
    void reset();

    static GLint Uniform_V2F_C4F_VertexTranformation() { return UNIFORM_V2F_C4F_VERTEX_TRANFORMATION; }
    static GLint Uniform_V2F_C4F_VertexPointSize() { return UNIFORM_V2F_C4F_VERTEX_POINT_SIZE; }
    static GLint Uniform_V2F_C4F_T2F_VertexTranformation() { return UNIFORM_V2F_C4F_T2F_VERTEX_TRANFORMATION; }
    static GLint Uniform_V2F_C4F_T2F_FragTextureSampler() { return UNIFORM_V2F_C4F_T2F_FRAG_TEXTURE_SAMPLER; }

    GLuint programID_v2f_c4f;
    GLuint programID_v2f_c4f_t2f;

private:
    unsigned int mainTextureId;
    int texsize;
    float aspect;
    BeatDetect *beatDetect;
    TextureManager *textureManager;

    GLuint noise_texture_lq_lite;
    GLuint noise_texture_lq;
    GLuint noise_texture_mq;
    GLuint noise_texture_hq;
    GLuint noise_texture_perlin;
    GLuint noise_texture_lq_vol;
    GLuint noise_texture_hq_vol;

    bool blur1_enabled;
    bool blur2_enabled;
    bool blur3_enabled;
    GLuint blur1_tex;
    GLuint blur2_tex;
    GLuint blur3_tex;

    float rand_preset[4];

    void SetupShaderQVariables(GLuint program, const Pipeline &q);
    void SetupShaderVariables(GLuint program, const Pipeline &pipeline, const PipelineContext &pipelineContext);
    void setupUserTexture(GLuint program, const UserTexture* texture);
    void setupUserTextureState(GLuint program, const UserTexture* texture);
    GLuint compilePresetShader(const ShaderEngine::PresentShaderType shaderType, Shader &shader, const std::string &shaderFilename);
    bool checkCompileStatus(GLuint shader, const std::string & shaderTitle);
    bool linkProgram(GLuint programID);
    void disablePresetShaders();
    GLuint loadPresetShader(const PresentShaderType shaderType, Shader &shader, std::string &shaderFilename);

    void deletePresetShader(Shader &shader);

    
    // programs generated from preset shader code
    GLuint programID_presetComp, programID_presetWarp;
    GLuint programID_blur1, programID_blur2;

    bool presetCompShaderLoaded, presetWarpShaderLoaded;


    GLuint CompileShaderProgram(const std::string & VertexShaderCode, const std::string & FragmentShaderCode, const std::string & shaderTypeString);


    static GLint UNIFORM_V2F_C4F_VERTEX_TRANFORMATION;
    static GLint UNIFORM_V2F_C4F_VERTEX_POINT_SIZE;
    static GLint UNIFORM_V2F_C4F_T2F_VERTEX_TRANFORMATION;
    static GLint UNIFORM_V2F_C4F_T2F_FRAG_TEXTURE_SAMPLER;
};

#endif /* SHADERENGINE_HPP_ */

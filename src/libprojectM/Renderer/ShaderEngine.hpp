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

#include "Pipeline.hpp"
#include "PipelineContext.hpp"
class ShaderEngine;
#include "TextureManager.hpp"

#include "HLSLTranslator.hpp"

#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include "Shader.hpp"
class ShaderEngine
{
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

    GLuint   blur1Program;
    GLuint   blur2Program;

    bool enabled;

    std::map<Shader*,GLuint> programs;

    void InitShaderProgram();
    void SetupShaderQVariables(GLuint program, const Pipeline &q);
    void SetupShaderVariables(GLuint program, const Pipeline &pipeline, const PipelineContext &pipelineContext);
    // void SetupUserTexture(CGprogram program, const UserTexture* texture);
    void SetupUserTextureState(const UserTexture* texture);
    GLuint makeShader(GLenum type, const char *filename);
    bool LoadHLSLProgram(GLenum shaderType, Shader &shader, std::string &shaderFilename);

public:
	ShaderEngine();
	virtual ~ShaderEngine();
    void RenderBlurTextures(const Pipeline  &pipeline, const PipelineContext &pipelineContext, const int texsize);
	void loadShader(GLenum shaderType, Shader &shader, std::string &shaderFilename);

	void setParams(const int texsize, const unsigned int texId, const float aspect, BeatDetect *beatDetect, TextureManager *textureManager);
	void reset();
	void setAspect(float aspect);
    std::string profileName;
};

#endif /* SHADERENGINE_HPP_ */

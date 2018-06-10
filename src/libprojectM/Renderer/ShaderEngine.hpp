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

#ifdef USE_CG
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#endif


#include "Pipeline.hpp"
#include "PipelineContext.hpp"
class ShaderEngine;
#include "TextureManager.hpp"

#include <cstdlib>
#include <iostream>
#include <map>
#include "Shader.hpp"
class ShaderEngine
{
#ifdef USE_CG


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

  CGcontext   myCgContext;
  CGprofile myCgProfile;
  CGprogram   blur1Program;
  CGprogram   blur2Program;

  bool enabled;

  std::map<Shader*,CGprogram> programs;

   std::string cgTemplate;
   std::string blurProgram;

 bool LoadCgProgram(Shader &shader, std::string &shaderFilename);
 bool checkForCgCompileError(std::string &context, const char *situation);
 void checkForCgError(const char *situation);

 void SetupCg();
 void SetupCgVariables(CGprogram program, const Pipeline &pipeline, const PipelineContext &pipelineContext);
 void SetupCgQVariables(CGprogram program, const Pipeline &pipeline);

 void SetupUserTexture(CGprogram program, const UserTexture* texture);
 void SetupUserTextureState(const UserTexture* texture);



#endif
public:
	ShaderEngine();
	virtual ~ShaderEngine();
#ifdef USE_CG
    void RenderBlurTextures(const Pipeline  &pipeline, const PipelineContext &pipelineContext, const int texsize);
	void loadShader(Shader &shader, std::string &shaderFilename);

	void setParams(const int texsize, const unsigned int texId, const float aspect, BeatDetect *beatDetect, TextureManager *textureManager);
	void enableShader(Shader &shader, const Pipeline &pipeline, const PipelineContext &pipelineContext);
	void disableShader();
	void reset();
	void setAspect(float aspect);
    std::string profileName;

#endif
    GLuint programID_v2f_c4f;
    GLuint programID_v2f_c4f_t2f;


    GLuint CompileShaderProgram(const std::string & VertexShaderCode, const std::string & FragmentShaderCode);

    static GLint Uniform_V2F_C4F_VertexTranformation() { return UNIFORM_V2F_C4F_VERTEX_TRANFORMATION; }
    static GLint Uniform_V2F_C4F_VertexPointSize() { return UNIFORM_V2F_C4F_VERTEX_POINT_SIZE; }
    static GLint Uniform_V2F_C4F_T2F_VertexTranformation() { return UNIFORM_V2F_C4F_T2F_VERTEX_TRANFORMATION; }
    static GLint Uniform_V2F_C4F_T2F_FragTextureSampler() { return UNIFORM_V2F_C4F_T2F_FRAG_TEXTURE_SAMPLER; }

    static GLint UNIFORM_V2F_C4F_VERTEX_TRANFORMATION;
    static GLint UNIFORM_V2F_C4F_VERTEX_POINT_SIZE;
    static GLint UNIFORM_V2F_C4F_T2F_VERTEX_TRANFORMATION;
    static GLint UNIFORM_V2F_C4F_T2F_FRAG_TEXTURE_SAMPLER;

};

#endif /* SHADERENGINE_HPP_ */

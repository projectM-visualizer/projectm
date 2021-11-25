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
#include <glm/vec3.hpp>


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
    bool loadPresetShaders(Pipeline &pipeline, const std::string &presetName);
    bool enableWarpShader(Shader &shader, const Pipeline &pipeline, const PipelineContext &pipelineContext, const glm::mat4 & mat_ortho);
    bool enableCompositeShader(Shader &shader, const Pipeline &pipeline, const PipelineContext &pipelineContext);
    void RenderBlurTextures(const Pipeline  &pipeline, const PipelineContext &pipelineContext);
    void setParams(int _texsizeX, int texsizeY,
                   float _aspectX, float _aspectY,
                   BeatDetect *beatDetect, TextureManager *_textureManager);
    void reset();

    static GLuint CompileShaderProgram(const std::string & VertexShaderCode, const std::string & FragmentShaderCode, const std::string & shaderTypeString);
    static bool checkCompileStatus(GLuint shader, const std::string & shaderTitle);
    static bool linkProgram(GLuint programID);


    GLuint programID_v2f_c4f;
    GLuint programID_v2f_c4f_t2f;

    GLint uniform_v2f_c4f_vertex_transformation;
    GLint uniform_v2f_c4f_vertex_point_size;
    GLint uniform_v2f_c4f_t2f_vertex_transformation;
    GLint uniform_v2f_c4f_t2f_frag_texture_sampler;

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

    GLint uniform_blur1_sampler;
    GLint uniform_blur1_c0;
    GLint uniform_blur1_c1;
    GLint uniform_blur1_c2;
    GLint uniform_blur1_c3;

    GLint uniform_blur2_sampler;
    GLint uniform_blur2_c0;
    GLint uniform_blur2_c5;
    GLint uniform_blur2_c6;

    GLuint vboBlur;
    GLuint vaoBlur;

    float rand_preset[4];
    glm::vec3 xlate[20];
    glm::vec3 rot_base[20];
    glm::vec3 rot_speed[20];

    void SetupShaderVariables(GLuint program, const Pipeline &pipeline, const PipelineContext &pipelineContext);
    void SetupTextures(GLuint program, const Shader &shader);
    GLuint compilePresetShader(const ShaderEngine::PresentShaderType shaderType, Shader &shader, const std::string &shaderFilename);

    void disablePresetShaders();
    GLuint loadPresetShader(const PresentShaderType shaderType, Shader &shader, std::string &shaderFilename);

    void deletePresetShader(Shader &shader);
    void validateProgram(const GLuint programID);

    
    // programs generated from preset shader code
    GLuint programID_presetComp, programID_presetWarp;

    bool presetCompShaderLoaded, presetWarpShaderLoaded;

    std::string m_presetName;
};

#endif /* SHADERENGINE_HPP_ */

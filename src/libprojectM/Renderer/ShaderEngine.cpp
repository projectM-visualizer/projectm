/*
 * ShaderEngine.cpp
 *
 *  Created on: Jul 18, 2008
 *      Author: pete
 */
#include <fstream>
#include <algorithm>
#include "ShaderEngine.hpp"
#include "BeatDetect.hpp"
#include "Texture.hpp"
#include "HLSLParser.h"
#include "GLSLGenerator.h"
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include <set>
#include <regex>

#ifdef USE_GLES
    #define GLSL_VERSION "300 es"
    #define SHADER_VERSION  M4::GLSLGenerator::Version_300_ES
#else
    #define GLSL_VERSION "330"
    #define SHADER_VERSION  M4::GLSLGenerator::Version_330
#endif


#define FRAND ((rand() % 7381)/7380.0f)


std::string presetWarpVertexShader(
    "#version "
    GLSL_VERSION
    "\n"
    "layout(location = 0) in vec2 vertex_position;\n"
    "layout(location = 1) in vec4 vertex_color;\n"
    "layout(location = 2) in vec2 vertex_texture;\n"
    ""
    "uniform mat4 vertex_transformation;\n"
    ""
    "out vec4 frag_COLOR;\n"
    "out vec4 frag_TEXCOORD0;\n"
    "out vec2 frag_TEXCOORD1;\n"
    ""
    "void main(){\n"
    "    vec4 position = vertex_transformation * vec4(vertex_position, 0.0, 1.0);\n"
    "    gl_Position = position;\n"
    "    frag_COLOR = vertex_color;\n"
    "    frag_TEXCOORD0.xy = vertex_texture;\n"
    "    frag_TEXCOORD0.zw = position.xy;\n"
    "    frag_TEXCOORD1 = vec2(0.0, 0.0);\n"
    "}\n");

std::string presetCompVertexShader(
    "#version "
    GLSL_VERSION
    "\n"
    "layout(location = 0) in vec2 vertex_position;\n"
    "layout(location = 1) in vec4 vertex_color;\n"
    "layout(location = 2) in vec2 vertex_texture;\n"
    "layout(location = 3) in vec2 vertex_rad_ang;\n"
    ""
    "out vec4 frag_COLOR;\n"
    "out vec2 frag_TEXCOORD0;\n"
    "out vec2 frag_TEXCOORD1;\n"
    ""
    "void main(){\n"
    "    vec4 position = vec4(vertex_position, 0.0, 1.0);\n"
    "    gl_Position = position;\n"
    "    frag_COLOR = vertex_color;\n"
    "    frag_TEXCOORD0 = vertex_texture;\n"
    "    frag_TEXCOORD1 = vertex_rad_ang;\n"
    "}\n");


const std::string ShaderEngine::v2f_c4f_vert(
    "#version "
    GLSL_VERSION
    "\n"
    ""
    "layout(location = 0) in vec2 vertex_position;\n"
    "layout(location = 1) in vec4 vertex_color;\n"
    ""
    "uniform mat4 vertex_transformation;\n"
    "uniform float vertex_point_size;\n"
    ""
    "out vec4 fragment_color;\n"
    ""
    "void main(){\n"
    "    gl_Position = vertex_transformation * vec4(vertex_position, 0.0, 1.0);\n"
    "    gl_PointSize = vertex_point_size;\n"
    "    fragment_color = vertex_color;\n"
    "}\n");

const std::string ShaderEngine::v2f_c4f_frag(
    "#version "
    GLSL_VERSION
    "\n"
    "precision mediump float;\n"
    ""
    "in vec4 fragment_color;\n"
    "out vec4 color;\n"
    ""
    "void main(){\n"
    "    color = fragment_color;\n"
    "}\n");

const std::string ShaderEngine::v2f_c4f_t2f_vert(
        "#version "
        GLSL_VERSION
        "\n"
        "layout(location = 0) in vec2 vertex_position;\n"
        "layout(location = 1) in vec4 vertex_color;\n"
        "layout(location = 2) in vec2 vertex_texture;\n"
        ""
        "uniform mat4 vertex_transformation;\n"
        ""
        "out vec4 fragment_color;\n"
        "out vec2 fragment_texture;\n"
        ""
        "void main(){\n"
        "    gl_Position = vertex_transformation * vec4(vertex_position, 0.0, 1.0);\n"
        "    fragment_color = vertex_color;\n"
        "    fragment_texture = vertex_texture;\n"
        "}\n");

const std::string ShaderEngine::v2f_c4f_t2f_frag(
        "#version "
        GLSL_VERSION
        "\n"
        "precision mediump float;\n"
        ""
        "in vec4 fragment_color;\n"
        "in vec2 fragment_texture;\n"
        ""
        "uniform sampler2D texture_sampler;\n"
        ""
        "out vec4 color;\n"
        ""
        "void main(){\n"
        "    color = fragment_color * texture(texture_sampler, fragment_texture.st);\n"
        "}\n");


std::string PresetShaderIncludes = ""
"#define  M_PI   3.14159265359\n"
"#define  M_PI_2 6.28318530718\n"
"#define  M_INV_PI_2  0.159154943091895\n"

"uniform float4   rand_frame;		// random float4, updated each frame\n"
"uniform float4   rand_preset;   // random float4, updated once per *preset*\n"
"uniform float4   _c0;  // .xy: multiplier to use on UV's to paste an image fullscreen, *aspect-aware*; .zw = inverse.\n"
"uniform float4   _c1, _c2, _c3, _c4;\n"
"uniform float4   _c5;  //.xy = scale,bias for reading blur1; .zw = scale,bias for reading blur2; \n"
"uniform float4   _c6;  //.xy = scale,bias for reading blur3; .zw = blur1_min,blur1_max\n"
"uniform float4   _c7;  // .xy ~= float2(1024,768); .zw ~= float2(1/1024.0, 1/768.0)\n"
"uniform float4   _c8;  // .xyzw ~= 0.5 + 0.5*cos(time * float4(~0.3, ~1.3, ~5, ~20))\n"
"uniform float4   _c9;  // .xyzw ~= same, but using sin()\n"
"uniform float4   _c10;  // .xyzw ~= 0.5 + 0.5*cos(time * float4(~0.005, ~0.008, ~0.013, ~0.022))\n"
"uniform float4   _c11;  // .xyzw ~= same, but using sin()\n"
"uniform float4   _c12;  // .xyz = mip info for main image (.x=#across, .y=#down, .z=avg); .w = unused\n"
"uniform float4   _c13;  //.xy = blur2_min,blur2_max; .zw = blur3_min, blur3_max.\n"
"uniform float4   _qa;  // q vars bank 1 [q1-q4]\n"
"uniform float4   _qb;  // q vars bank 2 [q5-q8]\n"
"uniform float4   _qc;  // q vars ...\n"
"uniform float4   _qd;  // q vars\n"
"uniform float4   _qe;  // q vars\n"
"uniform float4   _qf;  // q vars\n"
"uniform float4   _qg;  // q vars\n"
"uniform float4   _qh;  // q vars bank 8 [q29-q32]\n"

"// note: in general, don't use the current time w/the *dynamic* rotations!\n"
"uniform float4x3 rot_s1;  // four random, static rotations.  randomized @ preset load time.\n"
"uniform float4x3 rot_s2;  // minor translation component (<1).\n"
"uniform float4x3 rot_s3;\n"
"uniform float4x3 rot_s4;\n"

"uniform float4x3 rot_d1;  // four random, slowly changing rotations.\n"
"uniform float4x3 rot_d2;  \n"
"uniform float4x3 rot_d3;\n"
"uniform float4x3 rot_d4;\n"
"uniform float4x3 rot_f1;  // faster-changing.\n"
"uniform float4x3 rot_f2;\n"
"uniform float4x3 rot_f3;\n"
"uniform float4x3 rot_f4;\n"
"uniform float4x3 rot_vf1;  // very-fast-changing.\n"
"uniform float4x3 rot_vf2;\n"
"uniform float4x3 rot_vf3;\n"
"uniform float4x3 rot_vf4;\n"
"uniform float4x3 rot_uf1;  // ultra-fast-changing.\n"
"uniform float4x3 rot_uf2;\n"
"uniform float4x3 rot_uf3;\n"
"uniform float4x3 rot_uf4;\n"

"uniform float4x3 rot_rand1; // random every frame\n"
"uniform float4x3 rot_rand2;\n"
"uniform float4x3 rot_rand3;\n"
"uniform float4x3 rot_rand4;\n"

"#define time     _c2.x\n"
"#define fps      _c2.y\n"
"#define frame    _c2.z\n"
"#define progress _c2.w\n"
"#define bass _c3.x\n"
"#define mid  _c3.y\n"
"#define treb _c3.z\n"
"#define vol  _c3.w\n"
"#define bass_att _c4.x\n"
"#define mid_att  _c4.y\n"
"#define treb_att _c4.z\n"
"#define vol_att  _c4.w\n"
"#define q1 _qa.x\n"
"#define q2 _qa.y\n"
"#define q3 _qa.z\n"
"#define q4 _qa.w\n"
"#define q5 _qb.x\n"
"#define q6 _qb.y\n"
"#define q7 _qb.z\n"
"#define q8 _qb.w\n"
"#define q9 _qc.x\n"
"#define q10 _qc.y\n"
"#define q11 _qc.z\n"
"#define q12 _qc.w\n"
"#define q13 _qd.x\n"
"#define q14 _qd.y\n"
"#define q15 _qd.z\n"
"#define q16 _qd.w\n"
"#define q17 _qe.x\n"
"#define q18 _qe.y\n"
"#define q19 _qe.z\n"
"#define q20 _qe.w\n"
"#define q21 _qf.x\n"
"#define q22 _qf.y\n"
"#define q23 _qf.z\n"
"#define q24 _qf.w\n"
"#define q25 _qg.x\n"
"#define q26 _qg.y\n"
"#define q27 _qg.z\n"
"#define q28 _qg.w\n"
"#define q29 _qh.x\n"
"#define q30 _qh.y\n"
"#define q31 _qh.z\n"
"#define q32 _qh.w\n"

"#define aspect   _c0\n"
"#define texsize  _c7 // .xy = (w,h); .zw = (1/(float)w, 1/(float)h)\n"
"#define roam_cos _c8\n"
"#define roam_sin _c9\n"
"#define slow_roam_cos _c10\n"
"#define slow_roam_sin _c11\n"
"#define mip_x   _c12.x\n"
"#define mip_y   _c12.y\n"
"#define mip_xy  _c12.xy\n"
"#define mip_avg _c12.z\n"
"#define blur1_min _c6.z\n"
"#define blur1_max _c6.w\n"
"#define blur2_min _c13.x\n"
"#define blur2_max _c13.y\n"
"#define blur3_min _c13.z\n"
"#define blur3_max _c13.w\n"

"#define sampler_FC_main sampler_fc_main\n"
"#define sampler_PC_main sampler_pc_main\n"
"#define sampler_FW_main sampler_fw_main\n"
"#define sampler_PW_main sampler_pw_main\n"

"#define GetMain(uv) (tex2D(sampler_main,uv).xyz)\n"
"#define GetPixel(uv) (tex2D(sampler_main,uv).xyz)\n"
"#define GetBlur1(uv) (tex2D(sampler_blur1,uv).xyz*_c5.x + _c5.y)\n"
"#define GetBlur2(uv) (tex2D(sampler_blur2,uv).xyz*_c5.z + _c5.w)\n"
"#define GetBlur3(uv) (tex2D(sampler_blur3,uv).xyz*_c6.x + _c6.y)\n"

"#define lum(x) (dot(x,float3(0.32,0.49,0.29)))\n"
"#define tex2d tex2D\n"
"#define tex3d tex3D\n"
;


std::string blur_vert(
    "#version "
    GLSL_VERSION
    "\n"
    "layout(location = 0) in vec2 vertex_position;\n"
    "layout(location = 1) in vec2 vertex_texture;\n"
    ""
    "out vec2 fragment_texture;\n"
    ""
    "void main(){\n"
    "    gl_Position = vec4(vertex_position, 0.0, 1.0);\n"
    "    fragment_texture = vertex_texture;\n"
    "}\n");


std::string blur1_frag(
    "#version "
    GLSL_VERSION
    "\n"
    "precision mediump float;\n"
    ""
    "in vec2 fragment_texture;\n"
    ""
    "uniform sampler2D texture_sampler;\n"
    "uniform vec4 _c0; // source texsize (.xy), and inverse (.zw)\n"
    "uniform vec4 _c1; // w1..w4\n"
    "uniform vec4 _c2; // d1..d4\n"
    "uniform vec4 _c3; // scale, bias, w_div\n"
    ""
    "out vec4 color;\n"
    ""
    "void main(){\n"
    ""
    "   // LONG HORIZ. PASS 1:\n"
    "   #define srctexsize _c0\n"
    "   #define w1 _c1.x\n"
    "   #define w2 _c1.y\n"
    "   #define w3 _c1.z\n"
    "   #define w4 _c1.w\n"
    "   #define d1 _c2.x\n"
    "   #define d2 _c2.y\n"
    "   #define d3 _c2.z\n"
    "   #define d4 _c2.w\n"
    "   #define fscale _c3.x\n"
    "   #define fbias  _c3.y\n"
    "   #define w_div  _c3.z\n"
    ""
    "   // note: if you just take one sample at exactly uv.xy, you get an avg of 4 pixels.\n"
    "   vec2 uv2 = fragment_texture.xy + srctexsize.zw*vec2(1.0,1.0);     // + moves blur UP, LEFT by 1-pixel increments\n"
    ""
    "   vec3 blur = \n"
    "           ( texture( texture_sampler, uv2 + vec2( d1*srctexsize.z,0) ).xyz\n"
    "           + texture( texture_sampler, uv2 + vec2(-d1*srctexsize.z,0) ).xyz)*w1 +\n"
    "           ( texture( texture_sampler, uv2 + vec2( d2*srctexsize.z,0) ).xyz\n"
    "           + texture( texture_sampler, uv2 + vec2(-d2*srctexsize.z,0) ).xyz)*w2 +\n"
    "           ( texture( texture_sampler, uv2 + vec2( d3*srctexsize.z,0) ).xyz\n"
    "           + texture( texture_sampler, uv2 + vec2(-d3*srctexsize.z,0) ).xyz)*w3 +\n"
    "           ( texture( texture_sampler, uv2 + vec2( d4*srctexsize.z,0) ).xyz\n"
    "           + texture( texture_sampler, uv2 + vec2(-d4*srctexsize.z,0) ).xyz)*w4\n"
    "           ;\n"
    "   blur.xyz *= w_div;\n"
    ""
    "   blur.xyz = blur.xyz*fscale + fbias;\n"
    ""
    "   color.xyz = blur;\n"
    "   color.w   = 1.0;\n"
    "}\n");

std::string blur2_frag(
    "#version "
    GLSL_VERSION
    "\n"
    "precision mediump float;\n"
    ""
    "in vec2 fragment_texture;\n"
    ""
    "uniform sampler2D texture_sampler;\n"
    "uniform vec4 _c0; // source texsize (.xy), and inverse (.zw)\n"
    "uniform vec4 _c5; // w1,w2,d1,d2\n"
    "uniform vec4 _c6; // w_div, edge_darken_c1, edge_darken_c2, edge_darken_c3\n"
    ""
    "out vec4 color;\n"
    ""
    "void main(){\n"
    ""
    "   //SHORT VERTICAL PASS 2:\n"
    "   #define srctexsize _c0\n"
    "   #define w1 _c5.x\n"
    "   #define w2 _c5.y\n"
    "   #define d1 _c5.z\n"
    "   #define d2 _c5.w\n"
    "   #define edge_darken_c1 _c6.y\n"
    "   #define edge_darken_c2 _c6.z\n"
    "   #define edge_darken_c3 _c6.w\n"
    "   #define w_div   _c6.x\n"
    ""
    "   // note: if you just take one sample at exactly uv.xy, you get an avg of 4 pixels.\n"
    "   vec2 uv2 = fragment_texture.xy + srctexsize.zw*vec2(0,0);     // + moves blur UP, LEFT by TWO-pixel increments! (since texture is 1/2 the size of blur1_ps)\n"
    ""
    "   vec3 blur = \n"
    "           ( texture( texture_sampler, uv2 + vec2(0, d1*srctexsize.w) ).xyz\n"
    "           + texture( texture_sampler, uv2 + vec2(0,-d1*srctexsize.w) ).xyz)*w1 +\n"
    "           ( texture( texture_sampler, uv2 + vec2(0, d2*srctexsize.w) ).xyz\n"
    "           + texture( texture_sampler, uv2 + vec2(0,-d2*srctexsize.w) ).xyz)*w2\n"
    "           ;\n"
    "   blur.xyz *= w_div;\n"
    ""
    "   // tone it down at the edges:  (only happens on 1st X pass!)\n"
    "   float t = min( min(fragment_texture.x, fragment_texture.y), 1.0-max(fragment_texture.x, fragment_texture.y) );\n"
    "   t = sqrt(t);\n"
    "   t = edge_darken_c1 + edge_darken_c2*clamp(t*edge_darken_c3, 0.0, 1.0);\n"
    "   blur.xyz *= t;\n"
    ""
    "   color.xyz = blur;\n"
    "   color.w   = 1.0;\n"
    "}\n");



ShaderEngine::ShaderEngine() : presetCompShaderLoaded(false), presetWarpShaderLoaded(false)
{
    programID_v2f_c4f = CompileShaderProgram(v2f_c4f_vert, v2f_c4f_frag, "v2f_c4f");
    programID_v2f_c4f_t2f = CompileShaderProgram(v2f_c4f_t2f_vert, v2f_c4f_t2f_frag, "v2f_c4f_t2f");

    programID_blur1 = CompileShaderProgram(blur_vert, blur1_frag, "blur1");
    programID_blur2 = CompileShaderProgram(blur_vert, blur2_frag, "blur2");

    uniform_v2f_c4f_vertex_tranformation = glGetUniformLocation(programID_v2f_c4f, "vertex_transformation");
    uniform_v2f_c4f_vertex_point_size = glGetUniformLocation(programID_v2f_c4f, "vertex_point_size");
    uniform_v2f_c4f_t2f_vertex_tranformation = glGetUniformLocation(programID_v2f_c4f_t2f, "vertex_transformation");
    uniform_v2f_c4f_t2f_frag_texture_sampler = glGetUniformLocation(programID_v2f_c4f_t2f, "texture_sampler");

    uniform_blur1_sampler = glGetUniformLocation(programID_blur1, "texture_sampler");
    uniform_blur1_c0 = glGetUniformLocation(programID_blur1, "_c0");
    uniform_blur1_c1 = glGetUniformLocation(programID_blur1, "_c1");
    uniform_blur1_c2 = glGetUniformLocation(programID_blur1, "_c2");
    uniform_blur1_c3 = glGetUniformLocation(programID_blur1, "_c3");

    uniform_blur2_sampler = glGetUniformLocation(programID_blur2, "texture_sampler");
    uniform_blur2_c0 = glGetUniformLocation(programID_blur2, "_c0");
    uniform_blur2_c5 = glGetUniformLocation(programID_blur2, "_c5");
    uniform_blur2_c6 = glGetUniformLocation(programID_blur2, "_c6");

    // Initialize Blur vao/vbo
    float pointsBlur[16] = {
        -1.0, -1.0,     0.0,    1.0,
         1.0, -1.0,     1.0,    1.0,
        -1.0,  1.0,     0.0,    0.0,
         1.0,  1.0,     1.0,    0.0};

    glGenBuffers(1, &vboBlur);
    glGenVertexArrays(1, &vaoBlur);

    glBindVertexArray(vaoBlur);
    glBindBuffer(GL_ARRAY_BUFFER, vboBlur);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 2, pointsBlur, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0); // Positions

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2)); // Textures

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ShaderEngine::~ShaderEngine()
{
    glDeleteProgram(programID_v2f_c4f);
    glDeleteProgram(programID_v2f_c4f_t2f);

    glDeleteProgram(programID_blur1);
    glDeleteProgram(programID_blur2);

    glDeleteBuffers(1, &vboBlur);
    glDeleteVertexArrays(1, &vaoBlur);

    disablePresetShaders();
}

bool ShaderEngine::checkCompileStatus(GLuint shader, const std::string & shaderTitle) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE)
        return true;  // success

    int InfoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(shader, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        std::cerr << "Failed to compile shader '" << shaderTitle << "'. Error: " << &FragmentShaderErrorMessage[0] << std::endl;
    }

    return false;
}

void ShaderEngine::setParams(const int _texsizeX, const int _texsizeY, BeatDetect *_beatDetect,
        TextureManager *_textureManager)
{
    this->beatDetect = _beatDetect;
    this->textureManager = _textureManager;

    aspectX = 1;
    aspectY = 1;
    if (_texsizeX > _texsizeY)
        aspectY = (float)_texsizeY/(float)_texsizeX;
    else
        aspectX = (float)_texsizeX/(float)_texsizeY;

    this->texsizeX = _texsizeX;
    this->texsizeY = _texsizeY;
}


// compile a user-defined shader from a preset. returns program ID if successful.
GLuint ShaderEngine::compilePresetShader(const PresentShaderType shaderType, Shader &pmShader, const std::string &shaderFilename) {
    std::string program = pmShader.programSource;

    if (program.length() <= 0)
        return GL_FALSE;

    // replace "}" with return statement (this can probably be optimized for the GLSL conversion...)
    size_t found = program.rfind('}');
    if (found != std::string::npos)
    {
        //std::cout << "last '}' found at: " << int(found) << std::endl;
        program.replace(int(found), 1, "_return_value = float4(ret.xyz, 1.0);\n"
                                       "}\n");
    }
    else
        return GL_FALSE;

    // replace shader_body with entry point function
    found = program.find("shader_body");
    if (found != std::string::npos)
    {
        //std::cout << "first 'shader_body' found at: " << int(found) << std::endl;
        if (shaderType == PresentWarpShader) {
            program.replace(int(found), 11, "void PS(float4 _vDiffuse : COLOR, float4 _uv : TEXCOORD0, float2 _rad_ang : TEXCOORD1, out float4 _return_value : COLOR)\n");

        } else {
            program.replace(int(found), 11, "void PS(float4 _vDiffuse : COLOR, float2 _uv : TEXCOORD0, float2 _rad_ang : TEXCOORD1, out float4 _return_value : COLOR)\n");

        }
    }
    else
        return GL_FALSE;

    // replace "{" with some variable declarations
    found = program.find('{',found);
    if (found != std::string::npos)
    {
        //std::cout << "first '{' found at: " << int(found) << std::endl;
        const char *progMain = \
        "{\n"
        "float3 ret = 0;\n";
        program.replace(int(found), 1, progMain);
    }
    else
        return GL_FALSE;

    pmShader.textures.clear();


    // Add builtin textures
    pmShader.textures["main"] = textureManager->getTexture("main", GL_REPEAT, GL_LINEAR);
    pmShader.textures["fc_main"] = textureManager->getTexture("main", GL_CLAMP_TO_EDGE, GL_LINEAR);
    pmShader.textures["pc_main"] = textureManager->getTexture("main", GL_CLAMP_TO_EDGE, GL_NEAREST);
    pmShader.textures["fw_main"] = textureManager->getTexture("main", GL_REPEAT, GL_LINEAR);
    pmShader.textures["pw_main"] = textureManager->getTexture("main", GL_REPEAT, GL_NEAREST);

    pmShader.textures["noise_lq"] = textureManager->getTexture("noise_lq", GL_CLAMP_TO_EDGE, GL_LINEAR);
    pmShader.textures["noise_lq_lite"] = textureManager->getTexture("noise_lq_lite", GL_CLAMP_TO_EDGE, GL_LINEAR);
    pmShader.textures["noise_mq"] = textureManager->getTexture("noise_mq", GL_CLAMP_TO_EDGE, GL_LINEAR);
    pmShader.textures["noise_hq"] = textureManager->getTexture("noise_hq", GL_CLAMP_TO_EDGE, GL_LINEAR);
    pmShader.textures["noisevol_lq"] = textureManager->getTexture("noisevol_lq", GL_CLAMP_TO_EDGE, GL_LINEAR);
    pmShader.textures["noisevol_hq"] = textureManager->getTexture("noisevol_hq", GL_CLAMP_TO_EDGE, GL_LINEAR);



    // set up texture samplers for all samplers references in the shader program
    found = 0;
    found = program.find("sampler_", found);
    while (found != std::string::npos)
    {
        found += 8;
        size_t end = program.find_first_of(" ;,\n\r)", found);

        if (end != std::string::npos)
        {
            std::string sampler = program.substr((int) found, (int) end - found);
            std::string lowerCaseName(sampler);
            std::transform(lowerCaseName.begin(), lowerCaseName.end(), lowerCaseName.begin(), tolower);

            TextureSamplerDesc texDesc = textureManager->getTexture(sampler, GL_REPEAT, GL_LINEAR);

            if (texDesc.first == NULL)
            {
                if (lowerCaseName.substr(0, 4) == "rand" || lowerCaseName.substr(2, 5) == "_rand")
                {
                    texDesc = textureManager->getRandomTextureName(sampler);
                }
                else
                {
                    texDesc = textureManager->tryLoadingTexture(sampler);
                }
            }

            if (texDesc.first == NULL)
            {
                std::string extensionList = "";
                // prepare a list of extensions we looked for, so it's more clear what a texture is in error logs.
                if (textureManager->extensions.size() >= 1) {
                    extensionList = " (";
                    for (size_t x = 0; x < textureManager->extensions.size(); x++)
                    {
                        if (x != (textureManager->extensions.size() - 1)) {
                            extensionList += textureManager->extensions[x] + ", ";
                        }
                        else {
                            extensionList += "or " + textureManager->extensions[x];
                        }
                    }
                    extensionList += ")";
                }
                std::cerr << "Texture loading error for: " << sampler << extensionList << std::endl;
            }
            else
            {
                std::map<std::string, TextureSamplerDesc>::const_iterator iter = pmShader.textures.cbegin();
                for ( ; iter != pmShader.textures.cend(); ++iter)
                {
                    if (iter->first == sampler)
                        break;
                }

                if (iter == pmShader.textures.cend())
                    pmShader.textures[sampler] = texDesc;
            }
        }

        found = program.find("sampler_", found);
    }

    textureManager->clearRandomTextures();

    found = program.find("GetBlur3");
    if (found != std::string::npos)
    {
        blur1_enabled = blur2_enabled = blur3_enabled = true;
        pmShader.textures["blur3"] = textureManager->getTexture("blur3", GL_CLAMP_TO_EDGE, GL_LINEAR);
        pmShader.textures["blur2"] = textureManager->getTexture("blur2", GL_CLAMP_TO_EDGE, GL_LINEAR);
        pmShader.textures["blur1"] = textureManager->getTexture("blur1", GL_CLAMP_TO_EDGE, GL_LINEAR);
    }
    else
    {
        found = program.find("GetBlur2");
        if (found != std::string::npos)
        {
            blur1_enabled = blur2_enabled = true;
            pmShader.textures["blur2"] = textureManager->getTexture("blur2", GL_CLAMP_TO_EDGE, GL_LINEAR);
            pmShader.textures["blur1"] = textureManager->getTexture("blur1", GL_CLAMP_TO_EDGE, GL_LINEAR);
        }
        else
        {
            found = program.find("GetBlur1");
            if (found != std::string::npos)
            {
                blur1_enabled = true;
                pmShader.textures["blur1"] = textureManager->getTexture("blur1", GL_CLAMP_TO_EDGE, GL_LINEAR);
            }
        }
    }

    std::string fullSource;

    // prepend our HLSL template to the actual program source
    fullSource.append(PresetShaderIncludes);

    if (shaderType == PresentWarpShader) {
        fullSource.append(  "#define rad _rad_ang.x\n"
                            "#define ang _rad_ang.y\n"
                            "#define uv _uv.xy\n"
                            "#define uv_orig _uv.zw\n");
    } else {
        fullSource.append(  "#define rad _rad_ang.x\n"
                            "#define ang _rad_ang.y\n"
                            "#define uv _uv.xy\n"
                            "#define uv_orig _uv.xy\n"
                            "#define hue_shader _vDiffuse.xyz\n");
    }


    fullSource.append(program);

    std::string shaderTypeString;
    switch(shaderType) {
    case PresentWarpShader: shaderTypeString = "Warp"; break;
    case PresentCompositeShader: shaderTypeString = "Comp"; break;
    case PresentBlur1Shader: shaderTypeString = "Blur1"; break;
    case PresentBlur2Shader: shaderTypeString = "Blur2"; break;
    default:    shaderTypeString = "Other";
    }

    M4::GLSLGenerator generator;
    M4::Allocator allocator;

    M4::HLSLTree tree( &allocator );
    M4::HLSLParser parser(&allocator, &tree);

    // preprocess define macros
    std::string sourcePreprocessed;
    if (!parser.ApplyPreprocessor(shaderFilename.c_str(), fullSource.c_str(), fullSource.size(), sourcePreprocessed)) {
        std::cerr << "Failed to preprocess HLSL(step1) " << shaderTypeString << " shader" << std::endl;

#if !DUMP_SHADERS_ON_ERROR
        std::cerr << "Source: " << std::endl << fullSource << std::endl;
#else
        std::ofstream out("/tmp/shader_" + shaderTypeString + "_step1.txt");
            out << fullSource;
            out.close();
#endif
            return GL_FALSE;
    }

    // Remove previous shader declarations
    std::smatch matches;
    while(std::regex_search(sourcePreprocessed, matches, std::regex("sampler(2D|3D|)(\\s+|\\().*"))) {
        sourcePreprocessed.replace(matches.position(), matches.length(), "");
    }

    // Remove previous texsize declarations
    while(std::regex_search(sourcePreprocessed, matches, std::regex("float4\\s+texsize_.*"))) {
        sourcePreprocessed.replace(matches.position(), matches.length(), "");
    }

    // Declare samplers
    std::set<std::string> texsizes;
    std::map<std::string, TextureSamplerDesc>::const_iterator iter_samplers = pmShader.textures.cbegin();
    for ( ; iter_samplers != pmShader.textures.cend(); ++iter_samplers)
    {
        Texture * texture = iter_samplers->second.first;

        if (texture->type == GL_TEXTURE_3D) {
            sourcePreprocessed.insert(0, "uniform sampler3D sampler_" + iter_samplers->first + ";\n");
        } else {
            sourcePreprocessed.insert(0, "uniform sampler2D sampler_" + iter_samplers->first + ";\n");
        }

        texsizes.insert(iter_samplers->first);
        texsizes.insert(texture->name);
    }

    // Declare texsizes
    std::set<std::string>::const_iterator iter_texsizes = texsizes.cbegin();
    for ( ; iter_texsizes != texsizes.cend(); ++iter_texsizes)
    {
        sourcePreprocessed.insert(0, "uniform float4 texsize_" + *iter_texsizes + ";\n");
    }


    // transpile from HLSL (aka preset shader aka directX shader) to GLSL (aka OpenGL shader lang)

    // parse
    if( !parser.Parse(shaderFilename.c_str(), sourcePreprocessed.c_str(), sourcePreprocessed.size()) ) {
        std::cerr << "Failed to parse HLSL(step2) " << shaderTypeString << " shader" << std::endl;

#if !DUMP_SHADERS_ON_ERROR
        std::cerr << "Source: " << std::endl << sourcePreprocessed << std::endl;
#else
        std::ofstream out2("/tmp/shader_" + shaderTypeString + "_step2.txt");
            out2 << sourcePreprocessed;
            out2.close();
#endif
            return GL_FALSE;
    }

    // generate GLSL
    if (!generator.Generate(&tree, M4::GLSLGenerator::Target_FragmentShader, SHADER_VERSION, "PS")) {
        std::cerr << "Failed to transpile HLSL(step3) " << shaderTypeString << " shader to GLSL" << std::endl;
#if !DUMP_SHADERS_ON_ERROR
        std::cerr << "Source: " << std::endl << sourcePreprocessed << std::endl;
#else
        std::ofstream out2("/tmp/shader_" + shaderTypeString + "_step2.txt");
            out2 << sourcePreprocessed;
            out2.close();
#endif
        return GL_FALSE;
    }

    // now we have GLSL source for the preset shader program (hopefully it's valid!)
    // copmile the preset shader fragment shader with the standard vertex shader and cross our fingers
    GLuint ret = 0;
    if (shaderType == PresentWarpShader) {
        ret = CompileShaderProgram(presetWarpVertexShader, generator.GetResult(), shaderTypeString);  // returns new program
    } else {
        ret = CompileShaderProgram(presetCompVertexShader, generator.GetResult(), shaderTypeString);  // returns new program
    }

    if (ret != GL_FALSE) {
#ifdef DEBUG
        std::cerr << "Successful compilation of " << shaderTypeString << std::endl;
#endif
    } else {
        std::cerr << "Compilation error (step3) of " << shaderTypeString << std::endl;

#if !DUMP_SHADERS_ON_ERROR
        std::cerr << "Source:" << std::endl << generator.GetResult() << std::endl;
#else
        std::ofstream out3("/tmp/shader_" + shaderTypeString + "_step3.txt");
            out3 << generator.GetResult();
            out3.close();
#endif
    }

    return ret;
}


void ShaderEngine::SetupShaderVariables(GLuint program, const Pipeline &pipeline, const PipelineContext &context)
{
    // pass info from projectM to the shader uniforms
    // these are the inputs: http://www.geisswerks.com/milkdrop/milkdrop_preset_authoring.html#3f6

    float time_since_preset_start = context.time - context.presetStartTime;
    float time_since_preset_start_wrapped = time_since_preset_start - (int)(time_since_preset_start/10000)*10000;
    float mip_x = logf((float)texsizeX)/logf(2.0f);
    float mip_y = logf((float)texsizeX)/logf(2.0f);
    float mip_avg = 0.5f*(mip_x + mip_y);

    glUniform4f(glGetUniformLocation(program, "rand_frame"), (rand() % 100) * .01, (rand() % 100) * .01, (rand()% 100) * .01, (rand() % 100) * .01);
    glUniform4f(glGetUniformLocation(program, "rand_preset"), rand_preset[0], rand_preset[1], rand_preset[2], rand_preset[3]);

    glUniform4f(glGetUniformLocation(program, "_c0"), aspectX, aspectY, 1 / aspectX, 1 / aspectY);
    glUniform4f(glGetUniformLocation(program, "_c1"), 0.0, 0.0, 0.0, 0.0);
    glUniform4f(glGetUniformLocation(program, "_c2"), time_since_preset_start_wrapped, context.fps,  context.frame, context.progress);
    glUniform4f(glGetUniformLocation(program, "_c3"), beatDetect->bass/100, beatDetect->mid/100, beatDetect->treb/100, beatDetect->vol/100);
    glUniform4f(glGetUniformLocation(program, "_c4"), beatDetect->bass_att/100, beatDetect->mid_att/100, beatDetect->treb_att/100, beatDetect->vol_att/100);
    glUniform4f(glGetUniformLocation(program, "_c5"), pipeline.blur1x-pipeline.blur1n, pipeline.blur1n, pipeline.blur2x-pipeline.blur2n, pipeline.blur2n);
    glUniform4f(glGetUniformLocation(program, "_c6"), pipeline.blur3x-pipeline.blur3n, pipeline.blur3n, pipeline.blur1n, pipeline.blur1x);
    glUniform4f(glGetUniformLocation(program, "_c7"), texsizeX, texsizeY, 1 / (float) texsizeX, 1 / (float) texsizeY);

    glUniform4f(glGetUniformLocation(program, "_c8"), 0.5f+0.5f*cosf(context.time* 0.329f+1.2f),
                                                                      0.5f+0.5f*cosf(context.time* 1.293f+3.9f),
                                                                      0.5f+0.5f*cosf(context.time* 5.070f+2.5f),
                                                                      0.5f+0.5f*cosf(context.time*20.051f+5.4f));

    glUniform4f(glGetUniformLocation(program, "_c9"), 0.5f+0.5f*sinf(context.time* 0.329f+1.2f),
                                                                      0.5f+0.5f*sinf(context.time* 1.293f+3.9f),
                                                                      0.5f+0.5f*sinf(context.time* 5.070f+2.5f),
                                                                      0.5f+0.5f*sinf(context.time*20.051f+5.4f));

    glUniform4f(glGetUniformLocation(program, "_c10"),  0.5f+0.5f*cosf(context.time*0.0050f+2.7f),
                                                                        0.5f+0.5f*cosf(context.time*0.0085f+5.3f),
                                                                        0.5f+0.5f*cosf(context.time*0.0133f+4.5f),
                                                                        0.5f+0.5f*cosf(context.time*0.0217f+3.8f));

    glUniform4f(glGetUniformLocation(program, "_c11"),  0.5f+0.5f*sinf(context.time*0.0050f+2.7f),
                                                                        0.5f+0.5f*sinf(context.time*0.0085f+5.3f),
                                                                        0.5f+0.5f*sinf(context.time*0.0133f+4.5f),
                                                                        0.5f+0.5f*sinf(context.time*0.0217f+3.8f));

    glUniform4f(glGetUniformLocation(program, "_c12"), mip_x, mip_y, mip_avg, 0 );
    glUniform4f(glGetUniformLocation(program, "_c13"), pipeline.blur2n, pipeline.blur2x, pipeline.blur3n, pipeline.blur3x);


    glm::mat4 temp_mat[24];

    // write matrices
    for (int i=0; i<20; i++)
    {
        glm::mat4 mx, my, mz, mxlate;

        mx = glm::rotate(glm::mat4(1.0f), rot_base[i].x + rot_speed[i].x*context.time, glm::vec3(1.0f, 0.0f, 0.0f));
        my = glm::rotate(glm::mat4(1.0f), rot_base[i].y + rot_speed[i].y*context.time, glm::vec3(0.0f, 1.0f, 0.0f));
        mz = glm::rotate(glm::mat4(1.0f), rot_base[i].z + rot_speed[i].z*context.time, glm::vec3(0.0f, 0.0f, 1.0f));

        mxlate = glm::translate(glm::mat4(1.0f), glm::vec3(xlate[i].x, xlate[i].y, xlate[i].z));

        temp_mat[i] = mxlate * mx;
        temp_mat[i] = mz * temp_mat[i];
        temp_mat[i] = my * temp_mat[i];
    }

    // the last 4 are totally random, each frame
    for (int i=20; i<24; i++)
    {
        glm::mat4 mx, my, mz, mxlate;

        mx = glm::rotate(glm::mat4(1.0f), FRAND * 6.28f, glm::vec3(1.0f, 0.0f, 0.0f));
        my = glm::rotate(glm::mat4(1.0f), FRAND * 6.28f, glm::vec3(0.0f, 1.0f, 0.0f));
        mz = glm::rotate(glm::mat4(1.0f), FRAND * 6.28f, glm::vec3(0.0f, 0.0f, 1.0f));

        mxlate = glm::translate(glm::mat4(1.0f), glm::vec3(FRAND, FRAND, FRAND));

        temp_mat[i] = mxlate * mx;
        temp_mat[i] = mz * temp_mat[i];
        temp_mat[i] = my * temp_mat[i];
    }

    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_s1"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[0])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_s2"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[1])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_s3"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[2])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_s4"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[3])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_d1"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[4])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_d2"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[5])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_d3"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[6])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_d4"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[7])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_f1"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[8])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_f2"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[9])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_f3"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[10])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_f4"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[11])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_vf1"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[12])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_vf2"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[13])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_vf3"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[14])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_vf4"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[15])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_uf1"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[16])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_uf2"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[17])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_uf3"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[18])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_uf4"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[19])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_rand1"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[20])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_rand2"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[21])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_rand3"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[22])));
    glUniformMatrix3x4fv(glGetUniformLocation(program, "rot_rand4"), 1, GL_FALSE, glm::value_ptr(glm::mat3x4(temp_mat[23])));

    // set program uniform "_q[a-h]" values (_qa.x, _qa.y, _qa.z, _qa.w, _qb.x, _qb.y ... ) alias q[1-32]
    for (int i=0; i < 32; i+=4) {
        std::string varName = "_q";
        varName.push_back('a' + i/4);
        glUniform4f(glGetUniformLocation(program, varName.c_str()), pipeline.q[i], pipeline.q[i+1], pipeline.q[i+2], pipeline.q[i+3]);
    }
}

void ShaderEngine::SetupTextures(GLuint program, const Shader &shader)
{

    unsigned int texNum = 0;
    std::map<std::string, Texture*> texsizes;

    // Set samplers
    for (std::map<std::string, TextureSamplerDesc>::const_iterator iter_samplers = shader.textures.begin(); iter_samplers
                    != shader.textures.end(); ++iter_samplers)
    {
        std::string texName = iter_samplers->first;
        Texture * texture = iter_samplers->second.first;
        Sampler * sampler = iter_samplers->second.second;
        std::string samplerName = "sampler_" + texName;

        // https://www.khronos.org/opengl/wiki/Sampler_(GLSL)#Binding_textures_to_samplers
        GLint param = glGetUniformLocation(program, samplerName.c_str());
        if (param < 0) {
            // unused uniform have been optimized out by glsl compiler
            continue;
        }

        texsizes[texName] = texture;
        texsizes[texture->name] = texture;

        glActiveTexture(GL_TEXTURE0 + texNum);
        glBindTexture(texture->type, texture->texID);
        glBindSampler(texNum, sampler->samplerID);

        glUniform1i(param, texNum);
        texNum++;
    }


    // Set texsizes
    std::map<std::string, Texture*>::const_iterator iter_textures = texsizes.cbegin();
    for ( ; iter_textures != texsizes.cend(); ++iter_textures)
    {
        Texture * texture = iter_textures->second;

        std::string texsizeName = "texsize_" + iter_textures->first;
        GLint textSizeParam = glGetUniformLocation(program, texsizeName.c_str());
        if (textSizeParam >= 0) {
            glUniform4f(textSizeParam, texture->width, texture->height,
                            1 / (float) texture->width, 1 / (float) texture->height);
        } else {
            // unused uniform have been optimized out by glsl compiler
            continue;
        }
    }
}


void ShaderEngine::RenderBlurTextures(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{
    unsigned int passes = 0;
    if (blur3_enabled) {
        passes = 6;
    } else if (blur2_enabled) {
        passes = 4;
    } else if (blur1_enabled) {
        passes = 2;
    } else {
        return;
    }

    const float w[8] = { 4.0f, 3.8f, 3.5f, 2.9f, 1.9f, 1.2f, 0.7f, 0.3f };  //<- user can specify these
    float edge_darken = pipeline.blur1ed;
    float blur_min[3], blur_max[3];

    blur_min[0] = pipeline.blur1n;
    blur_min[1] = pipeline.blur2n;
    blur_min[2] = pipeline.blur3n;
    blur_max[0] = pipeline.blur1x;
    blur_max[1] = pipeline.blur2x;
    blur_max[2] = pipeline.blur3x;

    // check that precision isn't wasted in later blur passes [...min-max gap can't grow!]
    // also, if min-max are close to each other, push them apart:
    const float fMinDist = 0.1f;
    if (blur_max[0] - blur_min[0] < fMinDist) {
        float avg = (blur_min[0] + blur_max[0])*0.5f;
        blur_min[0] = avg - fMinDist*0.5f;
        blur_max[0] = avg - fMinDist*0.5f;
    }
    blur_max[1] = std::min(blur_max[0], blur_max[1]);
    blur_min[1] = std::max(blur_min[0], blur_min[1]);
    if (blur_max[1] - blur_min[1] < fMinDist) {
        float avg = (blur_min[1] + blur_max[1])*0.5f;
        blur_min[1] = avg - fMinDist*0.5f;
        blur_max[1] = avg - fMinDist*0.5f;
    }
    blur_max[2] = std::min(blur_max[1], blur_max[2]);
    blur_min[2] = std::max(blur_min[1], blur_min[2]);
    if (blur_max[2] - blur_min[2] < fMinDist) {
        float avg = (blur_min[2] + blur_max[2])*0.5f;
        blur_min[2] = avg - fMinDist*0.5f;
        blur_max[2] = avg - fMinDist*0.5f;
    }

    float fscale[3];
    float fbias[3];

    // figure out the progressive scale & bias needed, at each step,
    // to go from one [min..max] range to the next.
    float temp_min, temp_max;
    fscale[0] = 1.0f / (blur_max[0] - blur_min[0]);
    fbias [0] = -blur_min[0] * fscale[0];
    temp_min  = (blur_min[1] - blur_min[0]) / (blur_max[0] - blur_min[0]);
    temp_max  = (blur_max[1] - blur_min[0]) / (blur_max[0] - blur_min[0]);
    fscale[1] = 1.0f / (temp_max - temp_min);
    fbias [1] = -temp_min * fscale[1];
    temp_min  = (blur_min[2] - blur_min[1]) / (blur_max[1] - blur_min[1]);
    temp_max  = (blur_max[2] - blur_min[1]) / (blur_max[1] - blur_min[1]);
    fscale[2] = 1.0f / (temp_max - temp_min);
    fbias [2] = -temp_min * fscale[2];

    const std::vector<Texture*> & blurTextures = textureManager->getBlurTextures();
    const Texture * mainTexture = textureManager->getMainTexture();

    glBlendFunc(GL_ONE, GL_ZERO);
    glBindVertexArray(vaoBlur);

    for (unsigned int i=0; i<passes; i++)
    {
        // set pixel shader
        if ((i%2) == 0) {
            glUseProgram(programID_blur1);
            glUniform1i(uniform_blur1_sampler, 0);

        } else {
            glUseProgram(programID_blur2);
            glUniform1i(uniform_blur2_sampler, 0);

        }

        glViewport(0, 0, blurTextures[i]->width, blurTextures[i]->height);

        // hook up correct source texture - assume there is only one, at stage 0
        glActiveTexture(GL_TEXTURE0);
        if (i == 0) {
            glBindTexture(GL_TEXTURE_2D, mainTexture->texID);
        } else {
            glBindTexture(GL_TEXTURE_2D, blurTextures[i-1]->texID);
        }

        float srcw = (i==0) ? mainTexture->width : blurTextures[i-1]->width;
        float srch = (i==0) ? mainTexture->height : blurTextures[i-1]->height;


        float fscale_now = fscale[i/2];
        float fbias_now  = fbias[i/2];

        // set constants
        if (i%2==0)
        {
            // pass 1 (long horizontal pass)
            //-------------------------------------
            const float w1 = w[0] + w[1];
            const float w2 = w[2] + w[3];
            const float w3 = w[4] + w[5];
            const float w4 = w[6] + w[7];
            const float d1 = 0 + 2*w[1]/w1;
            const float d2 = 2 + 2*w[3]/w2;
            const float d3 = 4 + 2*w[5]/w3;
            const float d4 = 6 + 2*w[7]/w4;
            const float w_div = 0.5f/(w1+w2+w3+w4);
            //-------------------------------------
            //float4 _c0; // source texsize (.xy), and inverse (.zw)
            //float4 _c1; // w1..w4
            //float4 _c2; // d1..d4
            //float4 _c3; // scale, bias, w_div, 0
            //-------------------------------------
            glUniform4f(uniform_blur1_c0, srcw, srch, 1.0f/srcw, 1.0f/srch);
            glUniform4f(uniform_blur1_c1, w1,w2,w3,w4);
            glUniform4f(uniform_blur1_c2, d1,d2,d3,d4);
            glUniform4f(uniform_blur1_c3, fscale_now, fbias_now, w_div, 0.0);
        }
        else
        {
            // pass 2 (short vertical pass)
            //-------------------------------------
            const float w1 = w[0]+w[1] + w[2]+w[3];
            const float w2 = w[4]+w[5] + w[6]+w[7];
            const float d1 = 0 + 2*((w[2]+w[3])/w1);
            const float d2 = 2 + 2*((w[6]+w[7])/w2);
            const float w_div = 1.0f/((w1+w2)*2);
            //-------------------------------------
            //float4 _c0; // source texsize (.xy), and inverse (.zw)
            //float4 _c5; // w1,w2,d1,d2
            //float4 _c6; // w_div, edge_darken_c1, edge_darken_c2, edge_darken_c3
            //-------------------------------------
            glUniform4f(uniform_blur2_c0, srcw, srch, 1.0f/srcw, 1.0f/srch);
            glUniform4f(uniform_blur2_c5, w1,w2,d1,d2);
            // note: only do this first time; if you do it many times,
            // then the super-blurred levels will have big black lines along the top & left sides.
            if (i==1)
                glUniform4f(uniform_blur2_c6, w_div, (1-edge_darken), edge_darken, 5.0f); //darken edges
            else
                glUniform4f(uniform_blur2_c6, w_div, 1.0f, 0.0f, 5.0f); // don't darken
        }

        // draw fullscreen quad
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // save to blur texture
        glBindTexture(GL_TEXTURE_2D, blurTextures[i]->texID);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, blurTextures[i]->width, blurTextures[i]->height);
    }

    glBindVertexArray(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, 0);
}

bool ShaderEngine::linkProgram(GLuint programID) {
    glLinkProgram(programID);

    GLint program_linked;
    glGetProgramiv(programID, GL_LINK_STATUS, &program_linked);
    if (program_linked == GL_TRUE) {
        return true;	// success
	}

    int InfoLogLength;
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        std::cerr << "Failed to link program: " << &ProgramErrorMessage[0] << std::endl;
    }

    return false;
}


bool ShaderEngine::loadPresetShaders(Pipeline &pipeline, const std::string & presetName) {

    bool ok = true;

    // blur programs
    blur1_enabled = false;
    blur2_enabled = false;
    blur3_enabled = false;

    m_presetName = presetName;

    // compile and link warp and composite shaders from pipeline
    if (!pipeline.warpShader.programSource.empty()) {
        programID_presetWarp = loadPresetShader(PresentWarpShader, pipeline.warpShader, pipeline.warpShaderFilename);
        if (programID_presetWarp != GL_FALSE) {
            uniform_vertex_transf_warp_shader = glGetUniformLocation(programID_presetWarp, "vertex_transformation");
            presetWarpShaderLoaded = true;
        } else {
            ok = false;
        }
    }

    if (!pipeline.compositeShader.programSource.empty()) {
        programID_presetComp = loadPresetShader(PresentCompositeShader, pipeline.compositeShader, pipeline.compositeShaderFilename);
        if (programID_presetComp != GL_FALSE) {
            presetCompShaderLoaded = true;
        } else {
            ok = false;
        }
    }

//    std::cout << "Preset composite shader active: " << presetCompShaderLoaded << ", preset warp shader active: " << presetWarpShaderLoaded << std::endl;

    return ok;
}

GLuint ShaderEngine::loadPresetShader(const ShaderEngine::PresentShaderType shaderType, Shader &presetShader, std::string &shaderFilename) {
    GLuint program = compilePresetShader(shaderType, presetShader, shaderFilename);

    if (program == GL_FALSE) {
        // failed to compile
        return GL_FALSE;
    }

    return program;
}

// deactivate preset shaders
void ShaderEngine::disablePresetShaders() {
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
    rand_preset[0] = FRAND;
    rand_preset[1] = FRAND;
    rand_preset[2] = FRAND;
    rand_preset[3] = FRAND;

    unsigned int k = 0;
    do
    {
        for (int i=0; i<4; i++)
        {
            float xlate_mult = 1;
            float rot_mult = 0.9f*powf(k/8.0f, 3.2f);
            xlate[k].x = (FRAND*2-1)*xlate_mult;
            xlate[k].y = (FRAND*2-1)*xlate_mult;
            xlate[k].z = (FRAND*2-1)*xlate_mult;
            rot_base[k].x = FRAND*6.28f;
            rot_base[k].y = FRAND*6.28f;
            rot_base[k].z = FRAND*6.28f;
            rot_speed[k].x = (FRAND*2-1)*rot_mult;
            rot_speed[k].y = (FRAND*2-1)*rot_mult;
            rot_speed[k].z = (FRAND*2-1)*rot_mult;
            k++;
        }
    }
    while (k < sizeof(xlate)/sizeof(xlate[0]));
}

GLuint ShaderEngine::CompileShaderProgram(const std::string & VertexShaderCode, const std::string & FragmentShaderCode, const std::string & shaderTypeString){

#if defined(WIN32) && !defined(EYETUNE_WINRT)
	GLenum err = glewInit();
#endif /** WIN32 */

	// Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);


    // Compile Vertex Shader
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
    checkCompileStatus(VertexShaderID, "Vertex: " + shaderTypeString);


    // Compile Fragment Shader
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
    checkCompileStatus(FragmentShaderID, "Fragment: " + shaderTypeString);


    // Link the program
    GLuint programID = glCreateProgram();

    glAttachShader(programID, VertexShaderID);
    glAttachShader(programID, FragmentShaderID);
    bool linkOK = linkProgram(programID);

    glDetachShader(programID, VertexShaderID);
    glDetachShader(programID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return linkOK ? programID : GL_FALSE;
}

void ShaderEngine::validateProgram(const GLuint programID) {
    GLint Result = GL_FALSE;
    int InfoLogLength;

    glValidateProgram(programID);

    // Check the program
    glGetProgramiv(programID, GL_VALIDATE_STATUS, &Result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        fprintf(stderr, "%s\n", &ProgramErrorMessage[0]);
    }
}

// use the appropriate shader program for rendering the interpolation.
// it will use the preset shader if available, otherwise the textured shader
bool ShaderEngine::enableWarpShader(Shader &shader, const Pipeline &pipeline, const PipelineContext &pipelineContext, const glm::mat4 & mat_ortho) {
    if (presetWarpShaderLoaded) {
        glUseProgram(programID_presetWarp);

        SetupTextures(programID_presetWarp, shader);

        SetupShaderVariables(programID_presetWarp, pipeline, pipelineContext);

        glUniformMatrix4fv(uniform_vertex_transf_warp_shader, 1, GL_FALSE, glm::value_ptr(mat_ortho));

#if OGL_DEBUG
        validateProgram(programID_presetWarp);
#endif

        return true;
    }

    glUseProgram(programID_v2f_c4f_t2f);

    glUniformMatrix4fv(uniform_v2f_c4f_t2f_vertex_tranformation, 1, GL_FALSE, glm::value_ptr(mat_ortho));
    glUniform1i(uniform_v2f_c4f_t2f_frag_texture_sampler, 0);

    return false;
}

bool ShaderEngine::enableCompositeShader(Shader &shader, const Pipeline &pipeline, const PipelineContext &pipelineContext) {
    if (presetCompShaderLoaded) {
        glUseProgram(programID_presetComp);

        SetupTextures(programID_presetComp, shader);

        SetupShaderVariables(programID_presetComp, pipeline, pipelineContext);

#if OGL_DEBUG
        validateProgram(programID_presetComp);
#endif

        return true;
    }

    glUseProgram(programID_v2f_c4f_t2f);

    return false;
}

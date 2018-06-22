/*
 * ShaderEngine.cpp
 *
 *  Created on: Jul 18, 2008
 *      Author: pete
 */
#include <fstream>
#include <algorithm>
#include "PerlinNoise.hpp"
#include "ShaderEngine.hpp"
#include "BeatDetect.hpp"
#include "HLSLTranslator.hpp"

#ifdef USE_GLES
    #define GLSL_VERSION "300 es"
#else
    #define GLSL_VERSION "410"
#endif

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
    ""
    "uniform mat4 vertex_transformation;\n"
    ""
    "out vec4 frag_COLOR;\n"
    "out vec2 frag_TEXCOORD0;\n"
    "out vec2 frag_TEXCOORD1;\n"
    ""
    "void main(){\n"
    "    vec4 position = vertex_transformation * vec4(vertex_position, 0.0, 1.0);\n"
    "    gl_Position = position;\n"
    "    frag_COLOR = vertex_color;\n"
    "    frag_TEXCOORD0 = vertex_texture;\n"
    "    frag_TEXCOORD1 = vec2(0.0, 0.0);\n"
    "}\n");


std::string v2f_c4f_vert(
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

std::string v2f_c4f_frag(
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

std::string v2f_c4f_t2f_vert(
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

std::string v2f_c4f_t2f_frag(
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
//        "float2 a = 1.0 + 2.0 * 3.0 + 4.0;\n"
//        "float2 _uv, dx, dy, _c7;\n"
//        "float2 uv_y =(_uv.xy)-( float2(0,-1.2) + float2(dx.y,dy.y)*8 + float2(dx.x,dy.x)*4 )*(_c7).zw;\n"

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

"// previous-frame-image samplers:\n"
"uniform sampler2D sampler_main;\n"
"uniform sampler2D sampler_fc_main;\n"
"uniform sampler2D sampler_pc_main;\n"
"uniform sampler2D sampler_fw_main;\n"
"uniform sampler2D sampler_pw_main;\n"
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



"// built-in noise textures:\n"
"uniform sampler2D sampler_noise_lq;\n"
"uniform sampler2D sampler_noise_lq_lite;\n"
"uniform sampler2D sampler_noise_mq;\n"
"uniform sampler2D sampler_noise_hq;\n"
"uniform sampler3D sampler_noisevol_lq;\n"
"uniform sampler3D sampler_noisevol_hq;\n"
"uniform float4 texsize_noise_lq;\n"
"uniform float4 texsize_noise_lq_lite;\n"
"uniform float4 texsize_noise_mq;\n"
"uniform float4 texsize_noise_hq;\n"
"uniform float4 texsize_noisevol_lq;\n"
"uniform float4 texsize_noisevol_hq;\n"

"// procedural blur textures:\n"
"uniform sampler2D sampler_blur1;\n"
"uniform sampler2D sampler_blur2;\n"
"uniform sampler2D sampler_blur3;\n"
;


std::string BlurShader1(
"	uniform sampler2D sampler_main;\n"
"	uniform float4 _c0; // source texsize (.xy), and inverse (.zw)\n"
"	uniform float4 _c1; // w1..w4\n"
"	uniform float4 _c2; // d1..d4\n"
"	uniform float4 _c3; // scale, bias, w_div\n"

"	void PS( float2 uv       : TEXCOORD,\n"
"	     out float4 ret      : COLOR0      )\n"
"       {\n"
"	    // LONG HORIZ. PASS 1:\n"
"	    #define srctexsize _c0\n"
"	    #define w1 _c1.x\n"
"	    #define w2 _c1.y\n"
"	    #define w3 _c1.z\n"
"	    #define w4 _c1.w\n"
"	    #define d1 _c2.x\n"
"	    #define d2 _c2.y\n"
"	    #define d3 _c2.z\n"
"	    #define d4 _c2.w\n"
"	    #define fscale _c3.x\n"
"	    #define fbias  _c3.y\n"
"	    #define w_div  _c3.z\n"

"	    // note: if you just take one sample at exactly uv.xy, you get an avg of 4 pixels.\n"
"	    float2 uv2 = uv.xy + srctexsize.zw*float2(1,1);     // + moves blur UP, LEFT by 1-pixel increments\n"

"	    float3 blur = \n"
"	            ( tex2D( sampler_main, uv2 + float2( d1*srctexsize.z,0) ).xyz\n"
"	            + tex2D( sampler_main, uv2 + float2(-d1*srctexsize.z,0) ).xyz)*w1 +\n"
"	            ( tex2D( sampler_main, uv2 + float2( d2*srctexsize.z,0) ).xyz\n"
"	            + tex2D( sampler_main, uv2 + float2(-d2*srctexsize.z,0) ).xyz)*w2 +\n"
"	            ( tex2D( sampler_main, uv2 + float2( d3*srctexsize.z,0) ).xyz\n"
"	            + tex2D( sampler_main, uv2 + float2(-d3*srctexsize.z,0) ).xyz)*w3 +\n"
"	            ( tex2D( sampler_main, uv2 + float2( d4*srctexsize.z,0) ).xyz\n"
"	            + tex2D( sampler_main, uv2 + float2(-d4*srctexsize.z,0) ).xyz)*w4\n"
"	            ;\n"
"	    blur.xyz *= w_div;\n"

"	    blur.xyz = blur.xyz*fscale + fbias;\n"

"	    ret.xyz = blur;\n"
"	    ret.w   = 1;\n"
"	}\n"
);

std::string BlurShader2(
"	uniform sampler2D sampler_main;\n"
"	uniform float4 _c0; // source texsize (.xy), and inverse (.zw)\n"
"	uniform float4 _c5; // w1,w2,d1,d2\n"
"	uniform float4 _c6; // w_div, edge_darken_c1, edge_darken_c2, edge_darken_c3\n"

"	void PS( float2 uv       : TEXCOORD,\n"
"	     out float4 ret      : COLOR0      )\n"
"       {\n"
"	    //SHORT VERTICAL PASS 2:\n"
"	    #define srctexsize _c0\n"
"	    #define w1 _c5.x\n"
"	    #define w2 _c5.y\n"
"	    #define d1 _c5.z\n"
"	    #define d2 _c5.w\n"
"	    #define edge_darken_c1 _c6.y\n"
"	    #define edge_darken_c2 _c6.z\n"
"	    #define edge_darken_c3 _c6.w\n"
"	    #define w_div   _c6.x\n"

"	    // note: if you just take one sample at exactly uv.xy, you get an avg of 4 pixels.\n"
"	    float2 uv2 = uv.xy + srctexsize.zw*float2(1,0);     // + moves blur UP, LEFT by TWO-pixel increments! (since texture is 1/2 the size of blur1_ps)\n"

"	    float3 blur = \n"
"	            ( tex2D( sampler_main, uv2 + float2(0, d1*srctexsize.w) ).xyz\n"
"	            + tex2D( sampler_main, uv2 + float2(0,-d1*srctexsize.w) ).xyz)*w1 +\n"
"	            ( tex2D( sampler_main, uv2 + float2(0, d2*srctexsize.w) ).xyz\n"
"	            + tex2D( sampler_main, uv2 + float2(0,-d2*srctexsize.w) ).xyz)*w2\n"
"	            ;\n"
"	    blur.xyz *= w_div;\n"

"	    // tone it down at the edges:  (only happens on 1st X pass!)\n"
"	    float t = min( min(uv.x, uv.y), 1-max(uv.x,uv.y) );\n"
"	    t = sqrt(t);\n"
"	    t = edge_darken_c1 + edge_darken_c2*saturate(t*edge_darken_c3);\n"
"	    blur.xyz *= t;\n"

"	    ret.xyz = blur;\n"
"	    ret.w   = 1;\n"
"	}\n");



GLint ShaderEngine::UNIFORM_V2F_C4F_VERTEX_TRANFORMATION = 0;
GLint ShaderEngine::UNIFORM_V2F_C4F_VERTEX_POINT_SIZE = 0;
GLint ShaderEngine::UNIFORM_V2F_C4F_T2F_VERTEX_TRANFORMATION = 0;
GLint ShaderEngine::UNIFORM_V2F_C4F_T2F_FRAG_TEXTURE_SAMPLER = 0;



ShaderEngine::ShaderEngine() : presetCompShaderLoaded(false), presetWarpShaderLoaded(false)
{
    programID_v2f_c4f = CompileShaderProgram(v2f_c4f_vert, v2f_c4f_frag, "v2f_c4f");
    programID_v2f_c4f_t2f = CompileShaderProgram(v2f_c4f_t2f_vert, v2f_c4f_t2f_frag, "v2f_c4f_t2f");

    UNIFORM_V2F_C4F_VERTEX_TRANFORMATION = glGetUniformLocation(programID_v2f_c4f, "vertex_transformation");
    UNIFORM_V2F_C4F_VERTEX_POINT_SIZE = glGetUniformLocation(programID_v2f_c4f, "vertex_point_size");
    UNIFORM_V2F_C4F_T2F_VERTEX_TRANFORMATION = glGetUniformLocation(programID_v2f_c4f_t2f, "vertex_transformation");
    UNIFORM_V2F_C4F_T2F_FRAG_TEXTURE_SAMPLER = glGetUniformLocation(programID_v2f_c4f_t2f, "texture_sampler");
}

ShaderEngine::~ShaderEngine()
{
    glDeleteProgram(programID_v2f_c4f);
    glDeleteProgram(programID_v2f_c4f_t2f);

    glDeleteProgram(programID_blur1);
    glDeleteProgram(programID_blur2);

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

void ShaderEngine::setParams(const int texsize, const unsigned int texId, const float aspect, BeatDetect *beatDetect,
		TextureManager *textureManager)
{
	mainTextureId = texId;
	this->beatDetect = beatDetect;
	this->textureManager = textureManager;
	this->aspect = aspect;
	this->texsize = texsize;

	textureManager->setTexture("main", texId, texsize, texsize);

#ifndef GL_TRANSITION
	glGenTextures(1, &blur1_tex);
	glBindTexture(GL_TEXTURE_2D, blur1_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texsize/2, texsize/2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenTextures(1, &blur2_tex);
	glBindTexture(GL_TEXTURE_2D, blur2_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texsize / 4, texsize / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenTextures(1, &blur3_tex);
	glBindTexture(GL_TEXTURE_2D, blur3_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texsize / 8, texsize / 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#endif

	blur1_enabled = false;
	blur2_enabled = false;
	blur3_enabled = false;

	//std::cout << "Generating Noise Textures" << std::endl;

	PerlinNoise noise;
#ifndef GL_TRANSITION
	glGenTextures(1, &noise_texture_lq_lite);
	glBindTexture(GL_TEXTURE_2D, noise_texture_lq_lite);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 32, 32, 0, GL_LUMINANCE, GL_FLOAT, noise.noise_lq_lite);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	textureManager->setTexture("noise_lq_lite", noise_texture_lq_lite, 32, 32);

	glGenTextures(1, &noise_texture_lq);
	glBindTexture(GL_TEXTURE_2D, noise_texture_lq);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 256, 256, 0, GL_LUMINANCE, GL_FLOAT, noise.noise_lq);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	textureManager->setTexture("noise_lq", noise_texture_lq, 256, 256);

	glGenTextures(1, &noise_texture_mq);
	glBindTexture(GL_TEXTURE_2D, noise_texture_mq);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 256, 256, 0, GL_LUMINANCE, GL_FLOAT, noise.noise_mq);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	textureManager->setTexture("noise_mq", noise_texture_mq, 256, 256);

	glGenTextures(1, &noise_texture_hq);
	glBindTexture(GL_TEXTURE_2D, noise_texture_hq);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 256, 256, 0, GL_LUMINANCE, GL_FLOAT, noise.noise_hq);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	textureManager->setTexture("noise_hq", noise_texture_hq, 256, 256);

	glGenTextures(1, &noise_texture_perlin);
	glBindTexture(GL_TEXTURE_2D, noise_texture_perlin);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 512, 512, 0, GL_LUMINANCE, GL_FLOAT, noise.noise_perlin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	textureManager->setTexture("noise_perlin", noise_texture_perlin, 512, 512);

	 glGenTextures( 1, &noise_texture_lq_vol );
	 glBindTexture( GL_TEXTURE_3D, noise_texture_lq_vol );
	 glTexImage3D(GL_TEXTURE_3D,0,4,32,32,32,0,GL_LUMINANCE,GL_FLOAT,noise.noise_lq_vol);
	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	 textureManager->setTexture("noisevol_lq", noise_texture_lq_vol, 256, 256);

	 glGenTextures( 1, &noise_texture_hq_vol );
	 glBindTexture( GL_TEXTURE_3D, noise_texture_hq_vol );
	 glTexImage3D(GL_TEXTURE_3D,0,4,32,32,32,0,GL_LUMINANCE,GL_FLOAT,noise.noise_hq_vol);
	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	 textureManager->setTexture("noisevol_hq", noise_texture_hq_vol, 8, 8);
#endif
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
    size_t program_start = found;
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

    std::vector<std::string> standard_samplers = {
        "main",
        "fc_main",
        "pc_main",
        "fw_main",
        "pw_main",
        "noise_lq",
        "noise_lq_lite",
        "noise_mq",
        "noise_hq",
        "noisevol_lq",
        "noisevol_hq",
        "blur1",
        "blur2",
        "blur3",
    };


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
            UserTexture* texture = new UserTexture(sampler);

            texture->texID = textureManager->getTexture(texture->name);
            if (texture->texID != 0)
            {
                texture->width = textureManager->getTextureWidth(texture->name);
                texture->height = textureManager->getTextureHeight(texture->name);
            }
            else
            {

                // Insert uniform keyword for GLSL sampler declarations
                std::string sampler_lowercase(sampler);
                std::transform(sampler_lowercase.begin(), sampler_lowercase.end(), sampler_lowercase.begin(), ::tolower);
                std::vector<std::string>::iterator it = standard_samplers.begin();
                it = std::find(standard_samplers.begin(), standard_samplers.end(), sampler_lowercase);

                // Declare a uniform for sampler defined in preset but out from the shader body
                if (it == standard_samplers.end() && found < program_start) {
                    int index = found;
                    while(index >= 0 && program[index] != '\n') { index--; }
                    program.insert(index+1, "uniform ");
                    found += 8;
                    program_start += 8;
                }

                if (sampler.substr(0, 4) == "rand")
                {
                    std::string random_name = textureManager->getRandomTextureName(texture->name);
                    if (random_name.size() > 0)
                    {
                        texture->texID = textureManager->getTexture(random_name);
                        texture->width = textureManager->getTextureWidth(random_name);
                        texture->height = textureManager->getTextureHeight(random_name);
                    }
                }
                else
                {
                    std::string extensions[6];
                    extensions[0] = ".jpg";
                    extensions[1] = ".dds";
                    extensions[2] = ".png";
                    extensions[3] = ".tga";
                    extensions[4] = ".bmp";
                    extensions[5] = ".dib";

                    for (int x = 0; x < 6; x++)
                    {

                        std::string filename = texture->name + extensions[x];
                        texture->texID = textureManager->getTexture(filename);
                        if (texture->texID != 0)
                        {
                            texture->width = textureManager->getTextureWidth(filename);
                            texture->height = textureManager->getTextureHeight(filename);
                            break;
                        }
                    }

                }
            }
            if (texture->texID != 0 && pmShader.textures.find(texture->qname) == pmShader.textures.end())
                pmShader.textures[texture->qname] = texture;

            else
                delete (texture);
        }

        found = program.find("sampler_", found);
    }
    textureManager->clearRandomTextures();

    // add texture size vars
    found = 0;
    found = program.find("texsize_", found);
    while (found != std::string::npos)
    {
        found += 8;
        size_t end = program.find_first_of(" ;.,\n\r)", found);

        if (end != std::string::npos)
        {
            std::string tex = program.substr((int) found, (int) end - found);
            if (pmShader.textures.find(tex) != pmShader.textures.end())
            {
                UserTexture* texture = pmShader.textures[tex];
                texture->texsizeDefined = true;
                //std::cout << "texsize_" << tex << " found" << std::endl;
            }
        }
        found = program.find("texsize_", found);
    }

    // blur programs
    blur1_enabled = false;
    blur2_enabled = false;
    blur3_enabled = false;

    found = program.find("GetBlur3");
    if (found != std::string::npos)
        blur1_enabled = blur2_enabled = blur3_enabled = true;
    else
    {
        found = program.find("GetBlur2");
        if (found != std::string::npos)
            blur1_enabled = blur2_enabled = true;
        else
        {
            found = program.find("GetBlur1");
            if (found != std::string::npos)
                blur1_enabled = true;
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

    // transpile from HLSL (aka preset shader aka directX shader) to GLSL (aka OpenGL shader lang)
    HLSLTranslator translator = HLSLTranslator();
    std::unique_ptr<std::string> glslSource = translator.parse(shaderTypeString, shaderFilename.c_str(), fullSource);
    if (!glslSource) {
        std::cerr << "Failed to translate " << shaderTypeString << std::endl;
        return GL_FALSE;
    }

    // now we have GLSL source for the preset shader program (hopefully it's valid!)
    // copmile the preset shader fragment shader with the standard vertex shader and cross our fingers
    GLuint ret = 0;
    if (shaderType == PresentWarpShader) {
        ret = CompileShaderProgram(presetWarpVertexShader, *glslSource.get(), shaderTypeString);  // returns new program
    } else {
        ret = CompileShaderProgram(presetCompVertexShader, *glslSource.get(), shaderTypeString);  // returns new program
    }

    if (ret != GL_FALSE) {
        std::cerr << "Successfull compilation of " << shaderTypeString << std::endl;
    } else {
        std::cerr << "Compilation error of " << shaderTypeString << std::endl;

#ifndef DUMP_SHADERS_ON_ERROR
        std::cerr << "Source:" << std::endl << *glslSource.get() << std::endl;
#else
        std::ofstream out2("/tmp/shader_glsl_" + shaderTypeString + ".txt");
            out2 << *glslSource.get();
            out2.close();
#endif
    }

    return ret;
}


void ShaderEngine::SetupShaderVariables(GLuint program, const Pipeline &pipeline, const PipelineContext &context)
{
    // pass info from projectM to the shader uniforms
    // these are the inputs: http://www.geisswerks.com/milkdrop/milkdrop_preset_authoring.html#3f6

	GLfloat slow_roam_cos[4] =	{ 0.5f + 0.5f * (float)cos(context.time * 0.005), 0.5f + 0.5f * (float)cos(context.time * 0.008), 0.5f + 0.5f * (float)cos(context.time * 0.013), 0.5f + 0.5f * (float)cos(context.time * 0.022) };
	GLfloat roam_cos[4] =	{ 0.5f + 0.5f * cosf(context.time * 0.3), 0.5f + 0.5f * cosf(context.time * 1.3), 0.5f + 0.5f * cosf(context.time * 5), 0.5f + 0.5f * cosf(context.time * 20) };
	GLfloat slow_roam_sin[4] =	{ 0.5f + 0.5f * sinf(context.time * 0.005), 0.5f + 0.5f * sinf(context.time * 0.008), 0.5f + 0.5f * sinf(context.time * 0.013), 0.5f + 0.5f * sinf(context.time * 0.022) };
	GLfloat roam_sin[4] =	{ 0.5f + 0.5f * sinf(context.time * 0.3), 0.5f + 0.5f * sinf(context.time * 1.3), 0.5f + 0.5f * sinf(context.time * 5), 0.5f + 0.5f * sinf(context.time * 20) };

	glProgramUniform4fv(program, glGetUniformLocation(program, "slow_roam_cos"), 4, slow_roam_cos);
	glProgramUniform4fv(program, glGetUniformLocation(program, "roam_cos"), 4, roam_cos);
	glProgramUniform4fv(program, glGetUniformLocation(program, "slow_roam_sin"), 4, slow_roam_sin);
	glProgramUniform4fv(program, glGetUniformLocation(program, "roam_sin"), 4, roam_sin);

	glProgramUniform1f(program, glGetUniformLocation(program, "time"), context.time);
	glProgramUniform4f(program, glGetUniformLocation(program, "rand_preset"), rand_preset[0], rand_preset[1], rand_preset[2],	rand_preset[3]);
	glProgramUniform4f(program, glGetUniformLocation(program, "rand_frame"), (rand() % 100) * .01, (rand() % 100) * .01, (rand()% 100) * .01, (rand() % 100) * .01);
	glProgramUniform1f(program, glGetUniformLocation(program, "fps"), context.fps);
	glProgramUniform1f(program, glGetUniformLocation(program, "frame"), context.frame);
	glProgramUniform1f(program, glGetUniformLocation(program, "progress"), context.progress);

	glProgramUniform1f(program, glGetUniformLocation(program, "blur1_min"), pipeline.blur1n);
	glProgramUniform1f(program, glGetUniformLocation(program, "blur1_max"), pipeline.blur1x);
	glProgramUniform1f(program, glGetUniformLocation(program, "blur2_min"), pipeline.blur2n);
	glProgramUniform1f(program, glGetUniformLocation(program, "blur2_max"), pipeline.blur2x);
	glProgramUniform1f(program, glGetUniformLocation(program, "blur3_min"), pipeline.blur3n);
	glProgramUniform1f(program, glGetUniformLocation(program, "blur3_max"), pipeline.blur3x);

	glProgramUniform1f(program, glGetUniformLocation(program, "bass"), beatDetect->bass);
	glProgramUniform1f(program, glGetUniformLocation(program, "mid"), beatDetect->mid);
	glProgramUniform1f(program, glGetUniformLocation(program, "treb"), beatDetect->treb);
	glProgramUniform1f(program, glGetUniformLocation(program, "bass_att"), beatDetect->bass_att);
	glProgramUniform1f(program, glGetUniformLocation(program, "mid_att"), beatDetect->mid_att);
	glProgramUniform1f(program, glGetUniformLocation(program, "treb_att"), beatDetect->treb_att);
	glProgramUniform1f(program, glGetUniformLocation(program, "vol"), beatDetect->vol);
	glProgramUniform1f(program, glGetUniformLocation(program, "vol_att"), beatDetect->vol);

    glProgramUniform4f(program, glGetUniformLocation(program, "texsize"), texsize, texsize, 1 / (float) texsize, 1 / (float) texsize);
	glProgramUniform4f(program, glGetUniformLocation(program, "aspect"), 1 / aspect, 1, aspect, 1);

    /*
	if (blur1_enabled)
	{
		cgGLSetTextureParameter(program, glGetUniformLocation(program, "sampler_blur1"), blur1_tex);
		cgGLEnableTextureParameter(program, glGetUniformLocation(program, "sampler_blur1"));
	}
	if (blur2_enabled)
	{
		cgGLSetTextureParameter(glGetUniformLocation(program, "sampler_blur2"), blur2_tex);
		cgGLEnableTextureParameter(glGetUniformLocation(program, "sampler_blur2"));
	}
	if (blur3_enabled)
	{
		cgGLSetTextureParameter(glGetUniformLocation(program, "sampler_blur3"), blur3_tex);
		cgGLEnableTextureParameter(glGetUniformLocation(program, "sampler_blur3"));
	}
     */
}

void ShaderEngine::setupUserTexture(GLuint program, const UserTexture* texture)
{
	std::string samplerName = "sampler_" + texture->qname;

 	// FIXME: check if each texture binding will overwrite previous one

    // https://www.khronos.org/opengl/wiki/Sampler_(GLSL)#Binding_textures_to_samplers
	GLint param = glGetUniformLocation(program, samplerName.c_str());
    if (param < 0) {
        // FIXME: turn this on and fix it.
        // i think sampler names are carrying over from previous shaders...
//        std::cerr << "invalid uniform name " << samplerName << std::endl;
        return;
    }

    glUniform1i(param, 0);

    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texture->texID);

	if (texture->texsizeDefined)
	{
		std::string texsizeName = "texsize_" + texture->name;
        GLint textSizeParam = glGetUniformLocation(program, texsizeName.c_str());
        if (param >= 0) {
            glProgramUniform4f(program, textSizeParam, texture->width, texture->height,
                               1 / (float) texture->width, 1 / (float) texture->height);
        } else {
            std::cerr << "invalid texsizeName " << texsizeName << std::endl;
            return;
        }
	}
}

void ShaderEngine::setupUserTextureState(GLuint program, const UserTexture* texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->texID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->bilinear ? GL_LINEAR : GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->bilinear ? GL_LINEAR : GL_NEAREST);
#ifndef GL_TRANSITION
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE);
#endif
}

void ShaderEngine::SetupShaderQVariables(GLuint program, const Pipeline &q)
{
    // static shader code: q[1-32] are copies of _q[a-h] float4
    //#define q1 _qa.x
    //#define q2 _qa.y
    //#define q3 _qa.z
    //#define q4 _qa.w

	// FIXME: to validate

    // set program uniform "_q[a-h]" values (_qa.x, _qa.y, _qa.z, _qa.w, _qb.x, _qb.y ... )
    for (int i=0; i < 32; i+=4) {
        std::string varName = "q";
        varName.push_back('a' + i/4);
        int loc = glGetUniformLocation(program, varName.c_str());
        glProgramUniform4f(program, loc, q.q[i], q.q[i+1], q.q[i+2], q.q[i+3]);
    }
}

void ShaderEngine::setAspect(float aspect)
{
	this->aspect = aspect;
}

void ShaderEngine::RenderBlurTextures(const Pipeline &pipeline, const PipelineContext &pipelineContext, const int texsize)
{
#ifndef GL_TRANSITION
	if (blur1_enabled || blur2_enabled || blur3_enabled)
	{
        glUseProgram(programID_blur);

        glProgramUniform4f(programID_blur, glGetUniformLocation(programID_blur, "srctexsize"), texsize/2, texsize/2, 2 / (float) texsize,
                2 / (float) texsize);


		float tex[4][2] =
		{
		{ 0, 1 },
		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 } };

		glBlendFunc(GL_ONE, GL_ZERO);
		glColor4f(1.0, 1.0, 1.0, 1.0f);

		glBindTexture(GL_TEXTURE_2D, mainTextureId);
		glEnable(GL_TEXTURE_2D);

		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, tex);

		if (blur1_enabled)
		{
			float pointsold[4][2] =
			{
			{ 0, 1 },
			{ 0, 0 },
			{ 1, 0 },
			{ 1, 1 } };
			float points[4][2] =
						{
						{ 0, 0.5 },
						{ 0, 0 },
						{ 0.5, 0 },
						{ 0.5, 0.5 } };


			glVertexPointer(2, GL_FLOAT, 0, points);
			glBlendFunc(GL_ONE,GL_ZERO);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


			glBindTexture(GL_TEXTURE_2D, blur1_tex);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, texsize/2, texsize/2);


		}

		if (blur2_enabled)
		{


			float points[4][2] =
			{
			{ 0, 0.25 },
			{ 0, 0 },
			{ 0.25, 0 },
			{ 0.25, 0.25 } };

			glVertexPointer(2, GL_FLOAT, 0, points);
			glBlendFunc(GL_ONE,GL_ZERO);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);



						glBindTexture(GL_TEXTURE_2D, blur2_tex);
						glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, texsize/4, texsize/4);


		}

		if (blur3_enabled)
		{
            glProgramUniform4f(programID_blur, glGetUniformLocation(programID_blur, "srctexsize"), texsize/4, texsize/4, 4 / (float) texsize,
                               4/ (float) texsize);

			float points[4][2] =
			{
			{ 0, 0.125 },
			{ 0, 0 },
			{ 0.125, 0 },
			{ 0.125, 0.125 } };

			glVertexPointer(2, GL_FLOAT, 0, points);
			glBlendFunc(GL_ONE,GL_ZERO);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);



						glBindTexture(GL_TEXTURE_2D, blur3_tex);
						glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, texsize/8, texsize/8);


		}
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}
#endif
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

#pragma mark Preset Shaders

void ShaderEngine::loadPresetShaders(Pipeline &pipeline) {
    // compile and link warp and composite shaders from pipeline
    programID_presetWarp = loadPresetShader(PresentWarpShader, pipeline.warpShader, pipeline.warpShaderFilename);
    programID_presetComp = loadPresetShader(PresentCompositeShader, pipeline.compositeShader, pipeline.compositeShaderFilename);

    if (programID_presetComp != GL_FALSE)
        presetCompShaderLoaded = true;
    
    if (programID_presetWarp != GL_FALSE)
        presetWarpShaderLoaded = true;
    
    std::cout << "Preset composite shader active: " << presetCompShaderLoaded << ", preset warp shader active: " << presetWarpShaderLoaded << std::endl;
}

GLuint ShaderEngine::loadPresetShader(const ShaderEngine::PresentShaderType shaderType, Shader &presetShader, std::string &shaderFilename) {
    GLuint program = compilePresetShader(shaderType, presetShader, shaderFilename);

    if (program == GL_FALSE) {
        // failed to compile
        return GL_FALSE;
    }

    // pass texture info from preset to shader
    for (auto &userTexture : presetShader.textures) {
        setupUserTextureState(program, userTexture.second);
        setupUserTexture(program, userTexture.second);
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
	rand_preset[0] = (rand() % 100) * .01;
	rand_preset[1] = (rand() % 100) * .01;
	rand_preset[2] = (rand() % 100) * .01;
	rand_preset[3] = (rand() % 100) * .01;
}

GLuint ShaderEngine::CompileShaderProgram(const std::string & VertexShaderCode, const std::string & FragmentShaderCode, const std::string & shaderTypeString){

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;


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

    GLuint m_temp_vao;
    glGenVertexArrays(1, &m_temp_vao);
    glBindVertexArray(m_temp_vao);

    glValidateProgram(programID);

    glDeleteVertexArrays(1, &m_temp_vao);

    // Check the program
    glGetProgramiv(programID, GL_VALIDATE_STATUS, &Result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        fprintf(stderr, "%s\n", &ProgramErrorMessage[0]);
    }


    glDetachShader(programID, VertexShaderID);
    glDetachShader(programID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return linkOK ? programID : GL_FALSE;
}

// use the appropriate shader program for rendering the interpolation.
// it will use the preset shader if available, otherwise the textured shader
void ShaderEngine::enableWarpShader(Shader &shader, const Pipeline &pipeline, const PipelineContext &pipelineContext) {
    if (presetWarpShaderLoaded && PRESET_SHADERS_ENABLED) {
        glUseProgram(programID_presetWarp);

        for (std::map<std::string, UserTexture*>::const_iterator pos = shader.textures.begin(); pos	!= shader.textures.end(); ++pos)
                            setupUserTextureState(programID_presetWarp, pos->second);

        for (std::map<std::string, UserTexture*>::const_iterator pos = shader.textures.begin(); pos
                        != shader.textures.end(); ++pos)
                    setupUserTexture(programID_presetWarp, pos->second);

        SetupShaderVariables(programID_presetWarp, pipeline, pipelineContext);
        SetupShaderQVariables(programID_presetWarp, pipeline);

    } else {
        glUseProgram(programID_v2f_c4f_t2f);
    }
}

void ShaderEngine::enableCompositeShader(Shader &shader, const Pipeline &pipeline, const PipelineContext &pipelineContext) {
    if (presetCompShaderLoaded && PRESET_SHADERS_ENABLED) {
        glUseProgram(programID_presetComp);

        for (std::map<std::string, UserTexture*>::const_iterator pos = shader.textures.begin(); pos	!= shader.textures.end(); ++pos)
                            setupUserTextureState(programID_presetComp, pos->second);

        for (std::map<std::string, UserTexture*>::const_iterator pos = shader.textures.begin(); pos
                        != shader.textures.end(); ++pos)
                    setupUserTexture(programID_presetComp, pos->second);

        SetupShaderVariables(programID_presetComp, pipeline, pipelineContext);
        SetupShaderQVariables(programID_presetComp, pipeline);

    } else {
        glUseProgram(programID_v2f_c4f_t2f);
    }
}

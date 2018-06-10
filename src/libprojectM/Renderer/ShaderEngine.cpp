/*
 * ShaderEngine.cpp
 *
 *  Created on: Jul 18, 2008
 *      Author: pete
 */
#include <fstream>
#include "PerlinNoise.hpp"
#include "ShaderEngine.hpp"
#include "BeatDetect.hpp"
#include "HLSLTranslator.hpp"

#define GLSL_VERSION "410"

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


GLint ShaderEngine::UNIFORM_V2F_C4F_VERTEX_TRANFORMATION = 0;
GLint ShaderEngine::UNIFORM_V2F_C4F_VERTEX_POINT_SIZE = 0;
GLint ShaderEngine::UNIFORM_V2F_C4F_T2F_VERTEX_TRANFORMATION = 0;
GLint ShaderEngine::UNIFORM_V2F_C4F_T2F_FRAG_TEXTURE_SAMPLER = 0;



ShaderEngine::ShaderEngine()
{
    GLuint m_temp_vao;
    glGenVertexArrays(1, &m_temp_vao);
    glBindVertexArray(m_temp_vao);

    programID_v2f_c4f = CompileShaderProgram(v2f_c4f_vert, v2f_c4f_frag);
    programID_v2f_c4f_t2f = CompileShaderProgram(v2f_c4f_t2f_vert, v2f_c4f_t2f_frag);

    UNIFORM_V2F_C4F_VERTEX_TRANFORMATION = glGetUniformLocation(programID_v2f_c4f, "vertex_transformation");
    UNIFORM_V2F_C4F_VERTEX_POINT_SIZE = glGetUniformLocation(programID_v2f_c4f, "vertex_point_size");
    UNIFORM_V2F_C4F_T2F_VERTEX_TRANFORMATION = glGetUniformLocation(programID_v2f_c4f_t2f, "vertex_transformation");
    UNIFORM_V2F_C4F_T2F_FRAG_TEXTURE_SAMPLER = glGetUniformLocation(programID_v2f_c4f_t2f, "texture_sampler");
}

ShaderEngine::~ShaderEngine()
{
}

bool ShaderEngine::checkCompileStatus(GLuint shader, const char *shaderTitle) {
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE)
        return true;  // success

    char buffer[2048];
    glGetShaderInfoLog(shader, 2048, NULL, buffer);
    std::cerr << "Failed to compile shader '" << shaderTitle << "'. Error: " << buffer << std::endl;
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

	PerlinNoise		*noise = new PerlinNoise;
#ifndef GL_TRANSITION
	glGenTextures(1, &noise_texture_lq_lite);
	glBindTexture(GL_TEXTURE_2D, noise_texture_lq_lite);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 32, 32, 0, GL_LUMINANCE, GL_FLOAT, noise->noise_lq_lite);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	textureManager->setTexture("noise_lq_lite", noise_texture_lq_lite, 32, 32);

	glGenTextures(1, &noise_texture_lq);
	glBindTexture(GL_TEXTURE_2D, noise_texture_lq);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 256, 256, 0, GL_LUMINANCE, GL_FLOAT, noise->noise_lq);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	textureManager->setTexture("noise_lq", noise_texture_lq, 256, 256);

	glGenTextures(1, &noise_texture_mq);
	glBindTexture(GL_TEXTURE_2D, noise_texture_mq);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 256, 256, 0, GL_LUMINANCE, GL_FLOAT, noise->noise_mq);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	textureManager->setTexture("noise_mq", noise_texture_mq, 256, 256);

	glGenTextures(1, &noise_texture_hq);
	glBindTexture(GL_TEXTURE_2D, noise_texture_hq);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 256, 256, 0, GL_LUMINANCE, GL_FLOAT, noise->noise_hq);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	textureManager->setTexture("noise_hq", noise_texture_hq, 256, 256);

	glGenTextures(1, &noise_texture_perlin);
	glBindTexture(GL_TEXTURE_2D, noise_texture_perlin);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, 512, 512, 0, GL_LUMINANCE, GL_FLOAT, noise->noise_perlin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	textureManager->setTexture("noise_perlin", noise_texture_perlin, 512, 512);

	 glGenTextures( 1, &noise_texture_lq_vol );
	 glBindTexture( GL_TEXTURE_3D, noise_texture_lq_vol );
	 glTexImage3D(GL_TEXTURE_3D,0,4,32,32,32,0,GL_LUMINANCE,GL_FLOAT,noise->noise_lq_vol);
	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	 textureManager->setTexture("noisevol_lq", noise_texture_lq_vol, 256, 256);

	 glGenTextures( 1, &noise_texture_hq_vol );
	 glBindTexture( GL_TEXTURE_3D, noise_texture_hq_vol );
	 glTexImage3D(GL_TEXTURE_3D,0,4,32,32,32,0,GL_LUMINANCE,GL_FLOAT,noise->noise_hq_vol);
	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	 textureManager->setTexture("noisevol_hq", noise_texture_hq_vol, 8, 8);
#endif
}

// compile a user-defined shader from a preset. returns program ID if successful.
GLuint ShaderEngine::compilePresetShader(GLenum shaderType, Shader &pmShader, std::string &shaderFilename) {
    std::string program = pmShader.programSource;

    if (program.length() <= 0)
        return GL_FALSE;

    // replace "}" with return statement (this can probably be optimized for the GLSL conversion...)
    size_t found = program.rfind('}');
    if (found != std::string::npos)
    {
        //std::cout << "last '}' found at: " << int(found) << std::endl;
        program.replace(int(found), 1, "OUT.color.xyz=ret.xyz;\nOUT.color.w=1;\nreturn OUT;\n}");
    }
    else
        return GL_FALSE;

    // replace "{" with some variable declarations
    found = program.rfind('{');
    if (found != std::string::npos)
    {
        //std::cout << "first '{' found at: " << int(found) << std::endl;
        const char *progMain = \
        "{\n"
        "float2 uv_orig = uv;\n"
        "float rad=getrad(uv);\n"
        "float ang=getang(uv);\n"
        "float3 ret;\n"
        "outtype OUT;\n";
        program.replace(int(found), 1, progMain);
    }
    else
        return GL_FALSE;

    // replace shader_body with entry point function
    found = program.find("shader_body");
    if (found != std::string::npos)
    {
        //std::cout << "first 'shader_body' found at: " << int(found) << std::endl;
        program.replace(int(found), 11, "outtype projectm(float2 uv : TEXCOORD0)\n");
    }
    else
        return GL_FALSE;

    pmShader.textures.clear();

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

    // now we need to prepend the HLSL template to the program

    // transpile from HLSL (aka preset shader aka directX shader) to GLSL (aka OpenGL shader lang)
    HLSLTranslator translator = HLSLTranslator();
    std::unique_ptr<std::string> glslSource = translator.parse(shaderType, shaderFilename.c_str(), program);
    if (!glslSource) {
        std::cerr << "Failed to parse shader from " << shaderFilename << std::endl;
        std::cerr << "Original program: " << program << std::endl;
        return GL_FALSE;
    }
    
    // now we have GLSL source for the preset shader program (hopefully it's valid!)
    // copmile the preset shader fragment shader with the standard vertex shader and cross our fingers
    return CompileShaderProgram(*glslSource.get(), v2f_c4f_t2f_frag);  // returns new program
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

	glProgramUniform4f(program, glGetUniformLocation(program, "texsize"), texsize, texsize, 1 / (float) texsize, 1
			/ (float) texsize);
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

void ShaderEngine::setupUserTexture(const UserTexture* texture)
{
	std::string samplerName = "sampler_" + texture->qname;

    // https://www.khronos.org/opengl/wiki/Sampler_(GLSL)#Binding_textures_to_samplers
	GLint param = glGetUniformLocation(programID_preset, samplerName.c_str());
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
        GLint textSizeParam = glGetUniformLocation(programID_preset, texsizeName.c_str());
        if (param >= 0) {
            glProgramUniform4f(textSizeParam, texture->width, texture->height, 0,
                               1 / (float) texture->width, 1 / (float) texture->height);
        } else {
            std::cerr << "invalid texsizeName " << texsizeName << std::endl;
            return;
        }
	}
}

void ShaderEngine::setupUserTextureState( const UserTexture* texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->texID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->bilinear ? GL_LINEAR : GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->bilinear ? GL_LINEAR : GL_NEAREST);
#ifndef GL_TRANSITION
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrap ? GL_REPEAT : GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrap ? GL_REPEAT : GL_CLAMP);
#endif
}

void ShaderEngine::SetupShaderQVariables(GLuint program, const Pipeline &q)
{
    // set program uniform "q" values (q1, q2, ... q32)
    for (int i=0; i < 32; i++) {
        std::string varName = "q";
        varName.append(std::to_string(i+1));
        int loc = glGetUniformLocation(program, varName.c_str());
        glProgramUniform1f(program, loc, q.q[i]);
    }
}

void ShaderEngine::setAspect(float aspect)
{
	this->aspect = aspect;
}
void ShaderEngine::RenderBlurTextures(const Pipeline &pipeline, const PipelineContext &pipelineContext,
		const int texsize)
{
#ifndef GL_TRANSITION
	if (blur1_enabled || blur2_enabled || blur3_enabled)
	{
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
			glProgramUniform4f(glGetUniformLocation(blur1Program, "srctexsize"), 4, texsize/2, texsize/2, 2 / (float) texsize,
					2 / (float) texsize);
			glProgramUniform4f(glGetUniformLocation(blur2Program, "srctexsize"), 4, texsize/2 , texsize/2, 2 / (float) texsize,
								2 / (float) texsize);



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

//            cgGLBindProgram(blur1Program);

			glVertexPointer(2, GL_FLOAT, 0, points);
			glBlendFunc(GL_ONE,GL_ZERO);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);


			glBindTexture(GL_TEXTURE_2D, blur1_tex);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, texsize/2, texsize/2);


		}

		if (blur2_enabled)
		{
			glProgramUniform4f(glGetUniformLocation(blur1Program, "srctexsize"), 4, texsize/2, texsize/2, 2 / (float) texsize,
								2 / (float) texsize);
			glProgramUniform4f(glGetUniformLocation(blur2Program, "srctexsize"), 4, texsize/2, texsize/2, 2 / (float) texsize,
					2 / (float) texsize);



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
			glProgramUniform4f(glGetUniformLocation(blur2Program, "srctexsize"), 4, texsize/4, texsize/4, 4 / (float) texsize,
								4/ (float) texsize);
			glProgramUniform4f(glGetUniformLocation(blur2Program, "srctexsize"), 4, texsize / 4, texsize / 4, 4
					/ (float) texsize, 4 / (float) texsize);
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

void ShaderEngine::linkProgram(GLuint programID) {
    glLinkProgram(programID);

    GLint program_linked;
    glGetProgramiv(programID, GL_LINK_STATUS, &program_linked);
    if (program_linked != GL_TRUE) {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetProgramInfoLog(programID, 1024, &log_length, message);
        std::cerr << "Failed to link program: " << message << std::endl;
        return;
    }
    
}

#pragma mark Preset Shaders
void ShaderEngine::loadPresetShader(Shader &presetShader, std::string &shaderFilename)
{
    assert(!presetShaderProgramLoaded);
    
    assert(!presetShader.enabled);
    // i think they're always fragment shaders? not positive -mischa
    programID_preset = compilePresetShader(GL_FRAGMENT_SHADER, presetShader, shaderFilename);

    if (programID_preset == GL_FALSE) {
        // failed to compile
        return;
    }
    printf("linked shader %s\n", presetShader.presetPath.c_str());

    presetShaders[&presetShader] = programID_preset;
    
    // pass texture info from preset to shader
    for (auto &userTexture : presetShader.textures) {
        setupUserTextureState(userTexture.second);
        setupUserTexture(userTexture.second);
    }

    presetShader.enabled = true;
    presetShaderProgramLoaded = true;

}

// deactivate preset shaders
void ShaderEngine::disablePresetShaders() {
    if (!presetShaderProgramLoaded)
        return;
    
    glDeleteProgram(programID_preset);
    
    presetShaderProgramLoaded = false;
}

void ShaderEngine::reset()
{
    disablePresetShaders();
	rand_preset[0] = (rand() % 100) * .01;
	rand_preset[1] = (rand() % 100) * .01;
	rand_preset[2] = (rand() % 100) * .01;
	rand_preset[3] = (rand() % 100) * .01;
}

GLuint ShaderEngine::CompileShaderProgram(const std::string & VertexShaderCode, const std::string & FragmentShaderCode){

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;


    // Compile Vertex Shader
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
    checkCompileStatus(VertexShaderID, "built-in vertex shader");


    // Compile Fragment Shader
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
    checkCompileStatus(FragmentShaderID, "built-in fragment shader");

    
    // Link the program
    GLuint programID = glCreateProgram();
    
    glAttachShader(programID, VertexShaderID);
    glAttachShader(programID, FragmentShaderID);
    linkProgram(programID);

    glValidateProgram(programID);

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

    return programID;
}

// use the appropriate shader program for rendering the interpolation.
// it will use the preset shader if available, otherwise the textured shader
void ShaderEngine::enableInterpolationShader() {
    if (!presetShaderProgramLoaded) {
        glUseProgram(programID_v2f_c4f_t2f);
    } else {
        glUseProgram(programID_preset);
    }
}

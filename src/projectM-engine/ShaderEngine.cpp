/*
 * ShaderEngine.cpp
 *
 *  Created on: Jul 18, 2008
 *      Author: pete
 */
#include <fstream>
#include "PerlinNoise.hpp"
#include "ShaderEngine.hpp"

ShaderEngine::ShaderEngine()
{
#ifdef USE_CG
	SetupCg();

#endif

}

ShaderEngine::~ShaderEngine()
{
	// TODO Auto-generated destructor stub
}

#ifdef USE_CG

void ShaderEngine::SetParams(const int texsize, const unsigned int texId, const float aspect, BeatDetect *beatDetect,
		TextureManager *textureManager)
{
	mainTextureId = texId;
	this->beatDetect = beatDetect;
	this->textureManager = textureManager;
	this->aspect = aspect;
	this->texsize = texsize;

	textureManager->setTexture("main", texId, texsize, texsize);

	glGenTextures(1, &blur1_tex);
	glBindTexture(GL_TEXTURE_2D, blur1_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texsize, texsize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenTextures(1, &blur2_tex);
	glBindTexture(GL_TEXTURE_2D, blur2_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texsize / 2, texsize / 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenTextures(1, &blur3_tex);
	glBindTexture(GL_TEXTURE_2D, blur3_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texsize / 4, texsize / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	blur1_enabled = false;
	blur2_enabled = false;
	blur3_enabled = false;

	std::cout << "Generating Noise Textures" << std::endl;

	PerlinNoise noise;

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

	/*
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
	 */

}

bool ShaderEngine::LoadCgProgram(Shader &shader)
{
	//if (p != NULL) cgDestroyProgram(p);
	//p = NULL;
	std::string program = shader.programSource;

	if (program.length() > 0)
	{
		size_t found = program.rfind('}');
		if (found != std::string::npos)
		{
			//std::cout << "last '}' found at: " << int(found) << std::endl;
			program.replace(int(found), 1, "OUT.color.xyz=ret.xyz;\nOUT.color.w=1;\nreturn OUT;\n}");
		}
		else
			return false;
		found = program.rfind('{');
		if (found != std::string::npos)
		{
			//std::cout << "first '{' found at: " << int(found) << std::endl;
			program.replace(int(found), 1, "{\nfloat rad=getrad;\nfloat ang=getang;\n");
		}
		else
			return false;
		found = program.find("shader_body");
		if (found != std::string::npos)
		{
			//std::cout << "first 'shader_body' found at: " << int(found) << std::endl;
			program.replace(int(found), 11, "outtype projectm(float2 uv : TEXCOORD0)\n");
		}
		else
			return false;

		shader.textures.clear();

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
				if (texture->texID != 0 && shader.textures.find(texture->qname) == shader.textures.end())
					shader.textures[texture->qname] = texture;

				else
					delete (texture);
			}

			found = program.find("sampler_", found);
		}

		found = 0;
		found = program.find("texsize_", found);
		while (found != std::string::npos)
		{
			found += 8;
			size_t end = program.find_first_of(" ;.,\n\r)", found);

			if (end != std::string::npos)
			{
				std::string tex = program.substr((int) found, (int) end - found);
				if (shader.textures.find(tex) != shader.textures.end())
				{
					UserTexture* texture = shader.textures[tex];
					texture->texsizeDefined = true;
					std::cout << "texsize_" << tex << " found" << std::endl;
				}
			}
			found = program.find("texsize_", found);
		}

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

		std::string temp;

		temp.append(cgTemplate);
		temp.append(program);

		std::cout << "Cg: Compilation Results:" << std::endl << std::endl;
		std::cout << program << std::endl;

		CGprogram p = cgCreateProgram(myCgContext, CG_SOURCE, temp.c_str(),//temp.c_str(),
				myCgProfile, "projectm", NULL);

		checkForCgCompileError("creating shader program");
		if (p == NULL)
			return false;

		cgGLLoadProgram(p);

		if (checkForCgCompileError("loading shader program"))
		{
			p = NULL;
			return false;
		}

		programs[&shader] = p;

		return true;
	}
	else
		return false;
}

bool ShaderEngine::checkForCgCompileError(const char *situation)
{
	CGerror error;
	const char *string = cgGetLastErrorString(&error);
	error = cgGetError();
	if (error != CG_NO_ERROR)
	{
		std::cout << "Cg: Compilation Error" << std::endl;
		std::cout << "Cg: %" << situation << " - " << string << std::endl;
		if (error == CG_COMPILER_ERROR)
		{
			std::cout << "Cg: " << cgGetLastListing(myCgContext) << std::endl;

		}
		return true;
	}

	return false;
}

void ShaderEngine::checkForCgError(const char *situation)
{
	CGerror error;
	const char *string = cgGetLastErrorString(&error);

	if (error != CG_NO_ERROR)
	{
		std::cout << "Cg: %" << situation << " - " << string << std::endl;
		if (error == CG_COMPILER_ERROR)
		{
			std::cout << "Cg: " << cgGetLastListing(myCgContext) << std::endl;
		}
		exit(1);
	}
}

void ShaderEngine::SetupCg()
{
	std::string line;
	std::ifstream myfile(CMAKE_INSTALL_PREFIX "/share/projectM/shaders/projectM.cg");
	if (myfile.is_open())
	{
		while (!myfile.eof())
		{
			std::getline(myfile, line);
			cgTemplate.append(line + "\n");
		}
		myfile.close();
	}

	else
		std::cout << "Unable to load shader template" << std::endl;

	std::ifstream myfile2(CMAKE_INSTALL_PREFIX "/share/projectM/shaders/blur.cg");
	if (myfile2.is_open())
	{
		while (!myfile2.eof())
		{
			std::getline(myfile2, line);
			blurProgram.append(line + "\n");
		}
		myfile2.close();
	}

	else
		std::cout << "Unable to load blur template" << std::endl;

	myCgContext = cgCreateContext();
	checkForCgError("creating context");
	cgGLSetDebugMode(CG_FALSE);
	cgSetParameterSettingMode(myCgContext, CG_DEFERRED_PARAMETER_SETTING);

	myCgProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
	cgGLSetOptimalOptions(myCgProfile);
	checkForCgError("selecting fragment profile");

	profileName = cgGetProfileString(myCgProfile);
	std::cout << "Cg: Initialized profile: " << profileName << std::endl;

	blur1Program = cgCreateProgram(myCgContext, CG_SOURCE, blurProgram.c_str(), myCgProfile, "blur1", NULL);

	checkForCgCompileError("creating blur1 program");
	if (blur1Program == NULL)
		exit(1);
	cgGLLoadProgram(blur1Program);

	checkForCgError("loading blur1 program");

	blur2Program = cgCreateProgram(myCgContext, CG_SOURCE, blurProgram.c_str(), myCgProfile, "blur2", NULL);

	checkForCgCompileError("creating blur2 program");
	if (blur2Program == NULL)
		exit(1);
	cgGLLoadProgram(blur2Program);

	checkForCgError("loading blur2 program");

}

void ShaderEngine::SetupCgVariables(CGprogram program, const Pipeline &pipeline, const PipelineContext &context)
{

	float slow_roam_cos[4] =	{ 0.5 + 0.5 * cos(context.time * 0.005), 0.5 + 0.5 * cos(context.time * 0.008), 0.5 + 0.5 * cos(context.time * 0.013), 0.5 + 0.5 * cos(context.time * 0.022) };
	float roam_cos[4] =	{ 0.5 + 0.5 * cos(context.time * 0.3), 0.5 + 0.5 * cos(context.time * 1.3), 0.5 + 0.5 * cos(context.time * 5), 0.5	+ 0.5 * cos(context.time * 20) };
	float slow_roam_sin[4] =	{ 0.5 + 0.5 * sin(context.time * 0.005), 0.5 + 0.5 * sin(context.time * 0.008), 0.5 + 0.5 * sin(context.time * 0.013), 0.5 + 0.5 * sin(context.time * 0.022) };
	float roam_sin[4] =	{ 0.5 + 0.5 * sin(context.time * 0.3), 0.5 + 0.5 * sin(context.time * 1.3), 0.5 + 0.5 * sin(context.time * 5), 0.5	+ 0.5 * sin(context.time * 20) };

	cgGLSetParameter4fv(cgGetNamedParameter(program, "slow_roam_cos"), slow_roam_cos);
	cgGLSetParameter4fv(cgGetNamedParameter(program, "roam_cos"), roam_cos);
	cgGLSetParameter4fv(cgGetNamedParameter(program, "slow_roam_sin"), slow_roam_sin);
	cgGLSetParameter4fv(cgGetNamedParameter(program, "roam_sin"), roam_sin);

	cgGLSetParameter1f(cgGetNamedParameter(program, "time"), context.time);
	cgGLSetParameter4f(cgGetNamedParameter(program, "rand_preset"), rand_preset[0], rand_preset[1], rand_preset[2],	rand_preset[3]);
	cgGLSetParameter4f(cgGetNamedParameter(program, "rand_frame"), (rand() % 100) * .01, (rand() % 100) * .01, (rand()% 100) * .01, (rand() % 100) * .01);
	cgGLSetParameter1f(cgGetNamedParameter(program, "fps"), context.fps);
	cgGLSetParameter1f(cgGetNamedParameter(program, "frame"), context.frame);
	cgGLSetParameter1f(cgGetNamedParameter(program, "progress"), context.progress);

	cgGLSetParameter1f(cgGetNamedParameter(program, "blur1_min"), pipeline.warpShader.blur1n);
	cgGLSetParameter1f(cgGetNamedParameter(program, "blur1_max"), pipeline.warpShader.blur1x);
	cgGLSetParameter1f(cgGetNamedParameter(program, "blur2_min"), pipeline.warpShader.blur2n);
	cgGLSetParameter1f(cgGetNamedParameter(program, "blur2_max"), pipeline.warpShader.blur2x);
	cgGLSetParameter1f(cgGetNamedParameter(program, "blur3_min"), pipeline.warpShader.blur3n);
	cgGLSetParameter1f(cgGetNamedParameter(program, "blur3_max"), pipeline.warpShader.blur3x);

	cgGLSetParameter1f(cgGetNamedParameter(program, "bass"), beatDetect->bass);
	cgGLSetParameter1f(cgGetNamedParameter(program, "mid"), beatDetect->mid);
	cgGLSetParameter1f(cgGetNamedParameter(program, "treb"), beatDetect->treb);
	cgGLSetParameter1f(cgGetNamedParameter(program, "bass_att"), beatDetect->bass_att);
	cgGLSetParameter1f(cgGetNamedParameter(program, "mid_att"), beatDetect->mid_att);
	cgGLSetParameter1f(cgGetNamedParameter(program, "treb_att"), beatDetect->treb_att);
	cgGLSetParameter1f(cgGetNamedParameter(program, "vol"), beatDetect->vol);
	cgGLSetParameter1f(cgGetNamedParameter(program, "vol_att"), beatDetect->vol);

	cgGLSetParameter4f(cgGetNamedParameter(program, "texsize"), texsize, texsize, 1 / (float) texsize, 1
			/ (float) texsize);
	cgGLSetParameter4f(cgGetNamedParameter(program, "aspect"), 1 / aspect, 1, aspect, 1);

	//glBindTexture(GL_TEXTURE_2D, renderTarget->textureID[1]);

	/*
	 cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_main"),renderTarget->textureID[1]);
	 cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_main"));

	 cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_noise_lq_lite"),noise_texture_lq_lite);
	 cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_noise_lq_lite"));

	 cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_noise_mq"),noise_texture_mq);
	 cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_noise_mq"));

	 cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_noise_hq"),noise_texture_hq);
	 cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_noise_hq"));

	 cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_noise_lq"),noise_texture_lq);
	 cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_noise_lq"));

	 cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_noise_perlin"),noise_texture_perlin);
	 cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_noise_perlin"));

	 cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_noisevol_hq"),noise_texture_hq_vol);
	 cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_noisevol_hq"));

	 cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_noisevol_lq"),noise_texture_lq_vol);
	 cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_noisevol_lq"));
	 */

	if (blur1_enabled)
	{
		cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_blur1"), blur1_tex);
		cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_blur1"));
	}
	if (blur2_enabled)
	{
		cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_blur2"), blur2_tex);
		cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_blur2"));
	}
	if (blur3_enabled)
	{
		cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_blur3"), blur3_tex);
		cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_blur3"));
	}

	//cgGLSetParameter4f(cgGetNamedParameter(program, "texsize_noise_lq"), 256, 256, 1.0 / (float) 256, 1.0 / (float) 256);
	//cgGLSetParameter4f(cgGetNamedParameter(program, "texsize_noise_mq"), 64, 64, 1.0 / (float) 64, 1.0 / (float) 64);
	//cgGLSetParameter4f(cgGetNamedParameter(program, "texsize_noise_hq"), 32, 32, 1.0 / (float) 32, 1.0 / (float) 32);
	//cgGLSetParameter4f(cgGetNamedParameter(program, "texsize_noise_perlin"), 512, 512, 1.0 / (float) 512, 1.0
//			/ (float) 512);
	//cgGLSetParameter4f(cgGetNamedParameter(program, "texsize_noise_lq_lite"), 32, 32, 1.0 / (float) 32, 1.0
	//		/ (float) 32);
}

void ShaderEngine::SetupUserTexture(CGprogram program, const UserTexture* texture)
{
	std::string samplerName = "sampler_" + texture->qname;

	CGparameter param = cgGetNamedParameter(program, samplerName.c_str());
	checkForCgError("getting parameter");
	cgGLSetTextureParameter(param, texture->texID);
	checkForCgError("setting parameter");
	cgGLEnableTextureParameter(param);
	checkForCgError("enabling parameter");
	//std::cout<<texture->texID<<" "<<samplerName<<std::endl;

	if (texture->texsizeDefined)
	{
		std::string texsizeName = "texsize_" + texture->name;
		cgGLSetParameter4f(cgGetNamedParameter(program, texsizeName.c_str()), texture->width, texture->height, 1
				/ (float) texture->width, 1 / (float) texture->height);
		checkForCgError("setting parameter texsize");
	}


}

void ShaderEngine::SetupUserTextureState( const UserTexture* texture)
{
	    glBindTexture(GL_TEXTURE_2D, texture->texID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texture->bilinear ? GL_LINEAR : GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texture->bilinear ? GL_LINEAR : GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, texture->wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, texture->wrap ? GL_REPEAT : GL_CLAMP);
}

void ShaderEngine::SetupCgQVariables(Shader &shader, const PresetOutputs &q)
{
	CGprogram program = programs[&shader];

	if (shader.enabled)
	{
		cgGLSetParameter4f(cgGetNamedParameter(program, "_qa"), q.q1, q.q2, q.q3, q.q4);
		cgGLSetParameter4f(cgGetNamedParameter(program, "_qb"), q.q5, q.q6, q.q7, q.q8);
		cgGLSetParameter4f(cgGetNamedParameter(program, "_qc"), q.q9, q.q10, q.q11, q.q12);
		cgGLSetParameter4f(cgGetNamedParameter(program, "_qd"), q.q13, q.q14, q.q15, q.q16);
		cgGLSetParameter4f(cgGetNamedParameter(program, "_qe"), q.q17, q.q18, q.q19, q.q20);
		cgGLSetParameter4f(cgGetNamedParameter(program, "_qf"), q.q21, q.q22, q.q23, q.q24);
		cgGLSetParameter4f(cgGetNamedParameter(program, "_qg"), q.q25, q.q26, q.q27, q.q28);
		cgGLSetParameter4f(cgGetNamedParameter(program, "_qh"), q.q29, q.q30, q.q31, q.q32);
	}
}

void ShaderEngine::setAspect(float aspect)
{
	this->aspect = aspect;
}
void ShaderEngine::RenderBlurTextures(const Pipeline *pipeline, const PipelineContext &pipelineContext,
		const int texsize)
{
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

		glEnable(GL_TEXTURE_2D);

		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, tex);

		cgGLEnableProfile(myCgProfile);
		checkForCgError("enabling profile");

		if (blur1_enabled)
		{
			cgGLSetParameter4f(cgGetNamedParameter(blur1Program, "srctexsize"), texsize, texsize, 1 / (float) texsize,
					1 / (float) texsize);

			cgGLBindProgram(blur1Program);
			checkForCgError("binding blur1 program");

			float points[4][2] =
			{
			{ 0, 1 },
			{ 0, 0 },
			{ 1, 0 },
			{ 1, 1 } };

			glVertexPointer(2, GL_FLOAT, 0, points);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			glBindTexture(GL_TEXTURE_2D, blur1_tex);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, texsize, texsize);

			cgGLUnbindProgram(myCgProfile);
			checkForCgError("unbinding blur1 program");
		}

		if (blur2_enabled)
		{
			cgGLSetParameter4f(cgGetNamedParameter(blur2Program, "srctexsize"), texsize, texsize, 1 / (float) texsize,
					1 / (float) texsize);

			cgGLBindProgram(blur2Program);
			checkForCgError("binding blur2 program");

			float points[4][2] =
			{
			{ 0, 0.5 },
			{ 0, 0 },
			{ 0.5, 0 },
			{ 0.5, 0.5 } };

			glVertexPointer(2, GL_FLOAT, 0, points);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			glBindTexture(GL_TEXTURE_2D, blur2_tex);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, texsize / 2, texsize / 2);

		}

		if (blur3_enabled)
		{
			cgGLSetParameter4f(cgGetNamedParameter(blur2Program, "srctexsize"), texsize / 2, texsize / 2, 2
					/ (float) texsize, 2 / (float) texsize);
			float points[4][2] =
			{
			{ 0, 0.25 },
			{ 0, 0 },
			{ 0.25, 0 },
			{ 0.25, 0.25 } };

			glVertexPointer(2, GL_FLOAT, 0, points);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			glBindTexture(GL_TEXTURE_2D, blur3_tex);
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, texsize / 4, texsize / 4);
		}

		if (blur2_enabled || blur3_enabled)
		{
			cgGLUnbindProgram(myCgProfile);
			checkForCgError("unbinding blur2 program");
		}

		cgGLDisableProfile(myCgProfile);
		checkForCgError("disabling blur profile");

		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}
}

void ShaderEngine::loadShader(Shader &shader)
{
	if (shader.enabled)
	{
		cgDestroyProgram(programs[&shader]);
		programs.erase(&shader);
	}
	shader.enabled = LoadCgProgram(shader);
}

void ShaderEngine::disableShader()
{
	if (enabled)
	{
		cgGLUnbindProgram(myCgProfile);
		checkForCgError("disabling fragment profile");
		cgGLDisableProfile(myCgProfile);
		checkForCgError("disabling fragment profile");
	}
	enabled = false;
}

void ShaderEngine::enableShader(Shader &shader, const Pipeline *pipeline, const PipelineContext *pipelineContext)
{
	enabled = false;
	if (shader.enabled)
	{



		for (std::map<std::string, UserTexture*>::const_iterator pos = shader.textures.begin(); pos	!= shader.textures.end(); ++pos)
							SetupUserTextureState( pos->second);


		CGprogram program = programs[&shader];
		for (std::map<std::string, UserTexture*>::const_iterator pos = shader.textures.begin(); pos
						!= shader.textures.end(); ++pos)
					SetupUserTexture(program, pos->second);

		cgGLEnableProfile(myCgProfile);
		checkForCgError("enabling warp profile");

		cgGLBindProgram(program);
		checkForCgError("binding warp program");

		SetupCgVariables(program, *pipeline, *pipelineContext);

		enabled = true;
	}
}

void ShaderEngine::reset()
{
	rand_preset[0] = (rand() % 100) * .01;
	rand_preset[1] = (rand() % 100) * .01;
	rand_preset[2] = (rand() % 100) * .01;
	rand_preset[3] = (rand() % 100) * .01;
}

#endif

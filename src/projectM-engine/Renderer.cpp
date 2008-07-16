#include "Renderer.hpp"
#include "wipemalloc.h"
#include "math.h"
#include "Common.hpp"
#include "CustomShape.hpp"
#include "CustomWave.hpp"
#include "KeyHandler.hpp"
#include "TextureManager.hpp"
#include <iostream>
#include <algorithm>
#include <cassert>
#include <fstream>
#include "omptl/omptl"
#include "omptl/omptl_algorithm"
#include "PerlinNoise.hpp"

class Preset;

Renderer::Renderer(int width, int height, int gx, int gy, int texsize, BeatDetect *beatDetect, std::string _presetURL, std::string _titlefontURL, std::string _menufontURL): title_fontURL(_titlefontURL), menu_fontURL(_menufontURL), presetURL(_presetURL), m_presetName("None"), vw(width), vh(height), texsize(texsize), mesh(gx,gy)

{
	int x; int y;

#ifdef USE_CG
	this->compositeShadersEnabled = false;
	this->warpShadersEnabled = false;
#endif
	this->totalframes = 1;
	this->noSwitch = false;
	this->showfps = false;
	this->showtitle = false;
	this->showpreset = false;
	this->showhelp = false;
	this->showstats = false;
	this->studio = false;
	this->realfps=0;

	this->drawtitle=0;

	//this->title = "Unknown";

	/** Other stuff... */
	this->correction = true;
	this->aspect=(float)height/(float)width;;

	/// @bug put these on member init list
	this->renderTarget = new RenderTarget( texsize, width, height );
	this->textureManager = new TextureManager(presetURL);
	this->beatDetect = beatDetect;


#ifdef USE_FTGL
	/**f Load the standard fonts */

	title_font = new FTGLPixmapFont(title_fontURL.c_str());
	other_font = new FTGLPixmapFont(menu_fontURL.c_str());
	other_font->UseDisplayList(true);
	title_font->UseDisplayList(true);


	poly_font = new FTGLExtrdFont(title_fontURL.c_str());

	poly_font->UseDisplayList(true);
	poly_font->Depth(20);
	poly_font->FaceSize(72);

	poly_font->UseDisplayList(true);

#endif /** USE_FTGL */

	this->gridx=(float **)wipemalloc(gx * sizeof(float *));
		for(x = 0; x < gx; x++)
		{
			this->gridx[x] = (float *)wipemalloc(gy * sizeof(float));
		}
		this->gridy=(float **)wipemalloc(gx * sizeof(float *));
		for(x = 0; x < gx; x++)
		{
			this->gridy[x] = (float *)wipemalloc(gy * sizeof(float));
		}

		this->origx2=(float **)wipemalloc(gx * sizeof(float *));
		for(x = 0; x < gx; x++)
		{
			this->origx2[x] = (float *)wipemalloc(gy * sizeof(float));
		}
		this->origy2=(float **)wipemalloc(gx * sizeof(float *));
		for(x = 0; x < gx; x++)
		{
			this->origy2[x] = (float *)wipemalloc(gy * sizeof(float));
		}

		//initialize reference grid values
		for (x=0;x<gx;x++)
		{
			for(y=0;y<gy;y++)
			{

				float origx=x/(float)(gx-1);
				float origy=-((y/(float)(gy-1))-1);
				this->gridx[x][y]=origx;
				this->gridy[x][y]=origy;
				this->origx2[x][y]=( origx-.5)*2;
				this->origy2[x][y]=( origy-.5)*2;

			}
		}

#ifdef USE_CG
SetupCg();

std::cout<<"Generating Noise Textures"<<std::endl;
PerlinNoise noise;

glGenTextures( 1, &blur1_tex );
glBindTexture(GL_TEXTURE_2D, blur1_tex);
glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, texsize, texsize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

glGenTextures( 1, &blur2_tex );
glBindTexture(GL_TEXTURE_2D, blur2_tex);
glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, texsize / 2, texsize / 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

glGenTextures( 1, &blur3_tex );
glBindTexture(GL_TEXTURE_2D, blur3_tex);
glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, texsize / 4, texsize / 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


blur1_enabled = false;
blur2_enabled = false;
blur3_enabled = false;

glGenTextures( 1, &noise_texture_lq_lite );
glBindTexture( GL_TEXTURE_2D, noise_texture_lq_lite );
glTexImage2D(GL_TEXTURE_2D,0,4,32,32,0,GL_LUMINANCE,GL_FLOAT,noise.noise_lq_lite);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

glGenTextures( 1, &noise_texture_lq );
glBindTexture( GL_TEXTURE_2D, noise_texture_lq );
glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_LUMINANCE,GL_FLOAT,noise.noise_lq);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


glGenTextures( 1, &noise_texture_mq );
glBindTexture( GL_TEXTURE_2D, noise_texture_mq );
glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_LUMINANCE,GL_FLOAT,noise.noise_mq);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

glGenTextures( 1, &noise_texture_hq );
glBindTexture( GL_TEXTURE_2D, noise_texture_hq );
glTexImage2D(GL_TEXTURE_2D,0,4,256,256,0,GL_LUMINANCE,GL_FLOAT,noise.noise_hq);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

glGenTextures( 1, &noise_texture_perlin );
glBindTexture( GL_TEXTURE_2D, noise_texture_perlin );
glTexImage2D(GL_TEXTURE_2D,0,4,512,512,0,GL_LUMINANCE,GL_FLOAT,noise.noise_perlin);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


glGenTextures( 1, &noise_texture_lq_vol );
glBindTexture( GL_TEXTURE_3D, noise_texture_lq_vol );
glTexImage3D(GL_TEXTURE_3D,0,4,32,32,32,0,GL_LUMINANCE,GL_FLOAT,noise.noise_lq_vol);
glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


glGenTextures( 1, &noise_texture_hq_vol );
glBindTexture( GL_TEXTURE_3D, noise_texture_hq_vol );
glTexImage3D(GL_TEXTURE_3D,0,4,32,32,32,0,GL_LUMINANCE,GL_FLOAT,noise.noise_hq_vol);
glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

#endif
}

void Renderer::SetPipeline(Pipeline &pipeline)
{
#ifdef USE_CG

	rand_preset[0] = (rand()%100) * .01;
	rand_preset[1] = (rand()%100) * .01;
	rand_preset[2] = (rand()%100) * .01;
	rand_preset[3] = (rand()%100) * .01;

	if(warpShadersEnabled) cgDestroyProgram(myCgWarpProgram);
	if(compositeShadersEnabled) cgDestroyProgram(myCgCompositeProgram);

	warpShadersEnabled = LoadCgProgram(pipeline.shader.warp, myCgWarpProgram);
	compositeShadersEnabled = LoadCgProgram(pipeline.shader.composite, myCgCompositeProgram);

	pipeline.shader.warp.clear();
	pipeline.shader.composite.clear();

#endif
}

#ifdef USE_CG


bool Renderer::LoadCgProgram(std::string program, CGprogram &p)
{
	//if (p != NULL) cgDestroyProgram(p);
	//p = NULL;

	if (program.length() > 0)
	{

		 size_t found = program.rfind('}');
		     if (found!=std::string::npos)
		     {
		       //std::cout << "last '}' found at: " << int(found) << std::endl;
		       program.replace(int(found),1,"OUT.color.xyz=ret.xyz;\nOUT.color.w=1;\nreturn OUT;\n}");
		     }
		     else return false;
		     found = program.rfind('{');
		     if (found!=std::string::npos)
		     {
		        //std::cout << "first '{' found at: " << int(found) << std::endl;
		        program.replace(int(found),1,"{\nfloat rad=getrad;\nfloat ang=getang;\n");
		     }
		     else return false;
		     found = program.find("shader_body");
		     if (found!=std::string::npos)
		     {
		        //std::cout << "first 'shader_body' found at: " << int(found) << std::endl;
		        program.replace(int(found),11,"outtype projectm(float2 uv : TEXCOORD0)\n");
		     }
		     else return false;

		     found = 0;
		     found = program.find("sampler_", found);
		     while (found != std::string::npos)
		     {
		    	 found+=8;
		    	 size_t end = program.find_first_of(" ;,\n)", found);

		    	 if(end != std::string::npos)
		    	 {
		    		 std::string sampler = program.substr((int)found,(int)end-found);
		    		 std::cout<<sampler<<std::endl;
		    	 }

		    	 found = program.find("sampler_", found);
		     }


		     found = program.find("GetBlur3");
		     if (found!=std::string::npos)
		    	 blur1_enabled = blur2_enabled = blur3_enabled = true;
		     else
		     {
		    	  found = program.find("GetBlur2");
		    	  if (found!=std::string::npos)
		    	  	 blur1_enabled = blur2_enabled = true;
		    	  else
		    	  {
		    		  found = program.find("GetBlur1");
		    		  if (found!=std::string::npos)
		    			  blur1_enabled = true;
		    	  }
		     }


	std::string temp;

	temp.append(cgTemplate);
	temp.append(program);

	std::cout<<"Cg: Compilation Results:"<<std::endl<<std::endl;
	std::cout<<program<<std::endl;

    p = cgCreateProgram(myCgContext,
								CG_SOURCE,
								temp.c_str(),//temp.c_str(),
								myCgProfile,
								"projectm",
								NULL);

    checkForCgCompileError("creating shader program");
    if (p==NULL) return false;

    cgGLLoadProgram(p);

    if (checkForCgCompileError("loading shader program"))
        {
        	p = NULL;
        	return false;
        }

	return true;
	}
	else return false;
}

bool Renderer::checkForCgCompileError(const char *situation)
{
  CGerror error;
  const char *string = cgGetLastErrorString(&error);
  error = cgGetError();
  if (error != CG_NO_ERROR) {
	  std::cout<<"Cg: Compilation Error"<<std::endl;
    std::cout<<"Cg: %"<<situation<<" - "<<string<<std::endl;
    if (error == CG_COMPILER_ERROR) {
      std::cout<<"Cg: "<< cgGetLastListing( myCgContext )<<std::endl;

    }
    return true;
  }

  return false;
}

void Renderer::checkForCgError(const char *situation)
{
  CGerror error;
  const char *string = cgGetLastErrorString(&error);

  if (error != CG_NO_ERROR) {
	std::cout<<"Cg: %"<<situation<<" - "<<string<<std::endl;
	if (error == CG_COMPILER_ERROR) {
	std::cout<<"Cg: "<< cgGetLastListing( myCgContext )<<std::endl;
    }
    exit(1);
  }
}

void Renderer::SetupCg()
{
	 std::string line;
	  std::ifstream myfile (CMAKE_INSTALL_PREFIX  "/share/projectM/shaders/projectM.cg");
	  if (myfile.is_open())
	  {
	    while (! myfile.eof() )
	    {
	      std::getline (myfile,line);
	      cgTemplate.append(line + "\n");
	    }
	    myfile.close();
	  }

	  else std::cout << "Unable to load shader template" << std::endl;

	  std::ifstream myfile2 (CMAKE_INSTALL_PREFIX  "/share/projectM/shaders/blur.cg");
	  if (myfile2.is_open())
	 	  {
	 	    while (! myfile2.eof() )
	 	    {
	 	      std::getline (myfile2,line);
	 	      blurProgram.append(line + "\n");
	 	    }
	 	    myfile2.close();
	 	  }

	 else std::cout << "Unable to load blur template" << std::endl;


  myCgContext = cgCreateContext();
  checkForCgError("creating context");
  cgGLSetDebugMode( CG_FALSE );
  cgSetParameterSettingMode(myCgContext, CG_DEFERRED_PARAMETER_SETTING);


  myCgProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
  cgGLSetOptimalOptions(myCgProfile);
  checkForCgError("selecting fragment profile");

  std::cout<<"Cg: Initialized profile: "<<cgGetProfileString(myCgProfile)<<std::endl;


  blur1Program = cgCreateProgram(myCgContext,
								CG_SOURCE,
								blurProgram.c_str(),
								myCgProfile,
								"blur1",
								NULL);

  checkForCgCompileError("creating blur1 program");
  if (blur1Program==NULL) exit(1);
  cgGLLoadProgram(blur1Program);

  checkForCgError("loading blur1 program");

  blur2Program = cgCreateProgram(myCgContext,
  								CG_SOURCE,
  								blurProgram.c_str(),
  								myCgProfile,
  								"blur2",
  								NULL);

    checkForCgCompileError("creating blur2 program");
    if (blur2Program==NULL) exit(1);
    cgGLLoadProgram(blur2Program);

    checkForCgError("loading blur2 program");

}

void Renderer::SetupCgVariables(CGprogram program, const Pipeline &pipeline, const PipelineContext &context)
{

	float slow_roam_cos[4] = { 0.5 + 0.5*cos(context.time * 0.005),0.5 + 0.5*cos(context.time * 0.008),0.5 + 0.5*cos(context.time * 0.013),0.5 + 0.5*cos(context.time * 0.022)};
    float roam_cos[4] ={ 0.5 + 0.5*cos(context.time * 0.3),0.5 + 0.5*cos(context.time * 1.3),0.5 + 0.5*cos(context.time * 5),0.5 + 0.5*cos(context.time * 20)};
    float slow_roam_sin[4] = { 0.5 + 0.5*sin(context.time * 0.005),0.5 + 0.5*sin(context.time * 0.008),0.5 + 0.5*sin(context.time * 0.013),0.5 + 0.5*sin(context.time * 0.022)};
    float roam_sin[4] ={ 0.5 + 0.5*sin(context.time * 0.3),0.5 + 0.5*sin(context.time * 1.3),0.5 + 0.5*sin(context.time * 5),0.5 + 0.5*sin(context.time * 20)};

    cgGLSetParameter4fv(cgGetNamedParameter(program, "slow_roam_cos"), slow_roam_cos);
    cgGLSetParameter4fv(cgGetNamedParameter(program, "roam_cos"), roam_cos);
    cgGLSetParameter4fv(cgGetNamedParameter(program, "slow_roam_sin"), slow_roam_sin);
    cgGLSetParameter4fv(cgGetNamedParameter(program, "roam_sin"), roam_sin);

	cgGLSetParameter1f(cgGetNamedParameter(program, "time"), context.time);
	cgGLSetParameter4f(cgGetNamedParameter(program, "rand_preset"), rand_preset[0], rand_preset[1],rand_preset[2],rand_preset[3]);
	cgGLSetParameter4f(cgGetNamedParameter(program, "rand_frame"), (rand()%100) * .01,(rand()%100) * .01,(rand()%100) * .01,(rand()%100) * .01);
	cgGLSetParameter1f(cgGetNamedParameter(program, "fps"), context.fps);
	cgGLSetParameter1f(cgGetNamedParameter(program, "frame"), context.frame);
	cgGLSetParameter1f(cgGetNamedParameter(program, "progress"), context.progress);

	cgGLSetParameter1f(cgGetNamedParameter(program, "blur1_min"), pipeline.shader.blur1n);
	cgGLSetParameter1f(cgGetNamedParameter(program, "blur1_max"), pipeline.shader.blur1x);
	cgGLSetParameter1f(cgGetNamedParameter(program, "blur2_min"), pipeline.shader.blur2n);
	cgGLSetParameter1f(cgGetNamedParameter(program, "blur2_max"), pipeline.shader.blur2x);
	cgGLSetParameter1f(cgGetNamedParameter(program, "blur3_min"), pipeline.shader.blur3n);
	cgGLSetParameter1f(cgGetNamedParameter(program, "blur3_max"), pipeline.shader.blur3x);

	cgGLSetParameter1f(cgGetNamedParameter(program, "bass"), beatDetect->bass);
	cgGLSetParameter1f(cgGetNamedParameter(program, "mid"), beatDetect->mid);
	cgGLSetParameter1f(cgGetNamedParameter(program, "treb"), beatDetect->treb);
	cgGLSetParameter1f(cgGetNamedParameter(program, "bass_att"), beatDetect->bass_att);
	cgGLSetParameter1f(cgGetNamedParameter(program, "mid_att"), beatDetect->mid_att);
	cgGLSetParameter1f(cgGetNamedParameter(program, "treb_att"), beatDetect->treb_att);
	cgGLSetParameter1f(cgGetNamedParameter(program, "vol"), beatDetect->vol);
	cgGLSetParameter1f(cgGetNamedParameter(program, "vol_att"), beatDetect->vol);

	cgGLSetParameter4f(cgGetNamedParameter(program, "texsize"), renderTarget->texsize, renderTarget->texsize, 1/(float)renderTarget->texsize,1/(float)renderTarget->texsize);
  	cgGLSetParameter4f(cgGetNamedParameter(program, "aspect"), 1/aspect,1,aspect,1);

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

  	if (blur1_enabled)
  	{
  		cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_blur1"),blur1_tex);
  		cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_blur1"));
  	}
  	if (blur2_enabled)
  	{
  		cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_blur2"),blur2_tex);
  		cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_blur2"));
  	}
  	if (blur3_enabled)
  	{
  	 	cgGLSetTextureParameter(cgGetNamedParameter(program, "sampler_blur3"),blur3_tex);
  	 	cgGLEnableTextureParameter(cgGetNamedParameter(program, "sampler_blur3"));
  	}

  	cgGLSetParameter4f(cgGetNamedParameter(program, "texsize_noise_lq"), 256, 256,1.0/(float)256,1.0/(float)256);
  	cgGLSetParameter4f(cgGetNamedParameter(program, "texsize_noise_mq"), 64, 64,1.0/(float)64,1.0/(float)64);
  	cgGLSetParameter4f(cgGetNamedParameter(program, "texsize_noise_hq"), 32, 32,1.0/(float)32,1.0/(float)32);
  	cgGLSetParameter4f(cgGetNamedParameter(program, "texsize_noise_perlin"), 512, 512,1.0/(float)512,1.0/(float)512);
  	cgGLSetParameter4f(cgGetNamedParameter(program, "texsize_noise_lq_lite"), 32, 32,1.0/(float)32,1.0/(float)32);
}

void Renderer::SetupCgQVariables(CGprogram program, const PresetOutputs &q)
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

void Renderer::RenderBlurTextures(const Pipeline *pipeline, const PipelineContext &pipelineContext)
{
if(blur1_enabled || blur2_enabled || blur3_enabled)
{
  	float tex[4][2] = {{0, 1},
  			   {0, 0},
  			   {1, 0},
  			   {1, 1}};

  	glBlendFunc(GL_ONE, GL_ZERO);
	glColor4f(1.0, 1.0, 1.0, 1.0f);

	glEnable(GL_TEXTURE_2D);

 	glEnableClientState(GL_VERTEX_ARRAY);
  	glDisableClientState(GL_COLOR_ARRAY);
  	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
 	glTexCoordPointer(2,GL_FLOAT,0,tex);

	cgGLEnableProfile(myCgProfile);
    checkForCgError("enabling profile");

	if (blur1_enabled)
	{
	  cgGLSetParameter4f(cgGetNamedParameter(blur1Program, "srctexsize"), renderTarget->texsize, renderTarget->texsize, 1/(float)renderTarget->texsize,1/(float)renderTarget->texsize);


	  cgGLBindProgram(blur1Program);
	  checkForCgError("binding blur1 program");

	  	float points[4][2]  = {{0, 1},
	  			   {0, 0},
	  			   {1, 0},
	  			   {1, 1}};


	  	glVertexPointer(2,GL_FLOAT,0,points);

	  	glDrawArrays(GL_TRIANGLE_FAN,0,4);


	  	glBindTexture(GL_TEXTURE_2D, blur1_tex);
	  	glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,renderTarget->texsize, renderTarget->texsize);

	  cgGLUnbindProgram(myCgProfile);
	  checkForCgError("unbinding blur1 program");
	}


	if (blur2_enabled)
	{
	  cgGLSetParameter4f(cgGetNamedParameter(blur2Program, "srctexsize"), renderTarget->texsize, renderTarget->texsize, 1/(float)renderTarget->texsize,1/(float)renderTarget->texsize);

	  cgGLBindProgram(blur2Program);
	  checkForCgError("binding blur2 program");

	  	float points[4][2]  = {{0, 0.5},
	  			   {0, 0},
	  			   {0.5, 0},
	  			   {0.5, 0.5}};


	  	glVertexPointer(2,GL_FLOAT,0,points);

	  	glDrawArrays(GL_TRIANGLE_FAN,0,4);


	  	glBindTexture(GL_TEXTURE_2D, blur2_tex);
	  	glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,renderTarget->texsize/2, renderTarget->texsize/2);


	}


	if (blur3_enabled)
	{
	  cgGLSetParameter4f(cgGetNamedParameter(blur2Program, "srctexsize"), renderTarget->texsize/2, renderTarget->texsize/2, 2/(float)renderTarget->texsize,2/(float)renderTarget->texsize);
		float points[4][2]  = {{0, 0.25},
		  			   {0, 0},
		  			   {0.25, 0},
		  			   {0.25, 0.25}};


	  	glVertexPointer(2,GL_FLOAT,0,points);

	  	glDrawArrays(GL_TRIANGLE_FAN,0,4);


	  	glBindTexture(GL_TEXTURE_2D, blur3_tex);
	  	glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,renderTarget->texsize/4, renderTarget->texsize/4);
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

#endif

void Renderer::ResetTextures()
{
	textureManager->Clear();

	delete(renderTarget);
	renderTarget = new RenderTarget(texsize, vw, vh);
	reset(vw, vh);

	textureManager->Preload();
}

void Renderer::SetupPass1(const Pipeline* pipeline, const PipelineContext &pipelineContext)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	totalframes++;
	renderTarget->lock();
	glViewport( 0, 0, renderTarget->texsize, renderTarget->texsize );

	glEnable( GL_TEXTURE_2D );

	//If using FBO, switch to FBO texture
	if(this->renderTarget->useFBO)
		glBindTexture( GL_TEXTURE_2D, renderTarget->textureID[1] );
	else
		glBindTexture( GL_TEXTURE_2D, renderTarget->textureID[0] );

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();

	#ifdef USE_GLES1
		glOrthof(0.0, 1, 0.0, 1, -40, 40);
	#else
		glOrtho(0.0, 1, 0.0, 1, -40, 40);
	#endif

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

#ifdef USE_CG
		RenderBlurTextures(pipeline,pipelineContext);
#endif
}

void Renderer::RenderItems(const Pipeline* pipeline, const PipelineContext &pipelineContext)
{
	renderContext.time = pipelineContext.time;
	renderContext.texsize = texsize;
	renderContext.aspectCorrect = correction;
	renderContext.aspectRatio = aspect;
	renderContext.textureManager = textureManager;
	renderContext.beatDetect = beatDetect;

	for (std::vector<RenderItem*>::const_iterator pos = pipeline->drawables.begin(); pos != pipeline->drawables.end(); ++pos)
		(*pos)->Draw(renderContext);
}



void Renderer::FinishPass1()
{
	draw_title_to_texture();
		/** Restore original view state */
		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();

		glMatrixMode( GL_PROJECTION );
		glPopMatrix();

		renderTarget->unlock();

}

void Renderer::Pass2(const Pipeline *pipeline, const PipelineContext &pipelineContext)
{
	//BEGIN PASS 2
	//
	//end of texture rendering
	//now we copy the texture from the FBO or framebuffer to
	//video texture memory and render fullscreen.

	/** Reset the viewport size */
#ifdef USE_FBO
	if(renderTarget->renderToTexture)
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->renderTarget->fbuffer[1]);
		glViewport( 0, 0, this->renderTarget->texsize, this->renderTarget->texsize );
	}
	else
#endif
	glViewport( 0, 0, this->vw, this->vh );



	glBindTexture( GL_TEXTURE_2D, this->renderTarget->textureID[0] );

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
#ifdef USE_GLES1
	glOrthof(-0.5, 0.5, -0.5, 0.5, -40, 40);
#else
	glOrtho(-0.5, 0.5, -0.5, 0.5, -40, 40);
#endif
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLineWidth( this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize/512.0);


	CompositeOutput(pipeline, pipelineContext);


	glMatrixMode(GL_MODELVIEW);
	glTranslatef(-0.5, -0.5, 0);

	// When console refreshes, there is a chance the preset has been changed by the user
	refreshConsole();
	draw_title_to_screen(false);
	if(this->showhelp%2) draw_help();
	if(this->showtitle%2) draw_title();
	if(this->showfps%2) draw_fps(this->realfps);
	if(this->showpreset%2) draw_preset();
	if(this->showstats%2) draw_stats();
	glTranslatef(0.5 , 0.5, 0);

#ifdef USE_FBO
	if(renderTarget->renderToTexture)
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
#endif
}

void Renderer::RenderFrame(const Pipeline* pipeline, const PipelineContext &pipelineContext)
{

	SetupPass1(pipeline, pipelineContext);

#ifdef USE_CG
	if (warpShadersEnabled)
			{

  cgGLEnableProfile(myCgProfile);
  checkForCgError("enabling warp profile");
  cgGLBindProgram(myCgWarpProgram);
  checkForCgError("binding warp program");

  SetupCgVariables(myCgWarpProgram, *pipeline, pipelineContext);
			}


#endif
		Interpolation(pipeline);
#ifdef USE_CG
		if (warpShadersEnabled)
				{
  cgGLUnbindProgram(myCgProfile);
  checkForCgError("disabling fragment profile");
  cgGLDisableProfile(myCgProfile);
  checkForCgError("disabling fragment profile");
				}
#endif


	    RenderItems(pipeline,pipelineContext);
	    FinishPass1();
	    Pass2(pipeline, pipelineContext);
}
void Renderer::RenderFrame(PresetOutputs *presetOutputs, PresetInputs *presetInputs)
{
	SetupPass1(presetOutputs, *presetInputs);

#ifdef USE_CG
	if (warpShadersEnabled)
			{
  cgGLEnableProfile(myCgProfile);
  checkForCgError("enabling warp profile");
  cgGLBindProgram(myCgWarpProgram);
  checkForCgError("binding warp program");

  SetupCgVariables(myCgWarpProgram, *presetOutputs, *presetInputs);
  SetupCgQVariables(myCgWarpProgram, *presetOutputs);
			}
#endif
  Interpolation(presetOutputs, presetInputs);
#ifdef USE_CG
		if (warpShadersEnabled)
				{
  cgGLUnbindProgram(myCgProfile);
  checkForCgError("disabling fragment profile");
  cgGLDisableProfile(myCgProfile);
  checkForCgError("disabling fragment profile");
				}
#endif

	RenderItems(presetOutputs, *presetInputs);
	FinishPass1();

#ifdef USE_CG
	if (compositeShadersEnabled)
		SetupCgQVariables(myCgCompositeProgram, *presetOutputs);
#endif
    Pass2(presetOutputs, *presetInputs);
}


void Renderer::Interpolation(PresetOutputs *presetOutputs, PresetInputs *presetInputs)
{
	//Texture wrapping( clamp vs. wrap)
	if (presetOutputs->textureWrap==0)
	{
#ifdef USE_GLES1
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
#endif
	}
	else
	{ glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);}

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

	glColor4f(1.0, 1.0, 1.0, presetOutputs->screenDecay);

	glEnable(GL_TEXTURE_2D);

	int size = presetInputs->gy;

	float p[size*2][2];
	float t[size*2][2];

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,p);
	glTexCoordPointer(2,GL_FLOAT,0,t);

	for (int x=0;x<presetInputs->gx - 1;x++)
	{
		for(int y=0;y<presetInputs->gy;y++)
		{
		  t[y*2][0] = presetOutputs->x_mesh[x][y];
		  t[y*2][1] = presetOutputs->y_mesh[x][y];

		  p[y*2][0] = this->gridx[x][y];
		  p[y*2][1] = this->gridy[x][y];

		  t[(y*2)+1][0] = presetOutputs->x_mesh[x+1][y];
		  t[(y*2)+1][1] = presetOutputs->y_mesh[x+1][y];

		  p[(y*2)+1][0] = this->gridx[x+1][y];
		  p[(y*2)+1][1] = this->gridy[x+1][y];

		}
	       	glDrawArrays(GL_TRIANGLE_STRIP,0,size*2);
	}



	glDisable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}


void Renderer::Interpolation(const Pipeline *pipeline)
{
	//Texture wrapping( clamp vs. wrap)
	if (pipeline->textureWrap==0)
	{
#ifdef USE_GLES1
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
#endif
	}
	else
	{ glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);}

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

	glColor4f(1.0, 1.0, 1.0, pipeline->screenDecay);

	glEnable(GL_TEXTURE_2D);

	int size = mesh.width * 2;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	float p[size][2];
	float t[size][2];

	glVertexPointer(2,GL_FLOAT,0,p);
	glTexCoordPointer(2,GL_FLOAT,0,t);

	mesh.Reset();
	currentPipe = const_cast<Pipeline*>(pipeline);
	omptl::transform(mesh.p.begin(), mesh.p.end(), mesh.identity.begin(), mesh.p.begin(), &Renderer::PerPixel);

	for (int j=0;j<mesh.height - 1;j++)
	{
				for(int i=0;i<mesh.width;i++)
				{
					int index = j * mesh.width + i;
					int index2 = (j+1) * mesh.width + i;

					t[i*2][0] = mesh.p[index].x;
					t[i*2][1] = mesh.p[index].y;
					p[i*2][0] = mesh.identity[index].x;
					p[i*2][1] = mesh.identity[index].y;

					t[(i*2)+1][0] = mesh.p[index2].x;
					t[(i*2)+1][1] = mesh.p[index2].y;
					p[(i*2)+1][0] = mesh.identity[index2].x;
					p[(i*2)+1][1] = mesh.identity[index2].y;
  			  		}
		       	glDrawArrays(GL_TRIANGLE_STRIP,0,size);
	}

	glDisable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
Pipeline* Renderer::currentPipe;


Renderer::~Renderer()
{

	int x;


	if (renderTarget)
		delete(renderTarget);
	if (textureManager)
		delete(textureManager);

//std::cerr << "grid assign end" << std::endl;

	for(x = 0; x < mesh.width; x++)
		{
			free(this->gridx[x]);
			free(this->gridy[x]);
			free(this->origx2[x]);
			free(this->origy2[x]);
		}


		//std::cerr << "freeing grids" << std::endl;
		free(this->origx2);
		free(this->origy2);
		free(this->gridx);
		free(this->gridy);

	//std:cerr << "grid assign begin " << std::endl;
		this->origx2 = NULL;
		this->origy2 = NULL;
		this->gridx = NULL;
		this->gridy = NULL;

#ifdef USE_FTGL
//	std::cerr << "freeing title fonts" << std::endl;
	if (title_font)
		delete title_font;
	if (poly_font)
		delete poly_font;
	if (other_font)
		delete other_font;
//	std::cerr << "freeing title fonts finished" << std::endl;
#endif
//	std::cerr << "exiting destructor" << std::endl;
}


void Renderer::PerPixelMath(PresetOutputs * presetOutputs, PresetInputs * presetInputs)
{

	int x, y;
	float fZoom2, fZoom2Inv;


	for (x=0;x<mesh.width;x++)
	{
		for(y=0;y<mesh.height;y++)
		{
			fZoom2 = powf( presetOutputs->zoom_mesh[x][y], powf( presetOutputs->zoomexp_mesh[x][y], presetInputs->rad_mesh[x][y]*2.0f - 1.0f));
			fZoom2Inv = 1.0f/fZoom2;
			presetOutputs->x_mesh[x][y]= this->origx2[x][y]*0.5f*fZoom2Inv + 0.5f;
			presetOutputs->y_mesh[x][y]= this->origy2[x][y]*0.5f*fZoom2Inv + 0.5f;
		}
	}

	for (x=0;x<mesh.width;x++)
	{
		for(y=0;y<mesh.height;y++)
		{
			presetOutputs->x_mesh[x][y]  = ( presetOutputs->x_mesh[x][y] - presetOutputs->cx_mesh[x][y])/presetOutputs->sx_mesh[x][y] + presetOutputs->cx_mesh[x][y];
		}
	}

	for (x=0;x<mesh.width;x++)
	{
		for(y=0;y<mesh.height;y++)
		{
			presetOutputs->y_mesh[x][y] = ( presetOutputs->y_mesh[x][y] - presetOutputs->cy_mesh[x][y])/presetOutputs->sy_mesh[x][y] + presetOutputs->cy_mesh[x][y];
		}
	}

	float fWarpTime = presetInputs->time * presetOutputs->fWarpAnimSpeed;
	float fWarpScaleInv = 1.0f / presetOutputs->fWarpScale;
	float f[4];
	f[0] = 11.68f + 4.0f*cosf(fWarpTime*1.413f + 10);
	f[1] =  8.77f + 3.0f*cosf(fWarpTime*1.113f + 7);
	f[2] = 10.54f + 3.0f*cosf(fWarpTime*1.233f + 3);
	f[3] = 11.49f + 4.0f*cosf(fWarpTime*0.933f + 5);

	for (x=0;x<mesh.width;x++)
	{
		for(y=0;y<mesh.height;y++)
		{
			presetOutputs->x_mesh[x][y] += presetOutputs->warp_mesh[x][y]*0.0035f*sinf(fWarpTime*0.333f + fWarpScaleInv*(this->origx2[x][y]*f[0] - this->origy2[x][y]*f[3]));
			presetOutputs->y_mesh[x][y] += presetOutputs->warp_mesh[x][y]*0.0035f*cosf(fWarpTime*0.375f - fWarpScaleInv*(this->origx2[x][y]*f[2] + this->origy2[x][y]*f[1]));
			presetOutputs->x_mesh[x][y] += presetOutputs->warp_mesh[x][y]*0.0035f*cosf(fWarpTime*0.753f - fWarpScaleInv*(this->origx2[x][y]*f[1] - this->origy2[x][y]*f[2]));
			presetOutputs->y_mesh[x][y] += presetOutputs->warp_mesh[x][y]*0.0035f*sinf(fWarpTime*0.825f + fWarpScaleInv*(this->origx2[x][y]*f[0] + this->origy2[x][y]*f[3]));
		}
	}
	for (x=0;x<mesh.width;x++)
	{
		for(y=0;y<mesh.height;y++)
		{
			float u2 = presetOutputs->x_mesh[x][y] - presetOutputs->cx_mesh[x][y];
			float v2 = presetOutputs->y_mesh[x][y] - presetOutputs->cy_mesh[x][y];

			float cos_rot = cosf(presetOutputs->rot_mesh[x][y]);
			float sin_rot = sinf(presetOutputs->rot_mesh[x][y]);

			presetOutputs->x_mesh[x][y] = u2*cos_rot - v2*sin_rot + presetOutputs->cx_mesh[x][y];
			presetOutputs->y_mesh[x][y] = u2*sin_rot + v2*cos_rot + presetOutputs->cy_mesh[x][y];

		}
	}


	for (x=0;x<mesh.width;x++)
	{
		for(y=0;y<mesh.height;y++)
		{
			presetOutputs->x_mesh[x][y] -= presetOutputs->dx_mesh[x][y];
		}
	}



	for (x=0;x<mesh.width;x++)
	{
		for(y=0;y<mesh.height;y++)
		{
			presetOutputs->y_mesh[x][y] -= presetOutputs->dy_mesh[x][y];
		}
	}

}




void Renderer::reset(int w, int h)
{
	this->aspect=(float)h / (float)w;
	this -> vw = w;
	this -> vh = h;

	glShadeModel( GL_SMOOTH);

	glCullFace( GL_BACK );
	//glFrontFace( GL_CCW );

	glClearColor( 0, 0, 0, 0 );

	glViewport( 0, 0, w, h );

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

#ifndef USE_GLES1
	glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);
#endif
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable( GL_LINE_SMOOTH );


	glEnable(GL_POINT_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT);

#ifndef USE_GLES1
	glLineStipple(2, 0xAAAA);
#endif

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_MODULATE);

	//glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	if (!this->renderTarget->useFBO)
	{
		this->renderTarget->fallbackRescale(w, h);
	}
}




GLuint Renderer::initRenderToTexture()
{
	return renderTarget->initRenderToTexture();
}


void Renderer::draw_title_to_texture()
{
#ifdef USE_FTGL
	if (this->drawtitle>100)
	{
		draw_title_to_screen(true);
		this->drawtitle=0;
	}
#endif /** USE_FTGL */
}


float title_y;

void Renderer::draw_title_to_screen(bool flip)
{

#ifdef USE_FTGL
	if(this->drawtitle>0)
	{

	  //setUpLighting();

		//glEnable(GL_POLYGON_SMOOTH);
		//glEnable( GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);

		int draw;
		if (drawtitle>=80) draw = 80;
		else draw = drawtitle;

		float easein = ((80-draw)*.0125);
		float easein2 = easein * easein;

		if(drawtitle==1)
		{
			title_y = (float)rand()/RAND_MAX;
			title_y *= 2;
			title_y -= 1;
			title_y *= .6;
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glBlendFunc(GL_SRC_ALPHA_SATURATE,GL_ONE);
		glColor4f(1.0, 1.0, 1.0, 1.0);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glFrustum(-1, 1, -1 * (float)vh/(float)vw, 1 *(float)vh/(float)vw, 1, 1000);
		if (flip) glScalef(1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glTranslatef(-850, title_y * 850 *vh/vw  , easein2*900-900);

		glRotatef(easein2*360, 1, 0, 0);

		poly_font->Render(this->title.c_str() );

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		this->drawtitle++;

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);

		glDisable( GL_CULL_FACE);
		glDisable( GL_DEPTH_TEST);

		glDisable(GL_COLOR_MATERIAL);

		glDisable(GL_LIGHTING);
		glDisable(GL_POLYGON_SMOOTH);
	}
#endif /** USE_FTGL */
}



void Renderer::draw_title()
{
#ifdef USE_FTGL
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	//  glPushMatrix();
	//  glTranslatef(this->vw*.001,this->vh*.03, -1);
	//  glScalef(this->vw*.015,this->vh*.025,0);

	glRasterPos2f(0.01, 0.05);
	title_font->FaceSize( (unsigned)(20*(this->vh/512.0)));


	title_font->Render(this->title.c_str() );
	//  glPopMatrix();
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

#endif /** USE_FTGL */
}

void Renderer::draw_preset()
{
#ifdef USE_FTGL
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	//      glPushMatrix();
	//glTranslatef(this->vw*.001,this->vh*-.01, -1);
	//glScalef(this->vw*.003,this->vh*.004,0);


	glRasterPos2f(0.01, 0.01);

	title_font->FaceSize((unsigned)(12*(this->vh/512.0)));
	if(this->noSwitch) title_font->Render("[LOCKED]  " );
	title_font->FaceSize((unsigned)(20*(this->vh/512.0)));

	title_font->Render(this->presetName().c_str() );



	//glPopMatrix();
	// glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
#endif /** USE_FTGL */
}

void Renderer::draw_help( )
{

#ifdef USE_FTGL
//glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(0, 1, 0);
	//glScalef(this->vw*.02,this->vh*.02 ,0);


	title_font->FaceSize((unsigned)( 18*(this->vh/512.0)));

	glRasterPos2f(0.01, -0.05);
	title_font->Render("Help");

	glRasterPos2f(0.01, -0.09);
	title_font->Render("----------------------------");

	glRasterPos2f(0.01, -0.13);
	title_font->Render("F1: This help menu");

	glRasterPos2f(0.01, -0.17);
	title_font->Render("F2: Show song title");

	glRasterPos2f(0.01, -0.21);
	title_font->Render("F3: Show preset name");

	glRasterPos2f(0.01, -0.25);
	title_font->Render("F4: Show Rendering Settings");

	glRasterPos2f(0.01, -0.29);
	title_font->Render("F5: Show FPS");

	glRasterPos2f(0.01, -0.35);
	title_font->Render("F: Fullscreen");

	glRasterPos2f(0.01, -0.39);
	title_font->Render("L: Lock/Unlock Preset");

	glRasterPos2f(0.01, -0.43);
	title_font->Render("M: Show Menu");

	glRasterPos2f(0.01, -0.49);
	title_font->Render("R: Random preset");
	glRasterPos2f(0.01, -0.53);
	title_font->Render("N: Next preset");

	glRasterPos2f(0.01, -0.57);
	title_font->Render("P: Previous preset");

	glPopMatrix();
	//         glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

#endif /** USE_FTGL */
}

void Renderer::draw_stats()
{

#ifdef USE_FTGL
	char buffer[128];
	float offset= (this->showfps%2 ? -0.05 : 0.0);
	// glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(0.01, 1, 0);
	glRasterPos2f(0, -.05+offset);
	other_font->Render(this->correction ? "  aspect: corrected" : "  aspect: stretched");
	sprintf( buffer, " (%f)", this->aspect);
	other_font->Render(buffer);



	glRasterPos2f(0, -.09+offset);
	other_font->FaceSize((unsigned)(18*(vh/512.0)));

	sprintf( buffer, "       texsize: %d", renderTarget->texsize);
	other_font->Render(buffer);

	glRasterPos2f(0, -.13+offset);
	sprintf( buffer, "      viewport: %d x %d", vw, vh);

	other_font->Render(buffer);
	glRasterPos2f(0, -.17+offset);
	other_font->Render((renderTarget->useFBO ? "           FBO: on" : "           FBO: off"));

	glRasterPos2f(0, -.21+offset);
	sprintf( buffer, "          mesh: %d x %d", mesh.width, mesh.height);
	other_font->Render(buffer);

	glRasterPos2f(0, -.25+offset);
	sprintf( buffer, "      textures: %.1fkB", textureManager->getTextureMemorySize() /1000.0f);
	other_font->Render(buffer);
#ifdef USE_CG
	glRasterPos2f(0, -.29+offset);
	sprintf( buffer, "shader profile: %s", cgGetProfileString(myCgProfile));
	other_font->Render(buffer);

	glRasterPos2f(0, -.33+offset);
	sprintf( buffer, "   warp shader: %s", warpShadersEnabled ? "on" : "off");
	other_font->Render(buffer);

	glRasterPos2f(0, -.37+offset);
	sprintf( buffer, "   comp shader: %s", compositeShadersEnabled ? "on" : "off");
	other_font->Render(buffer);
#endif
	glPopMatrix();
	// glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);



#endif /** USE_FTGL */
}
void Renderer::draw_fps( float realfps )
{
#ifdef USE_FTGL
	char bufferfps[20];
	sprintf( bufferfps, "%.1f fps", realfps);
	// glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(0.01, 1, 0);
	glRasterPos2f(0, -0.05);
	title_font->FaceSize((unsigned)(20*(this->vh/512.0)));
	title_font->Render(bufferfps);

	glPopMatrix();
	// glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

#endif /** USE_FTGL */
}



void Renderer::CompositeOutput(const Pipeline* pipeline, const PipelineContext &pipelineContext)
{


	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Overwrite anything on the screen
	glBlendFunc(GL_ONE, GL_ZERO);
	glColor4f(1.0, 1.0, 1.0, 1.0f);

	glEnable(GL_TEXTURE_2D);

#ifdef USE_CG
	if (compositeShadersEnabled)
	{

  cgGLEnableProfile(myCgProfile);
  checkForCgError("enabling warp profile");

  cgGLBindProgram(myCgCompositeProgram);
  checkForCgError("binding warp program");

  SetupCgVariables(myCgCompositeProgram, *pipeline, pipelineContext);
	}
#endif


	float tex[4][2] = {{0, 1},
			   {0, 0},
			   {1, 0},
			   {1, 1}};

	float points[4][2] = {{-0.5, -0.5},
			      {-0.5,  0.5},
			      { 0.5,  0.5},
			      { 0.5,  -0.5}};

	glEnableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(2,GL_FLOAT,0,points);
	glTexCoordPointer(2,GL_FLOAT,0,tex);

	glDrawArrays(GL_TRIANGLE_FAN,0,4);

	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

#ifdef USE_CG
	if (compositeShadersEnabled)
		{
  cgGLUnbindProgram(myCgProfile);
  checkForCgError("disabling fragment profile");
  cgGLDisableProfile(myCgProfile);
  checkForCgError("disabling fragment profile");
		}
#endif

	for (std::vector<RenderItem*>::const_iterator pos = pipeline->compositeDrawables.begin(); pos != pipeline->compositeDrawables.end(); ++pos)
			;//(*pos)->Draw(renderContext);

}




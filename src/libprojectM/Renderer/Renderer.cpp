#include "Renderer.hpp"
#include "wipemalloc.h"
#include "math.h"
#include "Common.hpp"
#include "KeyHandler.hpp"
#include "TextureManager.hpp"
#include <iostream>
#include <algorithm>
#include <sys/stat.h>
#include <cassert>
#include "omptl/omptl"
#include "omptl/omptl_algorithm"
#include "UserTexture.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Preset;

Renderer::Renderer(int width, int height, int gx, int gy, int texsize, BeatDetect *beatDetect, std::string _presetURL,
		std::string _titlefontURL, std::string _menufontURL) :
	title_fontURL(_titlefontURL), menu_fontURL(_menufontURL), presetURL(_presetURL), m_presetName("None"), vw(width),
			vh(height), texsize(texsize), mesh(gx, gy)
{
	int x;
	int y;

	this->totalframes = 1;
	this->noSwitch = false;
	this->showfps = false;
	this->showtitle = false;
	this->showpreset = false;
	this->showhelp = false;
	this->showstats = false;
	this->studio = false;
	this->realfps = 0;

	this->drawtitle = 0;

	//this->title = "Unknown";

	/** Other stuff... */
	this->correction = true;
	this->aspect = (float) height / (float) width;;

	/// @bug put these on member init list
	this->renderTarget = new RenderTarget(texsize, width, height);
	this->textureManager = new TextureManager(presetURL);
	this->beatDetect = beatDetect;

#ifdef USE_FTGL
	/** Load the standard fonts if they do exist */
	struct stat buffer;

	if (stat( title_fontURL.c_str(), &buffer )) {
	  std::cout << "Could not open font file: " << title_fontURL << std::endl;
	  exit(1);
	}
	if (stat( menu_fontURL.c_str(), &buffer )) {
	  std::cout << "Could not open font file: " << menu_fontURL << std::endl;
	  exit(1);
	}

	title_font = new FTGLPixmapFont(title_fontURL.c_str());
	other_font = new FTGLPixmapFont(menu_fontURL.c_str());
	poly_font = new FTGLExtrdFont(title_fontURL.c_str());

	if(title_font->Error()) {
		fprintf(stderr, "Failed to open font %s\n", title_fontURL.c_str());
	} else {
		title_font->UseDisplayList(true);
	}

	other_font->UseDisplayList(true);

	if(poly_font->Error()) {
		fprintf(stderr, "Failed to open font %s\n", title_fontURL.c_str());
	} else {
		poly_font->UseDisplayList(true);
		poly_font->Depth(20);
		poly_font->FaceSize(72);
	}

#endif /** USE_FTGL */


    int size = (mesh.height - 1) *mesh.width * 4 * 2;
	p = ( float * ) wipemalloc ( size * sizeof ( float ) );


	for (int j = 0; j < mesh.height - 1; j++)
	{
        int base = j * mesh.width * 4 * 2;


		for (int i = 0; i < mesh.width; i++)
		{
			int index = j * mesh.width + i;
			int index2 = (j + 1) * mesh.width + i;

            int strip = base + i * 8;
            p[strip + 0] = mesh.identity[index].x;
            p[strip + 1] = mesh.identity[index].y;

            p[strip + 4] = mesh.identity[index2].x;
            p[strip + 5] = mesh.identity[index2].y;
		}
	}

    renderContext.programID_v2f_c4f = shaderEngine.programID_v2f_c4f;
    renderContext.programID_v2f_c4f_t2f = shaderEngine.programID_v2f_c4f_t2f;

#ifdef USE_CG
	shaderEngine.setParams(renderTarget->texsize, renderTarget->textureID[1], aspect, beatDetect, textureManager);
#endif

    // Interpolation VAO/VBO's
    glGenBuffers(1, &m_vbo_Interpolation);
    glGenVertexArrays(1, &m_vao_Interpolation);

    glBindVertexArray(m_vao_Interpolation);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_Interpolation);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0); // Positions

    glDisableVertexAttribArray(1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2)); // Textures

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // CompositeOutput VAO/VBO's
    glGenBuffers(1, &m_vbo_CompositeOutput);
    glGenVertexArrays(1, &m_vao_CompositeOutput);

    float composite_buffer_data[8][2] =
    {
        { -0.5, -0.5 },
        { 0, 1 },
        { -0.5, 0.5 },
        { 0, 0 },
        { 0.5, 0.5 },
        { 1, 0 },
        { 0.5, -0.5 },
        { 1, 1 } };


    glBindVertexArray(m_vao_CompositeOutput);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_CompositeOutput);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 2, composite_buffer_data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)0); // Positions

    glDisableVertexAttribArray(1);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*4, (void*)(sizeof(float)*2)); // Textures

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Renderer::SetPipeline(Pipeline &pipeline)
{
	currentPipe = &pipeline;
#ifdef USE_CG
	shaderEngine.reset();
	shaderEngine.loadShader(pipeline.warpShader, pipeline.warpShaderFilename);
	shaderEngine.loadShader(pipeline.compositeShader, pipeline.compositeShaderFilename);
#endif
}

void Renderer::ResetTextures()
{
	textureManager->Clear();

	delete (renderTarget);
	renderTarget = new RenderTarget(texsize, vw, vh);
	reset(vw, vh);

	textureManager->Preload();
}

void Renderer::SetupPass1(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{
	totalframes++;
	renderTarget->lock();
	glViewport(0, 0, renderTarget->texsize, renderTarget->texsize);

    renderContext.mat_ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -40.0f, 40.0f);

#ifdef USE_CG
	shaderEngine.RenderBlurTextures(pipeline, pipelineContext, renderTarget->texsize);
#endif
}

void Renderer::RenderItems(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{
	renderContext.time = pipelineContext.time;
	renderContext.texsize = texsize;
	renderContext.aspectCorrect = correction;
	renderContext.aspectRatio = aspect;
	renderContext.textureManager = textureManager;
	renderContext.beatDetect = beatDetect;

	for (std::vector<RenderItem*>::const_iterator pos = pipeline.drawables.begin(); pos != pipeline.drawables.end(); ++pos)
    {
      if (*pos != NULL)
      {
		(*pos)->Draw(renderContext);
      }
    }
}

void Renderer::FinishPass1()
{
	draw_title_to_texture();
	renderTarget->unlock();

}

void Renderer::Pass2(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{
	//BEGIN PASS 2
	//
	//end of texture rendering
	//now we copy the texture from the FBO or framebuffer to
	//video texture memory and render fullscreen.

	/** Reset the viewport size */
#ifdef USE_FBO
	if (renderTarget->renderToTexture)
	{
        glBindFramebuffer(GL_FRAMEBUFFER, this->renderTarget->fbuffer[1]);
		glViewport(0, 0, this->renderTarget->texsize, this->renderTarget->texsize);
	}
	else
#endif
		glViewport(0, 0, this->vw, this->vh);

	glBindTexture(GL_TEXTURE_2D, this->renderTarget->textureID[0]);

    renderContext.mat_ortho = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -40.0f, 40.0f);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLineWidth(this->renderTarget->texsize < 512 ? 1 : this->renderTarget->texsize / 512.0);

	CompositeOutput(pipeline, pipelineContext);

/* FTGL does not support OpenGL ES
#ifndef EMSCRIPTEN
	glMatrixMode(GL_MODELVIEW);
#endif
	glLoadIdentity();
	glTranslatef(-0.5, -0.5, 0);

	// When console refreshes, there is a chance the preset has been changed by the user
	refreshConsole();
	draw_title_to_screen(false);
	if (this->showhelp % 2)
		draw_help();
	if (this->showtitle % 2)
		draw_title();
	if (this->showfps % 2)
		draw_fps(this->realfps);
	if (this->showpreset % 2)
		draw_preset();
	if (this->showstats % 2)
		draw_stats();
	glTranslatef(0.5, 0.5, 0);
*/

#ifdef USE_FBO
	if (renderTarget->renderToTexture)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
}

void Renderer::RenderFrame(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{

#ifdef USE_FBO
    // when not 'renderToTexture', the user may use its own couple FBO/texture
    // so retrieve this external FBO if it exists, (0 means no FBO) and unbind it
    GLint externalFBO = 0;
    if (!renderTarget->renderToTexture)
    {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &externalFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
#endif

	SetupPass1(pipeline, pipelineContext);

#ifdef USE_CG
	shaderEngine.enableShader(currentPipe->warpShader, pipeline, pipelineContext);
#endif
	Interpolation(pipeline);
#ifdef USE_CG
	shaderEngine.disableShader();
#endif

	RenderItems(pipeline, pipelineContext);
	FinishPass1();

#ifdef USE_FBO
    // when not 'renderToTexture', the user may use its own couple FBO/texture
    // if it exists (0 means no external FBO)
    // then rebind it just before calling the final pass: Pass2
    if (!renderTarget->renderToTexture && externalFBO != 0)
        glBindFramebuffer(GL_FRAMEBUFFER, externalFBO);
#endif

	Pass2(pipeline, pipelineContext);
}

void Renderer::Interpolation(const Pipeline &pipeline)
{
	if (this->renderTarget->useFBO)
		glBindTexture(GL_TEXTURE_2D, renderTarget->textureID[1]);
	else
		glBindTexture(GL_TEXTURE_2D, renderTarget->textureID[0]);

	//Texture wrapping( clamp vs. wrap)
	if (pipeline.textureWrap == 0)
	{
#ifdef USE_GLES
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#else
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
	}
	else
	{
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

    glActiveTexture(GL_TEXTURE0);

    int size = (mesh.height - 1) * mesh.width * 4 * 2;

	if (pipeline.staticPerPixel)
	{
		for (int j = 0; j < mesh.height - 1; j++)
		{
            int base = j * mesh.width * 2 * 4;

			for (int i = 0; i < mesh.width; i++)
			{
                int strip = base + i * 8;
                p[strip + 2] = pipeline.x_mesh[i][j];
                p[strip + 3] = pipeline.y_mesh[i][j];

                p[strip + 6] = pipeline.x_mesh[i][j+1];
                p[strip + 7] = pipeline.y_mesh[i][j+1];
			}
		}

	}
	else
	{
		mesh.Reset();
		omptl::transform(mesh.p.begin(), mesh.p.end(), mesh.identity.begin(), mesh.p.begin(), &Renderer::PerPixel);

	for (int j = 0; j < mesh.height - 1; j++)
	{
            int base = j * mesh.width * 2 * 4;

			for (int i = 0; i < mesh.width; i++)
			{
                int strip = base + i * 8;
				int index = j * mesh.width + i;
				int index2 = (j + 1) * mesh.width + i;

                p[strip + 2] = mesh.p[index].x;
                p[strip + 3] = mesh.p[index].y;

                p[strip + 6] = mesh.p[index2].x;
                p[strip + 7] = mesh.p[index2].y;


			}
		}
	}

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_Interpolation);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, nullptr, GL_DYNAMIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, p, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(renderContext.programID_v2f_c4f_t2f);

    glUniformMatrix4fv(ShaderEngine::Uniform_V2F_C4F_T2F_VertexTranformation(), 1, GL_FALSE, glm::value_ptr(renderContext.mat_ortho));
    glUniform1i(ShaderEngine::Uniform_V2F_C4F_T2F_FragTextureSampler(), 0);

    glVertexAttrib4f(1, 1.0, 1.0, 1.0, pipeline.screenDecay);

    glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

    glBindVertexArray(m_vao_Interpolation);

	for (int j = 0; j < mesh.height - 1; j++)
		glDrawArrays(GL_TRIANGLE_STRIP,j* mesh.width* 2,mesh.width*2);

    glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
Pipeline* Renderer::currentPipe;

Renderer::~Renderer()
{

	int x;

	if (renderTarget)
		delete (renderTarget);
	if (textureManager)
		delete (textureManager);

	//std::cerr << "grid assign end" << std::endl;

	free(p);

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

    glDeleteBuffers(1, &m_vbo_Interpolation);
    glDeleteVertexArrays(1, &m_vao_Interpolation);

    glDeleteBuffers(1, &m_vbo_CompositeOutput);
    glDeleteVertexArrays(1, &m_vao_CompositeOutput);
}

void Renderer::reset(int w, int h)
{
	aspect = (float) h / (float) w;
	this -> vw = w;
	this -> vh = h;

#if USE_CG
	shaderEngine.setAspect(aspect);
#endif

	glCullFace(GL_BACK);
	//glFrontFace( GL_CCW );

#ifndef USE_GLES
    glEnable(GL_LINE_SMOOTH);
#endif


	glClearColor(0, 0, 0, 0);

	glViewport(0, 0, w, h);


    glEnable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClear(GL_COLOR_BUFFER_BIT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // TODO: how to port this to modern openGL ?
    // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


//	if (!this->renderTarget->useFBO)
//	{
//		this->renderTarget->fallbackRescale(w, h);
//	}
}

GLuint Renderer::initRenderToTexture()
{
	return renderTarget->initRenderToTexture();
}

void Renderer::draw_title_to_texture()
{
#ifdef USE_FTGL
	if (this->drawtitle > 100)
	{
		draw_title_to_screen(true);
		this->drawtitle = 0;
	}
#endif /** USE_FTGL */
}

float title_y;

void Renderer::draw_title_to_screen(bool flip)
{

#ifdef USE_FTGL
	if (this->drawtitle > 0)
	{

		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);

		int draw;
		if (drawtitle >= 80)
			draw = 80;
		else
			draw = drawtitle;

		float easein = ((80 - draw) * .0125);
		float easein2 = easein * easein;

		if (drawtitle == 1)
		{
			title_y = (float) rand() / RAND_MAX;
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

		glFrustum(-1, 1, -1 * (float) vh / (float) vw, 1 * (float) vh / (float) vw, 1, 1000);
		if (flip)
			glScalef(1, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glTranslatef(-850, title_y * 850 * vh / vw, easein2 * 900 - 900);

		glRotatef(easein2 * 360, 1, 0, 0);

		poly_font->Render(this->title.c_str());

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		this->drawtitle++;

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

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
	title_font->FaceSize((unsigned) (20 * (this->vh / 512.0)));

	title_font->Render(this->title.c_str());
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

	title_font->FaceSize((unsigned) (12 * (this->vh / 512.0)));
	if (this->noSwitch)
		title_font->Render("[LOCKED]  ");
	title_font->FaceSize((unsigned) (20 * (this->vh / 512.0)));

	title_font->Render(this->presetName().c_str());

	//glPopMatrix();
	// glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
#endif /** USE_FTGL */
}

void Renderer::draw_help()
{

#ifdef USE_FTGL
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(0, 1, 0);
	//glScalef(this->vw*.02,this->vh*.02 ,0);


	title_font->FaceSize((unsigned) (18 * (this->vh / 512.0)));

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
	float offset = (this->showfps % 2 ? -0.05 : 0.0);
	// glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(0.01, 1, 0);
	glRasterPos2f(0, -.05 + offset);
	other_font->Render(this->correction ? "  aspect: corrected" : "  aspect: stretched");
	sprintf(buffer, " (%f)", this->aspect);
	other_font->Render(buffer);

	glRasterPos2f(0, -.09 + offset);
	other_font->FaceSize((unsigned) (18 * (vh / 512.0)));

	sprintf(buffer, "       texsize: %d", renderTarget->texsize);
	other_font->Render(buffer);

	glRasterPos2f(0, -.13 + offset);
	sprintf(buffer, "      viewport: %d x %d", vw, vh);

	other_font->Render(buffer);
	glRasterPos2f(0, -.17 + offset);
	other_font->Render((renderTarget->useFBO ? "           FBO: on" : "           FBO: off"));

	glRasterPos2f(0, -.21 + offset);
	sprintf(buffer, "          mesh: %d x %d", mesh.width, mesh.height);
	other_font->Render(buffer);

	glRasterPos2f(0, -.25 + offset);
	sprintf(buffer, "      textures: %.1fkB", textureManager->getTextureMemorySize() / 1000.0f);
	other_font->Render(buffer);
#ifdef USE_CG
	glRasterPos2f(0, -.29 + offset);
	sprintf(buffer, "shader profile: %s", shaderEngine.profileName.c_str());
	other_font->Render(buffer);

	glRasterPos2f(0, -.33 + offset);
	sprintf(buffer, "   warp shader: %s", currentPipe->warpShader.enabled ? "on" : "off");
	other_font->Render(buffer);

	glRasterPos2f(0, -.37 + offset);
	sprintf(buffer, "   comp shader: %s", currentPipe->compositeShader.enabled ? "on" : "off");
	other_font->Render(buffer);
#endif
	glPopMatrix();
	// glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


#endif /** USE_FTGL */
}
void Renderer::draw_fps(float realfps)
{
#ifdef USE_FTGL
	char bufferfps[20];
	sprintf(bufferfps, "%.1f fps", realfps);
	// glBlendFunc(GL_ONE_MINUS_DST_COLOR,GL_ZERO);

	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPushMatrix();
	glTranslatef(0.01, 1, 0);
	glRasterPos2f(0, -0.05);
	title_font->FaceSize((unsigned) (20 * (this->vh / 512.0)));
	title_font->Render(bufferfps);

	glPopMatrix();
	// glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

#endif /** USE_FTGL */
}

void Renderer::CompositeOutput(const Pipeline &pipeline, const PipelineContext &pipelineContext)
{
    glUseProgram(renderContext.programID_v2f_c4f_t2f);

    glUniformMatrix4fv(ShaderEngine::Uniform_V2F_C4F_T2F_VertexTranformation(), 1, GL_FALSE, glm::value_ptr(renderContext.mat_ortho));
    glUniform1i(ShaderEngine::Uniform_V2F_C4F_T2F_FragTextureSampler(), 0);

	//Overwrite anything on the screen
	glBlendFunc(GL_ONE, GL_ZERO);
    glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

    glActiveTexture(GL_TEXTURE0);

#ifdef USE_CG
	shaderEngine.enableShader(currentPipe->compositeShader, pipeline, pipelineContext);
#endif

    glBindVertexArray(m_vao_CompositeOutput);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef USE_CG
	shaderEngine.disableShader();
#endif

	for (std::vector<RenderItem*>::const_iterator pos = pipeline.compositeDrawables.begin(); pos
			!= pipeline.compositeDrawables.end(); ++pos)
		(*pos)->Draw(renderContext);

}


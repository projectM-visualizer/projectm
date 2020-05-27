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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <chrono>
#include <ctime>

Pipeline* Renderer::currentPipe;

using namespace std::chrono;

class Preset;

#ifdef USE_TEXT_MENU


void Renderer::drawText(const char* string, GLfloat x, GLfloat y, GLfloat scale,
                        int horizontalAlignment = GLT_LEFT, int verticalAlignment = GLT_TOP)
{
	drawText(this->title_font, string, x, y, scale, horizontalAlignment, verticalAlignment);
}
void Renderer::drawText(GLTtext* text, const char* string, GLfloat x, GLfloat y, GLfloat scale,
	int horizontalAlignment = GLT_LEFT, int verticalAlignment = GLT_TOP)
{
	// Initialize glText
	gltInit();

	// Creating text
	text = gltCreateText();
	// Begin text drawing (this for instance calls glUseProgram)
	gltBeginDraw();

	// Draw text transparently first.
	gltColor(0.0f, 0.0f, 0.0f, 0.0f);

	gltSetText(text, string);
	// Where horizontal is either:
	// - GLT_LEFT (default)
	// - GLT_CENTER
	// - GLT_RIGHT

	// Where vertical is either:
	// - GLT_TOP (default)
	// - GLT_CENTER
	// - GLT_BOTTOM
	gltDrawText2DAligned(text, x, y, scale, horizontalAlignment, verticalAlignment);
	GLfloat textWidth = gltGetTextWidth(text, scale);
	
	float windowWidth = vw;
	if (horizontalAlignment == GLT_LEFT) {
		// if left aligned factor in X offset
		windowWidth = vw - x;
	}

	// if our window is greater than the text width, there is no overflow so let's display it normally.
	if (windowWidth > textWidth)
	{
		// redraw without transparency
		gltColor(1.0f, 1.0f, 1.0f, 1.0f);
		gltSetText(text, string);
		gltDrawText2DAligned(text, x, y, scale, horizontalAlignment, verticalAlignment);
	}
	else {
		// if the text is greater than the window width, we have a problem.

		// Option 1: Reduce text length until it fits.
		// If it's a single line of text then reduce the text and add a "..." to the end.
		// Before: Geiss & Sperl - Feedback (projectM idle HDR mix)
		// After: Geiss & Sperl - Feed...
		//
		// If it's multiline then just cut the text off.
		// Before: F1: This help menu
		//         UP: Increase Beat Sensitivity
		// After : F1: This help menu
		//         UP: Increase Beat Sensi 

		std::string substring(string);
		while (textWidth > windowWidth) {
			substring.pop_back();
			string = substring.c_str();
			gltSetText(text, string);

			gltDrawText2DAligned(text, x, y, scale, horizontalAlignment, verticalAlignment);
			textWidth = gltGetTextWidth(text, scale);
		}
		// if it's not multi-line then append a ...
		if (substring.find("\n") == -1) {
			substring.pop_back();
			substring.pop_back();
			substring.pop_back();
			substring += "...";
		}
		string = substring.c_str();

		// Option 2: Reduce the scale (size) of the text until it fits.
		/*
		while (textWidth > windowWidth) {
			scale = scale - 0.1;
			gltDrawText2DAligned(text, x, y, scale, horizontalAlignment, verticalAlignment);
			textWidth = gltGetTextWidth(text, scale);
		}
		*/

		// Redraw now that the text fits.
		gltColor(1.0f, 1.0f, 1.0f, 1.0f);
		gltSetText(text, string);
		gltDrawText2DAligned(text, x, y, scale, horizontalAlignment, verticalAlignment);
	}

	// Finish drawing text
	gltEndDraw();

	// Deleting text
	gltDeleteText(text);

	// Destroy glText
	gltTerminate();
}


#endif /** USE_TEXT_MENU */

Renderer::Renderer(int width, int height, int gx, int gy, BeatDetect* _beatDetect, std::string _presetURL,
                   std::string _titlefontURL, std::string _menufontURL, const std::string& datadir) :
	mesh(gx, gy), m_presetName("None"), m_datadir(datadir), vw(width), vh(height),
	title_fontURL(_titlefontURL), menu_fontURL(_menufontURL), presetURL(_presetURL)
{
	this->totalframes = 1;
	this->lastTimeFPS = nowMilliseconds();
	this->currentTimeFPS = nowMilliseconds();
	this->lastTimeToast = nowMilliseconds();
	this->currentTimeToast = nowMilliseconds();
	this->noSwitch = false;
	this->studio = false;
	this->realfps = 0;
	/* Set up the v xoffset and vy offset to 0 which is normal Only used for VR */
	this->vstartx = 0;
	this->vstarty = 0;

	this->drawtitle = 0;

	//this->title = "Unknown";

	/** Other stuff... */
	this->correction = true;

	/// @bug put these on member init list
	this->textureManager = nullptr;
	this->beatDetect = _beatDetect;

	textureRenderToTexture = 0;

	int size = (mesh.height - 1) * mesh.width * 4 * 2;
	p = static_cast<float *>(wipemalloc(size * sizeof(float)));


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

	renderContext.uniform_v2f_c4f_vertex_tranformation = shaderEngine.uniform_v2f_c4f_vertex_tranformation;
	renderContext.uniform_v2f_c4f_vertex_point_size = shaderEngine.uniform_v2f_c4f_vertex_point_size;
	renderContext.uniform_v2f_c4f_t2f_vertex_tranformation = shaderEngine.uniform_v2f_c4f_t2f_vertex_tranformation;
	renderContext.uniform_v2f_c4f_t2f_frag_texture_sampler = shaderEngine.uniform_v2f_c4f_t2f_frag_texture_sampler;

	// Interpolation VAO/VBO's
	glGenBuffers(1, &m_vbo_Interpolation);
	glGenVertexArrays(1, &m_vao_Interpolation);

	glBindVertexArray(m_vao_Interpolation);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_Interpolation);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, static_cast<void*>(nullptr)); // Positions

	glDisableVertexAttribArray(1);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2)); // Textures

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// CompositeOutput VAO/VBO's
	glGenBuffers(1, &m_vbo_CompositeOutput);
	glGenVertexArrays(1, &m_vao_CompositeOutput);

	float composite_buffer_data[8][2] =
	{
		{-0.5, -0.5},
		{0, 1},
		{-0.5, 0.5},
		{0, 0},
		{0.5, 0.5},
		{1, 0},
		{0.5, -0.5},
		{1, 1}
	};


	glBindVertexArray(m_vao_CompositeOutput);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_CompositeOutput);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 2, composite_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, static_cast<void*>(nullptr)); // Positions

	glDisableVertexAttribArray(1);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2)); // Textures

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// CompositeShaderOutput VAO/VBO's
	glGenBuffers(1, &m_vbo_CompositeShaderOutput);
	glGenVertexArrays(1, &m_vao_CompositeShaderOutput);

	glBindVertexArray(m_vao_CompositeShaderOutput);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_CompositeShaderOutput);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(composite_shader_vertex), static_cast<void*>(nullptr));
	// Positions

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(composite_shader_vertex), (void*)(sizeof(float) * 2));
	// Colors

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(composite_shader_vertex), (void*)(sizeof(float) * 6)); // UV

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(composite_shader_vertex), (void*)(sizeof(float) * 8));
	// RAD ANG

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

std::string Renderer::SetPipeline(Pipeline& pipeline)
{
	currentPipe = &pipeline;
	shaderEngine.reset();
	if (!shaderEngine.loadPresetShaders(pipeline, m_presetName))
	{
		return "Shader compilation error";
	}

	return std::string();
}

void Renderer::ResetTextures()
{
	textureManager->Clear();

	reset(vw, vh);

	textureManager->Preload();
}

void Renderer::SetupPass1(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	totalframes++;

	/*
	If FPS is displayed (by pressing F5 or by config):
		- check if 250 milliseconds has passed (1/4 of a second)
		- multiply the total rendered frames (totalframes) by the fraction of a second that passed to get the approximate frames per second count.
		- reset the totalframes and timer (lastTime) so we don't trigger for another 250 milliseconds.
	*/
	if ((m_displayModes & SHOW_FPS) > 0)
	{
		this->currentTimeFPS = nowMilliseconds();
		if (timeCheck(this->currentTimeFPS, this->lastTimeFPS, (double)250)) {
			this->realfps = totalframes * (1000 / 250);
			setFPS(realfps);
			totalframes = 0;
			this->lastTimeFPS = nowMilliseconds();
		}
	}
	glViewport(0, 0, texsizeX, texsizeY);

	renderContext.mat_ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -40.0f, 40.0f);
}

void Renderer::RenderItems(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	renderContext.time = pipelineContext.time;
	renderContext.texsize = nearestPower2(std::max(texsizeX, texsizeY));
	renderContext.aspectCorrect = correction;
	renderContext.aspectRatio = aspect;
	renderContext.textureManager = textureManager;
	renderContext.beatDetect = beatDetect;

	for (std::vector<RenderItem*>::const_iterator pos = pipeline.drawables.begin(); pos != pipeline.drawables.end(); ++
	     pos)
	{
		if (*pos != nullptr)
		{
			(*pos)->Draw(renderContext);
		}
	}
}

void Renderer::FinishPass1()
{
	draw_title_to_texture();

	textureManager->updateMainTexture();
}

void Renderer::Pass2(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	//BEGIN PASS 2
	//
	//end of texture rendering
	//now we copy the texture from the FBO or framebuffer to
	//video texture memory and render fullscreen.

	/** Reset the viewport size */
	if (textureRenderToTexture)
	{
		glViewport(0, 0, texsizeX, texsizeY);
	}
	else
		glViewport(vstartx, vstarty, this->vw, this->vh);

	if (shaderEngine.enableCompositeShader(currentPipe->compositeShader, pipeline, pipelineContext))
	{
		CompositeShaderOutput(pipeline, pipelineContext);
	}
	else
	{
		CompositeOutput(pipeline, pipelineContext);
	}

	// When console refreshes, there is a chance the preset has been changed by the user
	refreshConsole();
	// TODO:
	draw_title_to_screen(false);
	if ((m_displayModes & SHOW_HELP) > 0)
		draw_help();
	if ((m_displayModes & SHOW_TITLE) > 0)
		draw_title();
	if ((m_displayModes & SHOW_TOAST) > 0)
		draw_toast();
	if ((m_displayModes & SHOW_FPS) > 0)
		draw_fps();
	// this->realfps
	if ((m_displayModes & SHOW_PRESET) > 0)
		draw_preset();
	if ((m_displayModes & SHOW_STATS) > 0)
		draw_stats();
	if ((m_displayModes & SHOW_RATING) > 0)
		draw_rating();
	if ((m_displayModes & SHOW_INPUTTEXT) > 0)
		draw_inputText();
}

void Renderer::RenderFrame(const Pipeline& pipeline,
                           const PipelineContext& pipelineContext)
{
	RenderFrameOnlyPass1(pipeline, pipelineContext);

	Pass2(pipeline, pipelineContext);
}

void Renderer::RenderFrameOnlyPass1(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	shaderEngine.RenderBlurTextures(pipeline, pipelineContext);

	SetupPass1(pipeline, pipelineContext);

	Interpolation(pipeline, pipelineContext);

	RenderItems(pipeline, pipelineContext);

	FinishPass1();
}


void Renderer::RenderFrameOnlyPass2(const Pipeline& pipeline, const PipelineContext& pipelineContext, int xoffset,
                                    int yoffset, int eye)
{
	/* draw in a certain range of the screen */
	vstartx = xoffset;
	vstarty = yoffset;
	// ignore eye
	Pass2(pipeline, pipelineContext);
	vstartx = 0;
	vstarty = 0;
}


void Renderer::Interpolation(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager->getMainTexture()->texID);

	//Texture wrapping( clamp vs. wrap)
	if (pipeline.textureWrap == 0)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

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

				p[strip + 6] = pipeline.x_mesh[i][j + 1];
				p[strip + 7] = pipeline.y_mesh[i][j + 1];
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

	shaderEngine.enableWarpShader(currentPipe->warpShader, pipeline, pipelineContext, renderContext.mat_ortho);

	glVertexAttrib4f(1, 1.0, 1.0, 1.0, pipeline.screenDecay);

	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

	glBindVertexArray(m_vao_Interpolation);

	for (int j = 0; j < mesh.height - 1; j++)
		glDrawArrays(GL_TRIANGLE_STRIP, j * mesh.width * 2, mesh.width * 2);

	glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Renderer::~Renderer()
{
	if (textureManager)
		delete (textureManager);


	free(p);

	glDeleteBuffers(1, &m_vbo_Interpolation);
	glDeleteVertexArrays(1, &m_vao_Interpolation);

	glDeleteBuffers(1, &m_vbo_CompositeOutput);
	glDeleteVertexArrays(1, &m_vao_CompositeOutput);

	glDeleteTextures(1, &textureRenderToTexture);
}

void Renderer::reset(int w, int h)
{
	aspect = static_cast<float>(h) / static_cast<float>(w);
	this->vw = w;
	this->vh = h;

	glCullFace(GL_BACK);

#ifndef GL_TRANSITION
    glEnable(GL_LINE_SMOOTH);
#endif

	glClearColor(0, 0, 0, 0);

	glViewport(vstartx, vstarty, w, h);

	glEnable(GL_BLEND);

	texsizeX = w;
	texsizeY = h;

	// snap to 16x16 blocks
	texsizeX = ((texsizeX - 15) / 16) * 16;
	texsizeY = ((texsizeY - 15) / 16) * 16;

	m_fAspectX = (texsizeY > texsizeX) ? static_cast<float>(texsizeX) / static_cast<float>(texsizeY) : 1.0f;
	m_fAspectY = (texsizeX > texsizeY) ? static_cast<float>(texsizeY) / static_cast<float>(texsizeX) : 1.0f;
	m_fInvAspectX = 1.0f / m_fAspectX;
	m_fInvAspectY = 1.0f / m_fAspectY;

	InitCompositeShaderVertex();

	if (textureManager != nullptr)
	{
		delete textureManager;
	}
	textureManager = new TextureManager(presetURL, texsizeX, texsizeY, m_datadir);

	shaderEngine.setParams(texsizeX, texsizeY, beatDetect, textureManager);
	shaderEngine.reset();
	shaderEngine.loadPresetShaders(*currentPipe, m_presetName);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClear(GL_COLOR_BUFFER_BIT);
}

GLuint Renderer::initRenderToTexture()
{
	if (textureRenderToTexture == 0)
	{
		glGenTextures(1, &textureRenderToTexture);
		glBindTexture(GL_TEXTURE_2D, textureRenderToTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texsizeX, texsizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return textureRenderToTexture;
}

void Renderer::draw_title_to_texture()
{
#ifdef USE_TEXT_MENU
	if (this->drawtitle > 100)
	{
		draw_title_to_screen(true);
		this->drawtitle = 0;
	}
#endif /** USE_TEXT_MENU */
}

float title_y;

bool Renderer::timeCheck(const milliseconds currentTime, const milliseconds lastTime, const double difference) {
	milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
	double diff = ms.count();
	if (diff >= difference)
	{
		return true;
	} else {
		return false;
	}
}

void Renderer::setToastMessage(const std::string& theValue)
{
	// Initialize counters
	lastTimeToast= nowMilliseconds();
	currentTimeToast = nowMilliseconds();
	m_toastMessage = theValue;
	m_displayModes |= SHOW_TOAST;
}

	// TODO:
void Renderer::draw_title_to_screen(bool flip)
{
#ifdef USE_TEXT_MENU
	if (this->drawtitle > 0)
	{
	}
#endif /** USE_TEXT_MENU */
}

void Renderer::draw_title()
{
#ifdef USE_TEXT_MENU
	// TODO: investigate possible banner text for GUI
	// drawText(title_font, this->title.c_str(), 10, 20, 2.5);
#endif /** USE_TEXT_MENU */
}

void Renderer::draw_preset()
{
#ifdef USE_TEXT_MENU
	drawText(this->presetName().c_str(), 30, 20, 2.5);
#endif /** USE_TEXT_MENU */
}

void Renderer::draw_help()
{
#ifdef USE_TEXT_MENU
	// TODO: match winamp/milkdrop bindings
	drawText("\n"
	         "F1: This help menu""\n"
	         "F3: Show preset name""\n"
		     "F5: Show FPS""\n"
	         "L: Lock/Unlock Preset""\n"
	         "R: Random preset""\n"
	         "N: Next preset""\n"
	         "P: Previous preset""\n"
	         "UP: Increase Beat Sensitivity""\n"
	         "DOWN: Decrease Beat Sensitivity""\n"
	         "CTRL-F: Fullscreen", 30, 20, 2.5);

#endif /** USE_TEXT_MENU */
}

// TODO
void Renderer::draw_stats()
{
#ifdef USE_TEXT_MENU

	//sprintf(buffer, "      viewport: +%d,%d %d x %d", vstartx, vstarty, vw, vh);
	//sprintf(buffer, "          mesh: %d x %d", mesh.width, mesh.height);
	//sprintf(buffer, "       texsize: %d", renderTarget->texsize);
	//other_font->Render((renderTarget->useFBO ? "           FBO: on" : "           FBO: off"));
	//sprintf(buffer, "      textures: %.1fkB", textureManager->getTextureMemorySize() / 1000.0f);
	//sprintf(buffer, "shader profile: %s", shaderEngine.profileName.c_str());
	//sprintf(buffer, "   warp shader: %s", currentPipe->warpShader.enabled ? "on" : "off");
	//sprintf(buffer, "   comp shader: %s", currentPipe->compositeShader.enabled ? "on" : "off");

#endif /** USE_TEXT_MENU */
}

// TODO
void Renderer::draw_fps()
{
#ifdef USE_TEXT_MENU
	drawText(this->fps().c_str(), 30, 20, 2.5);
#endif /** USE_TEXT_MENU */
}

void Renderer::draw_rating() 
{
#ifdef USE_TEXT_MENU
	drawText(("Rating: " + this->rating()).c_str(), 30, 20, 2.5);
#endif
}

void Renderer::draw_inputText()
{
#ifdef USE_TEXT_MENU
	drawText(("Preset to load: " + inputText()).c_str(), 30, 20, 2.5);
#endif
}

void Renderer::draw_toast()
{
#ifdef USE_TEXT_MENU
	drawText(this->toastMessage().c_str(), (vw/2), (vh/2), 2.5, GLT_CENTER, GLT_CENTER);
#endif /** USE_TEXT_MENU */

	this->currentTimeToast = nowMilliseconds();
	if (timeCheck(this->currentTimeToast,this->lastTimeToast,(double)(TOAST_TIME*1000))) {
		this->currentTimeToast = nowMilliseconds();
		this->lastTimeToast = nowMilliseconds();
		clearDisplayMode(SHOW_TOAST);
	}
}

void Renderer::toggleDisplayMode(int displaymode)
{
	m_displayModes ^= displaymode;
	switch (displaymode)
	{
		case SHOW_HELP: clearDisplayMode(SHOW_STATS); break;
		case SHOW_INPUTTEXT: clearDisplayMode(SHOW_FPS | SHOW_PRESET | SHOW_RATING); break;
		case SHOW_FPS: clearDisplayMode(SHOW_RATING | SHOW_PRESET | SHOW_INPUTTEXT); break;
		case SHOW_PRESET: clearDisplayMode(SHOW_RATING | SHOW_FPS | SHOW_INPUTTEXT); break;
		case SHOW_RATING: clearDisplayMode(SHOW_FPS | SHOW_PRESET | SHOW_INPUTTEXT); break;
		case SHOW_STATS: clearDisplayMode(SHOW_HELP); break;
	}
}

void Renderer::clearDisplayMode(int displaymode)
{
	m_displayModes &= ~displaymode;
}
void Renderer::CompositeOutput(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager->getMainTexture()->texID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	renderContext.mat_ortho = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -40.0f, 40.0f);

	shaderEngine.enableCompositeShader(currentPipe->compositeShader, pipeline, pipelineContext);

	glUniformMatrix4fv(shaderEngine.uniform_v2f_c4f_t2f_vertex_tranformation, 1, GL_FALSE,
	                   value_ptr(renderContext.mat_ortho));
	glUniform1i(shaderEngine.uniform_v2f_c4f_t2f_frag_texture_sampler, 0);

	//Overwrite anything on the screen
	glBlendFunc(GL_ONE, GL_ZERO);
	glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

	glBindVertexArray(m_vao_CompositeOutput);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (std::vector<RenderItem*>::const_iterator pos = pipeline.compositeDrawables.begin(); pos
	     != pipeline.compositeDrawables.end(); ++pos)
		(*pos)->Draw(renderContext);

	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 * Calculates the nearest power of two to the given number using the
 * appropriate rule
 */
int Renderer::nearestPower2(int value)
{
	int x = value;
	int power = 0;

	if (x == 0)
	{
		return 0;
	}

	while ((x & 0x01) != 1)
	{
		x >>= 1;
	}

	if (x == 1)
	{
		return value;
	}
	x = value;
	while (x != 0)
	{
		x >>= 1;
		power++;
	}
	if (((1 << power) - value) <= (value - (1 << (power - 1))))
	{
		return 1 << power;
	}
	return 1 << (power - 1);
	return 0;
}

float Renderer::SquishToCenter(float x, float fExp)
{
	if (x > 0.5f)
		return powf(x * 2 - 1, fExp) * 0.5f + 0.5f;

	return (1 - powf(1 - x * 2, fExp)) * 0.5f;
}

void Renderer::UvToMathSpace(float u, float v, float* rad, float* ang)
{
	// (screen space = -1..1 on both axes; corresponds to UV space)
	// uv space = [0..1] on both axes
	// "math" space = what the preset authors are used to:
	//      upper left = [0,0]
	//      bottom right = [1,1]
	//      rad == 1 at corners of screen
	//      ang == 0 at three o'clock, and increases counter-clockwise (to 6.28).
	float px = (u * 2 - 1) * m_fAspectX; // probably 1.0
	float py = (v * 2 - 1) * m_fAspectY; // probably <1

	*rad = sqrtf(px * px + py * py) / sqrtf(m_fAspectX * m_fAspectX + m_fAspectY * m_fAspectY);
	*ang = atan2f(py, px);
	if (*ang < 0)
		*ang += 6.2831853071796f;
}

void Renderer::InitCompositeShaderVertex()
{
	// BUILD VERTEX LIST for final composite blit
	memset(m_comp_verts, 0, sizeof(composite_shader_vertex) * FCGSX * FCGSY);
	float fDivX = 1.0f / static_cast<float>(FCGSX - 2);
	float fDivY = 1.0f / static_cast<float>(FCGSY - 2);
	for (int j = 0; j < FCGSY; j++)
	{
		int j2 = j - j / (FCGSY / 2);
		float v = j2 * fDivY;
		v = SquishToCenter(v, 3.0f);
		float sy = -((v) * 2 - 1);
		for (int i = 0; i < FCGSX; i++)
		{
			int i2 = i - i / (FCGSX / 2);
			float u = i2 * fDivX;
			u = SquishToCenter(u, 3.0f);
			float sx = (u) * 2 - 1;
			composite_shader_vertex* pComp = &m_comp_verts[i + j * FCGSX];
			pComp->x = sx;
			pComp->y = sy;

			float rad, ang;
			UvToMathSpace(u, v, &rad, &ang);
			// fix-ups:
			if (i == FCGSX / 2 - 1)
			{
				if (j < FCGSY / 2 - 1)
					ang = 3.1415926535898f * 1.5f;
				else if (j == FCGSY / 2 - 1)
					ang = 3.1415926535898f * 1.25f;
				else if (j == FCGSY / 2)
					ang = 3.1415926535898f * 0.75f;
				else
					ang = 3.1415926535898f * 0.5f;
			}
			else if (i == FCGSX / 2)
			{
				if (j < FCGSY / 2 - 1)
					ang = 3.1415926535898f * 1.5f;
				else if (j == FCGSY / 2 - 1)
					ang = 3.1415926535898f * 1.75f;
				else if (j == FCGSY / 2)
					ang = 3.1415926535898f * 0.25f;
				else
					ang = 3.1415926535898f * 0.5f;
			}
			else if (j == FCGSY / 2 - 1)
			{
				if (i < FCGSX / 2 - 1)
					ang = 3.1415926535898f * 1.0f;
				else if (i == FCGSX / 2 - 1)
					ang = 3.1415926535898f * 1.25f;
				else if (i == FCGSX / 2)
					ang = 3.1415926535898f * 1.75f;
				else
					ang = 3.1415926535898f * 2.0f;
			}
			else if (j == FCGSY / 2)
			{
				if (i < FCGSX / 2 - 1)
					ang = 3.1415926535898f * 1.0f;
				else if (i == FCGSX / 2 - 1)
					ang = 3.1415926535898f * 0.75f;
				else if (i == FCGSX / 2)
					ang = 3.1415926535898f * 0.25f;
				else
					ang = 3.1415926535898f * 0.0f;
			}
			pComp->tu = u;
			pComp->tv = v;

			pComp->rad = rad;
			pComp->ang = ang;
		}
	}

	// build index list for final composite blit -
	// order should be friendly for interpolation of 'ang' value!
	int* cur_index = &m_comp_indices[0];
	for (int y = 0; y < FCGSY - 1; y++)
	{
		if (y == FCGSY / 2 - 1)
			continue;
		for (int x = 0; x < FCGSX - 1; x++)
		{
			if (x == FCGSX / 2 - 1)
				continue;
			bool left_half = (x < FCGSX / 2);
			bool top_half = (y < FCGSY / 2);
			bool center_4 = ((x == FCGSX / 2 || x == FCGSX / 2 - 1) && (y == FCGSY / 2 || y == FCGSY / 2 - 1));

			if ((static_cast<int>(left_half) + static_cast<int>(top_half) + static_cast<int>(center_4)) % 2)
			{
				*(cur_index + 0) = (y) * FCGSX + (x);
				*(cur_index + 1) = (y) * FCGSX + (x + 1);
				*(cur_index + 2) = (y + 1) * FCGSX + (x + 1);
				*(cur_index + 3) = (y + 1) * FCGSX + (x + 1);
				*(cur_index + 4) = (y + 1) * FCGSX + (x);
				*(cur_index + 5) = (y) * FCGSX + (x);
			}
			else
			{
				*(cur_index + 0) = (y + 1) * FCGSX + (x);
				*(cur_index + 1) = (y) * FCGSX + (x);
				*(cur_index + 2) = (y) * FCGSX + (x + 1);
				*(cur_index + 3) = (y) * FCGSX + (x + 1);
				*(cur_index + 4) = (y + 1) * FCGSX + (x + 1);
				*(cur_index + 5) = (y + 1) * FCGSX + (x);
			}

			cur_index += 6;
		}
	}
}


void Renderer::CompositeShaderOutput(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	// hue shader
	float shade[4][3] = {
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f}
	}; // for each vertex, then each comp.

	// pick 4 colors for the 4 corners
	for (int i = 0; i < 4; i++)
	{
		shade[i][0] = 0.6f + 0.3f * sinf(pipelineContext.time * 30.0f * 0.0143f + 3 + i * 21);
		shade[i][1] = 0.6f + 0.3f * sinf(pipelineContext.time * 30.0f * 0.0107f + 1 + i * 13);
		shade[i][2] = 0.6f + 0.3f * sinf(pipelineContext.time * 30.0f * 0.0129f + 6 + i * 9);
		float max = ((shade[i][0] > shade[i][1]) ? shade[i][0] : shade[i][1]);
		if (shade[i][2] > max) max = shade[i][2];
		for (int k = 0; k < 3; k++)
		{
			shade[i][k] /= max;
			shade[i][k] = 0.5f + 0.5f * shade[i][k];
		}
	}

	// interpolate the 4 colors & apply to all the verts
	for (int j = 0; j < FCGSY; j++)
	{
		for (int i = 0; i < FCGSX; i++)
		{
			composite_shader_vertex* pComp = &m_comp_verts[i + j * FCGSX];
			float x = pComp->x * 0.5f + 0.5f;
			float y = pComp->y * 0.5f + 0.5f;

			float col[3] = {1, 1, 1};
			for (int c = 0; c < 3; c++)
				col[c] = shade[0][c] * (x) * (y) +
					shade[1][c] * (1 - x) * (y) +
					shade[2][c] * (x) * (1 - y) +
					shade[3][c] * (1 - x) * (1 - y);

			pComp->Diffuse[0] = col[0];
			pComp->Diffuse[1] = col[1];
			pComp->Diffuse[2] = col[2];
			pComp->Diffuse[3] = 1.0;
		}
	}


	const int primCount = (FCGSX - 2) * (FCGSY - 2) * 6;
	composite_shader_vertex tempv[primCount];
	memset(tempv, 0, sizeof(composite_shader_vertex) * primCount);
	int src_idx = 0;
	for (int i = 0; i < primCount; i++)
	{
		tempv[i] = m_comp_verts[m_comp_indices[src_idx++]];
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_CompositeShaderOutput);

	glBufferData(GL_ARRAY_BUFFER, sizeof(composite_shader_vertex) * primCount, nullptr, GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(composite_shader_vertex) * primCount, tempv, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBlendFunc(GL_ONE, GL_ZERO);

	glBindVertexArray(m_vao_CompositeShaderOutput);

	// Now do the final composite blit, fullscreen;
	glDrawArrays(GL_TRIANGLES, 0, primCount);

	glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

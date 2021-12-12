#include "Renderer.hpp"
#include "wipemalloc.h"
#include "math.h"
#include "Common.hpp"
#include "KeyHandler.hpp"
#include "TextureManager.hpp"
#include "MilkdropWaveform.hpp"
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cassert>
#include "omptl/omptl_algorithm"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <ctime>
#include "stb_image_write.h"

using namespace std::chrono;

class Preset;

Renderer::Renderer(int width, int height, int gx, int gy, BeatDetect* _beatDetect, std::string _presetURL,
                   std::string _titlefontURL, std::string _menufontURL, const std::string& datadir)
    : mesh(gx, gy)
    , beatDetect(_beatDetect)
    , m_presetName("None")
    , m_datadir(datadir)
    , vw(width)
    , vh(height)
    , title_fontURL(std::move(_titlefontURL))
    , menu_fontURL(std::move(_menufontURL))
    , presetURL(std::move(_presetURL))
    , m_menuText(width)
{
	// This is the default help menu for applications that have not defined any custom menu.
	const char* defaultHelpMenu = "\n"
		"F1: This help menu""\n"
		"F3: Show preset name""\n"
		"F5: Show FPS""\n"
		"L: Lock/Unlock Preset""\n"
		"R: Random preset""\n"
		"N/P: [N]ext+ or [P]revious-reset""\n"
		"M/Return: Preset Menu (Arrow Up/Down & Page Up/Down to Navigate)""\n"
		"Arrow Up/Down: Increase or Decrease Beat Sensitivity""\n"
		"CTRL-F: Fullscreen";

	this->setHelpText(defaultHelpMenu);

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

	renderContext.uniform_v2f_c4f_vertex_transformation = shaderEngine.uniform_v2f_c4f_vertex_transformation;
	renderContext.uniform_v2f_c4f_vertex_point_size = shaderEngine.uniform_v2f_c4f_vertex_point_size;
	renderContext.uniform_v2f_c4f_t2f_vertex_transformation = shaderEngine.uniform_v2f_c4f_t2f_vertex_transformation;
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
	if (this->showfps)
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
    renderContext.viewportSizeX = vw;
    renderContext.viewportSizeY = vh;
	renderContext.aspectX = m_fAspectX;
	renderContext.aspectY = m_fAspectY;
    renderContext.invAspectX = m_fInvAspectX;
    renderContext.invAspectY = m_fInvAspectY;
	renderContext.textureManager = textureManager;
	renderContext.beatDetect = beatDetect;

	for (std::vector<RenderItem*>::const_iterator pos = pipeline.drawables.begin(); pos != pipeline.drawables.end(); ++pos)
	{
		if (*pos != nullptr)
			(*pos)->Draw(renderContext);
	}
	
	// If we have touch waveforms, render them.
	if (waveformList.size() >= 1) {
		RenderTouch(pipeline,pipelineContext);
	}
}

void Renderer::RenderTouch(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	Pipeline pipelineTouch;
	MilkdropWaveform wave;
	for(std::size_t x = 0; x < waveformList.size(); x++){
		pipelineTouch.drawables.push_back(&wave);
		wave = waveformList[x];

		// Render waveform
		for (std::vector<RenderItem*>::const_iterator pos = pipelineTouch.drawables.begin(); pos != pipelineTouch.drawables.end(); ++pos)
		{
			if (*pos != nullptr)
				(*pos)->Draw(renderContext);
		}
	}
}

void Renderer::FinishPass1()
{
	draw_title_to_texture();

	textureManager->updateMainTexture();
	if(writeNextFrameToFile) {
		debugWriteMainTextureToFile();
		writeNextFrameToFile = false;
    }

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
	if (this->showhelp == true)
		draw_help();
	if (this->showtitle == true)
		draw_title();
	if (this->showfps == true)
		draw_fps();
	// this->realfps
	if (this->showsearch == true)
		draw_search();
	if (this->showmenu == true)
		draw_menu();
	if (this->showpreset == true)
		draw_preset();
	if (this->showstats == true)
		draw_stats();
	// We should always draw toasts last so they are on top of other text (lp/menu).
	if (this->showtoast == true) 
		draw_toast();
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
		Pipeline *cp = currentPipe;
		omptl::transform(mesh.p.begin(), mesh.p.end(), mesh.identity.begin(), mesh.p.begin(),
			[cp](PixelPoint p, PerPixelContext &context) {
				return cp->PerPixel(p, context);
			});

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

#ifdef USE_TEXT_MENU
    MenuText::CleanUp();
#endif
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

	shaderEngine.setParams(texsizeX, texsizeY, m_fAspectX, m_fAspectY, beatDetect, textureManager);
	shaderEngine.reset();
	shaderEngine.loadPresetShaders(*currentPipe, m_presetName);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClear(GL_COLOR_BUFFER_BIT);

#ifdef USE_TEXT_MENU
    m_menuText.SetViewportWidth(w);

	// When the renderer resets, do a check to find out what the maximum number of lines we could display are.
	int r_textMenuPageSize = 0;
	int yOffset = textMenuYOffset;
	while (true) { // infinite loop, only satisifed when we have found the max lines of text on the screen.
		if (yOffset < vh - textMenuLineHeight) { // if yOffset could be displayed on the screen (vh), then we have room for the next line.
			r_textMenuPageSize++;
			yOffset = yOffset + textMenuLineHeight;
		}
		else { // if we reached the end of the screen, set textMenuPageSize and move on.
			textMenuPageSize = r_textMenuPageSize;
			break;
		}
	}
#endif
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

// If we touched on the renderer where there is an existing waveform.
bool Renderer::touchedWaveform(float x, float y, std::size_t i)
{
	if (waveformList[i].x > (x-0.05f) && waveformList[i].x < (x+0.05f) // if x +- 0.5f 
		&& ((waveformList[i].y > (y-0.05f) && waveformList[i].y < (y+0.05f)) // and y +- 0.5f 
		|| waveformList[i].mode == Line || waveformList[i].mode == DoubleLine || waveformList[i].mode == DerivativeLine ) // OR it's a line (and y doesn't matter)
		)
	{
		return true;
	}
	return false;
}

// Render a waveform when a touch event is triggered.
void Renderer::touch(float x, float y, int pressure, int type = 0)
{

	for (std::size_t i = 0; i < waveformList.size(); i++) {
		if (touchedWaveform(x, y, i))
		{
			// if we touched an existing waveform with left click, drag it and don't continue with adding another.
			touchDrag(x, y, pressure);
			return;
		}
	}

	touchx = x;
	touchy = y;
	touchp = pressure;

	// Randomly select colours on touch
	touchr = ((double)rand() / (RAND_MAX));
	touchb = ((double)rand() / (RAND_MAX));
	touchg = ((double)rand() / (RAND_MAX));
	toucha = ((double)rand() / (RAND_MAX));

	MilkdropWaveform wave;
	if (type == 0) {
		// If we touched randomly, then assign type to a random number between 0 and 8
		wave.mode = static_cast<MilkdropWaveformMode>((rand() % last));
	}
	else {
		wave.mode = static_cast<MilkdropWaveformMode>(type);
	}

	wave.additive = true;
	wave.modOpacityEnd = 1.1;
	wave.modOpacityStart = 0.0;
	wave.maximizeColors = true;
	wave.modulateAlphaByVolume = false;

	wave.r = touchr;
	wave.g = touchg;
	wave.b = touchb;
	wave.a = toucha;
    wave.x = touchx;
	wave.y = touchy;

	// add new waveform to waveformTouchList
	waveformList.push_back(wave);
}

// Move a waveform when dragging X, Y, and Pressure can change. We also extend the counters so it will stay on screen for as long as you click and drag.
void Renderer::touchDrag(float x, float y, int pressure)
{
	// if we left clicked & held in the approximate position of a waveform, snap to it and adjust x / y to simulate dragging.
	// For lines we don't worry about the x axis.
	for (std::size_t i = 0; i < waveformList.size(); i++) {
		if (touchedWaveform(x, y, i))
		{
			waveformList[i].x = x;
			waveformList[i].y = y;
		}
	}
	touchp = pressure;
}

// Remove waveform at X Y
void Renderer::touchDestroy(float x, float y)
{
	// if we right clicked approximately on the position of the waveform, then remove it from the waveform list.
	// For lines we don't worry about the x axis.
	for (std::size_t i = 0; i < waveformList.size(); i++) {
		if (touchedWaveform(x, y, i))
		{
			waveformList.erase(waveformList.begin() + i);
		}
	}
}

// Remove all waveforms
void Renderer::touchDestroyAll()
{
	waveformList.clear();
}

// turn search menu on / off
void Renderer::toggleSearchText() {
	this->showsearch = !this->showsearch;
	if (this->showsearch)
	{
		this->showfps = false;
		this->showtitle = false;
	}
}

std::string Renderer::getSearchText() const
{
    return m_searchText;
}

// search based on new key input
void Renderer::setSearchText(const std::string& theValue)
{
	m_searchText = m_searchText + theValue;
}

// reset search text backspace (reset)
void Renderer::resetSearchText()
{
	m_searchText = "";
}

// search text backspace (delete a key)
void Renderer::deleteSearchText()
{
	if (m_searchText.length() >= 1) {
		m_searchText = m_searchText.substr(0, m_searchText.size() - 1);
	}
}



void Renderer::debugWriteMainTextureToFile() const {
	GLuint fbo;
	auto mainTexture = textureManager->getMainTexture();

	const auto safeWriteFile = [](const auto frameNumber, auto data, const auto width, const auto height) {
		static const std::string prefix{"frame_texture_contents-"};
		const auto prefixLen = prefix.size();
		constexpr auto fileNameMaxLength = 150;
		constexpr auto fileExtensionLength = 4;
		char fileNameBuffer[fileNameMaxLength];
		std::memcpy(fileNameBuffer, prefix.data(), prefixLen);
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);
		const auto bytesWritten = std::strftime(fileNameBuffer + prefixLen, fileNameMaxLength - prefixLen, "%Y-%m-%d-%H:%M:%S", &tm);
		const auto offset = prefixLen + bytesWritten;
		const auto spaceLeft = fileNameMaxLength - offset;
		std::snprintf(fileNameBuffer + offset, spaceLeft - fileExtensionLength, "%d.bmp", frameNumber);
		stbi_write_bmp( fileNameBuffer, width, height, 4, data);
	};

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainTexture->texID, 0);
	auto dataSize = mainTexture->width * mainTexture->height * 4;
	GLubyte* pixels = new GLubyte[dataSize];
	glReadPixels(0, 0, mainTexture->width, mainTexture->height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);
	safeWriteFile(totalframes, pixels, mainTexture->width, mainTexture->height);
	delete[] pixels;
}

void Renderer::UpdateContext(PipelineContext& context)
{
    context.pixelsx = vw;
    context.pixelsy = vh;

    // It's actually the inverse of the aspect ratio.
    context.aspectx = m_fInvAspectX;
    context.aspecty = m_fInvAspectY;
}

void Renderer::setToastMessage(const std::string& theValue)
{
	// Initialize counters
	lastTimeToast= nowMilliseconds();
	currentTimeToast = nowMilliseconds();
	m_toastMessage = theValue;
	showtoast = true;
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

// render search text menu
void Renderer::draw_search()
{
#ifdef USE_TEXT_MENU
	std::string search = "Search: ";
	search = search + searchText();

    m_menuText.DrawBegin();
    m_menuText.Draw(search, 30, 20, 2.5);
    m_menuText.DrawEnd();
#endif /** USE_TEXT_MENU */
}

void Renderer::draw_title()
{
#ifdef USE_TEXT_MENU
	// TODO: investigate possible banner text for GUI
    // m_menuText.DrawBegin();
	// drawText(title_font, this->title.c_str(), 10, 20, 2.5);
    // m_menuText.DrawEnd();
#endif /** USE_TEXT_MENU */
}

void Renderer::draw_menu()
{
#ifdef USE_TEXT_MENU
	int menu_xOffset = 30; // x axis static point.
	int menu_yOffset = 60; // y axis start point.
	float windowHeight = vh;
	float alpha = 1.0;
	if (this->showsearch) // if search input is up, slightly dim preset menu
		alpha = 0.82f;
    m_menuText.DrawBegin();
	for (auto& it : m_presetList) { // loop over preset buffer
		if (menu_yOffset  < windowHeight - textMenuLineHeight) { // if we are not at the bottom of the screen, display preset name.
			if (it.id == m_activePresetID) { // if this is the active preset, add some color.
				m_menuText.Draw(it.name, menu_xOffset, menu_yOffset , 1.5,
                                MenuText::HorizontalAlignment::Left,
                                MenuText::VerticalAlignment::Top,
                                1.0, 0.1, 0.1, 1.0, true, m_searchText);
			}
			else {
				m_menuText.Draw(it.name, menu_xOffset, menu_yOffset , 1.5,
                                MenuText::HorizontalAlignment::Left,
                                MenuText::VerticalAlignment::Top,
                                1.0, 1.0, 1.0, alpha, true, m_searchText);
			}
		}
		menu_yOffset = menu_yOffset + textMenuLineHeight; // increase line y offset so we can track if we reached the bottom of the screen.
	}
    m_menuText.DrawEnd();
#endif /** USE_TEXT_MENU */
}

void Renderer::draw_preset()
{
#ifdef USE_TEXT_MENU
    m_menuText.DrawBegin();
	m_menuText.Draw(this->presetName(), 30, 20, 2.5);
    m_menuText.DrawEnd();
#endif /** USE_TEXT_MENU */
}

void Renderer::draw_help()
{
#ifdef USE_TEXT_MENU
	// TODO: match winamp/milkdrop bindings
    m_menuText.DrawBegin();
	m_menuText.Draw(this->helpText(), 30, 20, 2.5);
    m_menuText.DrawEnd();
#endif /** USE_TEXT_MENU */
}

// fake rounding - substr is good enough.
std::string Renderer::float_stats(float stat)
{
    std::string num_text = std::to_string(stat);
    std::string rounded = num_text.substr(0, num_text.find(".")+4);
	return rounded;
}

// TODO
void Renderer::draw_stats()
{
#ifdef USE_TEXT_MENU
	std::string stats = "\n";
	std::string warpShader = (!currentPipe->warpShader.programSource.empty()) ? "ON" : "OFF";
	std::string compShader = (!currentPipe->compositeShader.programSource.empty()) ? "ON" : "OFF";

	stats += "Render:""\n";
	stats += "Resolution: " + std::to_string(vw) + "x" + std::to_string(vh) + "\n";
	stats += "Mesh X: " + std::to_string(mesh.width) + "\n";
	stats += "Mesh Y: " + std::to_string(mesh.height) + "\n";
    stats += "Time: " + std::to_string(renderContext.time) + "\n";

	stats += "\n";
	stats += "Beat Detect:""\n";
	stats += "Sensitivity: " + float_stats(beatDetect->beatSensitivity) + "\n";
	stats += "Bass: " + float_stats(beatDetect->bass) + "\n";
	stats += "Mid Range: " + float_stats(beatDetect->mid) + "\n";
	stats += "Treble: " + float_stats(beatDetect->treb) + "\n";
	stats += "Volume: " + float_stats(beatDetect->vol) + "\n";

	stats += "\n";
	stats += "Preset:""\n";
	stats += "Warp Shader: " + warpShader + "\n";
	stats += "Composite Shader: " + compShader + "\n";
    m_menuText.DrawBegin();
	m_menuText.Draw(stats, 30, 20, 2.5);
    m_menuText.DrawEnd();
#endif /** USE_TEXT_MENU */
}

// TODO
void Renderer::draw_fps()
{
#ifdef USE_TEXT_MENU
    m_menuText.DrawBegin();
	m_menuText.Draw(this->fps(), 30, 20, 2.5);
    m_menuText.DrawEnd();
#endif /** USE_TEXT_MENU */
}

void Renderer::draw_toast()
{
#ifdef USE_TEXT_MENU
    m_menuText.DrawBegin();
	m_menuText.Draw(this->toastMessage(), (vw/2), (vh/2), 2.5,
                    MenuText::HorizontalAlignment::Center, MenuText::VerticalAlignment::Center);
    m_menuText.DrawEnd();
#endif /** USE_TEXT_MENU */

	this->currentTimeToast = nowMilliseconds();
	if (timeCheck(this->currentTimeToast,this->lastTimeToast,(double)(TOAST_TIME*1000))) {
		this->currentTimeToast = nowMilliseconds();
		this->lastTimeToast = nowMilliseconds();
		this->showtoast = false;
	}
}

void Renderer::CompositeOutput(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureManager->getMainTexture()->texID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	renderContext.mat_ortho = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -40.0f, 40.0f);

	shaderEngine.enableCompositeShader(currentPipe->compositeShader, pipeline, pipelineContext);

	glUniformMatrix4fv(shaderEngine.uniform_v2f_c4f_t2f_vertex_transformation, 1, GL_FALSE,
                       value_ptr(renderContext.mat_ortho));
	glUniform1i(shaderEngine.uniform_v2f_c4f_t2f_frag_texture_sampler, 0);

	//Overwrite anything on the screen
	glBlendFunc(GL_ONE, GL_ZERO);
	glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

	glBindVertexArray(m_vao_CompositeOutput);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (auto drawable : pipeline.compositeDrawables)
		drawable->Draw(renderContext);

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

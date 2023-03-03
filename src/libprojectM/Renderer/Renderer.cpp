#include "Renderer.hpp"
#include "wipemalloc.h"
#include "math.h"
#include "TextureManager.hpp"
#include "MilkdropWaveform.hpp"
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <omptl/omptl_algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <ctime>
#include "stb_image_write.h"

using namespace std::chrono;

class Preset;

Renderer::Renderer(int viewportWidth, int viewportHeight, int meshX, int meshY,
                   BeatDetect* beatDetect, std::vector<std::string>& textureSearchPaths)
    : m_perPixelMesh(meshX, meshY)
    , m_beatDetect(beatDetect)
    , m_viewportWidth(viewportWidth)
    , m_viewportHeight(viewportHeight)
    , m_textureSearchPaths(textureSearchPaths)
{
    ResetPerPointMeshBuffer();

    m_renderContext.programID_v2f_c4f = m_shaderEngine.programID_v2f_c4f;
    m_renderContext.programID_v2f_c4f_t2f = m_shaderEngine.programID_v2f_c4f_t2f;

    m_renderContext.uniform_v2f_c4f_vertex_transformation = m_shaderEngine.uniform_v2f_c4f_vertex_transformation;
    m_renderContext.uniform_v2f_c4f_vertex_point_size = m_shaderEngine.uniform_v2f_c4f_vertex_point_size;
    m_renderContext.uniform_v2f_c4f_t2f_vertex_transformation = m_shaderEngine.uniform_v2f_c4f_t2f_vertex_transformation;
    m_renderContext.uniform_v2f_c4f_t2f_frag_texture_sampler = m_shaderEngine.uniform_v2f_c4f_t2f_frag_texture_sampler;

	// Interpolation VAO/VBO's
	glGenBuffers(1, &m_vboInterpolation);
	glGenVertexArrays(1, &m_vaoInterpolation);

	glBindVertexArray(m_vaoInterpolation);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboInterpolation);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, static_cast<void*>(nullptr)); // Positions

	glDisableVertexAttribArray(1);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2)); // Textures

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// CompositeOutput VAO/VBO's
	glGenBuffers(1, &m_vboCompositeOutput);
	glGenVertexArrays(1, &m_vaoCompositeOutput);

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


	glBindVertexArray(m_vaoCompositeOutput);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboCompositeOutput);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 2, composite_buffer_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, static_cast<void*>(nullptr)); // Positions

	glDisableVertexAttribArray(1);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2)); // Textures

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// CompositeShaderOutput VAO/VBO's
	glGenBuffers(1, &m_vboCompositeShaderOutput);
	glGenVertexArrays(1, &m_vaoCompositeShaderOutput);

	glBindVertexArray(m_vaoCompositeShaderOutput);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboCompositeShaderOutput);

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

    ResetTextures();
}

void Renderer::SetPipeline(Pipeline& pipeline)
{
    m_currentPipeline = &pipeline;
    m_shaderEngine.reset();

	try {
        m_shaderEngine.loadPresetShaders(pipeline);

    }
    catch(const ShaderException& ex)
	{
		throw RenderException("Shader compilation error: " + ex.message());
	}
}

void Renderer::ResetTextures()
{
    m_textureManager = std::make_unique<TextureManager>(m_textureSearchPaths, m_mainTextureSizeX, m_mainTextureSizeY);
    m_shaderEngine.setParams(m_mainTextureSizeX, m_mainTextureSizeY, m_fAspectX, m_fAspectY, m_beatDetect, m_textureManager.get());
}

void Renderer::SetTextureSearchPaths(std::vector<std::string>& textureSearchPaths)
{
    m_textureSearchPaths = textureSearchPaths;
    ResetTextures();
}

void Renderer::SetPerPixelMeshSize(int meshX, int meshY)
{
    m_perPixelMesh = PerPixelMesh(meshX, meshY);
    ResetPerPointMeshBuffer();
}

void Renderer::SetupPass1(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	totalframes++;

	glViewport(0, 0, m_mainTextureSizeX, m_mainTextureSizeY);

    m_renderContext.mat_ortho = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -40.0f, 40.0f);
}

void Renderer::RenderItems(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
    m_renderContext.time = pipelineContext.time;
    m_renderContext.texsize = nearestPower2(std::max(m_mainTextureSizeX, m_mainTextureSizeY));
    m_renderContext.viewportSizeX = m_viewportWidth;
    m_renderContext.viewportSizeY = m_viewportHeight;
    m_renderContext.aspectX = m_fAspectX;
    m_renderContext.aspectY = m_fAspectY;
    m_renderContext.invAspectX = m_fInvAspectX;
    m_renderContext.invAspectY = m_fInvAspectY;
    m_renderContext.textureManager = m_textureManager.get();
    m_renderContext.beatDetect = m_beatDetect;

	for (std::vector<RenderItem*>::const_iterator pos = pipeline.drawables.begin(); pos != pipeline.drawables.end(); ++pos)
	{
		if (*pos != nullptr)
			(*pos)->Draw(m_renderContext);
	}
	
	// If we have touch waveforms, render them.
	if (m_waveformList.size() >= 1) {
		RenderTouch(pipeline,pipelineContext);
	}
}

void Renderer::RenderTouch(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	Pipeline pipelineTouch;
	MilkdropWaveform wave;
	for(std::size_t x = 0; x < m_waveformList.size(); x++){
		pipelineTouch.drawables.push_back(&wave);
		wave = m_waveformList[x];

		// Render waveform
		for (std::vector<RenderItem*>::const_iterator pos = pipelineTouch.drawables.begin(); pos != pipelineTouch.drawables.end(); ++pos)
		{
			if (*pos != nullptr)
				(*pos)->Draw(m_renderContext);
		}
	}
}

void Renderer::FinishPass1()
{
    m_textureManager->updateMainTexture();

    if (writeNextFrameToFile)
    {
        debugWriteMainTextureToFile();
        writeNextFrameToFile = false;
        frameDumpOutputFile = "";
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
    glViewport(0, 0, m_viewportWidth, m_viewportHeight);

	if (m_shaderEngine.enableCompositeShader(m_currentPipeline->compositeShader, pipeline, pipelineContext))
	{
		CompositeShaderOutput(pipeline, pipelineContext);
	}
	else
	{
		CompositeOutput(pipeline, pipelineContext);
	}
}

void Renderer::RenderFrameOnlyPass1(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
    m_shaderEngine.RenderBlurTextures(pipeline, pipelineContext);

	SetupPass1(pipeline, pipelineContext);

	Interpolation(pipeline, pipelineContext);

	RenderItems(pipeline, pipelineContext);

	FinishPass1();
}


void Renderer::RenderFrameOnlyPass2(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	Pass2(pipeline, pipelineContext);
}


void Renderer::Interpolation(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureManager->getMainTexture()->texID);

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

	int size = (m_perPixelMesh.height - 1) * m_perPixelMesh.width * 4 * 2;

	if (pipeline.staticPerPixel)
	{
		for (int j = 0; j < m_perPixelMesh.height - 1; j++)
		{
			int base = j * m_perPixelMesh.width * 2 * 4;

			for (int i = 0; i < m_perPixelMesh.width; i++)
			{
				int strip = base + i * 8;
                m_perPointMeshBuffer[strip + 2] = pipeline.x_mesh[i][j];
                m_perPointMeshBuffer[strip + 3] = pipeline.y_mesh[i][j];

                m_perPointMeshBuffer[strip + 6] = pipeline.x_mesh[i][j + 1];
                m_perPointMeshBuffer[strip + 7] = pipeline.y_mesh[i][j + 1];
			}
		}
	}
	else
	{
		m_perPixelMesh.Reset();
		Pipeline *cp = m_currentPipeline;
		omptl::transform(m_perPixelMesh.p.begin(), m_perPixelMesh.p.end(), m_perPixelMesh.identity.begin(), m_perPixelMesh.p.begin(),
			[cp](PixelPoint point, PerPixelContext &context) {
				return cp->PerPixel(point, context);
			});

		for (int j = 0; j < m_perPixelMesh.height - 1; j++)
		{
			int base = j * m_perPixelMesh.width * 2 * 4;

			for (int i = 0; i < m_perPixelMesh.width; i++)
			{
				int strip = base + i * 8;
				int index = j * m_perPixelMesh.width + i;
				int index2 = (j + 1) * m_perPixelMesh.width + i;

				m_perPointMeshBuffer[strip + 2] = m_perPixelMesh.p[index].x;
				m_perPointMeshBuffer[strip + 3] = m_perPixelMesh.p[index].y;

				m_perPointMeshBuffer[strip + 6] = m_perPixelMesh.p[index2].x;
				m_perPointMeshBuffer[strip + 7] = m_perPixelMesh.p[index2].y;
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vboInterpolation);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, nullptr, GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, m_perPointMeshBuffer, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_shaderEngine.enableWarpShader(m_currentPipeline->warpShader, pipeline, pipelineContext, m_renderContext.mat_ortho);

	glVertexAttrib4f(1, 1.0, 1.0, 1.0, pipeline.screenDecay);

	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);

	glBindVertexArray(m_vaoInterpolation);

	for (int j = 0; j < m_perPixelMesh.height - 1; j++)
		glDrawArrays(GL_TRIANGLE_STRIP, j * m_perPixelMesh.width * 2, m_perPixelMesh.width * 2);

	glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Renderer::~Renderer()
{
	free(m_perPointMeshBuffer);

	glDeleteBuffers(1, &m_vboInterpolation);
	glDeleteVertexArrays(1, &m_vaoInterpolation);

	glDeleteBuffers(1, &m_vboCompositeOutput);
	glDeleteVertexArrays(1, &m_vaoCompositeOutput);
}

void Renderer::reset(int viewportWidth, int viewportHeight)
{
	m_viewportWidth = viewportWidth;
	m_viewportHeight = viewportHeight;

	glCullFace(GL_BACK);

#ifndef GL_TRANSITION
    glEnable(GL_LINE_SMOOTH);
#endif

	glClearColor(0, 0, 0, 0);

	glViewport(0, 0, viewportWidth, viewportHeight);

	glEnable(GL_BLEND);

    m_mainTextureSizeX = viewportWidth;
    m_mainTextureSizeY = viewportHeight;

	// snap to 16x16 blocks
    m_mainTextureSizeX = ((m_mainTextureSizeX - 15) / 16) * 16;
    m_mainTextureSizeY = ((m_mainTextureSizeY - 15) / 16) * 16;

	m_fAspectX = (m_mainTextureSizeY > m_mainTextureSizeX) ? static_cast<float>(m_mainTextureSizeX) / static_cast<float>(m_mainTextureSizeY) : 1.0f;
	m_fAspectY = (m_mainTextureSizeX > m_mainTextureSizeY) ? static_cast<float>(m_mainTextureSizeY) / static_cast<float>(m_mainTextureSizeX) : 1.0f;

    m_fInvAspectX = 1.0f / m_fAspectX;
    m_fInvAspectY = 1.0f / m_fAspectY;

	InitCompositeShaderVertex();

    ResetTextures();

    m_shaderEngine.reset();
    try
    {
        if (m_currentPipeline)
        {
            m_shaderEngine.loadPresetShaders(*m_currentPipeline);
        }
    }
    catch(const ShaderException& ex)
    {
        throw RenderException("Shader compilation error: " + ex.message());
    }

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClear(GL_COLOR_BUFFER_BIT);
}

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
	if (m_waveformList[i].x > (x-0.05f) && m_waveformList[i].x < (x+0.05f) // if x +- 0.5f
		&& ((m_waveformList[i].y > (y-0.05f) && m_waveformList[i].y < (y+0.05f)) // and y +- 0.5f
		|| m_waveformList[i].mode == Line || m_waveformList[i].mode == DoubleLine || m_waveformList[i].mode == DerivativeLine ) // OR it's a line (and y doesn't matter)
		)
	{
		return true;
	}
	return false;
}

// Render a waveform when a touch event is triggered.
void Renderer::touch(float x, float y, int pressure, int type = 0)
{

	for (std::size_t i = 0; i < m_waveformList.size(); i++) {
		if (touchedWaveform(x, y, i))
		{
			// if we touched an existing waveform with left click, drag it and don't continue with adding another.
			touchDrag(x, y, pressure);
			return;
		}
	}

    m_touchX = x;
    m_touchY = y;

	// Randomly select colours on touch
    m_touchR = ((double)rand() / (RAND_MAX));
    m_touchB = ((double)rand() / (RAND_MAX));
    m_touchG = ((double)rand() / (RAND_MAX));
    m_touchA = ((double)rand() / (RAND_MAX));

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

	wave.r = m_touchR;
	wave.g = m_touchG;
	wave.b = m_touchB;
	wave.a = m_touchA;
    wave.x = m_touchX;
	wave.y = m_touchY;

	// add new waveform to waveformTouchList
    m_waveformList.push_back(wave);
}

// Move a waveform when dragging X, Y, and Pressure can change. We also extend the counters so it will stay on screen for as long as you click and drag.
void Renderer::touchDrag(float x, float y, int pressure)
{
	// if we left clicked & held in the approximate position of a waveform, snap to it and adjust x / y to simulate dragging.
	// For lines we don't worry about the x axis.
	for (std::size_t i = 0; i < m_waveformList.size(); i++) {
		if (touchedWaveform(x, y, i))
		{
            m_waveformList[i].x = x;
            m_waveformList[i].y = y;
		}
	}
}

// Remove waveform at X Y
void Renderer::touchDestroy(float x, float y)
{
	// if we right clicked approximately on the position of the waveform, then remove it from the waveform list.
	// For lines we don't worry about the x axis.
	for (std::size_t i = 0; i < m_waveformList.size(); i++) {
		if (touchedWaveform(x, y, i))
		{
            m_waveformList.erase(m_waveformList.begin() + i);
		}
	}
}

// Remove all waveforms
void Renderer::touchDestroyAll()
{
    m_waveformList.clear();
}

void Renderer::debugWriteMainTextureToFile() const
{
    std::string outputFile = frameDumpOutputFile;
    if (outputFile.empty())
    {
        static constexpr char prefix[] = "frame_texture_contents-";
        static constexpr auto prefixLen = sizeof(prefix) - 1;
        constexpr auto fileNameMaxLength = 150;
        constexpr auto fileExtensionLength = 4;
        std::vector<char> fileNameBuffer;
        fileNameBuffer.resize(fileNameMaxLength);
        std::memcpy(fileNameBuffer.data(), prefix, prefixLen);
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        const auto bytesWritten = std::strftime(fileNameBuffer.data() + prefixLen, fileNameMaxLength - prefixLen, "%Y-%m-%d-%H:%M:%S", &tm);
        const auto offset = prefixLen + bytesWritten;
        const auto spaceLeft = fileNameMaxLength - offset;
        std::snprintf(fileNameBuffer.data() + offset, spaceLeft - fileExtensionLength, "-%d.bmp", totalframes);
        outputFile = std::string(fileNameBuffer.data());
    }

    GLuint fbo;
    auto mainTexture = m_textureManager->getMainTexture();
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainTexture->texID, 0);
    auto dataSize = mainTexture->width * mainTexture->height * 3;
    GLubyte* pixels = new GLubyte[dataSize];
    glReadPixels(0, 0, mainTexture->width, mainTexture->height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);

    stbi_write_bmp(outputFile.c_str(), mainTexture->width, mainTexture->height, 3, pixels);

    delete[] pixels;
}

void Renderer::UpdateContext(PipelineContext& context)
{
    context.pixelsx = m_viewportWidth;
    context.pixelsy = m_viewportHeight;

    // It's actually the inverse of the aspect ratio.
    context.aspectx = m_fInvAspectX;
    context.aspecty = m_fInvAspectY;
}

void Renderer::CompositeOutput(const Pipeline& pipeline, const PipelineContext& pipelineContext)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textureManager->getMainTexture()->texID);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    m_renderContext.mat_ortho = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -40.0f, 40.0f);

    m_shaderEngine.enableCompositeShader(m_currentPipeline->compositeShader, pipeline, pipelineContext);

	glUniformMatrix4fv(m_shaderEngine.uniform_v2f_c4f_t2f_vertex_transformation, 1, GL_FALSE,
                       value_ptr(m_renderContext.mat_ortho));
	glUniform1i(m_shaderEngine.uniform_v2f_c4f_t2f_frag_texture_sampler, 0);

	//Overwrite anything on the screen
	glBlendFunc(GL_ONE, GL_ZERO);
	glVertexAttrib4f(1, 1.0, 1.0, 1.0, 1.0);

	glBindVertexArray(m_vaoCompositeOutput);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (auto drawable : pipeline.compositeDrawables)
		drawable->Draw(m_renderContext);

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
	memset(m_compositeVertices, 0, sizeof(composite_shader_vertex) * FCGSX * FCGSY);
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
			composite_shader_vertex* pComp = &m_compositeVertices[i + j * FCGSX];
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
	int* cur_index = &m_compositeIndices[0];
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
			composite_shader_vertex* pComp = &m_compositeVertices[i + j * FCGSX];
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
		tempv[i] = m_compositeVertices[m_compositeIndices[src_idx++]];
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vboCompositeShaderOutput);

	glBufferData(GL_ARRAY_BUFFER, sizeof(composite_shader_vertex) * primCount, nullptr, GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(composite_shader_vertex) * primCount, tempv, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBlendFunc(GL_ONE, GL_ZERO);

	glBindVertexArray(m_vaoCompositeShaderOutput);

	// Now do the final composite blit, fullscreen;
	glDrawArrays(GL_TRIANGLES, 0, primCount);

	glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::ResetPerPointMeshBuffer()
{
    if (m_perPointMeshBuffer != nullptr)
    {
        wipefree(m_perPointMeshBuffer);
    }

    int size = (m_perPixelMesh.height - 1) * m_perPixelMesh.width * 4 * 2;
    m_perPointMeshBuffer = static_cast<float *>(wipemalloc(size * sizeof(float)));

    for (int j = 0; j < m_perPixelMesh.height - 1; j++)
    {
        int base = j * m_perPixelMesh.width * 4 * 2;


        for (int i = 0; i < m_perPixelMesh.width; i++)
        {
            int index = j * m_perPixelMesh.width + i;
            int index2 = (j + 1) * m_perPixelMesh.width + i;

            int strip = base + i * 8;
            m_perPointMeshBuffer[strip + 0] = m_perPixelMesh.identity[index].x;
            m_perPointMeshBuffer[strip + 1] = m_perPixelMesh.identity[index].y;

            m_perPointMeshBuffer[strip + 4] = m_perPixelMesh.identity[index2].x;
            m_perPointMeshBuffer[strip + 5] = m_perPixelMesh.identity[index2].y;
        }
    }
}

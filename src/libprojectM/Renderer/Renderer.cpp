#include "Renderer.hpp"

using namespace std::chrono;

class Preset;

Renderer::Renderer(int viewportWidth, int viewportHeight)
{
    reset(viewportWidth, viewportHeight);
}

Renderer::~Renderer()
{
}

void Renderer::reset(int viewportWidth, int viewportHeight)
{
    m_mainTextureSizeX = viewportWidth;
    m_mainTextureSizeY = viewportHeight;

	// snap to 16x16 blocks
    m_mainTextureSizeX = ((m_mainTextureSizeX - 15) / 16) * 16;
    m_mainTextureSizeY = ((m_mainTextureSizeY - 15) / 16) * 16;
}


// Render a waveform when a touch event is triggered.
void Renderer::touch(float x, float y, int pressure, int type = 0)
{
}

// Move a waveform when dragging X, Y, and Pressure can change. We also extend the counters so it will stay on screen for as long as you click and drag.
void Renderer::touchDrag(float x, float y, int pressure)
{
}

// Remove waveform at X Y
void Renderer::touchDestroy(float x, float y)
{
}

// Remove all waveforms
void Renderer::touchDestroyAll()
{
}

#pragma once

#include "Audio/BeatDetect.hpp"

#include "ShaderEngine.hpp"
#include "projectM-opengl.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <string>


using namespace std::chrono;

// for final composite grid:
#define FCGSX 32 // final composite gridsize - # verts - should be EVEN.
#define FCGSY 24 // final composite gridsize - # verts - should be EVEN.
                 // # of grid *cells* is two less,
                 // since we have redundant verts along the center line in X and Y (...for clean 'ang' interp)
typedef struct
{
    float x, y;       // screen position + Z-buffer depth
    float Diffuse[4]; // diffuse color
    float tu, tv;     // DYNAMIC
    float rad, ang;   // STATIC
} composite_shader_vertex;

namespace libprojectM {
namespace Audio {
class BeatDetect;
}
} // namespace libprojectM

class Texture;
class TextureManager;
class TimeKeeper;

class RenderException : public std::exception
{
public:
    inline RenderException(std::string message)
        : m_message(std::move(message))
    {
    }

    virtual ~RenderException() = default;

    const std::string& message() const
    {
        return m_message;
    }

private:
    std::string m_message;
};

class Renderer
{

public:
    Renderer() = delete;

    Renderer(int viewportWidth, int viewportHeight,
             int meshX, int meshY,
             libprojectM::Audio::BeatDetect& beatDetect,
             std::vector<std::string>& textureSearchPaths);

    ~Renderer();

    void ResetTextures();
    void SetTextureSearchPaths(std::vector<std::string>& textureSearchPaths);
    void reset(int viewportWidth, int viewportHeight);

    bool timeCheck(const milliseconds currentTime, const milliseconds lastTime, const double difference);

    void setFPS(const int& theValue)
    {
        m_fps = std::to_string(theValue);
    }

    std::string fps() const
    {
        return m_fps;
    }

    milliseconds nowMilliseconds()
    {
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    }

    void touch(float x, float y, int pressure, int type);
    void touchDrag(float x, float y, int pressure);
    void touchDestroy(float x, float y);
    void touchDestroyAll();
    bool touchedWaveform(float x, float y, std::size_t i);

    std::string frameDumpOutputFile;

    milliseconds lastTimeFPS{nowMilliseconds()};
    milliseconds currentTimeFPS{nowMilliseconds()};

    int totalframes{1};
    float realfps{0.0};

    std::string title;

private:
    int nearestPower2(int value);
    void InitCompositeShaderVertex();
    float SquishToCenter(float x, float fExp);
    void UvToMathSpace(float u, float v, float* rad, float* ang);

    libprojectM::Audio::BeatDetect& m_beatDetect;
    std::unique_ptr<TextureManager> m_textureManager;
    ShaderEngine m_shaderEngine;

    std::string m_fps;

    float* m_perPointMeshBuffer{nullptr};

    int m_viewportWidth{0};
    int m_viewportHeight{0};

    std::vector<std::string>& m_textureSearchPaths; ///!< List of paths to search for texture files

    GLuint m_vboInterpolation{0};
    GLuint m_vaoInterpolation{0};

    GLuint m_vboCompositeOutput{0};
    GLuint m_vaoCompositeOutput{0};

    GLuint m_vboCompositeShaderOutput{0};
    GLuint m_vaoCompositeShaderOutput{0};

    composite_shader_vertex m_compositeVertices[FCGSX * FCGSY];
    int m_compositeIndices[(FCGSX - 2) * (FCGSY - 2) * 6];

    float m_touchX{0.0};  ///!< X position for touch waveform to start displaying(scale of 0 - 1 and not the exact coordinates)
    float m_touchY{0.0};  ///!< y position for touch waveform to start displaying(scale of 0 - 1 and not the exact coordinates)
    double m_touchR{0.0}; ///!< Red
    double m_touchG{0.0}; ///!< Green
    double m_touchB{0.0}; ///!< Blue
    double m_touchA{0.0}; ///!< Alpha

    int m_mainTextureSizeX{0};
    int m_mainTextureSizeY{0};

    float m_fAspectX{1.0};
    float m_fAspectY{1.0};
    float m_fInvAspectX{1.0};
    float m_fInvAspectY{1.0};
};

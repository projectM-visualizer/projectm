#pragma once

#include "Audio/BeatDetect.hpp"

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

    Renderer(int viewportWidth, int viewportHeight);

    ~Renderer();

    void reset(int viewportWidth, int viewportHeight);

    void touch(float x, float y, int pressure, int type);
    void touchDrag(float x, float y, int pressure);
    void touchDestroy(float x, float y);
    void touchDestroyAll();

private:
    int m_mainTextureSizeX{0};
    int m_mainTextureSizeY{0};
};

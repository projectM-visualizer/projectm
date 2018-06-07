
#ifndef RenderContext_hpp
#define RenderContext_hpp
#include "TextureManager.hpp"
#include "ShaderContext.hpp"

class BeatDetect;
class ShaderEngine;

class RenderContext
{
public:
    float time;
    int texsize;
    float aspectRatio;
    bool aspectCorrect;
    BeatDetect *beatDetect;
    TextureManager *textureManager;
    ShaderContext *shaderContext;
    
    RenderContext();
};

#endif /* RenderContext_hpp */

#pragma once

#include "projectM-opengl.h"

class Sampler
{
public:
    Sampler(const GLint _wrap_mode, const GLint _filter_mode);
    ~Sampler();

    GLuint samplerID;
    GLint wrap_mode;
    GLint filter_mode;
};
#include "Sampler.hpp"

Sampler::Sampler(const GLint _wrap_mode, const GLint _filter_mode)
    : wrap_mode(_wrap_mode)
    , filter_mode(_filter_mode)
{
    glGenSamplers(1, &samplerID);
    glSamplerParameteri(samplerID, GL_TEXTURE_MIN_FILTER, _filter_mode);
    glSamplerParameteri(samplerID, GL_TEXTURE_MAG_FILTER, _filter_mode);
    glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_S, _wrap_mode);
    glSamplerParameteri(samplerID, GL_TEXTURE_WRAP_T, _wrap_mode);
}


Sampler::~Sampler()
{
    glDeleteSamplers(1, &samplerID);
}

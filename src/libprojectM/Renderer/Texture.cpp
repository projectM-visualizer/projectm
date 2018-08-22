#include "Texture.hpp"

Sampler::Sampler(const GLint _wrap_mode, const GLint _filter_mode) :
    wrap_mode(_wrap_mode),
    filter_mode(_filter_mode)
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



Texture::Texture(const std::string &_name, const int _width, const int _height, const bool _userTexture) :
    type(GL_TEXTURE_2D),
    name(_name),
    width(_width),
    height(_height),
    userTexture(_userTexture)
{
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const std::string &_name, const GLuint _texID, const GLenum _type, const int _width, const int _height, const bool _userTexture) :
    texID(_texID),
    type(_type),
    name(_name),
    width(_width),
    height(_height),
    userTexture(_userTexture)
{
}


Texture::~Texture()
{
    glDeleteTextures(1, &texID);

    for(std::vector<Sampler*>::const_iterator iter = samplers.begin(); iter != samplers.end(); iter++)
    {
        delete (*iter);
    }
}


Sampler* Texture::getSampler(const GLint _wrap_mode, const GLint _filter_mode)
{
    for(std::vector<Sampler*>::const_iterator iter = samplers.begin(); iter != samplers.end(); iter++)
    {
        if ((*iter)->wrap_mode == _wrap_mode && (*iter)->filter_mode == _filter_mode)
        {
            return *iter;
        }
    }

    // Sampler not found -> adding it
    Sampler * sampler = new Sampler(_wrap_mode, _filter_mode);
    samplers.push_back(sampler);

    return sampler;
}

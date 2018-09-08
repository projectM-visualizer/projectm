#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <string>
#include <vector>
#include "projectM-opengl.h"


class Sampler
{
public:
    GLuint samplerID;
    GLint wrap_mode;
    GLint filter_mode;

    Sampler(const GLint _wrap_mode, const GLint _filter_mode);
    ~Sampler();
};


class Texture
{
public:

    GLuint texID;
    GLenum type;

    std::string name;
	int width;
	int height;
    bool userTexture;
    std::vector<Sampler*> samplers;

    Texture(const std::string & _name, const int _width, const int _height, const bool _userTexture);
    Texture(const std::string & _name, const GLuint _texID, const GLenum _type, const int _width, const int _height, const bool _userTexture);
    ~Texture();

    Sampler *getSampler(const GLint _wrap_mode, const GLint _filter_mode);
};

typedef std::pair<Texture*, Sampler*>   TextureSamplerDesc;

#endif /* TEXTURE_HPP_ */

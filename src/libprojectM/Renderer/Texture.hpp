#pragma once

#include "Sampler.hpp"

#include <string>
#include <vector>

class Texture
{
public:
    explicit Texture(const std::string& _name, int _width, int _height, bool _userTexture);
    explicit Texture(const std::string& _name, GLuint _texID, GLenum _type, int _width, int _height, bool _userTexture);

    ~Texture();

    /**
     * Binds the texture to the given texture unit.
     * @param slot The texture unit to bind the texture to.
     */
    void Bind(GLint slot) const;

    /**
     * Unbinds the texture to the given texture unit.
     * @param slot The texture unit to unbind the texture from.
     */
    void Unbind(GLint slot) const;

    Sampler* getSampler(const GLint _wrap_mode, const GLint _filter_mode);

    GLuint texID;
    GLenum type;

    std::string name;
    int width;
    int height;
    bool userTexture;
    std::vector<Sampler*> samplers;
};

using TextureSamplerDesc = std::pair<Texture*, Sampler*>;
